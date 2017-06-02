#include "Search.h"
#include <sstream>
#include <queue>
#include <stack>
#include <algorithm>
#include <random>


void Search::run(const Matrix m, const Search::Algorithm a, const int heuristic(Matrix&)) {
	reset();
	// start time measure..
	auto start = std::chrono::high_resolution_clock::now();
	// create a clone to not operate on the original Matrix object
	Matrix m_clone(m);
	// select algorithm
	switch (a) {
		case RAND: randomWalk(m_clone); break;
		case BFS: bfs(m_clone); break;
		case DFS: dfs(m_clone); break;
		case IDDFS: iddfs(m_clone); break;
		case ASTAR: astar(m_clone, heuristic); break;
		default: std::cout <<
		"Error. Invalid or no algorithm provided" << std::endl;
	}

	// ..end time measure
	auto end = std::chrono::high_resolution_clock::now();
	time = (float(std::chrono::duration_cast
		<std::chrono::milliseconds>(end - start).count())) / 1000;
}


void Search::printResults(const bool printSteps) {
	std::vector<std::string> list;
	std::ostringstream os_solved;
	if (!&goalNode->m) {
		std::cout << "Error. run() method was not called. Nothing to print" << std::endl;
		return;
	}
	os_solved << goalNode->m;
	int length = goalNode->getParentCount();
	while (goalNode) {
		std::ostringstream os;
		os << "(" << goalNode->trans.first;
		os << "," << goalNode->trans.second << ")" << std::endl;
		list.push_back(os.str());
		goalNode = goalNode->parent;
	}
	// remove root nodes information about its parent which is NULL!
	list.pop_back();
	// reverse list to start from root node
	std::reverse(list.begin(), list.end());
	// append solved puzzle Matrix
	list.push_back(os_solved.str());

	// print
	if (printSteps) {
		for (auto const& s : list) {
			std::cout << s;
		}
		std::cout << std::endl << goalNode->m;
	}
	// stats
	std::cout << "#nodes: " << nodecount << "  time: " << time << "s"
		<< "  length: " << length // includes goal node and root node!
		<< std::endl;
	// clean up
	delete goalNode;
	reset();
}


void Search::reset() {
	nodecount = 0;
	time = 0;
	explored.clear();
}


// RANDOM WALK
void Search::randomWalk(Matrix& m, const unsigned int n) {
	std::cout << m << std::endl;
	for (unsigned int i = 0; i < n; i++) {
		// 1. getAllMoves
		std::unordered_map<unsigned int, std::vector<Moves>> allMoves = m.getAllMoves();

		// 2. select one move at random
		// 2.1 rand seed
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 eng(rd()); // seed the generator
								// 2.2 select a piece at random
		std::vector<int> pieceList;
		std::unordered_map<unsigned int, std::vector<Moves>>::iterator it;
		for (it = allMoves.begin(); it != allMoves.end(); ++it) {
			pieceList.push_back(it->first);
		}
		std::uniform_int_distribution<> distr(0, pieceList.size() - 1); // range both inclusives
		unsigned int rand_piece = pieceList.at(distr(eng));
		// 2.3 select a move at random
		std::vector<Moves> pieceMoves = allMoves[rand_piece];
		std::uniform_int_distribution<> distr2(0, pieceMoves.size() - 1); // range both inclusives
		Moves rand_move = pieceMoves.at(distr2(eng));

		// 3. Execute move (applyMove)
		m.applyMove(rand_piece, rand_move);

		// 4. normalize resulting matrix
		m.normalize();

		std::cout << "(" << rand_piece << "," << rand_move << ")"
			<< std::endl << std::endl << m << std::endl;

		// 5. if goal stop, else goto 1.
		if (m.isSolved()) return;
	}
}


// BREADTH FIRST SEARCH
void Search::bfs(Matrix& m) {
	std::queue<std::shared_ptr<Node>> q;
	std::vector<std::shared_ptr<Node>> visited;
	// root node
	std::shared_ptr<Node> root(new Node(Matrix(m)));
	visited.push_back(root);
	q.push(root);
	// start search
	while (!q.empty()) {
		std::shared_ptr<Node> current = q.front();
		q.pop();
		// goal reached?
		if (current->m.isSolved()) {
			nodecount = visited.size();
			goalNode = new Node(*current);
			break;
		}
		// get all valid moves and add children nodes to queue if new
		std::unordered_map<unsigned int, std::vector<Moves>> allMoves = current->m.getAllMoves();
		std::unordered_map<unsigned int, std::vector<Moves>>::iterator it;
		for (it = allMoves.begin(); it != allMoves.end(); ++it) {
			for (unsigned int j = 0; j < it->second.size(); j++) {
				// create a child Matrix
				Matrix m_child = current->m.applyMoveCloning(it->first, it->second[j]);
				m_child.normalize();
				std::pair<int, Moves> transition(it->first, it->second[j]);
				// create a child Node
				std::shared_ptr<Node> child(new Node(m_child, current.get(), transition)); // can't be unique_ptr, because its added to two containers
				// if child was not already visited, add it to the visited list and the queue
				if (!std::any_of(visited.begin(), visited.end(), 
					[&child](const std::shared_ptr<Node>& n) { return child->m == n->m; })) {
					visited.push_back(child);
					q.push(child);
				}
			}
		}
	}
}


