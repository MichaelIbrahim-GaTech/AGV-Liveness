// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AGV.h"
using namespace std;
int main()
{
	// AGV1.txt is the example depicted on Fig.1 of the paper
	// AGV0.txt is the example depicted on Fig.1, with the modification of the edge 2->3 to 3->2
	// AGV2.txt is the example depicted on Fig.1, with extra node 22, 2->22->3 instead of 2->3
	// AGV3.txt is the path based merger on Fig.2a
	string file = "AGV3.txt";
	AGV g(file);
	if (g.IsLive())
		cout << file << " is live" << endl;
	else
		cout << file << " is not live" << endl;
}
