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
	set<int> subTreeVertices;
	bool subTreeVerticesUsed;
public:
	//Simulation(CondensedMultiGraph C, int vertex, int nodeCapacity);
	Simulation(CondensedMultiGraph C, int vertex, int nodeCapacity, set<int> _subTreeVertices);
	bool simulate(CondensedMultiGraph& C);
};

