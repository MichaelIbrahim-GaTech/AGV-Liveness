#include "DirectedAcyclicMultiGraph.h"
#include <queue>

DirectedAcyclicMultiGraph::DirectedAcyclicMultiGraph(const DirectedAcyclicMultiGraph& _g)
{
	C = _g.C;
	nh = _g.nh;
	for (int i = 0; i < _g.nodes.size(); i++)
	{
		nodes.push_back(_g.nodes[i]);
		capacities.push_back(_g.capacities[i]);
		directed.push_back(_g.directed[i]);
		major.push_back(_g.major[i]);
		reversedEdges.push_back(_g.reversedEdges[i]);
		collapsedPaths.push_back(_g.collapsedPaths[i]);
		nodeInDegree.push_back(_g.nodeInDegree[i]);
	}
}

DirectedAcyclicMultiGraph::DirectedAcyclicMultiGraph(CondensedMultiGraph* _C)
{
	C = _C;
	C->CondenseDirected(nodes, capacities, directed, &nh);
	for (int i = 0; i < nodes.size(); i++)
	{
		major.push_back(false);
		reversedEdges.push_back(multimap<int, int>());
		collapsedPaths.push_back(map<int, vector<int>>());
	}
	for (int i = 0; i < directed.size(); i++)
	{
		for (multimap<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			reversedEdges[itr->first].insert(pair<int, int>(i, itr->second));
		}
	}
	major[nh] = true;
	capacities[nh] = INFINITY;
	for (int i = 0; i < nodes.size(); i++)
	{
		if (capacities[i] > 0)
		{
			major[i] = true;
		}
		else if (nodes[i].size() > 1)
		{
			major[i] = true;
		}
		else if (C->isComplexComponent(nodes[i][0]))
		{
			major[i] = true;
		}
		else
		{
			set<int> OutNodes;
			for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
			{
				OutNodes.insert(itr->first);
			}
			int OutDegree = OutNodes.size();
			if (OutDegree >= 2)
			{
				major[i] = true;
			}
			else
			{
				set<int> InNodes;
				for (map<int, int>::iterator itr = reversedEdges[i].begin(); itr != reversedEdges[i].end(); itr++)
				{
					InNodes.insert(itr->first);
				}
				int InDegree = InNodes.size();
				if (InDegree >= 2)
				{
					major[i] = true;
				}
			}
		}
	}
	for (int i = 0; i < nodes.size(); i++)
	{
		set<int> InNodes;
		for (map<int, int>::iterator itr = reversedEdges[i].begin(); itr != reversedEdges[i].end(); itr++)
		{
			InNodes.insert(itr->first);
		}
		nodeInDegree.push_back(InNodes.size());
	}
	CollapseNonMajorNodesPaths();
	//major.clear();
}

void DirectedAcyclicMultiGraph::CollapseNonMajorNodesPaths()
{ 
	set<int> IgnoredNodes;
	for (int i = 0; i < nodes.size(); i++)
	{
		if (!major[i])
		{
			if ((reversedEdges[i].size() == 1) && (directed[i].size() == 1))
			{
				// put the necessary code to collapse this node
				IgnoredNodes.insert(i);
				int weight = directed[i].begin()->second + reversedEdges[i].begin()->second;
				int from = reversedEdges[i].begin()->first;
				int to = directed[i].begin()->first;
				reversedEdges[to].erase(i); reversedEdges[i].erase(from);
				directed[from].erase(i); directed[i].erase(to);
				directed[from].insert(pair<int, int>(to, weight));
				reversedEdges[to].insert(pair<int, int>(from, weight));
				// get the vertices that will result from this collapse
				vector<int> CollapsedVertices(nodes[i]);
				if (collapsedPaths[from].find(i) != collapsedPaths[from].end())
				{
					vector<int> ToAdd = collapsedPaths[from][i];
					for (int j = 0; j < ToAdd.size(); j++)
						CollapsedVertices.push_back(ToAdd[j]);
					collapsedPaths[from].erase(i);
				}
				if (collapsedPaths[i].find(to) != collapsedPaths[i].end())
				{
					vector<int> ToAdd = collapsedPaths[i][to];
					for (int j = 0; j < ToAdd.size(); j++)
						CollapsedVertices.push_back(ToAdd[j]);
					collapsedPaths[i].erase(to);
				}
				if (collapsedPaths[from].find(to) == collapsedPaths[from].end())
					collapsedPaths[from].insert(pair<int, vector<int>>(to, CollapsedVertices));
				else
				{
					for (int j = 0; j < CollapsedVertices.size(); j++)
						collapsedPaths[from][to].push_back(CollapsedVertices[j]);
				}
			}
		}
	}

	if (IgnoredNodes.size() > 0)
	{
		vector<int> NewIndices;
		for (int i = 0; i < nodes.size(); i++)
			NewIndices.push_back(i);
		for (set<int>::iterator itr = IgnoredNodes.begin(); itr != IgnoredNodes.end(); itr++)
			NewIndices[*itr] = -1;
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
		// check which nodes will have a different order
		map<int, int> Order;
		for (int i = 0; i < NewIndices.size(); i++)
		{
			if (NewIndices[i] != -1)
			{
				if (NewIndices[i] != i)
					Order.insert(pair<int, int>(NewIndices[i], i));
			}
		}
		if (Order.find(nh) != Order.end())
		{
			nh = Order[nh];
		}
		for (map<int, int>::iterator itr = Order.begin(); itr != Order.end(); itr++)
		{
			nodes[itr->second] = nodes[itr->first];
			directed[itr->second] = directed[itr->first];
			reversedEdges[itr->second] = reversedEdges[itr->first];
			collapsedPaths[itr->second] = collapsedPaths[itr->first];
			capacities[itr->second] = capacities[itr->first];
			nodeInDegree[itr->second] = nodeInDegree[itr->first];
			nodes[itr->first].clear();
			directed[itr->first].clear();
			reversedEdges[itr->first].clear();
			collapsedPaths[itr->first].clear();
		}
		for (int i = 0; i < IgnoredNodes.size(); i++)
		{
			nodes.pop_back();
			directed.pop_back();
			reversedEdges.pop_back();
			collapsedPaths.pop_back();
			capacities.pop_back();
			nodeInDegree.pop_back();
		}
		// modify the edges according to the new order
		for (int i = 0; i < nodes.size(); i++)
		{
			for (map<int, int>::iterator itr = Order.begin(); itr != Order.end(); itr++)
			{
				pair<multimap<int, int>::iterator, multimap<int, int>::iterator> result = directed[i].equal_range(itr->first);
				if (distance(result.first, result.second) > 0)
				{
					for (multimap<int, int>::iterator itr2 = result.first; itr2 != result.second; itr2++)
						directed[i].insert(pair<int, int>(itr->second, itr2->second));
					directed[i].erase(itr->first);
				}
				result = reversedEdges[i].equal_range(itr->first);
				if (distance(result.first, result.second) > 0)
				{
					for (multimap<int, int>::iterator itr2 = result.first; itr2 != result.second; itr2++)
						reversedEdges[i].insert(pair<int, int>(itr->second, itr2->second));
					reversedEdges[i].erase(itr->first);
				}
				map<int, vector<int>>::iterator itr2 = collapsedPaths[i].find(itr->first);
				if (itr2 != collapsedPaths[i].end())
				{
					collapsedPaths[i].insert(pair<int, vector<int>>(itr->second, itr2->second));
					collapsedPaths[i].erase(itr->first);
				}
			}
		}
	}
}

