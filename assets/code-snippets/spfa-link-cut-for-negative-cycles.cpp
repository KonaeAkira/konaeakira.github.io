#include <bits/stdc++.h>

const int MAX_N = 1000000;

// https://github.com/PetarV-/Algorithms/blob/master/Data%20Structures/Link-cut%20Tree.cpp
namespace lct
{
	/*
	 Petar 'PetarV' Velickovic
	 Data Structure: Link/cut Tree
	*/

	typedef long long lld;
	typedef unsigned long long llu;
	using namespace std;

	/*
	 The link/cut tree data structure enables us to efficiently handle a dynamic forest of trees.
	 It does so by storing a decomposition of the forest into "preferred paths", where a path is
	 preferred to another when it has been more recently accessed.
	 Each preferred path is stored in a splay tree which is keyed by depth.
	 
	 The tree supports the following operations:
		 - make_tree(v): create a singleton tree containing the node v
		 - find_root(v): find the root of the tree containing v
		 - link(v, w):   connect v to w
		                 (precondition: v is root of its own tree,
		                 and v and w are not in the same tree!)
		 - cut(v):       cut v off from its parent
		 - path(v):      access the path from the root of v's tree to v 
		                 (in order to e.g. perform an aggregate query on that path)
	 
	 More complex operations and queries are possible that require the data structure 
	 to be augmented with additional data. Here I will demonstrate the LCA(p, q)
	 (lowest common ancestor of p and q) operation.
	 
	 Complexity:    O(1) for make_tree
		            O(log n) amortized for all other operations
	*/

	int n, m;
	char cmd[101];
	int p, q;

	struct Node
	{
		int L, R, P;
		int PP;
	};

	Node LCT[MAX_N];

	inline void make_tree(int v)
	{
		if (v == -1) return;
		LCT[v].L = LCT[v].R = LCT[v].P = LCT[v].PP = -1;
	}

	inline void rotate(int v)
	{
		if (v == -1) return;
		if (LCT[v].P == -1) return;
		int p = LCT[v].P;
		int g = LCT[p].P;
		if (LCT[p].L == v)
		{
		    LCT[p].L = LCT[v].R;
		    if (LCT[v].R != -1)
		    {
		        LCT[LCT[v].R].P = p;
		    }
		    LCT[v].R = p;
		    LCT[p].P = v;
		}
		else
		{
		    LCT[p].R = LCT[v].L;
		    if (LCT[v].L != -1)
		    {
		        LCT[LCT[v].L].P = p;
		    }
		    LCT[v].L = p;
		    LCT[p].P = v;
		}
		LCT[v].P = g;
		if (g != -1)
		{
		    if (LCT[g].L == p)
		    {
		        LCT[g].L = v;
		    }
		    else
		    {
		        LCT[g].R = v;
		    }
		}
		// must preserve path-pointer!
		// (this only has an effect when g is -1)
		LCT[v].PP = LCT[p].PP;
		LCT[p].PP = -1;
	}

	inline void splay(int v)
	{
		if (v == -1) return;
		while (LCT[v].P != -1)
		{
		    int p = LCT[v].P;
		    int g = LCT[p].P;
		    if (g == -1) // zig
		    {
		        rotate(v);
		    }
		    else if ((LCT[p].L == v) == (LCT[g].L == p)) // zig-zig
		    {
		        rotate(p);
		        rotate(v);
		    }
		    else // zig-zag
		    {
		        rotate(v);
		        rotate(v);
		    }
		}
	}

	inline void expose(int v)
	{
		if (v == -1) return;
		splay(v); // now v is root of its aux. tree
		if (LCT[v].R != -1)
		{
		    LCT[LCT[v].R].PP = v;
		    LCT[LCT[v].R].P = -1;
		    LCT[v].R = -1;
		}
		while (LCT[v].PP != -1)
		{
		    int w = LCT[v].PP;
		    splay(w);
		    if (LCT[w].R != -1)
		    {
		        LCT[LCT[w].R].PP = w;
		        LCT[LCT[w].R].P = -1;
		    }
		    LCT[w].R = v;
		    LCT[v].P = w;
		    LCT[v].PP = -1;
		    splay(v);
		}
	}

	inline int find_root(int v)
	{
		if (v == -1) return -1;
		expose(v);
		int ret = v;
		while (LCT[ret].L != -1) ret = LCT[ret].L;
		expose(ret);
		return ret;
	}

