---
layout: post
title:  "Using the Shortest Path Faster Algorithm to find a negative cycle."
description: "The Shortest Path Faster Algorithm is an improvement over the Bellman-Ford Algorithm, and it can be used to find negative cycles in a directed weighted graph."
date:   2020-05-01 21:15 +0700
---

<script type="text/javascript" src="https://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"></script>

### Introduction

The Shortest Path Faster Algorithm (SPFA) is an improvement over the Bellman-Ford Algorithm. Both are used to calculate the shortest paths from a single source vertex in a weighted directed graph. The SPFA is almost always preferred over the Bellman-Ford Algorithm because of its speed. It has an (unproven) average runtime complexity of $$\mathcal{O}(m)$$.

The Bellman-Ford Algorithm is also used to detect/find negative cycles, and the SPFA can do that, too! In this article I will show how to use the SPFA to find a negative cycle, and propose a speedup using Link/Cut trees.

### Detecting a negative cycle

The Bellman-Ford Algorithm exploits the fact that in a graph with no negative cycles, the length of any shortest path is at most $$n-1$$. Therefore, we simply need to keep track of the length of the current shortest path for each vertex.

Below is the pseudocode for the SPFA with negative cycle detection. Here `Queue` is a First-In-First-Out queue and `w(u, v)` is the weight of the edge `(u, v)`, `dis[u]` is the weight of the current shortest path from the source to `u`, and `len[u]` is the length (in amount of edges) of the current shortest path from the source to `u`.

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
                dis[v] = dis[i] + w(u, v)
                if !Queue.contains(v):
                    Queue.push(v)
    return "no negative cycle detected"
```

It is worth noting that although the SPFA is very fast when there is no negative cycle, it slows down significantly in the presence of one, especially in this version, because it has to keep calculating the shortest distance over and over again until a path reaches length $$n$$. Further down in this article I will propose a way to halt the algorithm early.

### Reconstructing the negative cycle

Reconstructing the negative cycle can be done with a few modifications to the algorithm.

We will create a new array `pre`, where `pre[u]` is the direct predecessor of `u` in the current shortest path. Below is the new pseudocode.

```
function SPFA(G):
    for v in V(G):
        len[v] = 0
        dis[v] = 0
        pre[v] = null
        Queue.push(v)
    while !Queue.is_empty():
        u = Queue.pop()
        for (u, v) in E(G):
            if dis[u] + w(u, v) < dis[v]:
            	pre[v] = u
                len[v] = len[u] + 1
                if len[v] == n:
                    return "negative cycle detected"
                dis[v] = dis[i] + w(u, v)
                if !Queue.contains(v):
                    Queue.push(v)
    return "no negative cycle detected"
```

Then we create a trace function to find the negative cycle. Here `Stack` is a First-In-Last-Out stack.

```
function Trace(pre, v):
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

### Can we do better?

Let's define the _dependency graph_ $$D$$ of $$G$$ as the graph made of `(pre[u], u)` edges after the SPFA has terminated. Notice that in the case that $$G$$ has no negative cycles, $$D$$ is a Directed Acyclic Graph (DAG). Once there is a negative cycle, the dependency graph will also contain a cycle. This cycle in $$D$$ also corresponds to the negative cycle in $$G$$.

_Proof:_ **[WIP]**

If we can efficiently detect when a cycle in $$D$$ forms, we can terminate the SPFA early instead of waiting for some vertex to reach path length $$n$$. It turns out that we can do this with a link/cut tree.

A [link/cut tree][link-cut-tree-wikipedia] is a data structure for representing a set of rooted trees. It supports the following operations:

- `detach(u)`: disconnect `u` (and its subtree) from its parent.
- `attach(u, v)`: attach the rooted tree `u` to `v` as its subtree.
- `root(u)`: find the root of vertex `u`.

All the above operations have an amortized runtime of $$\mathcal{O}(log\>n)$$.

Instead of keeping track of the length of the shortest paths, the pseudocode below utilizes a link/cut tree to detect a negative cycle as it forms.

```
function SPFA(G):
    for v in V(G):
        len[v] = 0
        dis[v] = 0
        pre[v] = null
        Queue.push(v)
    while !Queue.is_empty():
        u = Queue.pop()
        for (u, v) in E(G):
            if dis[u] + w(u, v) < dis[v]:
                if pre[v] != null:
                    detach(v)
                if root(u) == v:
                    return "negative cycle detected"
                attach(v, u)
                dis[v] = dis[i] + w(u, v)
                if !Queue.contains(v):
                    Queue.push(v)
    return "no negative cycle detected"
```

This algorithm will work well on graphs with a negative cycle, as it can immediately detect once we have found a cycle and terminate. However, due to the extra cost of maintaining the link/cut tree, this algorithm is slower than the one mentioned in the beginning of this article for graphs with no negative cycles.

Reconstructing the negative cycle for this algorithm is very simple. It can be found by following `pre[]` array from `u` to `v`, adding edges `(pre[i], i)`, and completed by the edge `(u, v)`.

The runtime complexity of this algorithm is up to $$\mathcal{O}(nm\>log\>n)$$.

### Benchmarks

Let's call the algorithms the "Vanilla" version and the "Link/Cut" version.

I tested both algorithms on graphs with no negative cycles, and unsurprisngly, the vanilla version beats the link/cut version every time. The times for these tests are pretty standard for the SPFA:

No negative cycles | Vanilla | Link/Cut
Dense graphs, $$n=3000$$ | 9 ms | 15 ms
Sparse graphs, $$n=1000000$$ | 91 ms | 105 ms

Once I introduced negative cycles, the winner was clear. Here $$l$$ denotes the shortest length (in edges) of a negative cycle in the graph.

Short negative cycles | Vanilla | Link/Cut
Dense graphs, $$n=1000$$, $$l=30$$ | 254 ms | 1 ms
Sparse graphs, $$n=30000$$, $$l=500$$ | 393 ms | 3 ms

Long negative cycles | Vanilla | Link/Cut
Dense graphs, $$n=1000$$, $$l=300$$ | 257 ms | 1 ms
Sparse graphs, $$n=30000$$, $$l=5000$$ | 408 ms | 4 ms

We observe that the Link/Cut version is doing faster when there are negative cycles. This is because it immediately quits after finding a cycle, whereas in the case where there are no cycles, it continues to calculate the _pseudo_ (refer to the note at the beginning of the article) shortest paths to all vertices.

Here is the [C++ code](/assets/code-snippets/spfa-link-cut-for-negative-cycles.cpp) I used for the benchmark.

### Conclusion

Unless my testing method is flawed (which it might be, I'm only human), the Link/Cut version is a clear winner.

[link-cut-tree-wikipedia]: https://en.wikipedia.org/wiki/Link/cut_tree
