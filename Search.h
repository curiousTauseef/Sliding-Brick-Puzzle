#pragma once
#include "CostNode.h"
#include <map>
#include <chrono>
#include <cmath> // for abs(float)

class Search {

public:
	/* SEARCH ALGORITHMS
	* RAND  =  random walk (prints it's own output!)
	* BFS   =  breadth first search
	* DFS   =  depth first search
	* IDDFS =  iterative deepening depth first search
	* ASTAR =  A* search   */
	enum Algorithm { RAND, BFS, DFS, IDDFS, ASTAR };
	/* Implementations for different heuristic functions
	* encapsulated in a struct
	* (currently used for A* search algorithm only)
	* To add a new heuristic:
	*   - add its implementation to this struct in a new function
	*     the function argument must be "Matrix&" and the return type must be "const int"*/
	struct Heuristic {
		/* Manhatten distance between master brick and goal
		* The distance is calculated between the center of the master brick
		* and the center of the goal. The function disregards other bricks!
		* returns the exact number of moves that are necessary to overlap
		* the master brick with the goal fully if they are of the same
		* dimensions or partially if they're not. Works for master brick 1x1, 1x2, 2x1 and 2x2
		* returns 0 if master brick (2) overlaps the goal (-1) */
		static const int manhatten(Matrix& m) {
			float sumX = 0, sumY = 0;
			// biased master index
			std::vector<std::pair<int, int>> master_indices = m.getPieceIndices(2);
			std::pair<int, int> master_dim = m.getPieceDim(master_indices);
			for (unsigned int i = 0; i < master_indices.size(); i++) {
				sumX += master_indices[i].first;
				sumY += master_indices[i].second;
			}
			// all x indices / width and all y indices / height
			float masterX = sumX / (master_dim.first * master_dim.second);
			float masterY = sumY / (master_dim.first * master_dim.second);
			// biased goal index
			std::vector<std::pair<int, int>> goal_indices = m.getPieceIndices(-1);
			float goalX, goalY;
			/* goal_indices vector is empty, when master brick overlaps goal,
			* thus no -1 is in the matrix. In that case just set goal pos = master pos */
			if (goal_indices.size() != 0) {
				std::pair<int, int> goal_dim = m.getPieceDim(goal_indices);
				sumX = sumY = 0;
				for (unsigned int i = 0; i < goal_indices.size(); i++) {
					sumX += goal_indices[i].first;
					sumY += goal_indices[i].second;
				}
				goalX = sumX / (goal_dim.first * goal_dim.second);
				goalY = sumY / (goal_dim.first * goal_dim.second);
			}
			else {
				// no goal indices (-1) on field, thus goal is reached!
				// manhatten distance is minimum 0. return something smaller
				// here to complete solution in the end stage a few nodes faster
				return -1;
			}
			// calculate distance. center of master brick to center of goal
			// automatically crops e.g. 4.5 (float) to 4 (int)
			return (int) (std::abs(masterX - goalX) + std::abs(masterY - goalY));
		}
		/*  # of blocked cells between master brick and goal
		*  a blocked cell is a cell that has a value >2 but is not
		*  occupied by another block and between the master and the goal.
		*
		*  <EXAMPLES>
		*  +-G-G-----+    +---------+    +---------+    +---------+
		*  | X X X * |    | X X X X G    G X X X * |    | 2 * * * |
		*  | X 2 2 * |    | 2 X X X |    | X 2 2 * |    | X * * * |
		*  +---------+    +---------+    +---------+    +-G-------+
		*  G = goal
		*  X = cell that has to be evaluated here
		*  2 = master brick
		*  * = cell does not matter for this heuristic
		*
		*  <EXAMPLES WITH VALUES>
		*  +-G-G-----+    +---------+    +---------+    +---------+
		*  | 3 3 4 * |    | 0 4 5 0 G    G 3 4 5 * |    | 2 * * * |
		*  | 0 2 2 * |    | 2 6 7 0 |    | 0 2 2 * |    | 0 * * * |
		*  +---------+    +---------+    +---------+    +-G-------+
		*  3 blocking     4 blocking     3 blocking     0 blocking
		*
		*  This heuristic is ADMISSIBLE, because the # of blocked cells
		*  can only decrease towards the goal. If the master brick is moved
		*  closer towards the goal, there are fewer cells to be evaluated.
		*  Therefore, the total number of blocked cells cannot increase in this case.
		*  The # of blocked cells could increase, if an empty cell (0) swaps its place
		*  with a cell that contains a value > 2. The resulting state would not push
		*  the search closer to the goal but further away.
		*
		*  This heuristic proves to be better than the Manhatten distance,
		*  because it does not ignore the board configuration. It is important
		*  if a (potentially) blocked cell is emtpy (0) or occupied by another brick (>2)
		*  The # of blocked cells is reduced by sliding empty cells in the "potentially
		*  blocking cell" area and/or moving the master brick closer to the goal.
		*
		*  */
		static const int blocking(Matrix& m) {
			/* MASTER */
			std::vector<std::pair<int, int>> master_indices = m.getPieceIndices(2);
			std::pair<int, int> master_dim = m.getPieceDim(master_indices);
			int mX = master_indices.at(0).first; // master top left piece index x
			int mY = master_indices.at(0).second; // master top left piece index y
			int mW = master_dim.first; // master width
			int mH = master_dim.second; // master height
			/* GOAL */
			std::vector<std::pair<int, int>> goal_indices = m.getPieceIndices(-1);
			if (goal_indices.size() == 0)
				return -2; // goal is reached. return low enough value to end search faster
			std::pair<int, int> goal_dim = m.getPieceDim(goal_indices);
			int gX = goal_indices.at(0).first; // goal top left piece index x
			int gY = goal_indices.at(0).second; // goal top left piece index y
			int gW = goal_dim.first; // goal width
			int gH = goal_dim.second; // goal height

			/* counting blocking cells in the area between master brick and goal
			 * probably not the shortest solution, but somewhat lucid this way
			 * Besides, this works so TOUCH AT YOUR OWN RISK */
			int count = 0;
			if (mX == gX && mY != gY) {
				if (mY > gY) { // UP
					for (int i = mX; i < mX + mW; i++) {
						for (int j = mY; j > 0; j--) {
							if (m.at(j, i) > 2) count++;
						}
					}
				}
				else if (mY < gY) { // DOWN
					for (int i = mX; i < mX + mW; i++) {
						for (int j = mY; j < m.height - 1; j++) {
							if (m.at(j, i) > 2) count++;
						}
					}
				}
			}
			else if (mY == gY && mX != gX) {
				if (mX > gX) { // LEFT
					for (int i = mX; i > 0; i--) {
						for (int j = mY; j < mY + mH; j++) {
							if (m.at(j, i) > 2) count++;
						}
					}
				}
				else if (mX < gX) { // RIGHT
					for (int i = mX; i < m.width - 1; i++) {
						for (int j = mY; j < mY + mH; j++) {
							if (m.at(j, i) > 2) count++;
						}
					}
				}
			}
			else if (mX != gX && mY != gY) {
				int mX_gX = mX - gX;
				int mY_gY = mY - gY;
				if (mX_gX < 0 && mY_gY < 0) { // RIGHT, DOWN
					for (int i = 1; i < m.width - 1; i++) {
						for (int j = 1; j < m.height - 1; j++) {
							if (i >= mX && i < gX + gW && j >= mY && j < gY + gH) {
								if (m.at(j, i) > 2) count++;
							}
						}
					}
				}
				if (mX_gX > 0 && mY_gY < 0) { // LEFT, DOWN
					for (int i = 1; i < m.width - 1; i++) {
						for (int j = 1; j < m.height - 1; j++) {
							if (i < mX + mW && i >= gX && j >= mY && j < gY + gH) {
								if (m.at(j, i) > 2) count++;
							}
						}
					}
				}
				if (mX_gX < 0 && mY_gY > 0) { // RIGHT, UP
					for (int i = 1; i < m.width - 1; i++) {
						for (int j = 1; j < m.height - 1; j++) {
							if (i >= mX && i < gX + gW && j < mY + mH && j >= gY) {
								if (m.at(j, i) > 2) count++;
							}
						}
					}
				}
				if (mX_gX > 0 && mY_gY > 0) { // LEFT,  UP
					for (int i = 1; i < m.width - 1; i++) {
						for (int j = 1; j < m.height - 1; j++) {
							if (i < mX + mW && i >= gX && j < mY + mH && j >= gY) {
								if (m.at(j, i) > 2) count++;
							}
						}
					}
				}
			}
			/* this assigns a higher cost to board configurations
			* that block the master brick, thus pushing more towards
			* the goal. Had to be at least +3 (not 100% sure why..) */
			if (count != 0) count += 3;
			return count;
		}
	};

