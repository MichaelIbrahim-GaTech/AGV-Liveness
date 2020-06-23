#pragma once
#include "MultiGraph.h"
#include "CondensedMultiGraph.h"

class DirectedAcyclicMultiGraph
{
	CondensedMultiGraph* C;
	int nh;
	vector<vector<int>> nodes;
	vector<int> capacities;
	vector<map<int, int>> directed;
	vector<bool> major;
	vector<map<int, int>> reversedEdges;
public:
	DirectedAcyclicMultiGraph() {}
	DirectedAcyclicMultiGraph(const DirectedAcyclicMultiGraph&);
	DirectedAcyclicMultiGraph(CondensedMultiGraph* _C);
	set<int> TerminalNodes();
};

