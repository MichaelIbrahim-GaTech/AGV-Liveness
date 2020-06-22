#include "MultiGraph.h"

MultiGraph::MultiGraph()
{
	n = 0;
}

MultiGraph::MultiGraph(const MultiGraph& _g)
{
	n = _g.n;
	for (int i = 0; i < n; i++)
	{
		directed.push_back(_g.directed[i]);
		undirected.push_back(_g.undirected[i]);
	}
}

MultiGraph::MultiGraph(vector<map<int, int>> _directed)
{
	n = _directed.size();
	for (int i = 0; i < n; i++)
	{
		map<int, int> to_count;
		directed.push_back(to_count);
		undirected.push_back(to_count);
	}
	for (int i = 0; i < n; i++)
	{
		for (map<int, int>::iterator itr = _directed[i].begin(); itr != _directed[i].end(); itr++)
		{
			int from = i, to = itr->first, count = itr->second;
			if (from > to)
			{
				int temp = from;
				from = to; 
				to = temp;
			}
			if (directed[from].find(to) == directed[from].end())
			{
				directed[from].insert(pair<int, int>(to, count));
			}
			else
			{
				directed[from][to] += count;
			}
		}
	}
}

bool MultiGraph::AddVehicules(vector<map<int, int>> _undirected, bool clear )
{
	if (_undirected.size() != n)
		return false;
	if (clear)
	{
		undirected.clear();
		for (int i = 0; i < n; i++)
		{
			map<int, int> to_count;
			undirected.push_back(to_count);
		}
	}

	for (int i = 0; i < n; i++)
	{
		for (map<int, int>::iterator itr = _undirected[i].begin(); itr != _undirected[i].end(); itr++)
		{
			int from = i, to = itr->first, count = itr->second;
			if (from < to)
			{
				if (directed[from].find(to) == directed[from].end())
				{
					return false;
				}
				else
				{
					int cnt = directed[from][to];
					if (cnt < count)
						return false;
					else if (cnt == count)
						directed[from].erase(to);
					else
						directed[from][to] -= count;
				}
			}
			else
			{
				if (directed[to].find(from) == directed[to].end())
				{
					return false;
				}
				else
				{
					int cnt = directed[to][from];
					if (cnt < count)
						return false;
					else if (cnt == count)
						directed[to].erase(from);
					else
						directed[to][from] -= count;
				}
			}
			if (undirected[from].find(to) == undirected[from].end())
			{
				undirected[from].insert(pair<int, int>(to, count));
			}
			else
			{
				undirected[from][to] += count;
			}
		}
	}


	return true;
}

vector<vector<int>> MultiGraph::GetStronglyConnectedComponents()
{
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
		}
		for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			D[i].insert(itr->first);
			D[itr->first].insert(i);
		}
	}
	
	return Graph::GetStronglyConnectedComponents(D);
}

vector<int> MultiGraph::GetDegree(const vector<int>& vertices, const vector<bool>& considered)
{
	vector<int> degree;
	for (int i = 0; i < vertices.size(); i++)
		degree.push_back(0);
	for (int i = 0; i < vertices.size(); i++)
	{
		if (considered[i])
		{
			for (int j = i + 1; j < vertices.size(); j++)
			{
				if (considered[j])
				{
					int from = vertices[i], to = vertices[j];
					if (from > to)
					{
						int temp = from;
						from = to;
						to = temp;
					}
					if (undirected[from].find(to) != undirected[from].end())
					{
						degree[i] += undirected[from][to];
						degree[j] += undirected[from][to];
					}
					if (undirected[to].find(from) != undirected[to].end())
					{
						degree[i] += undirected[to][from];
						degree[j] += undirected[to][from];
					}
					if (directed[from].find(to) != directed[from].end())
					{
						degree[i] += directed[from][to];
						degree[j] += directed[from][to];
					}
				}
			}
		}
	}

	return degree;
}

vector<vector<int>> MultiGraph::RemoveAllOneDegreeVertices(const vector<vector<int>>& _ds)
{
	vector<vector<int>> result;
	for (int i = 0; i < _ds.size(); i++) 
	{
		vector<int> vertices = _ds[i];
		sort(vertices.begin(), vertices.end());
		vector<bool> considered;
		for (int j = 0; j < vertices.size(); j++)
		{
			considered.push_back(true);
		}
		bool changed = true;
		while (changed)
		{
			changed = false;
			vector<int> degree = GetDegree(vertices, considered);
			for (int j = 0; j < vertices.size(); j++)
			{
				if (considered[j])
				{
					if (degree[j] <= 1)
					{
						considered[j] = false;
						changed = true;
					}
				}
			}
		}
		vector<int> temp;
		for (int j = 0; j < vertices.size(); j++)
		{
			if (considered[j])
				temp.push_back(vertices[j]);
		}
		if (temp.size() > 1)
			result.push_back(temp);

	}
	return result;
}