	// no fancy constructors/destructors necessary
	Search() {};
	~Search() {};

	// run a selected search algorithm first...
	void run(const Matrix, const Search::Algorithm, const int heuristic(Matrix&) = Heuristic::manhatten);
	/* ...then print results (not for random walk!)
	 * on top of the default "nodecount", "time" and "length"
	 * output, printSteps = true will output a step by step solution*/
	void printResults(const bool printSteps = false);


private:
	/* "goalNode" and "nodecount" MUST be set by search
	* algorithm function (on completion) before returning.
	* Otherwise results will not print.
	* "time" is measured automatically */
	Node* goalNode;
	// # of nodes explored
	int nodecount;
	// time the search took in s 
	float time; 


	// resets the member variables (not goalNode)
	void reset();


	/** SEARCH ALGORITHMS **/
	
	// random walk (prints its output itself!)
	void randomWalk(Matrix&, const unsigned int = 3);
	// breadth first
	void bfs(Matrix&);
	// depth first
	void dfs(Matrix&);
	// iterative deepening depth first. depth limited to 100 (plenty!)
	void iddfs(Matrix&, const unsigned int = 100);
	Node* dls(Node&, int);
	std::vector<std::pair<int, std::shared_ptr<Node>>> explored;
	// A*. takes a function pointer for a heuristic function
	void astar(Matrix&, const int heuristic(Matrix&));
};