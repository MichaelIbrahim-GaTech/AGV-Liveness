#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include "Graph.h"
using namespace std;

class MultiGraph
{
	int n;
	vector<map<int, int>> directed;
	vector<map<int, int>> undirected;
	vector<int> GetDegree(const vector<int>& vertices, const vector<bool>& considered);
	vector<vector<int>> GetStronglyConnectedComponents();
	vector<vector<int>> RemoveAllOneDegreeVertices(const vector<vector<int>>& _ds);
	vector<int> CalculateCapacities(const vector<vector<int>>& _ds);
public:
	MultiGraph();
	MultiGraph(const MultiGraph& _g);
	MultiGraph(vector<map<int, int>> _directed);
	bool AddVehicules(vector<map<int, int>> _undirected, bool clear = true);
	void Condense(vector<vector<int>>& _vertices, vector<int>& capacities, vector<map<int, int>>& _directed, vector<map<int, int>>& _undirected);
};

