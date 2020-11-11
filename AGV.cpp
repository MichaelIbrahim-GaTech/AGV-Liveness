#include "AGV.h"

AGV::AGV(string fileName)
{
	ifstream file(fileName);
	int numberOfVertices, numberOfEdges, numberOfVehicules;
	vector<map<int, int>> guidePath;
	A.clear();
	file >> numberOfVertices >> numberOfEdges >> numberOfVehicules >> nh;
	for (int i = 0; i < numberOfVertices; i++)
	{
		guidePath.push_back(map<int, int>());
		A.push_back(map<int, int>());
	}

	for (int i = 0; i < numberOfEdges; i++)
	{
		int from, to, count;
		file >> from >> to >> count;
		if (guidePath[from].find(to) == guidePath[from].end())
		{
			guidePath[from][to] += count;
		}
		else
		{
			guidePath[from].insert(pair<int, int>(to, count));
		}
	}
	for (int i = 0; i < numberOfVehicules; i++)
	{
		int from, to, count;
		file >> from >> to >> count;
		if (A[from].find(to) == A[from].end())
		{
			A[from][to] += count;
		}
		else
		{
			A[from].insert(pair<int, int>(to, count));
		}
	}
	file.close();
	G = MultiGraph(guidePath, nh);
	Ghat = G;
	if (Ghat.AddVehicules(A))
		cout << "All vehicules added" << endl;
	else
		cout << "Problem in adding vehicules" << endl;
}

AGV::AGV(string fileName, bool condensed)
{
	ifstream file(fileName);
	int numberOfVertices, numberOfUndirectedEdges, numberOfDirectedEdges;
	vector<map<int, int>> guidePath;
	A.clear();
	file >> numberOfVertices >> numberOfUndirectedEdges >> numberOfDirectedEdges >> nh;
	for (int i = 0; i < numberOfVertices; i++)
	{
		guidePath.push_back(map<int, int>());
		A.push_back(map<int, int>());
	}
	int verticesCount = numberOfVertices;
	// read the capacities
	for (int i = 0; i < numberOfVertices; i++)
	{
		int capacity = 0;
		file >> capacity;
		if (capacity == 1)
		{
			guidePath.push_back(map<int, int>());
			A.push_back(map<int, int>());
			guidePath[i].insert(pair<int, int>(verticesCount, 2));
			A[i].insert(pair<int, int>(verticesCount, 1));
			verticesCount++;
		}
		else if (capacity > 1)
		{
			guidePath.push_back(map<int, int>());
			A.push_back(map<int, int>());
			guidePath[i].insert(pair<int, int>(verticesCount, capacity));
			verticesCount++;
		}
	}
	// read the undirected edges
	for (int i = 0; i < numberOfUndirectedEdges; i++)
	{
		int from, to, weight;
		file >> from >> to >> weight;
		if (weight == 1)
		{
			if (guidePath[from].find(to) == guidePath[from].end())
			{
				guidePath[from][to] += 1;
			}
			else
			{
				guidePath[from].insert(pair<int, int>(to, 1));
			}
		}
		else if (weight > 1)
		{
			vector<int> IntermediateVertices;
			for (int j = 1; j < weight; j++)
			{
				IntermediateVertices.push_back(verticesCount);
				guidePath.push_back(map<int, int>());
				A.push_back(map<int, int>());
				verticesCount++;
			}
			guidePath[from].insert(pair<int, int>(IntermediateVertices[0], 1));
			guidePath[IntermediateVertices[weight - 2]].insert(pair<int, int>(to, 1));
			for (int j = 0; j < weight - 2; j++)
			{
				guidePath[IntermediateVertices[j]].insert(pair<int, int>(IntermediateVertices[j + 1], 1));
			}
		}
	}
	// read the directed edges
	for (int i = 0; i < numberOfDirectedEdges; i++)
	{
		int from, to, weight;
		file >> from >> to >> weight;
		if (weight == 1)
		{
			if (guidePath[from].find(to) == guidePath[from].end())
			{
				guidePath[from][to] += 1;
			}
			else
			{
				guidePath[from].insert(pair<int, int>(to, 1));
			}
			if (A[from].find(to) == A[from].end())
			{
				A[from][to] += 1;
			}
			else
			{
				A[from].insert(pair<int, int>(to, 1));
			}
		}
		else if (weight > 1)
		{
			vector<int> IntermediateVertices;
			for (int j = 1; j < weight; j++)
			{
				IntermediateVertices.push_back(verticesCount);
				guidePath.push_back(map<int, int>());
				A.push_back(map<int, int>());
				verticesCount++;
			}
			guidePath[from].insert(pair<int, int>(IntermediateVertices[0], 1));
			guidePath[IntermediateVertices[weight - 2]].insert(pair<int, int>(to, 1));
			A[from].insert(pair<int, int>(IntermediateVertices[0], 1));
			A[IntermediateVertices[weight - 2]].insert(pair<int, int>(to, 1));
			for (int j = 0; j < weight - 2; j++)
			{
				guidePath[IntermediateVertices[j]].insert(pair<int, int>(IntermediateVertices[j + 1], 1));
				A[IntermediateVertices[j]].insert(pair<int, int>(IntermediateVertices[j + 1], 1));
			}
		}
	}
	file.close();
	G = MultiGraph(guidePath, nh);
	Ghat = G;
	if (Ghat.AddVehicules(A))
		cout << "All vehicules added" << endl;
	else
		cout << "Problem in adding vehicules" << endl;
}


