#include "Search.h"

using namespace std;

int main() {
	Search search;
	// run the first 2 level (0,1)
	for (int i = 0; i < 2; i++) {
		/* initialize a Matrix from a level textfile
		 * when passing this Matrix to the search.run function,
		 * a copy will be created inside the function so that the
		 * original Matrix instance can be reused */
		Matrix m("level/level" + std::to_string(i) + ".txt");
		cout << "----- level" << i << " -----" << endl;

		// Breadth first
		cout << "BFS" << endl;
		search.run(m, Search::Algorithm::BFS);
		search.printResults();

		// Depth first
		cout << "DFS" << endl;
		search.run(m, Search::Algorithm::DFS);
		search.printResults();

		// Iterative Deepening
		cout << "IDDFS" << endl;
		search.run(m, Search::Algorithm::IDDFS);
		search.printResults();

		// A* with Manhattan distance as heuristic
		cout << "A* manhatten" << endl;
		search.run(m, Search::Algorithm::ASTAR, Search::Heuristic::manhatten);
		search.printResults();

		// A* with Blocking cells as heuristic
		cout << "A* blocking" << endl;
		search.run(m, Search::Algorithm::ASTAR, Search::Heuristic::blocking);
		search.printResults();

		cout << endl;
	}

	return 0;
}
