#pragma once 

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include <random>
#include <stack>
#include <list>
#include <set>
#include <map>
using namespace std;
namespace GraphCycles
{
	// The implementation of the HalfAdjacencyMatrix can only tell if two nodes i and j are connected or not. 
	class HalfAdjacencyMatrix
	{
	public:
		// constructs the matrix given the total number of nodes.		
		// 1. As the adjacency Matrix is symmetrical it is sufficient for our purpose to store only one half of the matrix.
		// 2. The diagonal elements do not vanish if a node is connected to itself. In our case this is meaningless and can be neglected.
		// => our "HalfAdjacencyMatrix"" stores (nNodes * (nNodes - 1)) / 2 elements
		inline HalfAdjacencyMatrix(const size_t nNodes)
			: m_aBits((nNodes* (nNodes - 1)) / 2)
			, m_nNodes(nNodes)
			, m_nEdges(0)
			, m_nIndexFactor(1 + 2 * (nNodes - 2))
		{ }

		// copy-constructor
		inline HalfAdjacencyMatrix(const HalfAdjacencyMatrix& m)
			: m_aBits(m.m_aBits)
			, m_nNodes(m.m_nNodes)
			, m_nEdges(m.m_nEdges)
			, m_nIndexFactor(m.m_nIndexFactor)
		{ }

		// Connects the two objects i and j. Does nothing if the two are already connected.
		inline void connect(size_t i, size_t j)
		{
			// Ensure that nothing happens if the bit is already set! 
			if (m_aBits[index(i, j)])
				return;

			// Set the bit at the specified position and increase the number of total edges
			m_aBits[index(i, j)] = true;
			++m_nEdges;
		}

		// Disconnects the two objects i and j.
		inline void disconnect(size_t i, size_t j)
		{
			// Ensure that nothing happens if the bit is already unset!
			if (!m_aBits[index(i, j)])
				return;

			// Unset the bit!
			m_aBits[index(i, j)] = false;
			--m_nEdges;
		}

		// Returns true, if the two elements i and j are connected and false otherwise.
		// Additionally returns false if the two indices are equal!
		inline bool isConnected(size_t i, size_t j) const
		{
			if (i == j)
				return false;
			return m_aBits[index(i, j)];
		}
		inline bool operator()(size_t i, size_t j) const { return isConnected(i, j); }

		// Returns the total number of edges
		inline size_t getNumEdges() const { return m_nEdges; }

		// performs a xor operation on the two matrices and returns a new one.
		inline HalfAdjacencyMatrix operator^(const HalfAdjacencyMatrix& rhs) const
		{
			if (m_nNodes != rhs.m_nNodes)
				throw std::runtime_error("HalfAdjacencyMatrix::operator^(): The two matrices MUST be of the same size!");

			HalfAdjacencyMatrix result(m_nNodes);
			for (size_t i = 0; i < m_aBits.size(); ++i)
			{
				// XOR for each bit: If the bit is true for any of the two matrices AND the bits in both matrices are not equal
				// the bit is again true in the result matrix. 
				if ((m_aBits[i] || rhs.m_aBits[i]) && (m_aBits[i] != rhs.m_aBits[i]))
				{
					result.m_aBits[i] = 1;
					++result.m_nEdges;
				}
			}
			return result;
		}

		inline HalfAdjacencyMatrix& operator^= (const HalfAdjacencyMatrix& rhs)
		{
			if (m_nNodes != rhs.m_nNodes)
				throw std::runtime_error("HalfAdjacencyMatrix::operator^=(): The two matrices MUST be of the same size!");

			m_nEdges = 0;
			for (size_t i = 0; i < m_aBits.size(); ++i)
			{
				// XOR for each bit: If the bit is true for any of the two matrices AND the bits in both matrices are not equal
				// the bit is again true in the result matrix.				 
				if ((m_aBits[i] || rhs.m_aBits[i]) && (m_aBits[i] != rhs.m_aBits[i]))
				{
					m_aBits[i] = 1;
					++m_nEdges;
				}
				else
					m_aBits[i] = 0;
			}
			return *this;
		}

