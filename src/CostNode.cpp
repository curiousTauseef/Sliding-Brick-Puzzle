#include "CostNode.h"


CostNode::CostNode(const Matrix& m) : Node(m), cost(0) {
}


CostNode::CostNode(const CostNode& other) : Node(other), cost(other.cost) {
}


CostNode::CostNode(const Matrix& m, Node* parent, std::pair<int, Moves> trans)
	: Node(m, parent, trans), cost(0) {
}