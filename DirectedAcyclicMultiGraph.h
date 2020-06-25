#pragma once
#include "MultiGraph.h"
#include "CondensedMultiGraph.h"

#define INFINITY 1000000

class DirectedAcyclicMultiGraph
{
	CondensedMultiGraph* C;
	int nh;
	vector<vector<int>> nodes;
	vector<int> capacities;
	vector<map<int, int>> directed;
	vector<bool> major;
	vector<map<int, int>> reversedEdges;
	vector<map<int, vector<int>>> collapsedNodes;//from-to, vector of collapsed vertices
	//Get the topological order of the nodes of the DAG
	vector<int> TopologicalOrder();
	//This function 
	void CollapseNonMajorNodesPaths();
public:
	DirectedAcyclicMultiGraph() {}
	DirectedAcyclicMultiGraph(const DirectedAcyclicMultiGraph&);
	DirectedAcyclicMultiGraph(CondensedMultiGraph* _C);
	bool TerminalNodesCapacityLessThanAllInEdges();
	void Algorithm2();
};