bool DirectedAcyclicMultiGraph::TerminalNodesCapacityLessThanAllInEdges()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		if (directed[i].size() == 0)//this is a terminal node
		{
			bool AllLess = true;
			for (multimap<int, int>::iterator itr = reversedEdges[i].begin(); itr != reversedEdges[i].end(); itr++)
			{
				if (itr->second <= capacities[i])
				{
					AllLess = false;
					break;
				}
			}
			if (AllLess)
				return true;
		}
	}
	return false;
}

vector<int> DirectedAcyclicMultiGraph::TopologicalOrder()
{
	int n = nodes.size();
	vector<set<int>> D;
	for (int i = 0; i < n; i++)
	{
		D.push_back(set<int>());
	}
	for (int i = 0; i < n; i++)
	{
		for (multimap<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			D[i].insert(itr->first);
		}
	}
	return Graph::topologicalSort(D);
}


vector<bool> DirectedAcyclicMultiGraph::GetSn(vector<int> order, int n)
{
	vector<bool> Sn(order.size(), false);
	map<int, int> Index;
	for (int i = 0; i < order.size(); i++)
		Index.insert(pair<int, int>(order[i], i));
	stack<int> Explore;
	set<int> Visited;
	Explore.push(order[n]);
	while (!Explore.empty())
	{
		int current = Explore.top();
		Explore.pop();
		if (Visited.find(current) == Visited.end())
		{
			Visited.insert(current);
			Sn[Index[current]] = true;
			for (map<int, int>::iterator itr = directed[current].begin(); itr != directed[current].end(); itr++)
				Explore.push(itr->first);
		}
	}


	return Sn;
}

void DirectedAcyclicMultiGraph::GetProducerCycleBasedMerger(vector<int> _order, vector<bool> _Sn, int _n, int _u, vector<int> _path, set<int>& mergedNodes)
{
	int count = 0;
	map<int, int> NewIndices;
	vector<int> ReversedIndices;
	vector<set<int>> SubgraphDirected;
	vector<int> SubgraphCapacities;
	for (int i = _n; i <= _u; i++)
	{
		if (_Sn[i])
		{
			NewIndices.insert(pair<int, int>(_order[i], count));
			ReversedIndices.push_back(_order[i]);
			count++;
			SubgraphDirected.push_back(set<int>());
			SubgraphCapacities.push_back(capacities[_order[i]]);
		}
	}
	count = 0;
	for (int i = _n; i <= _u; i++)
	{
		if (_Sn[i])
		{
			for (map<int, int>::iterator itr = directed[_order[i]].begin(); itr != directed[_order[i]].end(); itr++)
			{
				if (NewIndices.find(itr->first) != NewIndices.end())//Sn[itr->first] = true
				{
					SubgraphDirected[count].insert(NewIndices[itr->first]);
				}
			}
			count++;
		}
	}
	for (int i = 0; i < _path.size() - 1; i++)
	{
		SubgraphDirected[NewIndices[_order[_path[i]]]].erase(NewIndices[_order[_path[i + 1]]]);
		SubgraphDirected[NewIndices[_order[_path[i + 1]]]].insert(NewIndices[_order[_path[i]]]);
	}

	vector<vector<int>> cycles = Graph::GetStronglyConnectedComponents(SubgraphDirected);
	for (int i = 0; i < cycles.size(); i++)
	{
		if (find(cycles[i].begin(), cycles[i].end(), NewIndices[_u]) != cycles[i].end())
		{
			mergedNodes.clear();
			for (int j = 0; j < _path.size(); j++)
				mergedNodes.insert(_order[_path[j]]);
			for (int j = 0; j < cycles[i].size(); j++)
			{
				mergedNodes.insert(ReversedIndices[cycles[i][j]]);
			}
			return;
		}
	}

}

