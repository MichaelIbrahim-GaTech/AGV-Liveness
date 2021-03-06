#include "Graph.h"

vector<set<int>> Graph::getTranspose(const vector<set<int>>& _d)
{
	vector<set<int>> d;
	for (int i = 0; i < _d.size(); i++)
		d.push_back(set<int>());
	for (int i = 0; i < _d.size(); i++)
	{
		for (set<int>::iterator j = _d[i].begin(); j != _d[i].end(); j++)
			d[*j].insert(i);
	}
	return d;
}

// A recursive function to print DFS starting from v 
vector<int> Graph::DFSUtil(int v, bool visited[], const vector<set<int>>& _d)
{
	// Mark the current node as visited and print it 
	visited[v] = true;
	vector<int> result;
	result.push_back(v);
	

	// Recur for all the vertices adjacent to this vertex 
	set<int>::iterator i;
	for (i = _d[v].begin(); i != _d[v].end(); ++i)
		if (!visited[*i])
		{
			vector<int> temp = DFSUtil(*i, visited, _d);
			for (int j = 0; j < temp.size(); j++)
				result.push_back(temp[j]);
		}
	return result;
}

void Graph::fillOrder(int v, bool visited[], stack<int>& Stack, const vector<set<int>>& _d)
{
	// Mark the current node as visited and print it 
	visited[v] = true;

	// Recur for all the vertices adjacent to this vertex 
	for (set<int>::iterator i = _d[v].begin(); i != _d[v].end(); ++i)
		if (!visited[*i])
			fillOrder(*i, visited, Stack, _d);

	// All vertices reachable from v are processed by now, push v  
	Stack.push(v);
}

vector<vector<int>> Graph::GetStronglyConnectedComponents(const vector<set<int>>& _d)
{
	vector<vector<int>> result;
	int n = _d.size();
	stack<int> Stack;

	// Mark all the vertices as not visited (For first DFS) 
	bool* visited = new bool[n];
	for (int i = 0; i < n; i++)
		visited[i] = false;

	// Fill vertices in stack according to their finishing times 
	for (int i = 0; i < n; i++)
		if (visited[i] == false)
			fillOrder(i, visited, Stack, _d);

	// Create a reversed graph 
	vector<set<int>> gr = getTranspose(_d);

	// Mark all the vertices as not visited (For second DFS) 
	for (int i = 0; i < n; i++)
		visited[i] = false;

	// Now process all vertices in order defined by Stack 
	while (Stack.empty() == false)
	{
		// Pop a vertex from stack 
		int v = Stack.top();
		Stack.pop();

		// Print Strongly connected component of the popped vertex 
		if (visited[v] == false)
		{
			vector<int> temp = DFSUtil(v, visited, gr);
			result.push_back(temp);
		}
	}
	return result;
}

// A recursive function that finds and prints bridges using 
// DFS traversal 
// u --> The vertex to be visited next 
// visited[] --> keeps tract of visited vertices 
// disc[] --> Stores discovery times of visited vertices 
// parent[] --> Stores parent vertices in DFS tree 
vector<pair<int, int>> Graph::bridgeUtil(int*time, int u, bool visited[], int disc[], int low[], int parent[], const vector<set<int>>& _d)
{
	vector<pair<int, int>> bridges;

	// Mark the current node as visited 
	visited[u] = true;

	// Initialize discovery time and low value 
	*time = *time + 1;
	disc[u] = low[u] = *time;

	// Go through all vertices aadjacent to this 
	for (set<int>::iterator i = _d[u].begin(); i != _d[u].end(); ++i)
	{
		int v = *i;  // v is current adjacent of u 

		// If v is not visited yet, then recur for it 
		if (!visited[v])
		{
			parent[v] = u;
			vector<pair<int,int>> temp = bridgeUtil(time, v, visited, disc, low, parent, _d);
			for (int j = 0; j < temp.size(); j++)
				bridges.push_back(temp[j]);
			// Check if the subtree rooted with v has a  
			// connection to one of the ancestors of u 
			//low[u] = min(low[u], low[v]);
			low[u] = (low[u] < low[v]) ? low[u] : low[v];

			// If the lowest vertex reachable from subtree  
			// under v is  below u in DFS tree, then u-v  
			// is a bridge 
			if (low[v] > disc[u])
			{
				pair<int, int> brdg(u, v);
				bridges.push_back(brdg);
			}
		}

		// Update low value of u for parent function calls. 
		else if (v != parent[u])
			//low[u] = min(low[u], disc[v]);
			low[u] = (low[u] < disc[v]) ? low[u] : disc[v];
	}

	return bridges;
}

