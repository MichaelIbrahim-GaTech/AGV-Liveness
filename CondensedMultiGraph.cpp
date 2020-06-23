#include "CondensedMultiGraph.h"

CondensedMultiGraph::CondensedMultiGraph(const CondensedMultiGraph& _g)
{
	Ghat = _g.Ghat;
	nh = _g.nh;
	for (int i = 0; i < _g.vertices.size(); i++)
	{
		vertices.push_back(_g.vertices[i]);
		capacities.push_back(_g.capacities[i]);
		undirected.push_back(_g.undirected[i]);
		directed.push_back(_g.directed[i]);
	}
}
CondensedMultiGraph::CondensedMultiGraph(MultiGraph* _g)
{
	Ghat = _g;
	// This function represent Algorithm 1 in the paper
	Ghat->Condense(vertices, capacities, undirected, directed, &nh);
}

vector<vector<int>> CondensedMultiGraph::GetStronglyConnectedComponents()
{
	int n = vertices.size();
	vector<set<int>> D;
	for (int i = 0; i < n; i++)
	{
		D.push_back(set<int>());
	}
	for (int i = 0; i < n; i++)
	{
		for (map<int, int>::iterator itr = undirected[i].begin(); itr != undirected[i].end(); itr++)
		{
			D[i].insert(itr->first);
			D[itr->first].insert(i);
		}
	}

	return Graph::GetStronglyConnectedComponents(D);
}

void CondensedMultiGraph::CondenseDirected(vector<vector<int>>& _nodes, vector<int>& _capacities, vector<map<int, int>>& _directed, int* _nh)
{
	_nodes = GetStronglyConnectedComponents();
	for (int i = 0; i < _nodes.size(); i++)
	{
		int capacity = 0;
		for (int j = 0; j < _nodes[i].size(); j++)
		{
			capacity += capacities[_nodes[i][j]];
		}
		_capacities.push_back(capacity);
	}
	vector<int> NewIndices;
	for (int i = 0; i < vertices.size(); i++)
	{
		NewIndices.push_back(-1);
	}
	for (int i = 0; i < _nodes.size(); i++)
	{
		for (int j = 0; j < _nodes[i].size(); j++)
		{
			NewIndices[_nodes[i][j]] = i;
		}
	}
	*_nh = NewIndices[nh];
	_directed.clear();
	for (int i = 0; i < _nodes.size(); i++)
	{
		_directed.push_back(map<int, int>());
	}
	for (int i = 0; i < directed.size(); i++)
	{
		for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			if (NewIndices[i] != NewIndices[itr->first])
			{
				if (_directed[NewIndices[i]].find(NewIndices[itr->first]) == _directed[NewIndices[i]].end())
					_directed[NewIndices[i]].insert(pair<int, int>(NewIndices[itr->first], itr->second));
				else
					_directed[NewIndices[i]][NewIndices[itr->first]] += itr->second;
			}
		}
	}
}

bool CondensedMultiGraph::isSingleChained()
{
	return false;
}