		// assign operator
		inline HalfAdjacencyMatrix& operator=(const HalfAdjacencyMatrix& rhs)
		{
			// if the number of nodes are not equal, abort!
			if (m_nNodes != rhs.m_nNodes)
				throw std::runtime_error("HalfAdjacencyMatrix::operator=(): Number of nodes are not equal!");

			m_aBits = rhs.m_aBits;
			m_nEdges = rhs.m_nEdges;
			return *this;
		}

		// Writes the matrix to a stream:
		void dump(std::ostream& os) const
		{
			os << m_nNodes << "\n";
			for (size_t i = 0; i < m_nNodes; ++i)
			{
				os << i << ": ";
				for (size_t j = 0; j < i; ++j)
					os << m_aBits[index(i, j)] << " ";
				os << "\n";
			}
		}

	private:
		// Provides the correct 1D index for m_aBits:
		inline size_t index(const size_t i, const size_t j) const
		{
			if (i < m_nNodes && j < m_nNodes && i != j)
			{
				// Formel: (j-i*(i-1-2*(N-2))/2)-1 WENN i<j!
				// Attention; for the calculation the values become negative which is not allowed for size_t.
				// However, the result of the calculation will always be positive!
				long long li = i, lj = j;
				if (i < j)
					return (lj - li * (li - m_nIndexFactor) / 2) - 1;
				else
					// swap the two indices!
					return (li - lj * (lj - m_nIndexFactor) / 2) - 1;
			}
			else
				throw std::out_of_range("HalfAdjacencyMatrix::index(i,j): The two indices must both be smaller than the number of total nodes AND they must not be equal!");
		}

		// All the bits:
		std::vector<bool> m_aBits;
		// number of total connections 
		size_t m_nEdges;
		// total number of Nodes:
		const size_t m_nNodes;
		// factor used for index
		const long long m_nIndexFactor;
	};

	template<class TObject>
	class Graph
	{
	public:
		typedef std::vector<TObject> NodeArray;
		typedef HalfAdjacencyMatrix CycleMatrix;
		typedef std::vector<CycleMatrix> CycleArray;
		typedef std::list<const TObject*> NodePath;

		typedef bool(*ConnectionFunc)(const TObject&, const TObject&, const void*);
		inline Graph(const NodeArray& aNodes, ConnectionFunc pfIsConnected, const void* pUserData = nullptr)
			: m_aNodes(aNodes)
			, m_adjMat(aNodes.size())
		{
			for (size_t i = 0; i < m_aNodes.size(); ++i)
			{
				for (size_t j = 0; j < i; ++j)
				{
					if ((*pfIsConnected)(m_aNodes[i], m_aNodes[j], pUserData))
						m_adjMat.connect(i, j);
				}
			}
		}

		inline Graph(const TObject* paNodes, const size_t nNodes, const size_t* paEdges, const size_t nEdges)
			: m_adjMat(nNodes)
		{
			for (size_t i = 0; i < nNodes; ++i)
				m_aNodes.push_back(paNodes[i]);
			for (size_t i = 0; i < nEdges; ++i)
				m_adjMat.connect(paEdges[2 * i], paEdges[2 * i + 1]);
		}