bool DirectedAcyclicMultiGraph::ExistProducerPathBasedMerger(vector<int> _order, vector<bool> _Sn, int _n, int _u, vector<int>& _path)
{
	int count = 0;
	map<int, int> NewIndices;
	vector<int> ReversedIndices;
	vector<map<int, int>> SubgraphDirected;
	vector<int> SubgraphCapacities;
	for (int i = _n; i <= _u; i++)
	{
		if (_Sn[i])
		{
			NewIndices.insert(pair<int, int>(_order[i], count));
			ReversedIndices.push_back(_order[i]);
			count++;
			SubgraphDirected.push_back(map<int, int>());
			SubgraphCapacities.push_back(capacities[_order[i]]);
		}
	}
	count = 0;
	for (int i = _n; i <= _u; i++)
	{
		if (_Sn[i])
		{
			for (map<int, int>::iterator itr = directed[_order[i]].begin(); itr != directed[_order[i]].end(); itr++)
			{
				if (NewIndices.find(itr->first) != NewIndices.end())//Sn[itr->first] = true
				{
					if (SubgraphDirected[count].find(NewIndices[itr->first]) == SubgraphDirected[count].end())
						SubgraphDirected[count].insert(pair<int, int>(NewIndices[itr->first], itr->second));
					else
					{
						int weight = SubgraphDirected[count][NewIndices[itr->first]];
						if (weight < itr->second)
							weight = itr->second;
						SubgraphDirected[count][NewIndices[itr->first]] = weight;
					}
				}
			}
			count++;
		}
	}

	vector<int> Path;
	ProducerPathBasedMerger PPBM;
	if (PPBM.Exist(SubgraphDirected, SubgraphCapacities, Path))
	{
		for (int i = 0; i < Path.size(); i++)
			_path.push_back(ReversedIndices[Path[i]]);
		return true;
	}
	else
	{
		return false;
	}

}

bool DirectedAcyclicMultiGraph::ExistAPathLeadingToNH(CondensedMultiGraph* _C)
{
	if (reversedEdges[nh].size() > 0)
	{
		*_C = *C;
		stack<int> Explore;
		Explore.push(nh);
		set<int> MergedNodes;
		while (!Explore.empty())
		{
			int Current = Explore.top();
			Explore.pop();
			if (MergedNodes.find(Current) == MergedNodes.end())
			{
				MergedNodes.insert(Current);
				for (multimap<int, int>::iterator itr = reversedEdges[Current].begin(); itr != reversedEdges[Current].end(); itr++)
				{
					if (MergedNodes.find(itr->first) == MergedNodes.end())
						Explore.push(itr->first);
				}
			}
		}
		// performe a merger here
		vector<int> MergedVertices;
		for (set<int>::iterator itr = MergedNodes.begin(); itr != MergedNodes.end(); itr++)
		{
			for (int i = 0; i < nodes[*itr].size(); i++)
			{
				MergedVertices.push_back(nodes[*itr][i]);
			}
		}
		// Add the merged vertices from collapsed paths
		for (set<int>::iterator i = MergedNodes.begin(); i != MergedNodes.end(); i++)
		{
			for (set<int>::iterator j = MergedNodes.begin(); j != MergedNodes.end(); j++)
			{
				if (collapsedPaths[*i].find(*j) != collapsedPaths[*i].end())
				{
					vector<int> ToAdd = collapsedPaths[*i][*j];
					for (int k = 0; k < ToAdd.size(); k++)
						MergedVertices.push_back(ToAdd[k]);
				}
			}
		}
		_C->MacroMerger(MergedVertices);
		return true;
	}
	else
	{
		return false;
	}
}


// This function is used by Case I of Algorithm 6, it performs a merger if it is feasible.
bool DirectedAcyclicMultiGraph::ExistAFeasibleMergerEdge(int from, int to, CondensedMultiGraph* _C)
{
	for (multimap<int, int>::iterator itr = directed[from].begin(); itr != directed[from].end(); itr++)
	{
		if (itr->first == to)
		{
			if (capacities[itr->first] >= itr->second)//feasible merger
			{
				vector<int> Path;
				Path.push_back(from);
				Path.push_back(to);
				vector<int> MergedVertices;
				// this function check if there are other nodes that could be merged due to a newly generated cycle
				GetMergedVertices(Path, MergedVertices);
				// performe a merger here
				*_C = *C;
				_C->MacroMerger(MergedVertices);
				return true;
			}
		}
	}
	return false;
}

