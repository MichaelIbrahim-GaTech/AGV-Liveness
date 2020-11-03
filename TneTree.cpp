#include "TneTree.h"

TneTree::TneTree()
{
	_size = 0;
}

TneTree::TneTree(vector<int> InTreeList, vector<int> InTreeParentList, vector<int> InTreeCapacitiesList, vector<int>InTreeEdgeCapacitiesList, set<int> SourceNodes, vector<pair<int, pair<int, int>>> SourceNodesEdges)
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
	_SourceNodes = SourceNodes;
	_SourceNodesEdges = SourceNodesEdges;

	for (int i = 0; i < InTreeParentList.size(); i++)
	{
		if (InTreeParentList[i] == 0)
			_RootChildren.push_back(i);
	}
}

TneTree::TneTree(const TneTree& it)
{
	_size = it._size;
	_InTreeCapacitiesList = it._InTreeCapacitiesList;
	_InTreeEdgeCapacitiesList = it._InTreeEdgeCapacitiesList;
	_InTreeList = it._InTreeList;
	_InTreeParentList = it._InTreeParentList;
	_Processed = it._Processed;
	_RootChildren = it._RootChildren;
	_SourceNodes = it._SourceNodes;
	_SourceNodesEdges = it._SourceNodesEdges;
}

void TneTree::MergeWithRoot(int node)
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

void TneTree::MarkAllEdgesAsUnprocessed()
{
	for (int i = 0; i < _Processed.size(); i++)
		_Processed[i] = false;
}

int TneTree::GetNextUnprocessedEdge()
{
	for (int i = 0; i < _RootChildren.size(); i++)
	{
		if (!_Processed[_RootChildren[i]])
			return _RootChildren[i];
	}
	return -1;
}

bool TneTree::ASourceNodeCouldBeMergedWithRoot(vector<int>& tree)
{
	for (int i = 0; i < _SourceNodesEdges.size(); i++)
	{
		for (int j = 0; j < _InTreeList[0].size(); j++)
		{
			if (_SourceNodesEdges[i].second.first == _InTreeList[0][j])
			{
				if (_InTreeCapacitiesList[0] >= _SourceNodesEdges[i].second.second)//feasible merger
				{
					tree = _InTreeList[0];
					tree.push_back(_SourceNodesEdges[i].first);
					return true;
				}
			}
		}
	}
	return false;
}