// DFS based function to find all bridges. It uses recursive  
// function bridgeUtil() 
vector<pair<int,int>> Graph::bridge(const vector<set<int>>& _d)
{
	vector<pair<int, int>> bridges;
	int n = _d.size();
	// Mark all the vertices as not visited 
	bool* visited = new bool[n];
	int* disc = new int[n];
	int* low = new int[n];
	int* parent = new int[n];
	int* time = new int;
	*time = 0;

	// Initialize parent and visited arrays 
	for (int i = 0; i < n; i++)
	{
		parent[i] = NIL;
		visited[i] = false;
	}

	// Call the recursive helper function to find Bridges 
	// in DFS tree rooted with vertex 'i' 
	for (int i = 0; i < n; i++)
		if (visited[i] == false)
		{
			vector<pair<int,int>> temp = bridgeUtil(time, i, visited, disc, low, parent, _d);
			for (int j = 0; j < temp.size(); j++)
				bridges.push_back(temp[j]);
		}

	return bridges;
}

// A recursive function used by topologicalSort 
void Graph::topologicalSortUtil(int v, bool visited[], stack<int>& Stack, const vector<set<int>>& _d)
{
	// Mark the current node as visited. 
	visited[v] = true;

	// Recur for all the vertices  
// adjacent to this vertex 
	for (set<int>::iterator i = _d[v].begin(); i != _d[v].end(); ++i)
		if (!visited[*i])
			topologicalSortUtil(*i, visited, Stack, _d);

	// Push current vertex to stack  
// which stores result 
	Stack.push(v);
}

// The function to do Topological Sort. 
// It uses recursive topologicalSortUtil() 
vector<int> Graph::topologicalSort(const vector<set<int>>& _d)
{
	int n = _d.size();
	stack<int> Stack;

	// Mark all the vertices as not visited 
	bool* visited = new bool[n];
	for (int i = 0; i < n; i++)
		visited[i] = false;

	// Call the recursive helper function 
// to store Topological 
	// Sort starting from all  
// vertices one by one 
	for (int i = 0; i < n; i++)
		if (visited[i] == false)
			topologicalSortUtil(i, visited, Stack, _d);

	// Print contents of stack 
	vector<int> order;
	while (Stack.empty() == false)
	{
		//cout << Stack.top() << " ";
		order.push_back(Stack.top());
		Stack.pop();
	}

	return order;
}

// A recursive function that uses visited[] and parent to 
// detect cycle in subgraph reachable from vertex v. 
bool Graph::isCyclicUtil(int v, bool visited[], int parent, const vector<set<int>>& _d)
{
	// Mark the current node as visited 
	visited[v] = true;

	// Recur for all the vertices adjacent to this vertex 
	for (set<int>::iterator i = _d[v].begin(); i != _d[v].end(); ++i)
	{
		// If an adjacent is not visited, then recur for  
		// that adjacent 
		if (!visited[*i])
		{
			if (isCyclicUtil(*i, visited, v, _d))
				return true;
		}

		// If an adjacent is visited and not parent of current 
		// vertex, then there is a cycle. 
		else if (*i != parent)
			return true;
	}
	return false;
}

// Returns true if the graph is a tree, else false. 
bool Graph::isTree(const vector<set<int>>& _d)
{
	// Mark all the vertices as not visited and not part of  
	// recursion stack 
	int n = _d.size();
	bool* visited = new bool[n];
	for (int i = 0; i < n; i++)
		visited[i] = false;

	// The call to isCyclicUtil serves multiple purposes. 
	// It returns true if graph reachable from vertex 0  
	// is cyclcic. It also marks all vertices reachable  
	// from 0. 
	if (isCyclicUtil(0, visited, -1, _d))
		return false;

	// If we find a vertex which is not reachable from 0  
	// (not marked by isCyclicUtil(), then we return false 
	for (int u = 0; u < n; u++)
		if (!visited[u])
			return false;

	return true;
}

// Prints all paths from 's' to 'd' 
void Graph::GetAllPaths(int s, const vector<set<int>>& _d, vector<vector<int>>& _paths)
{
	int n = _d.size();
	// Mark all the vertices as not visited 
	bool* visited = new bool[n];

	// Create an array to store paths 
	int* path = new int[n];
	int path_index = 0; // Initialize path[] as empty 

	// Initialize all vertices as not visited 
	for (int i = 0; i < n; i++)
		visited[i] = false;

	// Call the recursive helper function to print all paths 
	GetAllPathsUtil(s, visited, path, path_index, _d, _paths);
}

