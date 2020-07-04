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

void ProducerPathBasedMerger::AddKLToEdge(vector<vector<KLObject>>& EdgeKL, vector<int>& _capacities, vector<int>& V, int prevEdge, int currentEdge, int u2, int u3, int currentEdgeWeight)
{
	// 1- Generate and add all possible (that could be added) kl to to EdgeKL
	int CUhat = _capacities[_capacities.size() - 1];
	int Vu2u3 = V[u3] + _capacities[u2] - currentEdgeWeight;
	for (int i = 0; i < EdgeKL[prevEdge].size(); i++)
	{
		int Kp = EdgeKL[prevEdge][i].K - (V[u2] - Vu2u3);
		int Lp = EdgeKL[prevEdge][i].K - _capacities[u2];
		if (EdgeKL[prevEdge][i].L < Lp)
			Lp = EdgeKL[prevEdge][i].L;
		Lp = Lp - (V[u2] - Vu2u3);
		if ((Kp >= CUhat) && (Lp >= 0))
		{
			KLObject kl;
			kl.K = Kp;
			kl.L = Lp;
			kl.Considered = true;
			kl.AssociatedEdge = currentEdge;
			kl.prev = &EdgeKL[prevEdge][i];
			for (int j = 0; j < EdgeKL[currentEdge].size(); j++)
			{
				if (kl < EdgeKL[currentEdge][j])
				{
					kl.Considered = false;
				}
				else if (kl > EdgeKL[currentEdge][j])
				{
					EdgeKL[currentEdge][j].Considered = false;
				}
			}
			EdgeKL[currentEdge].push_back(kl);
		}
	}
	// 2- Remove kls that become (not maximal) from EdgeKL
	vector<int> NewIndices;
	int RemovedCount = 0;
	for (int i = 0; i < EdgeKL[currentEdge].size(); i++)
		NewIndices.push_back(i);
	for (int i = 0; i < EdgeKL[currentEdge].size(); i++)
		if (!EdgeKL[currentEdge][i].Considered)
		{
			NewIndices[i] = -1;
			RemovedCount++;
		}
	int i1 = 0, i2 = NewIndices.size() - 1;
	while (NewIndices[i2] == -1)
		i2--;
	while (i1 < i2)
	{
		if (NewIndices[i1] == -1)
		{
			NewIndices[i1] = NewIndices[i2];
			NewIndices[i2] = -1;
			while (NewIndices[i2] == -1)
				i2--;
		}
		i1++;
	}
	map<int, int> Order;
	for (int i = 0; i < NewIndices.size(); i++)
	{
		if (NewIndices[i] != -1)
		{
			if (NewIndices[i] != i)
				Order.insert(pair<int, int>(NewIndices[i], i));
		}
	}
	for (map<int, int>::iterator itr = Order.begin(); itr != Order.end(); itr++)
	{
		EdgeKL[currentEdge][itr->second] = EdgeKL[currentEdge][itr->first];
	}
	for (int i = 1; i < RemovedCount; i++)
	{
		EdgeKL[currentEdge].pop_back();
	}

}

bool ProducerPathBasedMerger::GetPathBasedMerger(vector<map<int, int>>& _directed, vector<int>& _capacities, vector<bool>& Snu, vector<int>& V, vector<int>& _path, int& _pathCapacity)
{
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
			for (map<int, int>::iterator itr1 = EdgesIndices[i].begin(); itr1 != EdgesIndices[i].end(); itr1++)
			{
				for (map<int, int>::iterator itr2 = ReversedEdgesIndices[i].begin(); itr2 != ReversedEdgesIndices[i].end(); itr2++)
				{
					// u1= itr2->first, u2 = i, u3 = itr1->first
					// prevEdge = itr2->second, currentEdge = itr1->second
					// current Edge Weight = _directed[i][itr1->first]
					AddKLToEdge(EdgeKL, _capacities, V, itr2->second, itr1->second, i, itr1->first, _directed[i][itr1->first]);
				}
			}
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
}

bool ProducerPathBasedMerger::Exist(vector<map<int, int>>& _directed, vector<int>& _capacities, vector<int>& _path, int& _pathCapacity)
{
	// phase 1
	vector<bool> Snu;
	vector<int> V(_directed.size(), -1);
	GetSnu(_directed, _capacities, Snu, V);
	// phase 2
	return GetPathBasedMerger(_directed, _capacities, Snu, V, _path, _pathCapacity);
}