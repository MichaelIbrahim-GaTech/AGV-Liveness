#include "DirectedAcyclicMultiGraph.h"

DirectedAcyclicMultiGraph::DirectedAcyclicMultiGraph(CondensedMultiGraph* _C)
{
	C = _C;
}

set<int> DirectedAcyclicMultiGraph::TerminalNodes()
{

	return set<int>();
}