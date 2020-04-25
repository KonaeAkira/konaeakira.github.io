#include <bits/stdc++.h>

const int N = 10000;
const int M = 40000;

const long long INFLL = std::numeric_limits<long long>::max();

std::vector<std::pair<int, int>> adj[N];
long long dis_1[N], dis_2[N];
std::queue<int> queue;
std::bitset<N> in_queue;

void spfa()
{
	dis_1[0] = 0;
	queue.push(0);
	in_queue.set(0);
	while (!queue.empty())
	{
		int u = queue.front();
		queue.pop();
		in_queue.reset(u);
		for (auto [v, w] : adj[u])
			if (dis_1[u] + w < dis_1[v])
			{
				dis_1[v] = dis_1[u] + w;
				if (!in_queue.test(v))
				{
					queue.push(v);
					in_queue.set(v);
				}
			}
	}
}

int ind, cnt, num[N], low[N], scc[N];
std::stack<int> stack;
std::bitset<N> on_stack;
std::vector<int> order;

void tarjan(int u)
{
	num[u] = low[u] = ++ind;
	stack.push(u);
	on_stack.set(u);
	for (auto [v, w] : adj[u])
		if (num[v] == 0)
		{
			tarjan(v);
			low[u] = std::min(low[u], low[v]);
		}
		else if (on_stack.test(v))
			low[u] = std::min(low[u], num[v]);
	if (num[u] == low[u])
	{
		++cnt;
		int v;
		do
		{
			v = stack.top();
			stack.pop();
			on_stack.reset(v);
			order.push_back(v);
			scc[v] = u;	
		} while (v != u);
	}
}

void segmented_spfa()
{
	order.reserve(N);
	tarjan(0);
	std::cerr << "Number of SCCs: " << cnt << std::endl;
	dis_2[0] = 0;
	for (int i = N - 1; i >= 0; )
	{
		int root = scc[order[i]];
		for (; i >= 0 && scc[order[i]] == root; --i)
			if (dis_2[order[i]] != INFLL)
			{
				queue.push(order[i]);
				in_queue.set(order[i]);
			}
		while (!queue.empty())
		{
			int u = queue.front();
			queue.pop();
			in_queue.reset(u);
			for (auto [v, w] : adj[u])
				if (dis_2[u] + w < dis_2[v])
				{
					dis_2[v] = dis_2[u] + w;
					if (scc[v] == root && !in_queue.test(v))
					{
						queue.push(v);
						in_queue.set(v);
					}
				}
		}
	}
}

int main()
{
	for (int i = 0; i < M; ++i)
	{
		int u, v, w;
		std::cin >> u >> v >> w;
		adj[u].emplace_back(v, w);
	}
	
	std::fill(dis_1, dis_1 + N, INFLL);
	std::fill(dis_2, dis_2 + N, INFLL);
	
	auto begin_1 = std::chrono::steady_clock::now();
	//spfa();
	segmented_spfa();
	auto end_1 = std::chrono::steady_clock::now();
	std::cout << "Segmented: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_1 - begin_1).count() << "ms" << std::endl;	
	
	auto begin_2 = std::chrono::steady_clock::now();
	spfa();
	//segmented_spfa();
	auto end_2 = std::chrono::steady_clock::now();
	std::cout << "Vanilla: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_2 - begin_2).count() << "ms" << std::endl;
	
	for (int i = 0; i < N; ++i)
		assert(dis_1[i] == dis_2[i]);	
	
	return 0;
}
