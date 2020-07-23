#include "TreeLiveness.h"

TreeLiveness::TreeLiveness()
{
}

TreeLiveness::TreeLiveness(const TreeLiveness& _g)
{
	nh = _g.nh;
	for (int i = 0; i < _g.capacities.size(); i++)
	{
		capacities.push_back(_g.capacities[i]);
		directed.push_back(_g.directed[i]);
		reversedEdges.push_back(_g.reversedEdges[i]);
		layers.push_back(_g.layers[i]);
		processed.push_back(_g.processed[i]);
		parent.push_back(_g.parent[i]);
	}
}
TreeLiveness::TreeLiveness(const DirectedAcyclicMultiGraph& _g)
{
	nh = _g.nh;
	for (int i = 0; i < _g.capacities.size(); i++)
	{
		capacities.push_back(_g.capacities[i]);
		directed.push_back(_g.directed[i]);
		reversedEdges.push_back(_g.reversedEdges[i]);
		layers.push_back(-1);
		processed.push_back(false);
		parent.push_back(-1);
	}
}

TreeLiveness& TreeLiveness::operator=(const DirectedAcyclicMultiGraph& _g)
{
	nh = _g.nh;
	for (int i = 0; i < _g.capacities.size(); i++)
	{
		capacities.push_back(_g.capacities[i]);
		directed.push_back(_g.directed[i]);
		reversedEdges.push_back(_g.reversedEdges[i]);
		layers.push_back(-1);
		processed.push_back(false);
		parent.push_back(-1);
	}
	return *this;
}

set<int> TreeLiveness::SetAllReachableNodes(set<int> Nodes, int currentLayer)
{
	stack<pair<int,int>> Explore;
	set<int> Visited;
	for (set<int>::iterator itr = Nodes.begin(); itr != Nodes.end(); itr++)
		Explore.push(pair<int,int>(*itr,parent[*itr]));
	while (!Explore.empty())
	{
		pair<int,int> current = Explore.top();
		Explore.pop();
		if (Visited.find(current.first) == Visited.end())
		{
			Visited.insert(current.first);
			for (map<int, int>::iterator itr = directed[current.first].begin(); itr != directed[current.first].end(); itr++)
				if (layers[itr->first] == -1)
					Explore.push(pair<int,int>(itr->first,current.first));
			if (layers[current.first] == -1)
			{
				layers[current.first] = currentLayer;
				parent[current.first] = current.second;
			}
		}
	}
	for (set<int>::iterator itr = Nodes.begin(); itr != Nodes.end(); itr++)
		Visited.erase(*itr);
	return Visited;
}

set<int> TreeLiveness::SetAllCoReachableNodes(set<int> Nodes, int currentLayer)
{
	stack<pair<int, int>> Explore;
	set<int> Visited;
	for (set<int>::iterator itr = Nodes.begin(); itr != Nodes.end(); itr++)
		Explore.push(pair<int, int>(*itr, parent[*itr]));
	while (!Explore.empty())
	{
		pair<int, int> current = Explore.top();
		Explore.pop();
		if (Visited.find(current.first) == Visited.end())
		{
			Visited.insert(current.first);
			for (map<int, int>::iterator itr = reversedEdges[current.first].begin(); itr != reversedEdges[current.first].end(); itr++)
				if (layers[itr->first] == -1)
					Explore.push(pair<int, int>(itr->first, current.first));
			if (layers[current.first] == -1)
			{
				layers[current.first] = currentLayer;
				parent[current.first] = current.second;
			}
		}
	}
	for (set<int>::iterator itr = Nodes.begin(); itr != Nodes.end(); itr++)
		Visited.erase(*itr);
	return Visited;
}