	inline void link(int v, int w) // attach v's root to w
	{
		if (v == -1 || w == -1) return;
		expose(v); 
		expose(w);
		LCT[v].L = w; // the root can only have right children in its splay tree, so no need to check
		LCT[w].P = v;
		LCT[w].PP = -1;
	}

	inline void cut(int v)
	{
		if (v == -1) return;
		expose(v);
		if (LCT[v].L != -1)
		{
		    LCT[LCT[v].L].P = -1;
		    LCT[v].L = -1;
		}
	}

	inline int LCA(int p, int q)
	{
		expose(p);
		splay(q);
		if (LCT[q].R != -1)
		{
		    LCT[LCT[q].R].PP = q;
		    LCT[LCT[q].R].P = -1;
		    LCT[q].R = -1;
		}
		
		int ret = q, t = q;
		while (LCT[t].PP != -1)
		{
		    int w = LCT[t].PP;
		    splay(w);
		    if (LCT[w].PP == -1) ret = w;
		    if (LCT[w].R != -1)
		    {
		        LCT[LCT[w].R].PP = w;
		        LCT[LCT[w].R].P = -1;
		    }
		    LCT[w].R = t;
		    LCT[t].P = w;
		    LCT[t].PP = -1;
		    t = w;
		}
		splay(q);
		
		return ret;
	}
}

const long long INF = std::numeric_limits<long long>::max() / 4;

struct edge { int u, v, w; };

int t, n, m;
std::vector<std::pair<int, int>> adj[MAX_N];

long long dis[MAX_N];
int pre[MAX_N], len[MAX_N], length;
bool in_queue[MAX_N];

bool spfa_link_cut()
{
	std::fill(dis, dis + n, 0);
	std::fill(pre, pre + n, -1);
	std::fill(in_queue, in_queue + n, true);
	std::queue<int> queue;
	for (int i = 0; i < n; ++i)
	{
		queue.push(i);
		lct::make_tree(i);
	}
	while (!queue.empty())
	{
		int u = queue.front();
		queue.pop();
		in_queue[u] = false;
		for (auto [v, w] : adj[u])
			if (dis[u] + w < dis[v])
			{
				dis[v] = dis[u] + w;
				if (pre[v] != -1)
					lct::cut(v);
				if (lct::find_root(u) == v)
				{
					for (int i = u; i != v; i = pre[i])
						++length;
					return true;
				}
				pre[v] = u;
				lct::link(v, u);
				if (!in_queue[v])
				{
					queue.push(v);
					in_queue[v] = true;
				}
			}
	}
	return false;
}

bool spfa_length_array()
{
	std::fill(dis, dis + n, 0);
	std::fill(len, len + n, 0);
	std::fill(in_queue, in_queue + n, true);
	std::queue<int> queue;
	for (int i = 0; i < n; ++i)
		queue.push(i);
	while (!queue.empty())
	{
		int u = queue.front();
		queue.pop();
		in_queue[u] = false;
		for (auto [v, w] : adj[u])
			if (dis[u] + w < dis[v])
			{
				dis[v] = dis[u] + w;
				len[v] = len[u] + 1;
				if (len[v] == n)
					return true;
				if (!in_queue[v])
				{
					queue.push(v);
					in_queue[v] = true;
				}
			}
	}
	return false;
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL); std::cout.tie(NULL);
	
	std::cin >> n >> m;
	for (int i = 0; i < m; ++i)
	{
		int u, v, w;
		std::cin >> u >> v >> w;
		adj[u].push_back({v, w});
	}
	
	auto begin_1 = std::chrono::steady_clock::now();
	bool f1 = spfa_link_cut();
	auto end_1 = std::chrono::steady_clock::now();
	std::cout << "Link/cut: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_1 - begin_1).count() << "ms" << std::endl;	
	
	auto begin_2 = std::chrono::steady_clock::now();
	bool f2 = spfa_length_array();
	auto end_2 = std::chrono::steady_clock::now();
	std::cout << "Length array: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_2 - begin_2).count() << "ms" << std::endl;
	
	assert(f1 == f2);
	if (f1) std::cout << "CYCLE LENGTH = " << length + 1 << "\n";
	else std::cout << "NO CYCLE\n";
	
	std::cout << '\n';
	
	return 0;
}
