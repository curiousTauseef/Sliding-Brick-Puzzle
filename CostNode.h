#pragma once
#include "Node.h"

class CostNode : public Node {

public:
	/* construct a root CostNode
	 * "cost" is 0 by default until set from outside! */
	CostNode(const Matrix&);
	// copy constructor
	CostNode(const CostNode&);
	/* construct a CostNode like a normal (non-root) Node
	 * "cost" is 0 by default until set from outside! */
	CostNode(const Matrix& m, Node* parent, std::pair<int, Moves> transition);
	// empty destructor
	~CostNode() {};


	// member variable. only difference between this class and Node
	int cost;


	/* compares two CostNodes by their total cost
	* used to sort priority_queue (A*) by increasing cost!
	* to get decreasing cost, switch ">" to "<" */
	struct LessThanByTotalCost {
		bool operator() (const std::shared_ptr<CostNode>& lhs, const std::shared_ptr<CostNode>& rhs) const {
			return lhs->cost > rhs->cost;
		}
	};
};