bool DirectedAcyclicMultiGraph::ExistAProducerMergerEdge(CondensedMultiGraph* _C)
{
	for (int i = 0; i < directed.size(); i++)
	{
		for (multimap<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			if (capacities[itr->first] >= itr->second)//feasible merger
			{
				if (i == nh) // feasible edge out of nh is a producer merger
				{
					vector<int> Path;
					Path.push_back(i);
					Path.push_back(itr->first);
					vector<int> MergedVertices;
					// this function check if there are other nodes that could be merged due to a newly generated cycle
					GetMergedVertices(Path, MergedVertices);
					// performe a merger here
					*_C = *C;
					_C->MacroMerger(MergedVertices);
					return true;
				}
				else if (capacities[i] >= itr->second) //path-based producer merger
				{
					vector<int> Path;
					Path.push_back(i);
					Path.push_back(itr->first);
					vector<int> MergedVertices;
					// this function check if there are other nodes that could be merged due to a newly generated cycle
					GetMergedVertices(Path, MergedVertices);
					// performe a merger here
					*_C = *C;
					_C->MacroMerger(MergedVertices);
					return true;
				}
				else if (directed[i].count(itr->first) > 1) //cycle-based producer merger
				{
					vector<int> Path;
					Path.push_back(i);
					Path.push_back(itr->first);
					vector<int> MergedVertices;
					// this function check if there are other nodes that could be merged due to a newly generated cycle
					GetMergedVertices(Path, MergedVertices);
					// performe a merger here
					*_C = *C;
					_C->MacroMerger(MergedVertices);
					return true;
				}
			}
		}
	}
	return false;
}
// This function implements Algorithm 2 of the paper
bool DirectedAcyclicMultiGraph::ExistAProducerMerger(CondensedMultiGraph* _C)
{
	vector<int> order = TopologicalOrder();
	vector<int> ReversedOrder(order.size(), -1);
	for (int i = 0; i < order.size(); i++)
		ReversedOrder[order[i]] = i;
	map<int, int> Index;
	for (int i = 0; i < order.size(); i++)
		Index.insert(pair<int, int>(order[i], i));
	for (int n = 0; n < order.size(); n++)
	{
		vector<bool> Sn = GetSn(order, n);
		vector<int> delta(order.size(), INFINITY);
		vector<set<int>> LU(order.size(), set<int>());
		delta[n] = 0; //delta[n]
		for (int u = n + 1; u < order.size(); u++)
		{
			// if u is in Sn
			if (Sn[u])
			{
				// delta[u] = max{0, min{delta[u']+w(u',u) - capacity[u]}}
				for (map<int, int>::iterator itr = reversedEdges[order[u]].begin(); itr != reversedEdges[order[u]].end(); itr++)
				{
					if (Sn[Index[itr->first]])
					{
						int rhs = delta[Index[itr->first]] + itr->second - capacities[order[u]];
						if (rhs < delta[u])
						{
							delta[u] = rhs;
							LU[u].clear();
							LU[u].insert(ReversedOrder[itr->first]);
						}
						else if (rhs == delta[u])
						{
							LU[u].insert(ReversedOrder[itr->first]);
						}
					}
				}
				if (delta[u] < 0)
					delta[u] = 0;

				if (delta[u] == 0)
				{
					set<int> InNodes;
					for (map<int, int>::iterator itr = reversedEdges[order[u]].begin(); itr != reversedEdges[order[u]].end(); itr++)
					{
						if (Sn[ReversedOrder[itr->first]])
							InNodes.insert(itr->first);
					}
					int InDegree = InNodes.size(); 
					// recheck if there are extra input from the same edge 
					if (InDegree == 1)
					{
						int count = reversedEdges[order[u]].count(*InNodes.begin());
						if (count > 1)
						{
							if (collapsedPaths[*InNodes.begin()].find(order[u]) != collapsedPaths[*InNodes.begin()].end())
								InDegree = count;
						}
					}
					// path based merger
					if (InDegree == 1)
					{
						vector<int> Path;
						if (ExistProducerPathBasedMerger(order, Sn, n, u, Path))
						{
							vector<int> MergedVertices;
							// this function check if there are other nodes that could be merged due to a newly generated cycle
							GetMergedVertices(Path, MergedVertices);
							// performe a merger here
							*_C = *C;
							_C->MacroMerger(MergedVertices);
							return true;
						}
					}
					// cycle based merger
					else if (InDegree > 1)
					{
						// construct a path pi from n to u
						vector<int> Path;
						int current = u;
						while (current > n)
						{
							Path.push_back(current);
							current = *LU[current].begin();//just pick any path
						}
						Path.push_back(n);
						reverse(Path.begin(), Path.end());

						set<int> MergedNodes;
						GetProducerCycleBasedMerger(order, Sn, n, u, Path, MergedNodes);
						// performe a merger here
						vector<int> MergedVertices;
						for (set<int>::iterator itr = MergedNodes.begin(); itr != MergedNodes.end(); itr++)
						{
							for (int i = 0; i < nodes[*itr].size(); i++)
							{
								MergedVertices.push_back(nodes[*itr][i]);
							}
						}
						// Add the merged vertices from collapsed paths
						for (set<int>::iterator i = MergedNodes.begin(); i != MergedNodes.end(); i++)
						{
							for (set<int>::iterator j = MergedNodes.begin(); j != MergedNodes.end(); j++)
							{
								if (collapsedPaths[*i].find(*j) != collapsedPaths[*i].end())
								{
									vector<int> ToAdd = collapsedPaths[*i][*j];
									for (int k = 0; k < ToAdd.size(); k++)
										MergedVertices.push_back(ToAdd[k]);
								}
							}
						}
						*_C = *C;
						_C->MacroMerger(MergedVertices);
						return true;
					}
				}
			}
		}
	}
	return false;
}


vector<CondensedMultiGraph> DirectedAcyclicMultiGraph::PickATerminalNodeAndCollapseFeasiblePaths()
{
	int Terminal = 0;
	priority_queue<pair<int,int>, vector<pair<int, int>>, greater<pair<int, int>> > pq;
	for (; Terminal < nodes.size(); Terminal++)
	{
		if (directed[Terminal].size() == 0)//this is a terminal node
		{
			int count = 0;
			for (multimap<int, int>::iterator itr = reversedEdges[Terminal].begin(); itr != reversedEdges[Terminal].end(); itr++)
			{
				if (itr->second <= capacities[Terminal])
				{
					count++;
				}
			}
			pq.push(make_pair(count, Terminal));
			if (count == 1)
				break;
		}
	}
	Terminal = pq.top().second;
	//if (Terminal == nodes.size())
	//{
	//	Terminal = 0;
	//	for (; Terminal < nodes.size(); Terminal++)
	//	{
	//		if (directed[Terminal].size() == 0)//this is a terminal node
	//			break;
	//	}
	//}
	// No Terminal Nodes (possibly something went wrong) in some place
	if(pq.empty()) 
		return vector<CondensedMultiGraph>();
	vector<CondensedMultiGraph> result;
	for (multimap<int, int>::iterator itr = reversedEdges[Terminal].begin(); itr != reversedEdges[Terminal].end(); itr++)
	{
		if (itr->second <= capacities[Terminal])
		{
			vector<int> Path;
			Path.push_back(itr->first);
			Path.push_back(Terminal);
			vector<int> MergedVertices;
			// this function check if there are other nodes that could be merged due to a newly generated cycle
			GetMergedVertices(Path, MergedVertices);
			// performe a merger here
			CondensedMultiGraph _C = *C;
			_C.MacroMerger(MergedVertices);
			result.push_back(_C);
		}
	}
	return result;
}

