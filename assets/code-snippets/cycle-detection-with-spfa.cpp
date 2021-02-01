#include <bits/stdc++.h>

const int MAX_N = 10000;
const long long INF = std::numeric_limits<long long>::max() / 4;

int t, n, m;
std::vector<std::pair<int, int>> adj[MAX_N];

long long dis[MAX_N];
int pre[MAX_N], len[MAX_N];
bool in_queue[MAX_N];

bool visited[MAX_N], on_stack[MAX_N];

bool detect_cycle()
{
    std::vector<int> vec;
    std::fill(on_stack, on_stack + n, false);
    std::fill(visited, visited + n, false);
    for (int i = 0; i < n; ++i)
        if (!visited[i])
        {
            for (int j = i; j != -1; j = pre[j])
                if (!visited[j])
                {
                    visited[j] = true;
                    vec.push_back(j);
                    on_stack[j] = true;
                }
                else
                {
                    if (on_stack[j])
                        return true;
                    break;
                }
            for (int j : vec)
                on_stack[j] = false;
            vec.clear();
        }
    return false;
}

bool spfa_early_terminate()
{
	std::fill(dis, dis + n, 0);
	std::fill(pre, pre + n, -1);
	std::fill(in_queue, in_queue + n, true);
	std::queue<int> queue;
	for (int i = 0; i < n; ++i)
		queue.push(i);
    int iter = 0;
	while (!queue.empty())
	{
		int u = queue.front();
		queue.pop();
		in_queue[u] = false;
		for (auto [v, w] : adj[u])
			if (dis[u] + w < dis[v])
			{
				pre[v] = u;
				dis[v] = dis[u] + w;
				if (++iter == n)
                {
                    iter = 0;
                    if (detect_cycle())
                        return true;
                }
				if (!in_queue[v])
				{
					queue.push(v);
					in_queue[v] = true;
				}
			}
	}
    if (detect_cycle())
        return true;
	return false;
}

bool spfa_path_length()
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
		adj[u].emplace_back(v, w);
	}
	
	auto begin_1 = std::chrono::steady_clock::now();
	bool f1 = spfa_early_terminate();
	auto end_1 = std::chrono::steady_clock::now();
	std::cout << "Early terminate: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_1 - begin_1).count() << "ms" << std::endl;	
	
	auto begin_2 = std::chrono::steady_clock::now();
	bool f2 = spfa_path_length();
	auto end_2 = std::chrono::steady_clock::now();
	std::cout << "Path length: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_2 - begin_2).count() << "ms" << std::endl;
	
	assert(f1 == f2);
	
	return 0;
}
