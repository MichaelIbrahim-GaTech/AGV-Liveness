#pragma once
#include<iostream>
#include <vector>
#include<set>
using namespace std;

class TneTree
{
public:
	int _size;
	vector<vector<int>> _InTreeList;
	vector<int> _InTreeParentList;
	vector<int> _InTreeEdgeCapacitiesList;
	vector<int> _InTreeCapacitiesList;
	vector<bool> _Processed;
	vector<int> _RootChildren;
	set<int> _SourceNodes;
	vector<pair<int, pair<int, int>>> _SourceNodesEdges;

	TneTree();
	TneTree(vector<int>, vector<int>, vector<int>, vector<int>, set<int>, vector<pair<int, pair<int, int>>>);
	TneTree(const TneTree&);

	void MergeWithRoot(int node);
	void MarkAllEdgesAsUnprocessed();
	int GetNextUnprocessedEdge();

	bool ASourceNodeCouldBeMergedWithRoot(vector<int>& tree);
};