// This function gets as input a path to be merged and its capacity
// And it checks if new cycles are generated from this merger
// Then, it returns the nodes to be merged and the capacity of the merged node
void DirectedAcyclicMultiGraph::GetMergedVertices(vector<int> _path, vector<int>& _mergedVertices)
{
	set<int> _mergedNodes;
	int n = nodes.size();
	vector<set<int>> D;
	for (int i = 0; i < n; i++)
	{
		D.push_back(set<int>());
	}
	for (int i = 0; i < n; i++)
	{
		for (map<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			D[i].insert(itr->first);
		}
	}

	for (int i = 0; i < _path.size() - 1; i++)
	{
		D[_path[i + 1]].insert(_path[i]);
	}

	vector<vector<int>> cycles = Graph::GetStronglyConnectedComponents(D);

	for(int c = 0; c < cycles.size(); c++)
	{
		// This is the component that contains the path 
		if (find(cycles[c].begin(), cycles[c].end(), _path[0]) != cycles[c].end())
		{
			for (int i = 0; i < cycles[c].size(); i++)
			{
				_mergedNodes.insert(cycles[c][i]);
			}
			// Add the merged vertices
			for (int i = 0; i < cycles[c].size(); i++)
			{
				for (int j = 0; j < nodes[cycles[c][i]].size(); j++)
				{
					_mergedVertices.push_back(nodes[cycles[c][i]][j]);
				}
			}
			// Add the merged vertices from collapsed paths
			for (int i = 0; i < cycles[c].size(); i++)
			{
				for (int j = 0; j < cycles[c].size(); j++)
				{
					if (collapsedPaths[cycles[c][i]].find(cycles[c][j]) != collapsedPaths[cycles[c][i]].end())
					{
						vector<int> ToAdd = collapsedPaths[cycles[c][i]][cycles[c][j]];
						for (int k = 0; k < ToAdd.size(); k++)
							_mergedVertices.push_back(ToAdd[k]);
					}
				}
			}

			return;
			
		}
	}
	//
	for (set<int>::iterator itr = _mergedNodes.begin(); itr != _mergedNodes.end(); itr++)
	{
		for (int i = 0; i < nodes[*itr].size(); i++)
		{
			_mergedVertices.push_back(nodes[*itr][i]);
		}
	}
}


bool DirectedAcyclicMultiGraph::IsTree()
{
	int n = nodes.size();
	vector<set<int>> D;
	for (int i = 0; i < n; i++)
	{
		D.push_back(set<int>());
	}
	for (int i = 0; i < n; i++)
	{
		for (multimap<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			if (D[i].find(itr->first) != D[i].end())
				return false;
			D[i].insert(itr->first);
			D[itr->first].insert(i);
		}
	}
	return Graph::isTree(D);
}

vector<int> DirectedAcyclicMultiGraph::Hash()
{
	vector<vector<int>> Vertices;
	for (int i = 0; i < nodes.size(); i++)
	{
		vector<int> temp;
		for (int j = 0; j < nodes[i].size(); j++)
		{
			for (int k = 0; k < C->vertices[nodes[i][j]].size(); k++)
			{
				temp.push_back(C->vertices[nodes[i][j]][k]);
			}
		}
		sort(temp.begin(), temp.end());
		Vertices.push_back(temp);
	}

	vector<pair<int, int> > vp;
	for (int i = 0; i < nodes.size(); i++) 
	{
		vp.push_back(make_pair(Vertices[i][0], i));
	}
	sort(vp.begin(), vp.end());

	vector<int> hash(Vertices[vp[0].second]);

	for (int i = 1; i < nodes.size(); i++)
	{
		hash.push_back(-1);
		for (int j = 0; j < Vertices[vp[i].second].size(); j++)
		{
			hash.push_back(Vertices[vp[i].second][j]);
		}
	}
	return hash;
}