		void computeFundamentalCycles()
		{
			// Already done? 
			if (!m_aFundamentalCycles.empty())
				return;

			std::unique_ptr<TreeNode[]> aTree(new TreeNode[m_aNodes.size()]);
			std::stack<size_t> nodeStack;

			// Start arbitrarily with the first Node!
			nodeStack.push(0);

			// Copy the adjacency matrix as it will be necessary to remove edges!
			HalfAdjacencyMatrix adjMat = m_adjMat;

			// At the beginning, all tree nodes point to itself as parent!
			for (size_t i = 0; i < m_aNodes.size(); ++i)
			{
				aTree[i].parent = &aTree[i];
				aTree[i].index = i;
			}

			// Loop until all nodes are removed from the stack!
			while (nodeStack.size() > 0)
			{
				// Next node index: 
				size_t currentNodeIndex = nodeStack.top();
				nodeStack.pop();
				TreeNode& currentTreeNode = aTree[currentNodeIndex];

				// Iterate though all edges connecting this node:
				for (size_t j = 0; j < m_aNodes.size(); ++j)
				{
					// not connected?
					if (!adjMat.isConnected(currentNodeIndex, j))
						continue;

					// Is the foreign node already in the tree?
					// This is the case, if the parent element of the TreeNode does not point to itself!
					if (aTree[j].parent != &aTree[j])
					{
						// Fundamental Cycle found!
						// Get unique paths from both nodes within the spanning tree!
						HalfAdjacencyMatrix pi(m_aNodes.size()), pj(m_aNodes.size());
						unique_tree_path(&aTree[currentNodeIndex], pi);
						unique_tree_path(&aTree[j], pj);

						// also the connection between currentNodeIndex and j has to be inserted to ONE of the two paths (which one does not matter)
						pi.connect(currentNodeIndex, j);

						// combine the two matrices with XOR (^) to obtain the fundamental cycle. 
						// XOR is necessary to exclude double root paths.
						m_aFundamentalCycles.push_back(pi ^ pj);

					}
					else
					{
						// The foreign node is not contained in the tree yet; add it now!
						aTree[j].parent = &currentTreeNode;
						// add the node to the search stack!
						nodeStack.push(j);
					}
					// Ether way remove this connection!
					adjMat.disconnect(currentNodeIndex, j);
				}
			}
		}

		// Iterates all cycles of the graph (Input Iterator)
		class CycleIterator
		{
		public:
			inline CycleIterator(Graph* g)
				: m_pGraph(g)
				, m_r(1)
				, m_currentCycle(g ? g->getNumNodes() : 0)
			{
				if (m_pGraph)
				{
					// ensure that the fundamental cycles are computed
					g->computeFundamentalCycles();
					// if there is just one fundamental cycle, then there are no cycles to iterate at all!
					if (g->getFundamentalCycles().size() >= 2)
					{
						m_bitset.insert(m_bitset.end(), g->getFundamentalCycles().size(), 0);
						// initialze the bitset, first element will be 1; bitset contains 10000..000 (a total of g->getFundamentalCycles().size() digits)
						// with this bitset first the fundamental cycles will be enumerated, then the combined cycles.
						fill(m_bitset.begin(), m_bitset.begin() + 1, 1);
						next();
					}
					else
						m_pGraph = nullptr;
				}
			}

			inline CycleIterator(const CycleIterator& rhs)
				: m_pGraph(rhs.m_pGraph)
				, m_bitset(rhs.m_bitset)
				, m_r(rhs.m_r)
				, m_currentCycle(rhs.m_currentCycle)
			{ }

			static inline CycleIterator End() { return CycleIterator(nullptr); }

			inline CycleIterator& operator=(const CycleIterator& rhs)
			{
				m_pGraph = rhs.m_pGraph;
				m_bitset = rhs.m_bitset;
				m_r = rhs.m_r;
			}

			inline bool operator==(const CycleIterator& rhs)
			{
				// equal graphs?
				if (m_pGraph != rhs.m_pGraph)
					return false;
				// sufficient to compare the bitsets
				return std::equal(m_bitset.begin(), m_bitset.end(), rhs.m_bitset.begin());
			}
			inline bool operator!=(const CycleIterator& rhs) { return !(*this == rhs); }

			inline const CycleMatrix& operator*() { return m_currentCycle; }
			inline const CycleMatrix* operator->() { return &m_currentCycle; }

			// prefix increment
			inline CycleIterator& operator++() { next(); return *this; }
			// postfix increment
			inline CycleIterator operator++(int)
			{
				CycleIterator tmp(*this);
				++* this;
				return tmp;
			}

