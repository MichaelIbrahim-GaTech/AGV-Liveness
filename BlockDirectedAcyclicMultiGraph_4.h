#pragma once
#include "DirectedAcyclicMultiGraph.h"
class BlockDirectedAcyclicMultiGraph_4
{
	vector<int> capacities;
	vector<bool> used;
	vector<multimap<int, int>> directed;
	vector<multimap<int, int>> reversedEdges;
	vector<map<int, vector<int>>> TNEs;

	DirectedAcyclicMultiGraph* U;
	int parentAP;

public:
	BlockDirectedAcyclicMultiGraph_4(DirectedAcyclicMultiGraph* _U, set<int> Block, int _parentAP);
	//call Algorithm2
	bool IsThereATerminalNodeThatDoesntHaveMaximalCapacity(CondensedMultiGraph* _C);
	//call Algorithm3
	bool ExistAMeregerSequenceForATerminalNode(CondensedMultiGraph* _C, bool& _terminate, bool& _simulate);
	//implements Algorithm4 in NAHS paper
	void Algorithm4_NAHS(CondensedMultiGraph* _C);
};