void TreeLiveness::ComputeLayers()
{
	//1- nh and all nodes reachable from nh are layer 1
	layers[nh] = 1;
	set<int> Nodes;
	Nodes.insert(nh);
	Nodes = SetAllReachableNodes(Nodes, 1);
	Nodes.insert(nh);
	// 2- compute layers reccursively
	int currentLayer = 2;
	while (!Nodes.empty())
	{
		if (currentLayer % 2 == 0)//even layer
		{
			Nodes = SetAllCoReachableNodes(Nodes, currentLayer);
		}
		else //odd layer
		{
			Nodes = SetAllReachableNodes(Nodes, currentLayer);
		}
		currentLayer++;
	}
	// 3- fill the children
	children.clear();
	for (int i = 0; i < capacities.size(); i++)
		children.push_back(set<int>());
	for (int i = 0; i < capacities.size(); i++)
		if (parent[i] != -1)
			children[parent[i]].insert(i);
}

bool TreeLiveness::ExistUnprocessed()
{
	for (int i = 0; i < capacities.size(); i++)
		if (!processed[i])
			return true;

	return false;
}

int TreeLiveness::PickANodeToProcess()
{
	for (int i = 0; i < capacities.size(); i++)
	{
		if (i == nh)
			continue;
		if (!processed[i])
		{
			//check for a leaf node
			bool leaf = children[i].empty();
			if (leaf)
				return i;
			// check for a node with all children processed
			bool AllChildrenProcessed = true;
			for (set<int>::iterator itr = children[i].begin(); itr != children[i].end(); itr++)
			{
				if (!processed[*itr])
				{
					AllChildrenProcessed = false;
					break;
				}
			}
			if (AllChildrenProcessed)
				return i;
		}
	}
	cout << "There are no leaf nodes" << endl;
	return -1;//if none is found there is a problems
}

vector<vector<int>> TreeLiveness::GetAllPathsEmanatingFromNode(int _n)
{
	int n = capacities.size();
	vector<set<int>> D;
	for (int i = 0; i < n; i++)
	{
		D.push_back(set<int>());
	}
	for (int i = 0; i < n; i++)
	{
		for (multimap<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			D[i].insert(itr->first);
		}
	}
	vector<vector<int>> Paths;
	Graph::GetAllPaths(_n, D, Paths);
	return Paths;
}

bool TreeLiveness::ExistAProducerMergerEmanatingFromNode(int n, vector<int>& _path, int& _capacity)
{
	_path.clear();
	for (map<int, int>::iterator itr = directed[n].begin(); itr != directed[n].end(); itr++)
	{
		_capacity = capacities[n] + capacities[itr->first] - itr->second;
		if (_capacity >= capacities[itr->first])
		{
			_path.push_back(n);
			_path.push_back(itr->first);
			return true;
		}
	}
	return false;

	vector<vector<int>> Paths = GetAllPathsEmanatingFromNode(n);
	for (int i = 0; i < Paths.size(); i++)
	{
		vector<int> MergedCapacities;
		MergedCapacities.push_back(capacities[Paths[i][0]]);
		for (int j = 1; j < Paths[i].size(); j++)
		{
			int capacity = MergedCapacities[j - 1] + capacities[Paths[i][j]] - directed[Paths[i][j - 1]].find(Paths[i][j])->second;
			if (capacity < 0)
				break;
			MergedCapacities.push_back(capacity);
		}
		for (int j = MergedCapacities.size() - 1; j > 0; j--)
		{
			if (MergedCapacities[j] >= capacities[Paths[i][j]])//This is a producer merger
			{
				_capacity = MergedCapacities[j];
				for (int k = 0; k <= j; k++)
					_path.push_back(Paths[i][k]);
				return true;
			}
		}
	}
	return false;
}

//////this function returns the parent of an odd layered node
////int TreeLiveness::Parent(int _n)
////{
////	for (map<int, int>::iterator itr = reversedEdges[_n].begin(); itr != reversedEdges[_n].end(); itr++)
////	{
////		if (layers[itr->first] <= layers[_n])
////			return itr->first;
////	}
////	return -1;//there is a problem
////}

