#pragma once
#include "MultiGraph.h"
class CondensedMultiGraph
{
	MultiGraph* Ghat;
	vector<vector<int>> vertices;
	vector<int> capacities;
	vector<map<int, int>> directed;
	vector<map<int, int>> undirected;
public:
	CondensedMultiGraph() {}
	CondensedMultiGraph(const CondensedMultiGraph& _g);
	CondensedMultiGraph(MultiGraph* _g);
};