		private:
			// does all the work:
			void next()
			{
				// safety...
				if (!m_pGraph)
					return;

				// get all fundamental cycles of the graph:
				const CycleArray& aFundamentalCycles = m_pGraph->getFundamentalCycles();

				// loop through cycles until a valid one is found!
				bool bValid = false;
				do
				{
					// Result:
					CycleMatrix M(m_pGraph->getNumNodes());

					// Generate the cycle for the current permutation:
					size_t nEdges = 0;
					for (size_t i = 0; i < aFundamentalCycles.size(); ++i)
						if (m_bitset[i])
						{
							M ^= aFundamentalCycles[i];
							nEdges += aFundamentalCycles[i].getNumEdges();
						}

					// validate the matrix:
					if (m_r == 2)
					{
						if (nEdges > M.getNumEdges())
							bValid = true;
					}
					else if (m_pGraph->validateCycleMatrix(M))
						bValid = true;

					// if the cycle is valid, copy it!
					if (bValid)
						m_currentCycle = M;

					// was this the last one?
					if (m_r > aFundamentalCycles.size())
					{
						m_pGraph = nullptr;			// the end is reached, all cycles have been iterated!
						m_bitset.clear();
						return;
					}

					// next permutation:
					// (as the ones are placed in the beginning of the bitstring, we have to use prev_permutation instead of next_permutation)
					if (!std::prev_permutation(m_bitset.begin(), m_bitset.end()))
					{
						// the number of r needs to be increased!
						++m_r;

						// Fill the bitstring with r times 1 and N-r times 0
						if (m_r <= aFundamentalCycles.size())
						{
							std::fill_n(m_bitset.begin(), m_r, 1);
							std::fill_n(m_bitset.rbegin(), aFundamentalCycles.size() - m_r, 0);
						}
					}

				} while (!bValid); // loop until a valid cycle was found
			}

			// Associated Graph:
			Graph* m_pGraph;
			// Bitset with the current permutation:
			std::vector<bool> m_bitset;
			// Current number of 1ns in the bitset
			size_t m_r;
			// Current Cycle:
			CycleMatrix m_currentCycle;
		};

		// Exhaustive!!!
		vector<vector<TObject>> computeAllCycles()
		{
			// if the fundamental cycles are not determined yet do it now!
			if (m_aFundamentalCycles.empty())
				computeFundamentalCycles();

			// all fundamental cycles also are cycles...
			m_aCycles = m_aFundamentalCycles;

			// Necessary for the combinatorics:
			std::vector<bool> v(m_aFundamentalCycles.size());
			// Combine each fundamental cycle with any other. 
			// attention: not only pairing (M_i ^ M_j) is relevant but also all other tuples (M_i ^ M_j ^ ... ^ M_N)! quite exhausting...
			// This requires combinatorics...
			// we pick r cycles from all fundamental cycles; starting with 2 cycles (pairs)
			for (size_t r = 2; r <= m_aFundamentalCycles.size(); ++r)
			{
				// Fill the bitstring with r times true and N-r times 0.
				std::fill_n(v.begin(), r, 1);
				std::fill_n(v.rbegin(), v.size() - r, 0);

				// Iterate through all combinations how r elements can be picked from N total cycles
				do
				{
					CycleMatrix M(m_aNodes.size());
					size_t nEdges = 0;
					for (size_t i = 0; i < m_aFundamentalCycles.size(); ++i)
						if (v[i])
						{							
							M ^= m_aFundamentalCycles[i];
							nEdges += m_aFundamentalCycles[i].getNumEdges();
						}				

					// now add the new combined cycle to the list!
					// IF it is valid, i.e. forms one connected cycle and not two (or more) distinct ones.
					// This is quite easy to determine when combining just two fundamental cycles.
					if (r == 2)
					{
						// When at least one edge was deleted from the adjacency matrix then the two fundamental cycles form one connected cycle 
						// as they shared at least one edge.
						if (nEdges > M.getNumEdges())
						{
							m_aCycles.push_back(M);
						}
					}
					else
					{
						// Here we have combined more than two cycles...
						// In principle one could keep track if two pairs are valid and then transfer this knowledge onto higher tuples.
						// However, this again exhausting; even for triples

						// We will use our knowledge on the cycle matrices we are using: We know that all nodes in the matrix which belong to the cycle have exactly 2 edges.
						// when we now start a deep search from any node in the matrix and counting the path length to the starting node this length must be equal to the 
						// total number of edges!
						// Again this is exhaustive but it is a very simple approach validating the cycles
						try {
							if (validateCycleMatrix(M))
							{
								m_aCycles.push_back(M);
							}
						}
						catch (std::runtime_error& e)
						{
							// Here is now the chance to debug wrong cycle matrices...
							std::cerr << "Error while validating the cycle matrix!\n"
								<< e.what() << std::endl;
							for (size_t i = 0; i < m_aFundamentalCycles.size(); ++i)
								if (v[i])
									m_aFundamentalCycles[i].dump(std::cerr);

							M.dump(std::cerr);

							throw;
						}
					}

					// is the matrix relevant, i.e. is the matrix forming ONE connected cycle!
				} while (std::prev_permutation(v.begin(), v.end()));
			}

			vector<vector<TObject>> result;
			for (const CycleMatrix& cycleMatrix : m_aCycles)
			{
				vector<TObject> temp;
				NodePath path = cycleMatrix2nodePath(cycleMatrix);
				for (const TObject* obj : path)
				{
					temp.push_back(*obj);
				}
				result.push_back(temp);
			}
			return result;
		}