// DEPTH FIRST SEARCH
void Search::dfs(Matrix& m) {
	std::stack<std::shared_ptr<Node>> s;
	std::vector<std::shared_ptr<Node>> visited;
	// root node
	std::shared_ptr<Node> root(new Node(Matrix(m)));
	visited.push_back(root);
	s.push(root);
	// start search
	while (!s.empty()) {
		std::shared_ptr<Node> current = s.top();
		s.pop();
		// goal reached?
		if (current->m.isSolved()) {
			nodecount = visited.size();
			goalNode = new Node(*current);
			break;
		}
		// iterate over visited container
		for (unsigned int i = 0; i < visited.size(); i++) {
			// i > 0 for the first iteration, where ONLY the root node is in the visited list
			if (i > 0 && visited.at(i)->m == current->m) { 
				break;
			}
			// ..add node to "visited" if not & explore its children (add them to the stack for later evaluation)
			if (i == visited.size() - 1) {
				visited.push_back(current); // the break at the end of this block is necessary because of this line!!
				// get all valid moves and add children nodes to stack if new
				std::unordered_map<unsigned int, std::vector<Moves>> allMoves = current->m.getAllMoves();
				std::unordered_map<unsigned int, std::vector<Moves>>::iterator it;
				for (it = allMoves.begin(); it != allMoves.end(); ++it) {
					for (unsigned int j = 0; j < it->second.size(); j++) {
						// create a child Matrix
						Matrix m_new = current->m.applyMoveCloning(it->first, it->second[j]);
						m_new.normalize();
						std::pair<int, Moves> transition(it->first, it->second[j]);
						/* create a child Node
						 * unique_ptr is sufficient here, because the instance is added to only one new owner (stack) */
						std::unique_ptr<Node> child(new Node(m_new, current.get(), transition)); 
						s.push(std::move(child)); // move ownership because the stack takes shared_ptr
					}
				}
				break;
			}
		}
	}
}


// ITERATIVE DEEPENING DEPTH FIRST SEARCH
void Search::iddfs(Matrix& m, const unsigned int limit) {
	// root node
	std::shared_ptr<Node> root(new Node(Matrix(m)));
	// start search
	for (unsigned int depth = 0; depth < limit; depth++) {
		explored.push_back(std::pair<int, std::shared_ptr<Node>>(0, root));
		Node *n = dls(*root.get(), depth);
		// goal will be reached, if this is not a nullptr!
		if (n) {
			goalNode = new Node(*n);
			break;
		}
		/* explored container is cleared before depth limit
		 * is increased and search starts again */
		explored.clear();
	}
}


// iterative deepening depth first helper function
Node* Search::dls(Node& current, int depth) {
	// goal reached?
	if (depth == 0 && current.m.isSolved()) {
		return &current;
	}
	if (depth > 0) {
		// explore all children
		std::unordered_map<unsigned int, std::vector<Moves>> allMoves = current.m.getAllMoves();
		std::unordered_map<unsigned int, std::vector<Moves>>::iterator it;
		for (it = allMoves.begin(); it != allMoves.end(); ++it) {
			for (unsigned int j = 0; j < it->second.size(); j++) {
				// create a child Matrix
				Matrix m_new = current.m.applyMoveCloning(it->first, it->second[j]);
				m_new.normalize();
				std::pair<int, Moves> transition(it->first, it->second[j]);
				// create a child Node
				std::shared_ptr<Node> child(new Node(m_new, &current, transition));
				// iterate over already explored nodes
				for (unsigned int i = 0; i < explored.size(); i++) {
					// search for duplicate state and if this state is further away from the root node
					if (explored.at(i).second->m == child->m && explored.at(i).first > depth) {
						break;
					}
					// add node to explored nodes
					if (i == explored.size() - 1) {
						nodecount++;
						explored.push_back(std::pair<int, std::shared_ptr<Node>>(depth, child));
						// recursive call
						Node *ptr = dls(*child.get(), depth - 1);
						if (ptr) {
							return new Node(*ptr);
						}
						break;
					}
				}
			}
		}
	}
	return nullptr;
}


// A* SEARCH
void Search::astar(Matrix& m, const int heuristic(Matrix&)) {
	// priority queue as container, to always continue exploring the most promising node
	std::priority_queue<std::shared_ptr<CostNode>, std::vector<std::shared_ptr<CostNode>>, CostNode::LessThanByTotalCost> pq;
	std::vector<std::shared_ptr<CostNode>> visited;
	// root (cost is heuristic only, because g(0) = 0)
	std::shared_ptr<CostNode> root(new CostNode(Matrix(m)));
	root->cost = heuristic(root->m);
	pq.push(root);
	visited.push_back(root);
	// start search
	while (!pq.empty()) {
		std::shared_ptr<CostNode> current = pq.top();
		pq.pop();
		// goal reached?
		if (current->m.isSolved()) {
			nodecount = visited.size();
			goalNode = new CostNode(*current);
			break;
		}
		// get all valid moves and add children nodes to priority queue if new
		std::unordered_map<unsigned int, std::vector<Moves>> allMoves = current->m.getAllMoves();
		std::unordered_map<unsigned int, std::vector<Moves>>::iterator it;
		for (it = allMoves.begin(); it != allMoves.end(); ++it) {
			for (unsigned int j = 0; j < it->second.size(); j++) {
				// create a child Matrix
				Matrix m_new = current->m.applyMoveCloning(it->first, it->second[j]);
				m_new.normalize();
				std::pair<int, Moves> transition(it->first, it->second[j]);
				// create a child CostNode
				std::shared_ptr<CostNode> child(new CostNode(m_new, current.get(), transition));
				/* if child was not already visited calculate it's cost,
				 * then add it to the visited list and the priority queue */
				if (!std::any_of(visited.begin(), visited.end(),
					[&child](const std::shared_ptr<Node>& n) { return child->m == n->m; })) {
					int g = child->getParentCount();
					int h = heuristic(m_new);
					child->cost = g + h; // f(n) = g(n) [step cost] + h(n) [heuristic]
					visited.push_back(child);
					pq.push(child);
				}
			}
		}
	}
}