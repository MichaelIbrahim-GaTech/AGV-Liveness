#include "CondensedMultiGraph.h"

bool operator < (const CondensedMultiGraph& lhs, const CondensedMultiGraph& rhs)
{
	return lhs.vertices.size() < rhs.vertices.size();
}

bool operator > (const CondensedMultiGraph& lhs, const CondensedMultiGraph& rhs)
{
	return lhs.vertices.size() > rhs.vertices.size();
}

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

void CondensedMultiGraph::CondenseDirected(vector<vector<int>>& _nodes, vector<int>& _capacities, vector<multimap<int, int>>& _directed, int* _nh)
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
		_directed.push_back(multimap<int, int>());
	}
	for (int i = 0; i < directed.size(); i++)
	{
		for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			if (NewIndices[i] != NewIndices[itr->first])
			{
				//since it is a multi-map, you keep adding
				_directed[NewIndices[i]].insert(pair<int, int>(NewIndices[itr->first], itr->second));
			}
		}
	}
}

bool CondensedMultiGraph::isSingleChained()
{
	return (vertices.size() == 1);
}

void CondensedMultiGraph::MacroMerger(vector<int> _mergedVertices)
{
	sort(_mergedVertices.begin(), _mergedVertices.end());
	int NewVertex = _mergedVertices[0];
	set<int> SetMergedVertices;
	for (int i = 1; i < _mergedVertices.size(); i++)
		SetMergedVertices.insert(_mergedVertices[i]);
	// 1- remove edges between merged vertices
	for (int i = 0; i < _mergedVertices.size(); i++)
	{
		for (int j = 0; j < _mergedVertices.size(); j++)
		{
			if (i != j)
			{
				directed[_mergedVertices[i]].erase(_mergedVertices[j]);
				undirected[_mergedVertices[i]].erase(_mergedVertices[j]);
			}
		}
	}
	//2- Move all edges from merged vertices to the first merged vertex
	for (int i = 1; i < _mergedVertices.size(); i++)
	{
		for (int j = 0; j < vertices[_mergedVertices[i]].size(); j++)
			vertices[NewVertex].push_back(vertices[_mergedVertices[i]][j]);
		for (map<int, int>::iterator itr = directed[_mergedVertices[i]].begin(); itr != directed[_mergedVertices[i]].end(); itr++)
		{
			directed[NewVertex].insert(*itr);
		}
		for (map<int, int>::iterator itr = undirected[_mergedVertices[i]].begin(); itr != undirected[_mergedVertices[i]].end(); itr++)
		{
			if (undirected[NewVertex].find(itr->first) == undirected[NewVertex].end())
				undirected[NewVertex].insert(*itr);
			else
				undirected[NewVertex][itr->first] += itr->second;
		}
		vertices[_mergedVertices[i]].clear();
		directed[_mergedVertices[i]].clear();
		undirected[_mergedVertices[i]].clear();
	}
	// 2b- Calculate the capacity of the macro-vertex
	int _capacity = Ghat->CalculateCapacity(vertices[NewVertex]);
	// 3- modify all edges with the new vertex info
	for (int i = 0; i < vertices.size(); i++)
	{
		multimap<int, int> temp(directed[i]);
		directed[i].clear();
		for (map<int, int>::iterator itr = temp.begin(); itr != temp.end(); itr++)
		{
			if (SetMergedVertices.find(itr->first) == SetMergedVertices.end())
				directed[i].insert(*itr);
			else
				directed[i].insert(pair<int, int>(NewVertex, itr->second));
		}

	}
	// 4- 
	capacities[NewVertex] = _capacity;
	vector<int> NewIndices;
	for (int i = 0; i < vertices.size(); i++)
		NewIndices.push_back(i);
	for (int i = 1; i < _mergedVertices.size(); i++)
		NewIndices[_mergedVertices[i]] = -1;
	nh = NewIndices[nh];
	if (nh == -1)
		nh = NewVertex;
	int i1 = 0, i2 = NewIndices.size() - 1;
	while (NewIndices[i2] == -1)
		i2--;
	while (i1 < i2)
	{
		if (NewIndices[i1] == -1)
		{
			NewIndices[i1] = NewIndices[i2];
			NewIndices[i2] = -1;
			while (NewIndices[i2] == -1)
				i2--;
		}
		i1++;
	}
	// 5- check which vertices will have a different order
	map<int, int> Order;
	for (int i = 0; i < NewIndices.size(); i++)
	{
		if (NewIndices[i] != -1)
		{
			if (NewIndices[i] != i)
				Order.insert(pair<int, int>(NewIndices[i], i));
		}
	}
	if (Order.find(nh) != Order.end())
	{
		nh = Order[nh];
	}
	for (map<int, int>::iterator itr = Order.begin(); itr != Order.end(); itr++)
	{
		vertices[itr->second] = vertices[itr->first];
		directed[itr->second] = directed[itr->first];
		undirected[itr->second] = undirected[itr->first];
		capacities[itr->second] = capacities[itr->first];
		vertices[itr->first].clear();
		directed[itr->first].clear();
		undirected[itr->first].clear();
	}
	for (int i = 1; i < _mergedVertices.size(); i++)
	{
		vertices.pop_back();
		directed.pop_back();
		undirected.pop_back();
		capacities.pop_back();
	}
	// 6- modify the edges according to the new order
	for (int i = 0; i < vertices.size(); i++)
	{
		//modify the code to be a multigraph

		for (map<int, int>::iterator itr = Order.begin(); itr != Order.end(); itr++)
		{
			pair<multimap<int,int>::iterator, multimap<int, int>::iterator> result = directed[i].equal_range(itr->first);
			if (distance(result.first, result.second) > 0)
			{
				for (multimap<int, int>::iterator itr2 = result.first; itr2 != result.second; itr2++)
				directed[i].insert(pair<int, int>(itr->second, itr2->second));
				directed[i].erase(itr->first);
			}

			map<int, int>::iterator itr2 = undirected[i].find(itr->first);
			if (itr2 != undirected[i].end())
			{
				undirected[i].insert(pair<int, int>(itr->second, itr2->second));
				undirected[i].erase(itr->first);
			}
		}
	}
}