#pragma once
#include <vector>
#include <map>
#include <stack>
#include <set>
#include "DirectedAcyclicMultiGraph.h"
using namespace std;
class TreeLiveness
{
	int nh;
	vector<int> capacities;
	vector<int> layers;
	vector<int> parent;
	vector<set<int>> children;
	vector<bool> processed;
	vector<multimap<int, int>> directed;
	vector<multimap<int, int>> reversedEdges;
	bool ExistUnprocessed();
	int PickANodeToProcess();
	set<int> SetAllReachableNodes(set<int> Nodes, int currentLayer);
	set<int> SetAllCoReachableNodes(set<int> Nodes, int currentLayer);
	void ComputeLayers();
	vector<int> GetAllCoReachableNodes(int node);

	vector<vector<int>> GetAllPathsEmanatingFromNode(int n);
	bool ExistAProducerMergerEmanatingFromNode(int n, vector<int>& Path, int& _capacity);
	//int Parent(int _n);
	void Merge(vector<int> _mergedVertices, int _capacity);
public:
	TreeLiveness();
	TreeLiveness(const TreeLiveness&);
	TreeLiveness(const DirectedAcyclicMultiGraph& _g);
	TreeLiveness& operator=(const DirectedAcyclicMultiGraph& _g);
	bool IsLive();
};

