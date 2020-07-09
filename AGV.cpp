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

bool AGV::IsLive()
{
	CondensedMultiGraph C_Ghat(&Ghat);

	priority_queue<CondensedMultiGraph, vector<CondensedMultiGraph>, less<vector<CondensedMultiGraph>::value_type>> pqueue;
	pqueue.push(C_Ghat);
	stack<CondensedMultiGraph> STACK;
	STACK.push(C_Ghat);
	
	// Implement Algorithm 3 here.
	while (!STACK.empty())
	{
		CondensedMultiGraph C = STACK.top();
		STACK.pop();
		if (C.isSingleChained())
			return true;
		DirectedAcyclicMultiGraph G = DirectedAcyclicMultiGraph(&C);
		CondensedMultiGraph Cd;
		if (G.TerminalNodesCapacityLessThanAllInEdges())
		{
			// Do Nothing
		}
		else if (G.ExistAPathLeadingToNH(&Cd))
		{
			STACK.push(Cd);
		}
		else if (G.ExistAProducerMerger(&Cd))
		{
			STACK.push(Cd);
		}
		else
		{
			vector<CondensedMultiGraph> Cds = G.PickATerminalNodeAndCollapseFeasiblePaths();
			for (vector<CondensedMultiGraph>::iterator itr = Cds.begin(); itr != Cds.end(); itr++)
				STACK.push(*itr);
		}
	}
	return false;
}