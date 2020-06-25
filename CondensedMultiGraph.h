#pragma once
#include "MultiGraph.h"
class CondensedMultiGraph
{
	MultiGraph* Ghat;
	int nh;
	vector<vector<int>> vertices;
	vector<int> capacities;
	vector<map<int, int>> undirected;
	vector<map<int, int>> directed;
	vector<vector<int>> GetStronglyConnectedComponents();
public:
	// just in case we want to use priority queue instead of stack in Algorithm 3
	friend bool operator < (const CondensedMultiGraph& lhs, const CondensedMultiGraph& rhs);
	friend bool operator > (const CondensedMultiGraph& lhs, const CondensedMultiGraph& rhs);
	CondensedMultiGraph() {}
	CondensedMultiGraph(const CondensedMultiGraph& _g);
	CondensedMultiGraph(MultiGraph* _g);
	bool isSingleChained();
	void CondenseDirected(vector<vector<int>>& _nodes, vector<int>& capacities, vector<map<int, int>>& _directed, int* _nh);
};

