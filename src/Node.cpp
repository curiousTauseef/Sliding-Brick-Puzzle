#include "Node.h"


Node::Node(const Matrix& m) : m(m) {
	parent = nullptr;
}


Node::Node(const Matrix& m, Node* parent, std::pair<int, Moves> trans)
	: m(m), parent(parent), trans(trans) {
}


Node::Node(const Node& other) {
	swap(*this, other);
}


const int Node::getParentCount() const {
	Node *n = parent;
	int count = 0;
	while (n) {
		count++;
		n = n->parent;
	}
	return count;
}


void swap(Node& lhs, const Node& rhs) {
	Node *n1 = 0; // current
	Node *n2 = 0; // next

	if (rhs.parent == nullptr) { // only true for root node
		lhs.parent = nullptr;
		return;
	}
	else {
		lhs.m = rhs.m;
		lhs.parent = new Node(rhs.m, rhs.parent, rhs.trans);
		lhs.trans = rhs.trans;
		n1 = &lhs;
		n2 = rhs.parent;
	}
	while (n2) {
		n1->parent = new Node(n2->m, n2->parent, n2->trans);
		n1 = n1->parent;
		n2 = n2->parent;
	}
	n1->parent = nullptr;
}