bool DirectedAcyclicMultiGraph::SubgraphContainsProducerCycle(vector<int> subgraph, CondensedMultiGraph* _C)
{
	// Get all the cycles for that subgraph
	int* aObjects1 = new int[subgraph.size()];
	for (int j = 0; j < subgraph.size(); j++)
		aObjects1[j] = subgraph[j];
	vector<int> edges;
	for (int j = 0; j < subgraph.size(); j++)
	{
		for (int k = j + 1; k < subgraph.size(); k++)
		{
			if (reversedEdges[subgraph[j]].count(subgraph[k]) + directed[subgraph[j]].count(subgraph[k]) > 0)
			{
				edges.push_back(j);
				edges.push_back(k);
			}
		}
	}
	size_t* aEdges1 = new size_t[edges.size()];
	for (int j = 0; j < edges.size(); j++)
		aEdges1[j] = edges[j];
	GraphCycles::Graph<int> g(aObjects1, subgraph.size(), aEdges1, edges.size() / 2);
	vector<vector<int>> allCycles = g.computeAllCycles();

	for (int c = 0; c < allCycles.size(); c++)
	{
		allCycles[c].pop_back();
		vector<map<int, int>> _directed;
		for (int i = 0; i < allCycles[c].size(); i++)
			_directed.push_back(map<int, int>());
		for (int i = 0; i < allCycles[c].size(); i++)
		{
			for (multimap<int, int>::iterator itr = directed[allCycles[c][i]].begin(); itr != directed[allCycles[c][i]].end(); itr++)
			{
				int prev = (allCycles[c].size() + i - 1) % allCycles[c].size();
				int next = (i + 1) % allCycles[c].size();
				if (itr->first == allCycles[c][prev])
				{
					if (_directed[i].find(prev) == _directed[i].end())
					{
						_directed[i].insert(pair<int, int>(prev, itr->second));
					}
					else
					{
						if (itr->second < _directed[i][prev])
							_directed[i][prev] = itr->second;
					}
				}
				else if (itr->first == allCycles[c][next])
				{
					if (_directed[i].find(next) == _directed[i].end())
					{
						_directed[i].insert(pair<int, int>(next, itr->second));
					}
					else
					{
						if (itr->second < _directed[i][next])
							_directed[i][next] = itr->second;
					}
				}
			}
		}
		vector<int> _capacities;
		for (int i = 0; i < allCycles[c].size(); i++)
			_capacities.push_back(capacities[allCycles[c][i]]);
		Cycle cycle(_capacities, _directed);
		if (cycle.IsProducer())
		{
			vector<int> MergedVertices;
			for (int itr = 0; itr < allCycles[c].size(); itr++)
			{
				for (int i = 0; i < nodes[allCycles[c][itr]].size(); i++)
				{
					MergedVertices.push_back(nodes[allCycles[c][itr]][i]);
				}
			}
			// Add the merged vertices from collapsed paths
			for (int i = 0; i < allCycles[c].size(); i++)
			{
				for (int j = 0; j < allCycles[c].size(); j++)
				{
					if (collapsedPaths[allCycles[c][i]].find(allCycles[c][j]) != collapsedPaths[allCycles[c][i]].end())
					{
						vector<int> ToAdd = collapsedPaths[allCycles[c][i]][allCycles[c][j]];
						for (int k = 0; k < ToAdd.size(); k++)
							MergedVertices.push_back(ToAdd[k]);
					}
				}
			}
			*_C = *C;
			_C->MacroMerger(MergedVertices);
			return true;
		}
	}
	return false;
}

bool DirectedAcyclicMultiGraph::ExistACycleOld(CondensedMultiGraph* _C)
{
	int n = nodes.size();
	vector<set<int>> D;
	for (int i = 0; i < n; i++)
	{
		D.push_back(set<int>());
	}
	for (int i = 0; i < n; i++)
	{
		for (multimap<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			D[i].insert(itr->first);
			D[itr->first].insert(i);
		}
	}
	vector<pair<int, int>> bridges = Graph::bridge(D);
	vector<pair<int, int>> realBridges;
	vector<int> BridgesIncidence(n, 0);
	for (int i = 0; i < bridges.size(); i++)
	{
		int count = reversedEdges[bridges[i].first].count(bridges[i].second)
			+ reversedEdges[bridges[i].second].count(bridges[i].first);
		if (count == 1)
		{
			D[bridges[i].first].erase(bridges[i].second);
			D[bridges[i].second].erase(bridges[i].first);
			BridgesIncidence[bridges[i].first]++;
			BridgesIncidence[bridges[i].second]++;
			realBridges.push_back(bridges[i]);
		}
	}
	vector<vector<int>> subgraphs = Graph::GetStronglyConnectedComponents(D);
	vector<vector<int>> roots;
	vector<int> SubgraphsBridgesIncidence(subgraphs.size(), 0);
	for (int i = 0; i < subgraphs.size(); i++)
	{
		for (int j = 0; j < subgraphs[i].size(); j++)
		{
			SubgraphsBridgesIncidence[i] += BridgesIncidence[subgraphs[i][j]];
		}
	}
	// iterate over the leaf subgraph that consist of a single nodes
	// if the edge is going to the leaf perform a merger
	for (int i = 0; i < subgraphs.size(); i++)
	{
		if ((subgraphs[i].size() == 1) && (SubgraphsBridgesIncidence[i] == 1))
		{
			if (directed[subgraphs[i][0]].empty())//the edge is going into the node nothing is out of it
			{
				// perform a merger
				vector<int> Path;
				Path.push_back(reversedEdges[subgraphs[i][0]].begin()->first);
				Path.push_back(subgraphs[i][0]);
				vector<int> MergedVertices;
				// this function check if there are other nodes that could be merged due to a newly generated cycle
				GetMergedVertices(Path, MergedVertices);
				// performe a merger here
				CondensedMultiGraph _C = *C;
				_C.MacroMerger(MergedVertices);
				return true;
			}
		}
	}
	// iterate over the leaf subgraph that consist of more than one node
	// if any of the cycles of this subgraph 
	for (int i = 0; i < subgraphs.size(); i++)
	{
		if ((subgraphs[i].size() > 1))// && (SubgraphsBridgesIncidence[i] == 1))
		{
			if (SubgraphContainsProducerCycle(subgraphs[i], _C))
				return true;
		}
	}
	//
	return false;
}

