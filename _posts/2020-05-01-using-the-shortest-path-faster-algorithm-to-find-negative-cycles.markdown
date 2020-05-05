---
layout: post
title:  "Using the Shortest Path Faster Algorithm to find a negative cycle."
description: "The Shortest Path Faster Algorithm is an improvement over the Bellman-Ford Algorithm, and it can be used to find negative cycles in a directed weighted graph."
date: 2020-05-05 12:20 +0700
---

<script type="text/javascript" src="https://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"></script>

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/80x15.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.

### Introduction

The Shortest Path Faster Algorithm (SPFA) is an improvement over the Bellman-Ford Algorithm. Both are used to calculate the shortest paths from a single source vertex in a weighted directed graph. The SPFA is almost always preferred over the Bellman-Ford Algorithm because of its speed. It has an (unproven) average runtime complexity of $$\mathcal{O}(m)$$.

The Bellman-Ford Algorithm is also used to detect/find negative cycles, and the SPFA can do that, too! In this article I will show how to use the SPFA to find a negative cycle, and propose a method to terminate the algorithm early in the presence of a negative cycle.

### Detecting a negative cycle

The Bellman-Ford Algorithm exploits the fact that in a graph with no negative cycles, the length of any shortest path is at most $$n-1$$. Therefore, we simply need to keep track of the length of the current shortest path for each vertex.

Below is the pseudocode for the SPFA with negative cycle detection.
- `Queue` is a First-In-First-Out queue.
- `w(u, v)` is the weight of the edge `(u, v)`.
- `dis[u]` is the weight of the current shortest path from the source to `u`.
- `len[u]` is the length (in amount of edges) of the current shortest path from the source to `u`.

**Note:** I have modified the SPFA so that every vertex has a starting "shortest path" of $$0$$. This has the same effect as creating an imaginary source vertex $$s$$ and creating an edge with weight $$0$$ from $$s$$ to all other vertices. This ensures that the algorithm can detect a negative cycle regardless of the graph's connectivity. Now the only purpose of the algorithm is to detect a negative cycle. If you still want to calculate the shortest paths from a source vertex in case there are no negative cycles, then _do not_ make this modification.

```
function SPFA(G):
    for v in V(G):
        len[v] = 0
        dis[v] = 0
        Queue.push(v)
    while !Queue.is_empty():
        u = Queue.pop()
        for (u, v) in E(G):
            if dis[u] + w(u, v) < dis[v]:
                len[v] = len[u] + 1
                if len[v] == n:
                    return "negative cycle detected"
                dis[v] = dis[u] + w(u, v)
                if !Queue.contains(v):
                    Queue.push(v)
    return "no negative cycle detected"
```

It is worth noting that although the SPFA is very fast when there is no negative cycle, it slows down significantly in the presence of one, especially in this version, because it has to keep calculating the shortest distance over and over again until a path reaches length $$n$$. Further down in this article I will propose a way to halt the algorithm early.

### Reconstructing the negative cycle

Reconstructing the negative cycle can be done with a few modifications to the algorithm.

We will create a new array `pre`, where `pre[u]` is the direct predecessor of `u` in the current shortest path. We will have to update `pre[v]` whenever we have the condition `dis[u] + w(u, v) < dis[v]`.

```
// some code before
if dis[u] + w(u, v) < dis[v]:
    pre[v] = u
    len[v] = len[u] + 1
// some code after
```

Then we create a trace function to find the negative cycle. Here `Stack` is a First-In-Last-Out stack.

```
function Trace(pre[], v):
    while !Stack.contains(v):
        Stack.push(v)
        v = pre[v]
    cycle = [v]
    while Stack.top() != v:
        cycle.insert(Stack.pop())
    cycle.insert(v)
    return cycle
```

**Note:** The vertex `v` in the `SPFA` function that triggers the negative cycle condition doesn't neccesarily have to be part of a negative cycle. It may be a vertex that is reachable from a negative cycle.

