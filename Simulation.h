#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <queue>
#include "MultiGraph.h"
#include "CondensedMultiGraph.h"
#include "DirectedAcyclicMultiGraph.h"
#include "TreeLiveness.h"
#include "Algorithm5.h"
#include "BlockCutpointTree.h"
class Simulation
{
	int originalVertex;
	int originalNodeCapacity;
	CondensedMultiGraph originalC;
public:
	Simulation(CondensedMultiGraph C, int vertex, int nodeCapacity);
	bool simulate(CondensedMultiGraph& C);
};

