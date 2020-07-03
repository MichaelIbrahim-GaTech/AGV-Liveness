#include "DirectedAcyclicMultiGraph.h"

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
		reversedEdges.push_back(reversedEdges[i]);
		collapsedNodes.push_back(collapsedNodes[i]);
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
		collapsedNodes.push_back(map<int, vector<int>>());
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
		else
		{
			int OutDegree = directed[i].size();
			if (OutDegree >= 2)
			{
				major[i] = true;
			}
			else
			{

				int InDegree = reversedEdges[i].size();
				if (InDegree >= 2)
				{
					major[i] = true;
				}
			}
		}
	}
	//CollapseNonMajorNodesPaths();
}

void DirectedAcyclicMultiGraph::CollapseNonMajorNodesPaths()
{ 
	for (int i = nodes.size() - 1; i >= 0; i--)
	{
		if (!major[i])
		{
			if ((reversedEdges[i].size() == 1) && (directed[i].size() == 1))
			{
				// put the necessary code to collapse this node
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

bool DirectedAcyclicMultiGraph::ExistProducerPathBasedMerger(vector<int> _order, vector<bool> _Sn, int _n, int _u, vector<int>& _path, int& _pathCapacity)
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
	if (PPBM.Exist(SubgraphDirected, SubgraphCapacities, Path, _pathCapacity))
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
		_C->MacroMerger(MergedVertices, INFINITY);
		return true;
	}
	else
	{
		return false;
	}
}

// This function implements Algorithm 2 of the paper
bool DirectedAcyclicMultiGraph::ExistAProducerMerger(CondensedMultiGraph* _C)
{
	vector<int> order = TopologicalOrder();
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
							LU[u].insert(itr->first);
						}
						else if (rhs == delta[u])
						{
							LU[u].insert(itr->first);
						}
					}
				}
				if (delta[u] < 0)
					delta[u] = 0;

				if (delta[u] == 0)
				{
					int InDegree = 0;
					set<int> InNodes;
					for (map<int, int>::iterator itr = reversedEdges[order[u]].begin(); itr != reversedEdges[order[u]].end(); itr++)
					{
						if (Sn[Index[itr->first]])
						{
							InNodes.insert(itr->first);
						}
					}
					InDegree = InNodes.size();
					// path based merger
					if (InDegree == 1)
					{
						vector<int> Path;
						int capacity = 0;
						if (ExistProducerPathBasedMerger(order, Sn, n, u, Path, capacity))
						{
							set<int> MergedNodes;
							for (int i = 0; i < Path.size(); i++)
								MergedNodes.insert(Path[i]);
							// performe a merger here
							vector<int> MergedVertices;
							for (set<int>::iterator itr = MergedNodes.begin(); itr != MergedNodes.end(); itr++)
							{
								for (int i = 0; i < nodes[*itr].size(); i++)
								{
									MergedVertices.push_back(nodes[*itr][i]);
								}
							}
							*_C = *C;
							_C->MacroMerger(MergedVertices, capacity);
							return true;
						}
					}
					// cycle based merger
					else if (InDegree > 1)
					{
						vector<int> Path;
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
	for (; Terminal < nodes.size(); Terminal++)
	{
		if (directed[Terminal].size() == 0)//this is a terminal node
			break;
	}
	// No Terminal Nodes (possibly something went wrong) in some place
	if(Terminal == nodes.size()) 
		return vector<CondensedMultiGraph>();
	vector<CondensedMultiGraph> result;
	for (multimap<int, int>::iterator itr = reversedEdges[Terminal].begin(); itr != reversedEdges[Terminal].end(); itr++)
	{
		if (itr->second <= capacities[Terminal])
		{
			vector<int> MergedVertices(nodes[Terminal]);
			for (int i = 0; i < nodes[itr->first].size(); i++)
				MergedVertices.push_back(nodes[itr->first][i]);
			int capacity = capacities[Terminal] + capacities[itr->first] - itr->second;
			CondensedMultiGraph _C = *C;
			_C.MacroMerger(MergedVertices, capacity);
			result.push_back(_C);
		}
	}
	return result;
}