#!/usr/bin/python

# Calculate in-degree of given graph

import networkx as nx
import sys

g = nx.read_edgelist(sys.argv[1], create_using=nx.DiGraph())

nodes = dict()

for node in g.in_degree_iter():
    if node[1] not in nodes:
        nodes[node[1]] = 0
    nodes[node[1]] += 1

print(nodes)

exit(0)
