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

bool AGV::IsLive()
{
	CondensedMultiGraph C_Ghat(&Ghat);
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
		G.Algorithm2();
	}
	return false;
}