bool DirectedAcyclicMultiGraph::IsThereATerminalNodeThatDoesntHaveMaximalCapacity(CondensedMultiGraph* _C)
{
	//run Algorithm 2 on all terminal nodes and check if there is
	
	for (int Terminal = 0; Terminal < nodes.size(); Terminal++)
	{
		if (directed[Terminal].size() == 0)//this is a terminal node
		{
			// Get all \tilde{T}(n) in-tree elements
			stack<int> ToExplore;
			ToExplore.push(Terminal);
			set<int> InTreeElements;
			vector<int> InTreeList;
			vector<int> InTreeParentList;
			vector<int> InTreeEdgeCapacitiesList;
			while (!ToExplore.empty())
			{
				int node = ToExplore.top();
				ToExplore.pop();
				if (directed[node].size() <= 1)
				{
					InTreeElements.insert(node);
					InTreeList.push_back(node);
					if (InTreeList.size() == 1) // root of the tree
					{
						InTreeParentList.push_back(-1);
						InTreeEdgeCapacitiesList.push_back(0);
					}
					else
					{
						InTreeParentList.push_back(find(InTreeList.begin(), InTreeList.end(), directed[node].begin()->first) - InTreeList.begin());
						InTreeEdgeCapacitiesList.push_back(directed[node].begin()->second);
					}
					for (multimap<int,int>::iterator itr = reversedEdges[node].begin(); itr != reversedEdges[node].end(); itr++)
					{
						if (InTreeElements.find(itr->first) == InTreeElements.end())
						{
							ToExplore.push(itr->first);
						}
					}
				}
			}
			// If \tilde{T}(n) consists of one element, then ignore it.
			// else we will implement algorithm 2.
			if (InTreeElements.size() > 1)
			{
				vector<int> InTreeCapacitiesList;
				for (int i = 0; i < InTreeList.size(); i++)
					InTreeCapacitiesList.push_back(capacities[InTreeList[i]]);

				InTree it(InTreeList, InTreeParentList, InTreeCapacitiesList, InTreeEdgeCapacitiesList);

				// call algorithm 2
				vector<int> MSEQ;
				if (Algorithm2::MacroMerger(it, MSEQ))// there is a t-merger
				{
					// performe a merger here
					vector<int> MergedVertices;
					for (vector<int>::iterator itr = MSEQ.begin(); itr != MSEQ.end(); itr++)
					{
						for (int i = 0; i < nodes[*itr].size(); i++)
						{
							MergedVertices.push_back(nodes[*itr][i]);
						}
					}
					// Add the merged vertices from collapsed paths
					for (vector<int>::iterator i = MSEQ.begin(); i != MSEQ.end(); i++)
					{
						for (vector<int>::iterator j = MSEQ.begin(); j != MSEQ.end(); j++)
						{
							if (collapsedPaths[*i].find(*j) != collapsedPaths[*i].end())
							{
								vector<int> ToAdd = collapsedPaths[*i][*j];
								for (int k = 0; k < ToAdd.size(); k++)
									MergedVertices.push_back(ToAdd[k]);
							}
						}
					}
					*_C = *C;
					_C->MacroMerger(MergedVertices);
					return true;
				}
			}

		}
	}
	return false;
}

bool DirectedAcyclicMultiGraph::ExistAMeregerSequenceForATerminalNode(CondensedMultiGraph* _C, bool& _terminate)
{
	// We keep track with all feasible T(n,e) by the vector of maps TNEs
	TNEs.clear();
	for (int i = 0; i < nodes.size(); i++)
		TNEs.push_back(map<int, vector<int>>());

	//Iterate over all terminal nodes and find all feasible T(n,e)
	for (int Terminal = 0; Terminal < nodes.size(); Terminal++)
	{
		if (directed[Terminal].size() == 0)//this is a terminal node
		{
			int count = 0;
			for (map<int, int>::iterator itr = reversedEdges[Terminal].begin(); itr != reversedEdges[Terminal].end(); itr++)
			{
				// if e is infeasible then T(n,e) has no feasible mergers
				// if e is feasible, construct T(n,e) and search for a merger that include a source node
				if (capacities[Terminal] >= itr->second)
				{
					if (directed[itr->first].size() > 1)//it is a source node
					{
						vector<int> Path;
						Path.push_back(Terminal);
						Path.push_back(itr->first);
						TNEs[Terminal].insert(pair<int, vector<int>>(itr->first, Path));
						count++;
					}
					else//Construct T(n,e) then call algorithm3
					{
						stack<int> ToExplore;
						ToExplore.push(itr->first);
						set<int> InTreeElements;
						InTreeElements.insert(Terminal);
						vector<int> InTreeList;
						InTreeList.push_back(Terminal);
						vector<int> InTreeParentList;
						InTreeParentList.push_back(-1);
						vector<int> InTreeEdgeCapacitiesList;
						InTreeEdgeCapacitiesList.push_back(0);
						set<int> SourceNodes;
						while (!ToExplore.empty())
						{
							int node = ToExplore.top();
							ToExplore.pop();

							if (directed[node].size() <= 1)
							{
								InTreeElements.insert(node);
								InTreeList.push_back(node);
								InTreeParentList.push_back(find(InTreeList.begin(), InTreeList.end(), directed[node].begin()->first) - InTreeList.begin());
								InTreeEdgeCapacitiesList.push_back(directed[node].begin()->second);
								for (multimap<int, int>::iterator itr = reversedEdges[node].begin(); itr != reversedEdges[node].end(); itr++)
								{
									if (InTreeElements.find(itr->first) == InTreeElements.end())
									{
										ToExplore.push(itr->first);
									}
								}
							}
							else // it is a source node
							{
								SourceNodes.insert(node);
							}
						}

						vector<int> InTreeCapacitiesList;
						for (int i = 0; i < InTreeList.size(); i++)
							InTreeCapacitiesList.push_back(capacities[InTreeList[i]]);

						//
						vector<pair<int, pair<int, int>>> SourceNodesEdges;
						for (set<int>::iterator itr2 = SourceNodes.begin(); itr2 != SourceNodes.end(); itr2++)
						{
							for (map<int, int>::iterator itr3 = directed[*itr2].begin(); itr3 != directed[*itr2].end(); itr3++)
							{
								if (InTreeElements.find(itr3->first) != InTreeElements.end())
								{
									SourceNodesEdges.push_back(pair<int, pair<int, int>>(*itr2, *itr3));
								}
							}
						}
						TneTree tne(InTreeList, InTreeParentList, InTreeCapacitiesList, InTreeEdgeCapacitiesList, SourceNodes, SourceNodesEdges);
						vector<int> Tree;
						// call algorithm 3
						if (Algorithm3::MergingSource(tne, Tree))
						{
							TNEs[Terminal].insert(pair<int, vector<int>>(itr->first, Tree));
							count++;
						}
					}
				}
			}
			if (count == 0)
			{
				_terminate = true;
				return true;
			}
			else if (count == 1)
			{
				// basic edge merger here
				vector<int> Path;
				Path.push_back(TNEs[Terminal].begin()->first);
				Path.push_back(Terminal);
				vector<int> MergedVertices;
				// this function check if there are other nodes that could be merged due to a newly generated cycle
				GetMergedVertices(Path, MergedVertices);
				// performe a merger here
				*_C = *C;
				_C->MacroMerger(MergedVertices);

				return true;
			}
		}
	}
	_terminate = false;
	return false;
}

