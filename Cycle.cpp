#include "Cycle.h"


Cycle::Cycle(vector<int> _capacities, vector<map<int, int>> _directed)
{
	n = _directed.size();
	capacities = _capacities;
	directed = _directed;
	for (int i = 0; i < n; i++)
		reversedEdges.push_back(map<int, int>());
	for (int i = 0; i < n; i++)
	{
		for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			reversedEdges[itr->first].insert(pair<int, int>(i, itr->second));
		}
	}
}

void Cycle::Merge(int n1, int n2, int _capacity)
{
	int NewVertex = n1;
	int MergedVertices = n2;
	if (n1 > n2)
	{
		NewVertex = n2;
		MergedVertices = n1;
	}
	// 1- remove edges between merged vertices
	directed[n1].erase(n2);
	directed[n2].erase(n1);
	reversedEdges[n1].erase(n2);
	reversedEdges[n2].erase(n1);
	//2- Move all edges from merged vertices to the first merged vertex
	for (map<int, int>::iterator itr = directed[MergedVertices].begin(); itr != directed[MergedVertices].end(); itr++)
	{
		if (directed[NewVertex].find(itr->first) == directed[NewVertex].end())
			directed[NewVertex].insert(*itr);
		else
		{
			if(directed[NewVertex][itr->first] > itr->second)
				directed[NewVertex][itr->first] = itr->second;
		}
	}
	directed[MergedVertices].clear();
	// 2b- Calculate the capacity of the macro-vertex
	capacities[NewVertex] = _capacity;
	// 3- modify all edges with the new vertex info
	for (int i = 0; i < n; i++)
	{
		if (directed[i].find(MergedVertices) != directed[i].end())
		{
			if (directed[i].find(NewVertex) == directed[i].end())
				directed[i].insert(pair<int, int>(NewVertex, directed[i][MergedVertices]));
			else
			{
				if (directed[i][NewVertex] > directed[i][MergedVertices])
					directed[i][NewVertex] = directed[i][MergedVertices];
			}
			directed[i].erase(MergedVertices);
		}
	}
	// 4- 
	for (int i = MergedVertices + 1; i < n; i++)
	{
		directed[i - 1] = directed[i];
		capacities[i - 1] = capacities[i];
	}
	directed.pop_back();
	capacities.pop_back();
	n--;
	// 5- modify the edges according to the new order
	for (int itr = MergedVertices + 1; itr <= n; itr++)
	{
		for (int i = 0; i < n; i++)
		{
			if (directed[i].find(itr) != directed[i].end())
			{
				directed[i].insert(pair<int, int>(itr - 1, directed[i][itr]));
				directed[i].erase(itr);
			}
		}
	}
	// 6- update the remaing of the parameters
	reversedEdges.clear();
	for (int i = 0; i < n; i++)
		reversedEdges.push_back(map<int, int>());
	for (int i = 0; i < n; i++)
	{
		for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			reversedEdges[itr->first].insert(pair<int, int>(i, itr->second));
		}
	}
}

bool Cycle::PerformFeasibleMergers(bool direction)
{
	bool AnyFeasible = false;
	for (int i = 0; i < n; i++)
	{
		int next = 0;
		if (direction)
			next = (i + 1) % n;
		else
			next = (n + i - 1) % n;
		if (directed[i].find(next) != directed[i].end())
		{
			if (directed[i][next] <= capacities[next])
			{
				AnyFeasible = true;
				break;
			}
		}
	}
	if (!AnyFeasible)
		return false;


	bool change = true;
	while (change)
	{
		change = false;
		for (int i = 0; i < n; i++)
		{
			int next = 0;
			if (direction)
				next = (i + 1) % n;
			else
				next = (n + i - 1) % n;
			if (directed[i].find(next) != directed[i].end())
			{
				if (directed[i][next] <= capacities[next])
				{
					int capacity = capacities[i] + capacities[next] - directed[i][next];
					if ((capacities[i] == INFINITY) || (capacities[next] == INFINITY))
						capacity = INFINITY;
					Merge(i, next, capacity);
					change = true;
					break;
				}
			}
		}
	}

	return true;
}

bool Cycle::PerformProducerMergers(bool direction)
{
	bool AnyProducer = false;
	for (int i = 0; i < n; i++)
	{
		int next = 0;
		if (direction)
			next = (i + 1) % n;
		else
			next = (n + i - 1) % n;
		if (directed[i].find(next) != directed[i].end())
		{
			if (directed[i][next] <= capacities[next])
			{
				if ((capacities[i] >= directed[i][next]) || (capacities[i] == INFINITY))
				{
					AnyProducer = true;
					break;
				}
			}
		}
	}
	if (!AnyProducer)
		return false;


	bool change = true;
	while (change)
	{
		change = false;
		for (int i = 0; i < n; i++)
		{
			int next = 0;
			if (direction)
				next = (i + 1) % n;
			else
				next = (n + i - 1) % n;
			if (directed[i].find(next) != directed[i].end())
			{
				if (directed[i][next] <= capacities[next])
				{
					if (capacities[i] >= directed[i][next])
					{
						int capacity = capacities[i] + capacities[next] - directed[i][next];
						Merge(i, next, capacity);
						change = true;
						break;
					}
					else if (capacities[i] == INFINITY)
					{
						int capacity = INFINITY;
						Merge(i, next, capacity);
						change = true;
						break;
					}
				}
			}
		}
	}

	return true;
}

bool Cycle::AllEdgesInOneDirection()
{
	for (int i = 0; i < n; i++)
		if (directed[i].size() != 1)
			return false;
	return true;
}

bool Cycle::IsCollapsable(bool direction)
{
	if (n == 1)
		return true;
	bool change = true;
	while (change)
	{
		change = false;
		change = change || PerformFeasibleMergers(direction);
		change = change || PerformProducerMergers(!direction);
		if (n == 1)
			return true;
		if (AllEdgesInOneDirection())
			return true;
	}
	return false;
}

bool Cycle::IsProducer()
{
	int temp_n = n;
	vector<int> temp_capacities = capacities;
	vector<map<int, int>> temp_directed = directed;
	vector<map<int, int>> temp_reversedEdges = reversedEdges;
	
	if (IsCollapsable(true))
		return true;
	n = temp_n;
	capacities = temp_capacities;
	directed = temp_directed;
	reversedEdges = temp_reversedEdges;

	return IsCollapsable(false);
}