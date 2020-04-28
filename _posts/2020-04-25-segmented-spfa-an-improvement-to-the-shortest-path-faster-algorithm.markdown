---
layout: post
title:  "Segmented SPFA: An improvement to the Shortest Path Faster Algorithm."
description: "An algorithm which gives a significant speedup over the Shortest Path Faster Algorithm for graphs with many Strongly Connected Components."
date:   2020-04-25
---

<script type="text/javascript" src="https://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"></script>

### Summary

The Shortest Path Faster Algorithm (SPFA) is an improvement of the Bellman-Ford algorithm. It computes the shortest paths from a single source vertex to all other vertices in a weighted directed graph. The SPFA has a worst-case runtime equal to that of the Bellman-Ford algorithm, but in practice it has been observed to be much faster.

In this post I will present the Segmented Shortest Path Faster Algorithm, which gives a significant speedup to the SPFA on 'hard' graphs with many Strongly Connected Components (SCC).

### The algorithm

Given a directed weighted graph with no negative cycles, the algorithm starts by partitioning the graph into SCCs. This forms a Directed Acyclic Graph (DAG) of SCCs. Then the algorithm uses the SPFA to find shortest paths in each SCC in topological order. In each iteration, the algorithm also relaxes vertices outside of the current SCC, but doesn't add those vertices to the queue yet.

The SPFA can be slow in case a vertex is relaxed with a non-optimal value and it is used to relax other vertices. Then a second relaxtion has to 'catch up' and undo what the first relaxation propagated. The motivation for segmenting the graph is to limit 'catching up' to within the same SCC.

Below is the pseudocode for the Segmented SPFA. Here `Tarjan(G, s)` is Tarjan's algorithm for finding SCCs.

```
procedure SegmentedSPFA(G, s):
    TopoOrder = Tarjan(G, s)
    for v in V(G) \ {s}:
        d[v] = inf
    d[s] = 0
    for SCC in TopoOrder:
        for v in SCC:
            if d[v] != inf:
                Queue.push(v)
        while !Queue.is_empty():
            u = Queue.pop()
            for (u, v) in E(G):
                if d[u] + w(u, v) < d[v]:
                    d[v] = d[i] + w(u, v)
                    if SCC.contains(v) and !Queue.contains(v):
                        Queue.push(v)
```

The runtime of the Segmented SPFA is the time it takes to find SCCs and order topologically (can be done in 1 pass of Tarjan's algorithm), and running the SPFA for each of the SCCs. In the worst case (when there is only 1 SCC), the complexity can be $$\mathcal{O}(VE)$$, which gives no improvement over the SFPA.

### Proof of correctness

We can prove the correctness of the algorithm via induction.

Let $$C_i$$ ($$i \geq 2$$) be the component that the algorithm is currently working on. Note that $$C_i$$ has to be reachable from the source, otherwise we can ignore this component. We assume that for all $$j \in \{1, ..., i - 1\}$$ the algorithm has already computed the correct shortest path to vertices in $$C_j$$. Then there must be at least $$1$$ vertex in $$C_i$$ for which the correct shortest path has been computed. Let's call this vertex $$s_i$$.

_Proof:_ Let $$x$$ be an arbitrary vertex in $$C_i$$. Let $$p(x)$$ be a a vertex so that it is a direct predecessor of $$x$$ for at least one shortest path from the source to $$x$$. Because of the topological ordering, $$p(x)$$ is either in $$C_i$$ or in some $$C_j$$ with $$j \lt i$$. Also, there cannot exist a cycle of the form $$\{x, p(x), p(p(x)), ..., x\}$$, because this implies that there is a negative cycle. We cannot add $$\|C_i\|$$ directed edges contained in $$C_i$$ without creating a cycle, thus at least one edge must point out of $$C_i$$, i.e. there exists a vertex $$x \in C_i$$ so that $$p(x) \in C_j$$ with $$j \lt i$$.

Because the shortest path to $$s_i$$ is finite, it is added to the queue for $$C_i$$. The Shortest Path Faster Algorithm then takes care of the rest and computes the correct shortest paths for all vertices in $$C_i$$ (It only needs one vertex with a correct shortest path to be in the queue. Other vertices slow down the algorithm by propagating suboptimal paths, but do not affect the correctness of the algorithm). It also relaxes edges going out from $$C_i$$, completing the precondition for $$C_{i+1}$$.

The base case is trivial, because the source is part of $$C_1$$. And the shortest path to the source is $$0$$, otherwise we can show that there exists a negative cycle.

### Benchmarks

For randomly generated graphs, the SPFA is expected to run in $$\mathcal{O}(E)$$ (unproven), so the Segmented SPFA gives no significant runtime improvement and can even be slower because it has to find all SCCs in the graph.

However, for graphs for which the SPFA takes longer ([zhougelin][hard-graphs] shows us one way to construct such a graph), the runtime is dominated by the SPFA and we can really observe the speedup that the Segmented version gives. I will call such graphs 'hard' graphs.

You can download the [C++ code][benchmark-code] I used to assess the performance of both algorithms. For each entry I used a sample size of 100.

First I tested both algorithms on **easy graphs**, with varying amounts of SCCs. These are connected graphs of 100000 vertices and 400000 edges.

Algorithm | 1 SCC | 5 SCCs | 20 SCCs | 100 SCCs | 500 SCCs
Segmented | 41 ms | 42 ms | 41 ms | 40 ms | 39 ms
Vanilla | 17 ms | 19 ms | 18 ms | 13 ms | 11 ms

Unsurprisingly, the Segmented version is slower in all cases, because it computes all SCCs, whereas the vanilla version just zooms through the graph.

Then I tested both algorithms on **hard graphs** (test cases inspired by [zhougelin's idea][hard-graphs], but designing the test cases so that they are still solveable with the SPFA instead of making it approach $$\mathcal{O}(VE)$$). Again, these are connected graphs of 100000 vertices and 400000 edges.

Algorithm | 1 SCC | 5 SCCs | 20 SCCs | 100 SCCs | 500 SCCs
Segmented | 397 ms | 377 ms | 312 ms | 128 ms | 49 ms
Vanilla | 377 ms | 362 ms | 293 ms | 260 ms | 271 ms

This is where the Segmented version begins to show some potential. The runtime in these cases are dominated by the time spent on SPFA, not Tarjan's algorithm.

What if I design even harder test cases? Putting the SPFA up against the Segmented version like this seems unfair, but I'll do it just because. This time, the graphs have 10000 vertices and 40000 edges.

Algorithm | 1 SCC | 5 SCCs | 20 SCCs | 100 SCCs | 500 SCCs
Segmented | 1085 ms | 540 ms | 162 ms | 32 ms | 7 ms
Vanilla | 1056 ms | 1104 ms | 1110 ms | 1107 ms | 1099 ms

### Conclusion

The Segmented Shortest Path Algorithm works best when the graph can be partitioned into lots of SCCs. Its speedup over the vanilla Shortest Path Algorithm is only noticable when the graph is 'hard', i.e. the runtime is dominated by the SPFA part of the algorithm.

For randomly generated graphs, it is still best to use the Shortest Path Faster Algorithm.

For hard graphs with few SCCs, it is still best to use the Shortest Path Faster Algorithm.

_Further note: Segmenting can also be done for other path-finding algorithms like Dijkstra's algorithm. I haven't looked into that yet, but perhaps I will follow up on that in a future blog post._

[benchmark-code]: /assets/code-snippets/segmented-spfa-benchmark.cpp
[hard-graphs]: http://poj.org/showmessage?message_id=136458