void DirectedAcyclicMultiGraph::Algorithm4_NAHS(CondensedMultiGraph* _C)
{
	vector<int> L;
	set<int> Lset;
	vector<vector<int>> Lparts;
	vector<int> Li_part;
	vector<int> M;
	int v = 0;
	for (int i = 0; i < nodes.size(); i++)
	{
		if (reversedEdges[i].empty())
		{
			v = i;
			break;
		}
	}
	if (directed[v].size() + reversedEdges[v].size() <= 2)
	{
		L.push_back(v);
		Lset.insert(v);
		Li_part.push_back(v);
		while ((directed[v].size() > 0) && (directed[v].size() + reversedEdges[v].size() <= 2))
		{
			v = directed[v].begin()->first;
			Li_part.push_back(v);
		}
		Li_part.pop_back();
		Lparts.push_back(Li_part);
	}

	while (Lset.find(v) == Lset.end())
	{
		L.push_back(v);
		Lset.insert(v);
		if (directed[v].size() > 0)
		{
			Li_part.clear(); 
			Li_part.push_back(v);
			v = directed[v].begin()->first;
			while ((directed[v].size() > 0) && (directed[v].size() + reversedEdges[v].size() <= 2))
			{
				Li_part.push_back(v);
				v = directed[v].begin()->first;
			}
			Lparts.push_back(Li_part);
		}
		else
		{
			int e = Lparts[Lparts.size() - 1][Lparts[Lparts.size() - 1].size() - 1]; // the last element in the last part
			for (map<int, vector<int>>::iterator itr = TNEs[v].begin(); itr != TNEs[v].end(); itr++)
			{
				if (itr->first != e)
				{
					Li_part = itr->second;
				}
			}
			int vd = Li_part[Li_part.size() - 1];


			for (map<int, int>::iterator itr = directed[vd].begin(); itr != directed[vd].end(); itr++)
			{
				if (find(Li_part.begin(), Li_part.end(), itr->first) == Li_part.end())//e is different from T(v,e')
				{
					v = itr->first;
					break;
				}
			}

			while ((directed[v].size() > 0) && (directed[v].size() + reversedEdges[v].size() <= 2))
			{
				Li_part.push_back(v);
				v = directed[v].begin()->first;
			}
			Lparts.push_back(Li_part);
		}
	}

	for (int i = 0; i < L.size(); i++)
	{
		if (L[i] == v)
		{
			//Perform a merger for all elements from L[v] to the end
			set<int> MergedNodes;
			for (int j = i; j < L.size(); j++)
			{
				for (int k = 0; k < Lparts[j].size(); k++)
					MergedNodes.insert(Lparts[j][k]);
			}
			vector<int> MergedVertices;

			for (set<int>::iterator itr = MergedNodes.begin(); itr != MergedNodes.end(); itr++)
			{
				for (int i = 0; i < nodes[*itr].size(); i++)
				{
					MergedVertices.push_back(nodes[*itr][i]);
				}
			}
			// Add the merged vertices from collapsed paths
			for (set<int>::iterator i = MergedNodes.begin(); i != MergedNodes.end(); i++)
			{
				for (set<int>::iterator j = MergedNodes.begin(); j != MergedNodes.end(); j++)
				{
					if (collapsedPaths[*i].find(*j) != collapsedPaths[*i].end())
					{
						vector<int> ToAdd = collapsedPaths[*i][*j];
						for (int k = 0; k < ToAdd.size(); k++)
							MergedVertices.push_back(ToAdd[k]);
					}
				}
			}
			*_C = *C;
			_C->MacroMerger(MergedVertices);
		}
	}
}

vector<int> DirectedAcyclicMultiGraph::GetArticulationPoints(vector<int>& Level)
{
	int n = nodes.size();
	vector<set<int>> D;
	for (int i = 0; i < n; i++)
	{
		D.push_back(set<int>());
	}
	for (int i = 0; i < n; i++)
	{
		for (multimap<int, int>::iterator itr = directed[i].begin(); itr != directed[i].end(); itr++)
		{
			D[i].insert(itr->first);
			D[itr->first].insert(i);
		}
	}
	vector<int> AP = Graph::AP(D, Level);
	return AP;
}


int DirectedAcyclicMultiGraph::GetNodeFromVertexNumber(int _vertex)
{
	for (int i = 0; i < C->vertices.size(); i++)
	{
		for (int j = 0; j < C->vertices[i].size(); j++)
		{
			if (C->vertices[i][j] == _vertex)
			{
				for (int n = 0; n < nodes.size(); n++)
				{
					for (int k = 0; k < nodes[n].size(); k++)
					{
						if (nodes[n][k] == i)
							return n;
					}
				}
			}
		}
	}
	return false;
}