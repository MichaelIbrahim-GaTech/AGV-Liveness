#pragma once
#include<iostream>
#include <vector>
using namespace std;
class InTree
{
public:
	int _size;
	vector<vector<int>> _InTreeList;
	vector<int> _InTreeParentList;
	vector<int> _InTreeEdgeCapacitiesList;
	vector<int> _InTreeCapacitiesList;
	vector<bool> _Processed;
	vector<int> _RootChildren;

	InTree();
	InTree(vector<int>, vector<int>, vector<int>, vector<int>);
	InTree(const InTree&);

	void MergeWithRoot(int node);
	void MarkAllEdgesAsUnprocessed();
	int GetNextUnprocessedEdge();

};