		NodePath cycleMatrix2nodePath(const CycleMatrix& m) const
		{
			// Here we have to perform a deep search mechanism to transform the CycleMatrix (which contains only indices of the graph)
			// to a path which actually contains pointers to the objects itself
			// The search algorithm can be simplified using our knowledge on the CycleMatrix: 
			// Each and every node in the CycleMatrix will contain 0 or 2 edges and when we go through the path we will always return at the starting point.

			// We will determine the path by first finding any edge in the matrix and then follow the path through the matrix recursively.
			// As soon as we have found the starting object the function returns.

			NodePath path;
			// Find any edge in the matrix:
			for (size_t i = 0; i < m_aNodes.size(); ++i)
			{
				for (size_t j = 0; j < m_aNodes.size(); ++j)
				{
					if (m.isConnected(i, j))
					{
						// Add the Object i AND j to the path and start the recursion with object j!
						path.push_back(&m_aNodes[i]);
						path.push_back(&m_aNodes[j]);
						cycleMatrix2nodePath_recursion(m, path, j, i, i);

						// Return the result!
						return path;
					}
				}
			}
			// When we are here, the matrix does not contain any edges!
			throw std::runtime_error("Graph::cycleMatrix2nodePath(): Given Cycle Matrix does not contain any edges!");
		}

		// Returns all computed fundamental cycles
		inline const CycleArray& getFundamentalCycles() const { return m_aFundamentalCycles; }
		// Returns the number of nodes:
		inline size_t getNumNodes() const { return m_aNodes.size(); }

	private:
		// i: The node which has to be investigated
		// previousNode: The node which was investigated before node i; necessary to forbid going backwards
		// startNode: The node which was investigated first; necessary to determine when the recursion can be stopped
		void cycleMatrix2nodePath_recursion(const CycleMatrix& m, NodePath& path, const size_t i, const size_t previousNode, const size_t startingNode) const
		{
			// Find the next connection of the given node, not going back
			for (size_t j = 0; j < m_aNodes.size(); ++j)
			{
				// Are the two elements connected AND is the new element not the previous node?
				if (m.isConnected(i, j) && j != previousNode)
				{
					// Add the Object j to the path! 
					path.push_back(&m_aNodes[j]);
					// The starting point is not reached yet?
					if (j != startingNode)
						cycleMatrix2nodePath_recursion(m, path, j, i, startingNode);
					return; 	// We are done!
				}
			}
			// When we are here, we have found a dead end!
			throw std::runtime_error("Graph::cycleMatrix2nodePath_recursion(): Found a dead end!");
		}

	public:
		void dump(std::ostream& os)
		{
			// Print basic information about the graph
			os << "Graph contains " << m_aNodes.size() << " Nodes and " << m_adjMat.getNumEdges() << " Edges\n";

			// print cycles:
			os << "Printing " << m_aFundamentalCycles.size() << " fundamental cycles:\n";
			for (const CycleMatrix& cycleMatrix : m_aFundamentalCycles)
			{
				bool bFirst = true;
				NodePath path = cycleMatrix2nodePath(cycleMatrix);
				for (const TObject* obj : path)
				{
					if (bFirst)
						os << *obj;
					else
						os << "-" << *obj;
					bFirst = false;
				}
				os << "\n";
			}

			os << "Printing ALL " << m_aCycles.size() << " cycles:\n";
			for (const CycleMatrix& cycleMatrix : m_aCycles)
			{
				// cycleMatrix.dump(os);
				bool bFirst = true;
				NodePath path = cycleMatrix2nodePath(cycleMatrix);
				for (const TObject* obj : path)
				{
					if (bFirst)
						os << *obj;
					else
						os << "-" << *obj;
					bFirst = false;
				}
				os << "\n";
				//cycleMatrix.dump(os);
			}
		}

