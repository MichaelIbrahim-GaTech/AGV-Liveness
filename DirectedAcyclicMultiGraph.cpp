#include "DirectedAcyclicMultiGraph.h"

DirectedAcyclicMultiGraph::DirectedAcyclicMultiGraph(const DirectedAcyclicMultiGraph& _g)
{
	C = _g.C;
	nh = _g.nh;
	for (int i = 0; i < _g.nodes.size(); i++)
	{
		nodes.push_back(_g.nodes[i]);
		capacities.push_back(_g.capacities[i]);
		directed.push_back(_g.directed[i]);
		major.push_back(_g.major[i]);
		reversedEdges.push_back(reversedEdges[i]);
		collapsedNodes.push_back(collapsedNodes[i]);
	}
}

DirectedAcyclicMultiGraph::DirectedAcyclicMultiGraph(CondensedMultiGraph* _C)
{
	C = _C;
	C->CondenseDirected(nodes, capacities, directed, &nh);
	for (int i = 0; i < nodes.size(); i++)
	{
		major.push_back(false);
		reversedEdges.push_back(map<int, int>());
		collapsedNodes.push_back(map<int, vector<int>>());
	}
	for (int i = 0; i < directed.size(); i++)
	{
		for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			reversedEdges[itr->first].insert(pair<int, int>(i, itr->second));
		}
	}
	major[nh] = true;
	capacities[nh] = INFINITY;
	for (int i = 0; i < nodes.size(); i++)
	{
		if (capacities[i] > 0)
		{
			major[i] = true;
		}
		else
		{
			int OutDegree = directed[i].size();
			if (OutDegree >= 2)
			{
				major[i] = true;
			}
			else
			{

				int InDegree = reversedEdges[i].size();
				if (InDegree >= 2)
				{
					major[i] = true;
				}
			}
		}
	}
	CollapseNonMajorNodesPaths();
}

void DirectedAcyclicMultiGraph::CollapseNonMajorNodesPaths()
{ 
	for (int i = nodes.size() - 1; i >= 0; i--)
	{
		if (!major[i])
		{
			if ((reversedEdges[i].size() == 1) && (directed[i].size() == 1))
			{
				// put the necessary code to collapse this node
			}
		}
	}
}

bool DirectedAcyclicMultiGraph::TerminalNodesCapacityLessThanAllInEdges()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		if (directed[i].size() == 0)//this is a terminal node
		{
			bool AllLess = true;
			for (map<int, int>::iterator itr = reversedEdges[i].begin(); itr != reversedEdges[i].end(); itr++)
			{
				if (itr->second <= capacities[i])
				{
					AllLess = false;
					break;
				}
			}
			if (AllLess)
				return true;
		}
	}
	return false;
}

vector<int> DirectedAcyclicMultiGraph::TopologicalOrder()
{
	int n = nodes.size();
	vector<set<int>> D;
	for (int i = 0; i < n; i++)
	{
		D.push_back(set<int>());
	}
	for (int i = 0; i < n; i++)
	{
		for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			D[i].insert(itr->first);
		}
	}
	return Graph::topologicalSort(D);
}

bool DirectedAcyclicMultiGraph::ExistAPathLeadingToNH(CondensedMultiGraph* _C)
{
	if (reversedEdges[nh].size() > 0)
	{
		*_C = *C;
		// performe a merger here
		vector<int> MergedVertices(nodes[nh]);
		for (map<int, int>::iterator itr = reversedEdges[nh].begin(); itr != reversedEdges[nh].end(); itr++)
		{
			for (int i = 0; i < nodes[itr->first].size(); i++)
			{
				MergedVertices.push_back(nodes[itr->first][i]);
			}
		}
		_C->MacroMerger(MergedVertices, INFINITY);
		return true;
	}
	else
	{
		return false;
	}
}

// This function implements Algorithm 2 of the paper
bool DirectedAcyclicMultiGraph::ExistAProducerMerger(CondensedMultiGraph* _C)
{
	vector<int> order = TopologicalOrder();
	return false;
}


vector<CondensedMultiGraph> DirectedAcyclicMultiGraph::PickATerminalNodeAndCollapseFeasiblePaths()
{
	return vector<CondensedMultiGraph>();
}