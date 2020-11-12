#include "BlockDirectedAcyclicMultiGraph_4.h"

BlockDirectedAcyclicMultiGraph_4::BlockDirectedAcyclicMultiGraph_4(DirectedAcyclicMultiGraph* _U, set<int> Block, int _parentAP)
	: used(_U->capacities.size(), false)
{
	U = _U;
	parentAP = _parentAP;
	capacities = _U->capacities;
	for (set<int>::iterator itr = Block.begin(); itr != Block.end(); itr++)
		used[*itr] = true;

	for (int i = 0; i < used.size(); i++)
	{
		directed.push_back(multimap<int, int>());
		reversedEdges.push_back(multimap<int, int>());
	}
	for (int i = 0; i < used.size(); i++)
	{
		if (used[i])
		{
			for (map<int, int>::iterator itr = _U->directed[i].begin(); itr != _U->directed[i].end(); itr++)
			{
				if (used[itr->first])
					directed[i].insert(*itr);
			}
			for (map<int, int>::iterator itr = _U->reversedEdges[i].begin(); itr != _U->reversedEdges[i].end(); itr++)
			{
				if (used[itr->first])
					reversedEdges[i].insert(*itr);
			}
		}
	}
}

bool BlockDirectedAcyclicMultiGraph_4::IsThereATerminalNodeThatDoesntHaveMaximalCapacity(CondensedMultiGraph* _C)
{
	//run Algorithm 2 on all terminal nodes and check if there is

	for (int Terminal = 0; Terminal < used.size(); Terminal++)
	{
		if (!used[Terminal])
			continue;
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
					for (multimap<int, int>::iterator itr = reversedEdges[node].begin(); itr != reversedEdges[node].end(); itr++)
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
						for (int i = 0; i < U->nodes[*itr].size(); i++)
						{
							MergedVertices.push_back(U->nodes[*itr][i]);
						}
					}
					// Add the merged vertices from collapsed paths
					for (vector<int>::iterator i = MSEQ.begin(); i != MSEQ.end(); i++)
					{
						for (vector<int>::iterator j = MSEQ.begin(); j != MSEQ.end(); j++)
						{
							if (U->collapsedPaths[*i].find(*j) != U->collapsedPaths[*i].end())
							{
								vector<int> ToAdd = U->collapsedPaths[*i][*j];
								for (int k = 0; k < ToAdd.size(); k++)
									MergedVertices.push_back(ToAdd[k]);
							}
						}
					}
					*_C = *(U->C);
					_C->MacroMerger(MergedVertices);
					return true;
				}
			}

		}
	}
	return false;
}

bool BlockDirectedAcyclicMultiGraph_4::ExistAMeregerSequenceForATerminalNode(CondensedMultiGraph* _C, bool& _terminate, bool& _simulate, bool& _parentFault)
{
	_parentFault = false;
	// We keep track with all feasible T(n,e) by the vector of maps TNEs
	TNEs.clear();
	for (int i = 0; i < used.size(); i++)
		TNEs.push_back(map<int, vector<int>>());

	vector<int> Terminals;
	for (int Terminal = 0; Terminal < used.size(); Terminal++)
	{
		if (!used[Terminal])
			continue;
		if (Terminal == parentAP)
			continue;
		Terminals.push_back(Terminal);
	}
	Terminals.push_back(parentAP);
	//Iterate over all terminal nodes and find all feasible T(n,e)
	for (int TerminalItr = 0; TerminalItr < Terminals.size(); TerminalItr++)
	{
		int Terminal = Terminals[TerminalItr];

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
				if (Terminal == parentAP)
					_parentFault = true;
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
				U->GetMergedVertices(Path, MergedVertices);
				// performe a merger here
				*_C = *(U->C);
				_C->MacroMerger(MergedVertices);
				if (Terminal == parentAP)
					_simulate = true;

				return true;
			}
		}
	}
	_terminate = false;
	return false;
}


void BlockDirectedAcyclicMultiGraph_4::Algorithm4_NAHS(CondensedMultiGraph* _C)
{
	vector<int> L;
	set<int> Lset;
	vector<vector<int>> Lparts;
	vector<int> Li_part;
	vector<int> M;
	int v = 0;
	for (int i = 0; i < used.size(); i++)
	{
		if (used[i])
		{
			if (reversedEdges[i].empty())
			{
				v = i;
				break;
			}
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
				for (int i = 0; i < U->nodes[*itr].size(); i++)
				{
					MergedVertices.push_back(U->nodes[*itr][i]);
				}
			}
			// Add the merged vertices from collapsed paths
			for (set<int>::iterator i = MergedNodes.begin(); i != MergedNodes.end(); i++)
			{
				for (set<int>::iterator j = MergedNodes.begin(); j != MergedNodes.end(); j++)
				{
					if (U->collapsedPaths[*i].find(*j) != U->collapsedPaths[*i].end())
					{
						vector<int> ToAdd = U->collapsedPaths[*i][*j];
						for (int k = 0; k < ToAdd.size(); k++)
							MergedVertices.push_back(ToAdd[k]);
					}
				}
			}
			*_C = *(U->C);
			_C->MacroMerger(MergedVertices);
			return;
		}
	}
}
