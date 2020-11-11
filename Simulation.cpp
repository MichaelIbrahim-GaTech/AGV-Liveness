#include "Simulation.h"


Simulation::Simulation(CondensedMultiGraph _C, int _vertex, int _nodeCapacity)
{
	originalVertex = _vertex;
	originalNodeCapacity = _nodeCapacity;
	originalC = _C;
}

bool Simulation::simulate(CondensedMultiGraph& finalC)
{
	stack<pair<CondensedMultiGraph, int>> STACK; //int is the level in the computation tree
	STACK.push(make_pair(originalC, 0));

	// Implement main Algorithm here.
	int i = 0;
	int level;
	while (!STACK.empty())
	{
		CondensedMultiGraph C = STACK.top().first;
		level = STACK.top().second;
		STACK.pop();
		DirectedAcyclicMultiGraph G = DirectedAcyclicMultiGraph(&C);
		// check if the capacity of the vertex is greater than or equal to orinial capacity
		int originalNode = G.GetNodeFromVertexNumber(originalVertex);
		if (G.capacities[originalNode] >= originalNodeCapacity)
		{
			finalC = C;
			return true;
		}

		// you don't need to check if C will be single chained, since you are only simulation a subtree
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
			if (find(art.begin(), art.end(), originalNode) == art.end())
			{
				return false;
			}
			else // construct the block-cutpoint tree and run algorithm 5
			{
				BlockCutpointTree BCT(&G, art, apLevel, originalNode);

				while (true)
				{
					int Block = BCT.GetNextBlockToProcess();
					if (Block == -1) // finished processing and no more blocks could be explored
					{
						return false;
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
		cout << "While cycle repeated " << ++i << " times, tree level = " << level << endl;
	}
	return false;
}