vector<int> TreeLiveness::GetAllCoReachableNodes(int node)
{

	stack<int> Explore;
	set<int> Visited;
	Explore.push(node);
	while (!Explore.empty())
	{
		int current = Explore.top();
		Explore.pop();
		if (Visited.find(current) == Visited.end())
		{
			Visited.insert(current);
			for (map<int, int>::iterator itr = reversedEdges[current].begin(); itr != reversedEdges[current].end(); itr++)
				Explore.push(itr->first);
		}
	}
	vector<int> Nodes;
	for (set<int>::iterator itr = Visited.begin(); itr != Visited.end(); itr++)
		if (*itr != node)
			Nodes.push_back(*itr);
	return Nodes;
}

void TreeLiveness::Merge(vector<int> _mergedVertices, int _capacity)
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
			}
		}
	}

	//2- Move all edges from merged vertices to the first merged vertex
	for (int i = 1; i < _mergedVertices.size(); i++)
	{
		for (map<int, int>::iterator itr = directed[_mergedVertices[i]].begin(); itr != directed[_mergedVertices[i]].end(); itr++)
		{
			directed[NewVertex].insert(*itr);
		}
		directed[_mergedVertices[i]].clear();
	}
	// 2b- Calculate the capacity of the macro-vertex
	capacities[NewVertex] = _capacity;
	// 3- modify all edges with the new vertex info
	for (int i = 0; i < capacities.size(); i++)
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
	vector<int> NewIndices;
	for (int i = 0; i < capacities.size(); i++)
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
		directed[itr->second] = directed[itr->first];
		capacities[itr->second] = capacities[itr->first];
		directed[itr->first].clear();
	}
	for (int i = 1; i < _mergedVertices.size(); i++)
	{
		directed.pop_back();
		capacities.pop_back();
	}
	// 6- modify the edges according to the new order
	for (int i = 0; i < capacities.size(); i++)
	{
		for (map<int, int>::iterator itr = Order.begin(); itr != Order.end(); itr++)
		{
			pair<multimap<int, int>::iterator, multimap<int, int>::iterator> result = directed[i].equal_range(itr->first);
			if (distance(result.first, result.second) > 0)
			{
				for (multimap<int, int>::iterator itr2 = result.first; itr2 != result.second; itr2++)
					directed[i].insert(pair<int, int>(itr->second, itr2->second));
				directed[i].erase(itr->first);
			}
		}
	}
	// 7- modify layers, processed and reversedEdges
	layers.clear();
	processed.clear();
	reversedEdges.clear();
	parent.clear();
	for (int i = 0; i < capacities.size(); i++)
	{
		layers.push_back(-1);
		processed.push_back(false);
		reversedEdges.push_back(multimap<int, int>());
		parent.push_back(-1);
	}
	for (int i = 0; i < capacities.size(); i++)
	{
		for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			reversedEdges[itr->first].insert(pair<int, int>(i, itr->second));
		}
	}
}

bool TreeLiveness::IsLive()
{
	while (true)
	{
		if (capacities.size() == 1)
			return true;
		if (reversedEdges[nh].size() > 0)//collapse all paths leading to nh 
		{
			//perform a merger
			vector<int> Nodes = GetAllCoReachableNodes(nh);
			Nodes.push_back(nh);
			Merge(Nodes, INFINITY);
		}
		else
		{
			ComputeLayers();
			for (int i = 0; i < capacities.size(); i++)
				processed[i] = false;
			bool BREAK = false;
			while (ExistUnprocessed() && !BREAK)
			{
				int n = PickANodeToProcess();
				if (layers[n] % 2 == 0) //even layer
				{
					vector<int> Path;
					int capacity = 0;
					if (ExistAProducerMergerEmanatingFromNode(n, Path, capacity))
					{
						//perform a merger
						Merge(Path, capacity);
						BREAK = true;
					}
					else
					{
						processed[n] = true;
					}
				}
				else // odd layer
				{
					int np = parent[n];
					if (directed[np].find(n)->second <= capacities[n]) // feasible merger
					{
						//perform a merger
						vector<int> Path;
						Path.push_back(np);
						Path.push_back(n);
						int capacity = capacities[n] + capacities[np] - directed[np].find(n)->second;
						Merge(Path, capacity);
						BREAK = true;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	return false;
}