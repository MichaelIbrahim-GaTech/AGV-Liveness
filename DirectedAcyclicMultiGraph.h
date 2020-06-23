#pragma once
#include "MultiGraph.h"
#include "CondensedMultiGraph.h"

class DirectedAcyclicMultiGraph
{
	CondensedMultiGraph* C;
	int nh;
public:
	DirectedAcyclicMultiGraph() {}
	DirectedAcyclicMultiGraph(const DirectedAcyclicMultiGraph&);
	DirectedAcyclicMultiGraph(CondensedMultiGraph* _C);
	set<int> TerminalNodes();
};