	private:
		NodeArray 				m_aNodes;
		HalfAdjacencyMatrix 	m_adjMat;
		CycleArray 				m_aFundamentalCycles;
		CycleArray 				m_aCycles;

		struct TreeNode
		{
			size_t index;
			TreeNode* parent;
		};
		// Function recursively finds the unique path within the tree from the given node to the root of the tree
		inline void unique_tree_path(TreeNode* pNode, HalfAdjacencyMatrix& adjMat)
		{
			if (pNode->parent != pNode)
			{
				adjMat.connect(pNode->index, pNode->parent->index);
				unique_tree_path(pNode->parent, adjMat);
			}
		}

		// Function validates a cycle matrix:
		bool validateCycleMatrix(const CycleMatrix& m)
		{
			// We will use our knowledge on the cycle matrices we are using: We know that all nodes in the matrix which belong to the cycle have exactly 2 edges.
			// when we now start a deep search from any node in the matrix and counting the path length to the starting node this length must be equal to the 
			// total number of edges
			// Again this is exhaustive but it is a very simple approach validating the cycles

			size_t pathLength = 0;
			// Find any edge in the matrix:
			for (size_t i = 0; i < m_aNodes.size(); ++i)
			{
				for (size_t j = 0; j < m_aNodes.size(); ++j)
				{
					if (m.isConnected(i, j))
					{
						// Increment the pathLength and start the recursion
						++pathLength;
						std::set<size_t> aVisited;
						aVisited.insert(i);
						validateCycleMatrix_recursion(m, pathLength, j, i, aVisited);

						// If the result equals the number of edges than the matrix is valid!
						// Version 3:
						//		- From the recursion, the path length will not account for the last edge connecting the starting node
						//		  with the last node from the recursion.
						return pathLength + 1 == m.getNumEdges();
					}
				}
			}
			// When we are here, the matrix does not contain any edges!
			throw std::runtime_error("Graph::validateCycleMatrix(): Given Cycle Matrix does not contain any edges!");
		}
		// i: The node which has to be investigated
		// previousNode: The node which was investigated before node i; 
		// startNode: The node which was investigated first; necessary to determine when the recursion can be stopped
		void validateCycleMatrix_recursion(const CycleMatrix& m, size_t& pathLength, const size_t i, size_t previousNode, std::set<size_t>& aVisited) const
		{
			// The path length is also a measure for the recursion steps. 
			// If the recursion takes too long, we abort it and throw an error message. 
			// If you expect cycles which are longer than 500 edges, you have to increase this number.
			// Also note that there is a limit of maximal recursion levels which cannot be exceeded. 
			// If your cycles exceed that maximum length you will have to come up with another validation method.
			if (pathLength > 500)
				throw std::runtime_error("Graph::validateCycleMatrix_recursion(): Maximum recursion level reached.");

			// Find the next connection of the given node, not going back
			for (size_t j = 0; j < m_aNodes.size(); ++j)
			{
				// Are the two elements connected? Exclude the path back (otherwise the return condition might be falsely triggered)
				if (m.isConnected(i, j) && j != previousNode)
				{
					// Was this node not visisted before?
					auto ppVisited = aVisited.find(j);
					if (ppVisited != aVisited.end())
					{
						// This node was already visited, therefore we are done here!
						// In a cycle each node just appears once. As soon as we meet any node which was already visited
						// one cycle is closed and the CycleMatrix either contains one contiguous cycle or many disconnected cycles.
						return;
					}

					// This node was not visited yet, increment the path length and insert this node to the visited list:
					++pathLength;
					aVisited.insert(i);

					// Call the next recursion:
					validateCycleMatrix_recursion(m, pathLength, j, i, aVisited);
					return;
				}
			}
			// When we are here, we have found a dead end!
			throw std::runtime_error("Graph::validateCycleMatrix_recursion(): Found a dead end!");
		}
	};

}


