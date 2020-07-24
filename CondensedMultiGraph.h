#pragma once
#include "MultiGraph.h"
class CondensedMultiGraph
{
	MultiGraph* Ghat;
	int nh;
	vector<int> capacities;
	vector<map<int, int>> undirected;
	vector<multimap<int, int>> directed;
	vector<vector<int>> GetStronglyConnectedComponents();
public:
	vector<vector<int>> vertices;
	// just in case we want to use priority queue instead of stack in Algorithm 3
	friend bool operator < (const CondensedMultiGraph& lhs, const CondensedMultiGraph& rhs);
	friend bool operator > (const CondensedMultiGraph& lhs, const CondensedMultiGraph& rhs);
	CondensedMultiGraph() {}
	CondensedMultiGraph(const CondensedMultiGraph& _g);
	CondensedMultiGraph(MultiGraph* _g);
	bool isSingleChained();
	bool isComplexComponent(int v);
	void CondenseDirected(vector<vector<int>>& _nodes, vector<int>& _capacities, vector<multimap<int, int>>& _directed, int* _nh);
	void MacroMerger(vector<int> _mergedVertices);
};