bool AGV::ExploredBefore(int _nodes, vector<int> _hash)
{
	// This condition will be executed only once when the graph is checked for the first time
	if (ExploredConfigurations.empty())
	{
		for (int i = 0; i < _nodes; i++)
			ExploredConfigurations.push_back(vector<vector<int>>());
		return false;
	}

	for (int i = 0; i < ExploredConfigurations[_nodes].size(); i++)
	{
		if (ExploredConfigurations[_nodes][i].size() == _hash.size())
		{
			if (std::equal(ExploredConfigurations[_nodes][i].begin(), ExploredConfigurations[_nodes][i].end(), _hash.begin()))
			{
				return true;
			}
		}
	}

	ExploredConfigurations[_nodes].push_back(_hash);

	return false;
}

bool AGV::IsLive()
{
	CondensedMultiGraph C_Ghat(&Ghat);

	//priority_queue<CondensedMultiGraph, vector<CondensedMultiGraph>, less<vector<CondensedMultiGraph>::value_type>> pqueue;
	//pqueue.push(C_Ghat);
	stack<pair<CondensedMultiGraph, int>> STACK; //int is the level in the computation tree
	STACK.push(make_pair(C_Ghat, 0));

	// Implement main Algorithm here.
	int i = 0;
	int level;
	while (!STACK.empty())
	{
		CondensedMultiGraph C = STACK.top().first;
		level = STACK.top().second;
		STACK.pop();
		if (C.isSingleChained())
			return true;
		DirectedAcyclicMultiGraph G = DirectedAcyclicMultiGraph(&C);


		if (G.IsTree())
		{
			cout << "Tree" << endl;
			TreeLiveness TL = G;
			if (TL.IsLive())
				return true;
		}
		else
		{
			CondensedMultiGraph Cd;
			bool terminate = false;
			if (G.TerminalNodesCapacityLessThanAllInEdges())
			{
				// Do Nothing
				cout << "  nothing pushing to stack at level = " << level << endl;
			}
			else if (G.ExistAProducerMergerEdge(&Cd))
			{
				STACK.push(make_pair(Cd, level + 1));
				cout << "  pushing to stack because there is a producer merger at level = " << level << endl;
			}
			else if (G.ExistAPathLeadingToNH(&Cd))
			{
				STACK.push(make_pair(Cd, level + 1));
				cout << "  pushing to stack because there is a path leading to n_h at level = " << level << endl;
			}
			else
			{
				vector<int> apLevel;
				vector<int> art = G.GetArticulationPoints(apLevel);
				if (art.size() == 0) // run algorithm 4 without constructing the tree
				{
					Algorithm5 alg5;
					if (alg5.OneStep(G, &Cd))
					{
						STACK.push(make_pair(Cd, level + 1));
						cout << "  pushing to stack due to Algorithm 5 at level = " << level << endl;
					}
					else
					{
						// Do Nothing (Terminate)
						cout << "  nothing pushing to stack due to Algorithm 5 at level = " << level << endl;
					}
				}
				else // construct the block-cutpoint tree and run algorithm 5
				{
					BlockCutpointTree BCT(&G, art, apLevel);

					while (true)
					{
						int Block = BCT.GetNextBlockToProcess();
						if (Block == -1) // finished processing and no more blocks could be explored
						{
							break;
						}
						int parentAP = BCT.ParentAP[Block];
						BlockType type = BCT.Types[Block];
						cout << "processing a block corresponding to case  " << type << " of Algorithm 6 at level = " << level << endl;

						if (type == BlockType::CASE_I)
						{
							int from = BCT.ParentAP[Block];
							if (from == -1)
								from = G.nh;
							set<int> temp = BCT.Blocks[Block];
							temp.erase(from);
							int to = *temp.begin();
							if (G.ExistAFeasibleMergerEdge(from, to, &Cd))
							{
								STACK.push(make_pair(Cd, level + 1));
							}
							else
							{
								//push nothing
								cout << "  nothing pushing to stack due to Algorithm 6 case I at level = " << level << endl;
							}
							break;
						}
						else if (type == BlockType::CASE_II)
						{
							// if it has a subtree that we could simulate
							if (!BCT.Children[Block].empty())
							{
								int to = BCT.ParentAP[Block];
								if (to == -1)
									to = G.nh;
								set<int> temp = BCT.Blocks[Block];
								temp.erase(to);
								int from = *temp.begin();
								if (G.ExistAFeasibleMergerEdge(from, to, &Cd))
								{
									// DO SIMULATION CODE HERE
									int capacity = G.capacities[parentAP];
									int vertex = G.C->vertices[G.nodes[parentAP][0]][0];
									Simulation sim(Cd, vertex, capacity);
									if (sim.simulate(Cd))
									{
										STACK.push(make_pair(Cd, level + 1));
										cout << "  pushing to stack due to Algorithm 6 Case II at level = " << level << endl;
										break;
									}
								}
							}
							// mark block as processed and get another block.
							BCT.Processed[Block] = true;
							cout << "  Marking block as processed due to Algorithm 6 case II at level = " << level << endl;
						}
						else if (type == BlockType::CASE_III)
						{
							Algorithm5 alg5;
							BlockDirectedAcyclicMultiGraph_3 BDAG(&G, BCT.Blocks[Block], parentAP);
							if (alg5.OneStep(BDAG, &Cd))
							{
								STACK.push(make_pair(Cd, level + 1));
								cout << "  pushing to stack due to Algorithm 6 Case III at level = " << level << endl;
							}
							else
							{
								// Do Nothing (Terminate)
								cout << "  nothing pushing to stack due to Algorithm 6 Case III at level = " << level << endl;
							}
							break;
						}
						else if (type == BlockType::CASE_IV)
						{
							Algorithm5 alg5;
							BlockDirectedAcyclicMultiGraph_4 BDAG(&G, BCT.Blocks[Block], parentAP);
							bool simulate = false;
							if (alg5.OneStep(BDAG, &Cd, simulate))
							{
								if (simulate)
								{
									// DO SIMULATION CODE HERE
									int capacity = G.capacities[parentAP];
									int vertex = G.C->vertices[G.nodes[parentAP][0]][0];
									Simulation sim(Cd, vertex, capacity);
									if (sim.simulate(Cd))
									{
										STACK.push(make_pair(Cd, level + 1));
										cout << "  pushing to stack due to Algorithm 6 Case IV at level = " << level << endl;
										break;
									}
									else
									{
										// mark block as processed and get another block if the simulation ends with decreasing the capacity of the parent ap
										BCT.Processed[Block] = true;
										cout << "  Marking block as processed due to Algorithm 6 case IV at level = " << level << endl;
									}
								}
								else
								{
									STACK.push(make_pair(Cd, level + 1));
									cout << "  pushing to stack due to Algorithm 6 Case IV at level = " << level << endl;
									break;
								}
							}
							else
							{
								// Do Nothing (Terminate)
								cout << "  nothing pushing to stack due to Algorithm 6 Case III at level = " << level << endl;
								break;
							}
						}
						else // CASE V
						{
							Algorithm5 alg5;
							BlockDirectedAcyclicMultiGraph BDAG(&G, BCT.Blocks[Block]);
							if (alg5.OneStep(BDAG, &Cd))
							{
								STACK.push(make_pair(Cd, level + 1));
								cout << "  pushing to stack due to Algorithm 6 Case V at level = " << level << endl;
							}
							else
							{
								// Do Nothing (Terminate)
								cout << "  nothing pushing to stack due to Algorithm 6 Case 5 at level = " << level << endl;
							}
							break;
						}

					}
				}
			}
		}
		cout << "While cycle repeated " << ++i << " times, tree level = " << level << endl;
	}
	return false;
}
