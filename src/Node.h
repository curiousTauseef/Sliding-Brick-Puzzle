#pragma once
#include <memory>
#include "Matrix.h"


class Node {

public:
	//Node() {};
	// root node..
	Node(const Matrix&);
	// ..all other nodes
	Node(const Matrix& m, Node* parent, std::pair<int, Moves> transition);
	// cpy constructor
	Node(const Node&);
	// empty destructor
	~Node() {};


	// Matrix (current state of the puzzle)
	Matrix m;
	// reference to the parent node (only nullptr for root node)
	Node* parent;
	// transition (move on a piece) that stood between parent and this node
	std::pair<int, Moves> trans;


	/* copy & swap idiom
	* copy constructor and assign operator */
	friend void swap(Node&, const Node&);
	// for backtracing step cost (depth in tree)
	const int getParentCount() const;

};