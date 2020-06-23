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
	CondensedMultiGraph() {}
	CondensedMultiGraph(const CondensedMultiGraph& _g);
	CondensedMultiGraph(MultiGraph* _g);
	bool isSingleChained();
	void CondenseDirected(vector<vector<int>>& _nodes, vector<int>& capacities, vector<map<int, int>>& _directed, int* _nh);
};