// A recursive function to print all paths from 'u' to 'd'. 
// visited[] keeps track of vertices in current path. 
// path[] stores actual vertices and path_index is current 
// index in path[] 
void Graph::GetAllPathsUtil(int u, bool visited[], int path[], int& path_index, const vector<set<int>>& _d, vector<vector<int>>& _paths)
{
	// Mark the current node and store it in path[] 
	visited[u] = true;
	path[path_index] = u;
	path_index++;

	// If current vertex is same as destination, then print 
	// current path[] 
	if (_d[u].empty())
	{
		vector<int> temp;
		for (int i = 0; i < path_index; i++)
			temp.push_back(path[i]);
		_paths.push_back(temp);
	}
	else // If current vertex is not destination 
	{
		// Recur for all the vertices adjacent to current vertex 
		for (set<int>::iterator i = _d[u].begin(); i != _d[u].end(); ++i)
			if (!visited[*i])
				GetAllPathsUtil(*i, visited, path, path_index, _d, _paths);
	}

	// Remove current vertex from path[] and mark it as unvisited 
	path_index--;
	visited[u] = false;
}

// A recursive function that find articulation points using DFS traversal 
// u --> The vertex to be visited next 
// visited[] --> keeps tract of visited vertices 
// disc[] --> Stores discovery times of visited vertices 
// parent[] --> Stores parent vertices in DFS tree 
// ap[] --> Store articulation points 
void Graph::APUtil(int u, bool visited[], int disc[], int low[], int parent[], bool ap[], const vector<set<int>>& _d)
{
	// A static variable is used for simplicity, we can avoid use of static 
	// variable by passing a pointer. 
	static int time = 0;

	// Count of children in DFS Tree 
	int children = 0;

	// Mark the current node as visited 
	visited[u] = true;

	// Initialize discovery time and low value 
	disc[u] = low[u] = ++time;

	// Go through all vertices aadjacent to this 
	set<int>::iterator i;
	for (i = _d[u].begin(); i != _d[u].end(); ++i)
	{
		int v = *i;  // v is current adjacent of u 

		// If v is not visited yet, then make it a child of u 
		// in DFS tree and recur for it 
		if (!visited[v])
		{
			children++;
			parent[v] = u;
			APUtil(v, visited, disc, low, parent, ap, _d);

			// Check if the subtree rooted with v has a connection to 
			// one of the ancestors of u 
			//low[u] = min(low[u], low[v]);
			if (low[v] < low[u])
				low[u] = low[v];

			// u is an articulation point in following cases 

			// (1) u is root of DFS tree and has two or more chilren. 
			if (parent[u] == NIL && children > 1)
				ap[u] = true;

			// (2) If u is not root and low value of one of its child is more 
			// than discovery value of u. 
			if (parent[u] != NIL && low[v] >= disc[u])
				ap[u] = true;
		}

		// Update low value of u for parent function calls. 
		else if (v != parent[u])
		{
			//low[u] = min(low[u], disc[v]);
			if (disc[v] < low[u])
				low[u] = disc[v];
		}
	}
}

// The function to do DFS traversal. It uses recursive function APUtil() 
vector<int> Graph::AP(const vector<set<int>>& _d, vector<int>& Level)
{
	int n = _d.size();
	// Mark all the vertices as not visited 
	bool* visited = new bool[n];
	int* disc = new int[n];
	int* low = new int[n];
	int* parent = new int[n];
	bool* ap = new bool[n]; // To store articulation points 

	// Initialize parent and visited, and ap(articulation point) arrays 
	for (int i = 0; i < n; i++)
	{
		parent[i] = NIL;
		visited[i] = false;
		ap[i] = false;
	}

	// Call the recursive helper function to find articulation points 
	// in DFS tree rooted with vertex 'i' 
	for (int i = 0; i < n; i++)
		if (visited[i] == false)
			APUtil(i, visited, disc, low, parent, ap, _d);

	for (int i = 0; i < n; i++)
		Level.push_back(low[i]);

	// Now ap[] contains articulation points, print them 
	vector<int> result;
	for (int i = 0; i < n; i++)
		if (ap[i] == true)
			result.push_back(i);


	return result;
}