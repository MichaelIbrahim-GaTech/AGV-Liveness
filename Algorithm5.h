#pragma once
#include "Algorithm2.h"
#include "Algorithm3.h"
#include "CondensedMultiGraph.h"
#include "DirectedAcyclicMultiGraph.h"
#include "BlockDirectedAcyclicMultiGraph.h"
#include "BlockDirectedAcyclicMultiGraph_3.h"
#include "BlockDirectedAcyclicMultiGraph_4.h"

class Algorithm5
{
public:
	bool OneStep(DirectedAcyclicMultiGraph U, CondensedMultiGraph* Cd);
	bool OneStep(BlockDirectedAcyclicMultiGraph U, CondensedMultiGraph* Cd);
	bool OneStep(BlockDirectedAcyclicMultiGraph_3 U, CondensedMultiGraph* Cd);
	bool OneStep(BlockDirectedAcyclicMultiGraph_4 U, CondensedMultiGraph* Cd, bool& _simulate);
};