vector<int> MultiGraph::CalculateCapacities(const vector<vector<int>>& _ds)
{
	vector<int> capacities;
	for (int i = 0; i < _ds.size(); i++)
	{
		int capacity = 0;
		vector<set<int>> edges;//adjacency list of edges
		for (int j = 0; j < _ds[i].size(); j++)
			edges.push_back(set<int>());
		for (int j = 0; j < _ds[i].size(); j++)
		{
			for (int k = j + 1; k < _ds[i].size(); k++)
			{
				map<int, int>::iterator it = undirected[_ds[i][j]].find(_ds[i][k]);
				if (it != undirected[_ds[i][j]].end())
				{
					edges[j].insert(k);
					edges[k].insert(j);
				}
				it = undirected[_ds[i][k]].find(_ds[i][j]);
				if (it != undirected[_ds[i][k]].end())
				{
					edges[k].insert(j);
					edges[j].insert(k);
				}
				if (_ds[i][j] < _ds[i][k])
				{
					it = directed[_ds[i][j]].find(_ds[i][k]);
					if (it != directed[_ds[i][j]].end())
					{
						edges[j].insert(k);
						edges[k].insert(j);
						capacity += it->second;
					}
				}
				else
				{
					it = directed[_ds[i][k]].find(_ds[i][j]);
					if (it != directed[_ds[i][k]].end())
					{
						edges[k].insert(j);
						edges[j].insert(k);
						capacity += it->second;
					}
				}
			}
		}
		vector<pair<int,int>> bridges = Graph::bridge(edges);
		for (int j = 0; j < bridges.size(); j++)
		{
			// we substract an edge if it was free
			if (undirected[_ds[i][bridges[j].first]].find(_ds[i][bridges[j].second]) == undirected[_ds[i][bridges[j].first]].end())
			{
				// we substract an edge if it was free
				if (undirected[_ds[i][bridges[j].second]].find(_ds[i][bridges[j].first]) == undirected[_ds[i][bridges[j].second]].end())
				{
					int from = _ds[i][bridges[j].first], to = _ds[i][bridges[j].second];
					if (from > to)
					{
						int temp = from;
						from = to;
						to = temp;
					}
					// remove a bridge from capacity if it is a single edge no a multi-edge
					if (directed[from].find(to)->second <= 1)
						capacity--;
				}
			}
		}
		capacities.push_back(capacity);
	}
	return capacities;
}
// This function represent Algorithm 1 in the paper
void MultiGraph::Condense(vector<vector<int>>& _vertices, vector<int>& capacities, vector<map<int, int>>& _directed, vector<map<int, int>>& _undirected)
{
	vector<vector<int>> SCC = GetStronglyConnectedComponents();
	_vertices = RemoveAllOneDegreeVertices(SCC);
	capacities = CalculateCapacities(_vertices);
	vector<bool> UsedVertices;
	vector<int> NewIndices;
	for (int i = 0; i < n; i++)
	{
		UsedVertices.push_back(false);
		NewIndices.push_back(-1);
	}
	for (int i = 0; i < _vertices.size(); i++)
	{
		for (int j = 0; j < _vertices[i].size(); j++)
		{
			UsedVertices[_vertices[i][j]] = true;
			NewIndices[_vertices[i][j]] = i;
		}
	}
	for (int i = 0; i < n; i++)
	{
		if (!UsedVertices[i])
		{
			vector<int> temp;
			temp.push_back(i);
			NewIndices[i] = _vertices.size();
			_vertices.push_back(temp);
			capacities.push_back(0);
		}
	}
	_directed.clear(); _undirected.clear();
	for (int i = 0; i < _vertices.size(); i++)
	{
		_directed.push_back(map<int, int>());
		_undirected.push_back(map<int, int>());
	}

	for (int i = 0; i < n; i++)
	{
		for (map<int, int>::iterator itr = undirected[i].begin(); itr != undirected[i].end(); itr++)
		{
			if (NewIndices[i] != NewIndices[itr->first])
			{
				if (_undirected[NewIndices[i]].find(NewIndices[itr->first]) == _undirected[NewIndices[i]].end())
					_undirected[NewIndices[i]].insert(pair<int, int>(NewIndices[itr->first], itr->second));
				else
					_undirected[NewIndices[i]][NewIndices[itr->first]] += itr->second;
			}
		}
		for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			if (NewIndices[i] != NewIndices[itr->first])
			{
				int _from = NewIndices[i], _to = NewIndices[itr->first];
				if (_from > _to)
				{
					int temp = _from;
					_from = _to;
					_to = temp;
				}
				if (_directed[_from].find(_to) == _directed[_from].end())
					_directed[_from].insert(pair<int, int>(_to, itr->second));
				else
					_directed[_from][_to] += itr->second;
			}
		}
	}

}