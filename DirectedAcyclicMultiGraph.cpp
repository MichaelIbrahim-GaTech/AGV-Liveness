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
	}
	for (int i = 0; i < directed.size(); i++)
	{
		for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			reversedEdges[itr->first].insert(pair<int, int>(i, itr->second));
		}
	}
	for (int i = 0; i < nodes.size(); i++)
	{
		if (capacities[i] > 0)
		{
			major[i] = true;
		}
		else
		{
			int OutDegree = 0;
			for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
			{
				OutDegree += itr->second;
			}
			if (OutDegree >= 2)
			{
				major[i] = true;
			}
			else
			{

				int InDegree = 0;
				for (map<int, int>::iterator itr = reversedEdges[i].begin(); itr != reversedEdges[i].end(); itr++)
				{
					InDegree += itr->second;
				}
				if (InDegree >= 2)
				{
					major[i] = true;
				}
			}
		}
	}
}

set<int> DirectedAcyclicMultiGraph::TerminalNodes()
{

	return set<int>();
}

void DirectedAcyclicMultiGraph::Algorithm2()
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
	vector<int> order = Graph::topologicalSort(D);
}