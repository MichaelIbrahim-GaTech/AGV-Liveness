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
public:
	bool Exist(vector<map<int, int>>& _directed, vector<int>& _capacities, vector<int>& _path, int& _pathCapacity);
};

