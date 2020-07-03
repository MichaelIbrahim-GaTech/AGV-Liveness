#include "ProducerPathBasedMerger.h"

void ProducerPathBasedMerger::GetSnu(vector<map<int, int>>& _directed, vector<int>& _capacities, vector<bool>& _Snu, vector<int>& V)
{
	V[_directed.size() - 1] = _capacities[_directed.size() - 1];
	for (int i = _directed.size() - 2; i >= 0; i--)
	{
		for (map<int, int>::iterator itr = _directed[i].begin(); itr != _directed[i].end(); itr++)
		{
			int NewV = _capacities[i] + V[itr->first] - itr->second;
			if (NewV > V[i])
				V[i] = NewV;
		}
	}
	set<int> ConsideredNodes;
	stack<int> Explore;
	Explore.push(0);
	while (!Explore.empty())
	{
		int current = Explore.top();
		Explore.pop();
		ConsideredNodes.insert(current);
		for (map<int, int>::iterator itr = _directed[current].begin(); itr != _directed[current].end(); itr++)
		{
			if (ConsideredNodes.find(itr->first) == ConsideredNodes.end())
			{
				if (V[itr->first] >= 0)
				{
					Explore.push(itr->first);
				}
			}
		}
	}

	set<int> NotConsideredNodes;
	for (int i = 0; i < V.size(); i++)
	{
		if (ConsideredNodes.find(i) == ConsideredNodes.end())
		{
			_Snu.push_back(false);
			NotConsideredNodes.insert(i);
		}
		else
			_Snu.push_back(true);
	}
	for (int i = 0; i < V.size(); i++)
	{
		if (_Snu[i])
		{
			for (set<int>::iterator itr = NotConsideredNodes.begin(); itr != NotConsideredNodes.end(); itr++)
				_directed[i].erase(*itr);
		}
		else
		{
			_directed[i].clear();
		}
	}
}

bool ProducerPathBasedMerger::Exist(vector<map<int, int>>& _directed, vector<int>& _capacities, vector<int>& _path, int& _pathCapacity)
{
	// phase 1
	vector<bool> Snu;
	vector<int> V(_directed.size(), -1);
	GetSnu(_directed, _capacities, Snu, V);
	// phase 2
	int CUhat = _capacities[_capacities.size() - 1];// capacity of u hat (the final node)
	int EdgeCount = 0;
	int FinalEdge = -1;// The edge leading to \hat{u}
	vector<map<int, int>> EdgesIndices;
	vector<map<int, int>> ReversedEdgesIndices;
	for (int i = 0; i < _directed.size(); i++)
	{
		EdgesIndices.push_back(map<int, int>());
		ReversedEdgesIndices.push_back(map<int, int>());
		for (map<int, int>::iterator itr = _directed[i].begin(); itr != _directed[i].end(); itr++)
		{
			EdgesIndices[i].insert(pair<int, int>(itr->first, EdgeCount));
			if (itr->first == _directed.size() - 1)
				FinalEdge = EdgeCount;
			EdgeCount++;
		}
	}
	for (int i = 0; i < _directed.size(); i++)
	{
		for (map<int, int>::iterator itr = EdgesIndices[i].begin(); itr != EdgesIndices[i].end(); itr++)
		{
			ReversedEdgesIndices[itr->first].insert(pair<int, int>(i, itr->second));
		}
	}
	vector<vector<KLObject>> EdgeKL(EdgeCount, vector<KLObject>());
	// 1- Add KL corresponding to edges (n,u)
	for (map<int, int>::iterator itr = _directed[0].begin(); itr != _directed[0].end(); itr++)
	{
		int Vnu = _capacities[0] + V[itr->first] - itr->second;
		if (Vnu >= CUhat)
		{
			KLObject kl;
			kl.K = Vnu;
			kl.L = Vnu - _capacities[0];
			kl.Considered = true;
			kl.AssociatedEdge = EdgesIndices[0][itr->first];
			kl.prev = NULL;
			EdgeKL[EdgesIndices[0][itr->first]].push_back(kl);
		}
	}
	// 2- Iterate over all the nodes
	for (int i = 1; i < _directed.size() - 1; i++)
	{
		if (Snu[i])
		{
			// need to implement
		}
	}


	if (EdgeKL[FinalEdge].empty())
	{
		return false;
	}
	else
	{
		int current = _directed.size() - 1;
		_pathCapacity = _capacities[current];
		_path.clear();
		_path.push_back(current);
		KLObject* currentKL = &EdgeKL[FinalEdge][0];
		while (current > 0)
		{
			int edge = currentKL->AssociatedEdge;
			for (map<int, int>::iterator itr = ReversedEdgesIndices[current].begin(); itr != ReversedEdgesIndices[current].end(); itr++)
			{
				if (ReversedEdgesIndices[current][itr->second] == edge)
				{
					_pathCapacity += _capacities[itr->second] - _directed[itr->second][current];
					_path.push_back(itr->second);
					current = itr->second;
					currentKL = currentKL->prev;
					break;
				}
			}
		}
		reverse(_path.begin(), _path.end());
		return true;
	}

	return false;
}