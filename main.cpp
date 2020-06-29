// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "AGV.h"
using namespace std;
int main()
{
	string file = "AGV2.txt";
	AGV g(file);
	if (g.IsLive())
		cout << file << " is live" << endl;
	else
		cout << file << " is not live" << endl;
}
