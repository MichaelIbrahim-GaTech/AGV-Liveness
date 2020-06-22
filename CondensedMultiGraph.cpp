#include "CondensedMultiGraph.h"

CondensedMultiGraph::CondensedMultiGraph(const CondensedMultiGraph& _g)
{
	Ghat = _g.Ghat;
	for (int i = 0; i < _g.vertices.size(); i++)
	{
		vertices.push_back(_g.vertices[i]);
		capacities.push_back(_g.capacities[i]);
		directed.push_back(_g.directed[i]);
		undirected.push_back(_g.undirected[i]);
	}
}
CondensedMultiGraph::CondensedMultiGraph(MultiGraph* _g)
{
	Ghat = _g;
	// This function represent Algorithm 1 in the paper
	Ghat->Condense(vertices, capacities, directed, undirected);
}