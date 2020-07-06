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
	// AGV4.txt is a cycle based merger on Fig.2b
	// AGV5.txt is a cycle based merger on Fig.3a
	// AGV6.txt is a path based producer merger similar to Fig.3b
	// AGV7.txt is the example depicted on Fig.4
	// AGV8.txt is an example similar to the rev-prod-path-based-merger-check.pdf
	// AGV9.txt is an example similar to the rev-prod-path-based-merger-check.pdf
	// AGV10.txt is the example depicted on Fig.5a
	// AGV11.txt is the example depicted on Fig.5b
	string file = "AGV11.txt";
	AGV g(file);
	if (g.IsLive())
		cout << file << " is live" << endl;
	else
		cout << file << " is not live" << endl;
}