The runtime complexity of the SPFA algorithm is at most $$\mathcal{O}(nm)$$, and the trace function takes $$\mathcal{O}(n)$$.

### Early termination

As mentioned before, the SPFA will be really slow in the presence of a negative cycle, as it has to keep computing until a path reaches length $$n$$. In this section I will propose a way to terminate the algorithm early while keeping the worst-case runtime the same.

Let's define the _dependency graph_ $$D$$ of $$G$$ as the graph made of `(pre[u], u)` edges after the SPFA has terminated. Notice that in the case that $$G$$ has no negative cycles, $$D$$ is a Directed Acyclic Graph (DAG). Once there is a negative cycle, the dependency graph will also contain a cycle. This cycle in $$D$$ also corresponds to the negative cycle in $$G$$.

_Proof:_ **[WIP]**

If we can detect when a cycle in $$D$$ forms, we can terminate the SPFA early instead of waiting for some vertex to reach path length $$n$$. A simple DFS on the induced graph $$D$$ will do the trick. But since the DFS takes $$\mathcal{O}(n)$$ (the induced graph $$D$$ has $$n$$ vertices and at most $$n$$ edges), we will only do this step every $$n^{th}$$ iteration of the inner loop.

Below is the pseudocode for the early-terminated SPFA.
- `iter` is a counter for us to see how many iterations of the inner loop have passed.
- `detect_cycle(G, pre[])` is a function that returns `true` iff there is a cycle in the induced graph $$D$$. There are many ways you can implement this to run in $$\mathcal{O}(n)$$ and in this post I will not be discussing a specific implementation.

```
function SPFA(G):
    for v in V(G):
        len[v] = 0
        dis[v] = 0
        Queue.push(v)
    iter = 0
    while !Queue.is_empty():
        u = Queue.pop()
        for (u, v) in E(G):
            if dis[u] + w(u, v) < dis[v]:
                pre[v] = u
                dis[v] = dis[u] + w(u, v)
                iter = iter + 1
                if iter == n:
                    iter = 0
                    if detect_cycle(G, pre):
                        return "negative cycle detected"
                dis[v] = dis[u] + w(u, v)
                if !Queue.contains(v):
                    Queue.push(v)
    if detect_cycle(G, pre):
        return "negative cycle detected"
    return "no negative cycle detected"
```

Worst-case time complexity: $$\mathcal{O}(nm)$$.

Because the function `detect_cycle()` only runs once every $$n$$ iterations, the worst-case runtime complexity stays the same. However, because this version terminates early, we can expect some pretty huge performance speedups as demonstrated in the next section.

### Benchmarks

First I tested both variants (Path Length and Early Termination) on graphs with no negative cycles to demonstrate the awesomeness of the Shortest Path Faster Algorithm. For this, I used randomly-generated graphs with 1e5 vertices and 4e5 edges and a sample size of 100.
- Path length: 4 ms.
- Early termination: 4 ms.

Then, I tested both on graphs _with_ negative cycles. Again, I used randomly generated graphs with 1e5 vertices and 4e5 edges and a sample size of 100.
- Path length: N/A.
- Early termination: 4 ms.

The path-length version really took a hit here (it wouldn't finish within 3 minutes so I stopped benchmarking that variant for this test case), whereas the early-terminated version doesn't seem to be affected at all.

Then I tested both variants on some smaller graphs so that the path-length variant can still produce a result within a reasonable time frame. This time, graphs have 1e4 vertices and 3e4 edges.
- Path length: 1737 ms.
- Early termination: 0 ms.

The [C++ Code](/assets/code-snippets/cycle-detection-with-spfa.cpp) I used to benchmark the algorithm.

### Conclusion

It is clear (both from the theoretical runtime and from my experiments) that the early-terminated SPFA is a great algorithm to detect/find a negative cycle in a directed acyclic graph.

However, it still uses the Shortest Path Faster Algorithm as its backbone, and one can easily generate graphs for which the SPFA takes $$\mathcal{O}(nm)$$ time.