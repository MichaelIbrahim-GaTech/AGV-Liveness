#pragma once
#include "Algorithm2.h"
#include "Algorithm3.h"
#include "CondensedMultiGraph.h"
#include "DirectedAcyclicMultiGraph.h"

class Algorithm5
{
	bool IsThereATerminalNodeThatDoesntHaveMaximalCapacity();
public:
	bool OneStep(DirectedAcyclicMultiGraph U, CondensedMultiGraph* Cd);
};

