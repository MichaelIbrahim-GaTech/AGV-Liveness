#pragma once
#include <vector>
#include <map>
#include <set>
#include <stack>
#include "KLObject.h"
using namespace std;

class ProducerPathBasedMerger
{
	// This function implemnts phase 1 of rev-prod-path-based-merer-check.pdf
	// It returns a bool vector Snu where Snu[u] == true means that u is in the subgraph
	// Also, it returns a vector of integers "V"
	// Finally, this algorithm modifies _directed vector.
	void GetSnu(vector<map<int, int>>& _directed, vector<int>& _capacities, vector<bool>& Snu, vector<int>& V);
	bool GetPathBasedMerger(vector<map<int, int>>& _directed, vector<int>& _capacities, vector<bool>& Snu, vector<int>& V, vector<int>& _path, int& _pathCapacity);
	void AddKLToEdge(vector<vector<KLObject>>& EdgeKL, vector<int>& capacities, vector<int>& V, int prevEdge, int currentEdge, int u2, int u3, int currentEdgeWeight);
public:
	bool Exist(vector<map<int, int>>& _directed, vector<int>& _capacities, vector<int>& _path, int& _pathCapacity);
};

