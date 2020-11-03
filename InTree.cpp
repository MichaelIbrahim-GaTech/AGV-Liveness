#include "InTree.h"

InTree::InTree()
{
	_size = 0;
}

InTree::InTree(vector<int> InTreeList, vector<int> InTreeParentList, vector<int> InTreeCapacitiesList, vector<int>InTreeEdgeCapacitiesList)
{
	_size = InTreeList.size();
	_InTreeList.clear();
	_Processed.clear();
	_RootChildren.clear();
	for (int i = 0; i < InTreeList.size(); i++)
	{
		vector<int> temp;
		temp.push_back(InTreeList[i]);
		_InTreeList.push_back(temp);
		_Processed.push_back(false);
	}
	_InTreeParentList = InTreeParentList;
	_InTreeCapacitiesList = InTreeCapacitiesList;
	_InTreeEdgeCapacitiesList = InTreeEdgeCapacitiesList;

	for (int i = 0; i < InTreeParentList.size(); i++)
	{
		if (InTreeParentList[i] == 0)
			_RootChildren.push_back(i);
	}
}

InTree::InTree(const InTree& it)
{
	_size = it._size;
	_InTreeCapacitiesList = it._InTreeCapacitiesList;
	_InTreeEdgeCapacitiesList = it._InTreeEdgeCapacitiesList;
	_InTreeList = it._InTreeList;
	_InTreeParentList = it._InTreeParentList;
	_Processed = it._Processed;
	_RootChildren = it._RootChildren;
}

void InTree::MergeWithRoot(int node)
{
	_size--;
	_InTreeCapacitiesList[0] += _InTreeCapacitiesList[node] - _InTreeEdgeCapacitiesList[node];
	_InTreeList[0].push_back(_InTreeList[node][0]);

	for (int i = 0; i < _InTreeList.size(); i++)
	{
		if (_InTreeParentList[i] == node)
			_InTreeParentList[i] = 0;
	}
	_RootChildren.clear();
	for (int i = 0; i < _InTreeParentList.size(); i++)
	{
		if (_InTreeParentList[i] == 0)
			_RootChildren.push_back(i);
	}

}

void InTree::MarkAllEdgesAsUnprocessed()
{
	for (int i = 0; i < _Processed.size(); i++)
		_Processed[i] = false;
}

int InTree::GetNextUnprocessedEdge()
{
	for (int i = 0; i < _RootChildren.size(); i++)
	{
		if (!_Processed[_RootChildren[i]])
			return _RootChildren[i];
	}
	return -1;
}