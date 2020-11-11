#include "Algorithm5.h"


bool Algorithm5::OneStep(DirectedAcyclicMultiGraph U, CondensedMultiGraph* Cd)
{
	// IN this implementation we don't check for the first 2 if conditions, because we already did at AGV::IsLive function
	bool terminate = false;
	if (U.IsThereATerminalNodeThatDoesntHaveMaximalCapacity(Cd))
	{
		cout << "Algorithm 5 - tree macro meger" << endl;
		return true;
	}
	else if (U.ExistAMeregerSequenceForATerminalNode(Cd, terminate))
	{
		if (terminate)
		{
			cout << "Algorithm 5 - terminate" << endl;
			return false;
		}
		cout << "Algorithm 5 - single edge basic merger" << endl;
		return true;
	}
	//else if(U.ExistACycleOld(Cd))
	//{
	//	return true;
	//	cout << "Algorithm 5 - cycle generating merger" << endl;
	//}
	else
	{
		U.Algorithm4_NAHS(Cd);
		cout << "Algorithm 5 - macro merger in proposition 41" << endl;
		return true;
	}

	cout << "Algorithm 5 - problem" << endl;
	return false;
}


bool Algorithm5::OneStep(BlockDirectedAcyclicMultiGraph U, CondensedMultiGraph* Cd)
{
	// IN this implementation we don't check for the first 2 if conditions, because we already did at AGV::IsLive function
	bool terminate = false;
	if (U.IsThereATerminalNodeThatDoesntHaveMaximalCapacity(Cd))
	{
		cout << "Algorithm 5 - tree macro meger" << endl;
		return true;
	}
	else if (U.ExistAMeregerSequenceForATerminalNode(Cd, terminate))
	{
		if (terminate)
		{
			cout << "Algorithm 5 - terminate" << endl;
			return false;
		}
		cout << "Algorithm 5 - single edge basic merger" << endl;
		return true;
	}
	else
	{
		U.Algorithm4_NAHS(Cd);
		cout << "Algorithm 5 - macro merger in proposition 41" << endl;
		return true;
	}

	cout << "Algorithm 5 - problem" << endl;
	return false;
}

bool Algorithm5::OneStep(BlockDirectedAcyclicMultiGraph_3 U, CondensedMultiGraph* Cd)
{
	// IN this implementation we don't check for the first 2 if conditions, because we already did at AGV::IsLive function
	bool terminate = false;
	if (U.IsThereATerminalNodeThatDoesntHaveMaximalCapacity(Cd))
	{
		cout << "Algorithm 5 - tree macro meger" << endl;
		return true;
	}
	else if (U.ExistAMeregerSequenceForATerminalNode(Cd, terminate))
	{
		if (terminate)
		{
			cout << "Algorithm 5 - terminate" << endl;
			return false;
		}
		cout << "Algorithm 5 - single edge basic merger" << endl;
		return true;
	}
	else
	{
		U.Algorithm4_NAHS(Cd);
		cout << "Algorithm 5 - macro merger in proposition 41" << endl;
		return true;
	}

	cout << "Algorithm 5 - problem" << endl;
	return false;
}

bool Algorithm5::OneStep(BlockDirectedAcyclicMultiGraph_4 U, CondensedMultiGraph* Cd, bool& _simulate)
{
	// IN this implementation we don't check for the first 2 if conditions, because we already did at AGV::IsLive function
	bool terminate = false;
	if (U.IsThereATerminalNodeThatDoesntHaveMaximalCapacity(Cd))
	{
		cout << "Algorithm 5 - tree macro meger" << endl;
		return true;
	}
	else if (U.ExistAMeregerSequenceForATerminalNode(Cd, terminate, _simulate))
	{
		if (terminate)
		{
			cout << "Algorithm 5 - terminate" << endl;
			return false;
		}
		cout << "Algorithm 5 - single edge basic merger" << endl;
		return true;
	}
	else
	{
		U.Algorithm4_NAHS(Cd);
		cout << "Algorithm 5 - macro merger in proposition 41" << endl;
		return true;
	}

	cout << "Algorithm 5 - problem" << endl;
	return false;
}