---
layout: post
title:  "Using the Shortest Path Faster Algorithm to find a negative cycle."
description: "The Shortest Path Faster Algorithm is an improvement over the Bellman-Ford Algorithm, and it can also be used to find negative cycles in a directed weighted graph."
date:   2020-04-30
---

<script type="text/javascript" src="https://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"></script>

### Introduction

The Shortest Path Faster Algorithm (SPFA) is an improvement over the Bellman-Ford Algorithm. Both are used to calculate the shortest paths from a single source vertex in a weighted directed graph. The SPFA is almost always preferred over the Bellman-Ford Algorithm because of its speed. It has an (unproven) average runtime complexity of $$\mathcal{O}(m)$$.

The Bellman-Ford Algorithm is also used to detect/find negative cycles, and the SPFA can do that, too!

### Detecting a negative cycle

The Bellman-Ford Algorithm exploits the fact that in a graph with no negative cycles, the length of any shortest path is at most $$n-1$$. Therefore, we simply need to keep track of the length of the current shortest path for each vertex.

Below is the pseudocode for the SPFA with negative cycle detection. Here `Queue` is a First-In-First-Out queue and `w(u, v)` is the weight of the edge `(u, v)`, `dis[u]` is the weight of the current shortest path from the source to `u`, and `len[u]` is the length (in amount of edges) of the current shortest path from the source to `u`.

```
function SPFA(G, s):
    len[s] = 0
    for v in V(G) \ {s}:
        dis[v] = inf
    dis[s] = 0
    Queue.push(s)
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

### Reconstructing the negative cycle

Reconstructing the negative cycle can be done with a few modifications to the algorithm.

We will create a new array `pre`, where `pre[u]` is the direct predecessor of `u` in the current shortest path. Below is the new pseudocode.

```
function SPFA(G, s):
    len[s] = 0
    for v in V(G) \ {s}:
        dis[v] = inf
    dis[s] = 0
    Queue.push(s)
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

An important thing to note here is that the vertex `v` in the `SPFA` function that triggers the negative cycle condition doesn't neccesarily have to be part of a negative cycle. It is just a vertex that is reachable from a negative cycle.

The runtime complexity of the SPFA algorithm is at most $$\mathcal{O}(nm)$$, and the trace function takes $$\mathcal{O}(n)$$.
