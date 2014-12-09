#!/usr/bin/env python3

import sys
import random

if len(sys.argv) is not 5:
    print(" input_graph output_graph changed_nodes start_node height\n")
    exit()

input_file = sys.argv[1]
output_file = sys.argv[2]
start_node = int(sys.argv[3])
height = int(sys.argv[4])

o_graph = dict()
p_graph = dict()
# Construct adj list for original graph
with open(input_file) as f:
    for line in f:
        raw_data = list()
        if len(line.split("\t")) >= 2:
            raw_data = [int(x) for x in line.split("\t")]
        else:
            raw_data = [int(x) for x in line.split()]

        if raw_data[1] not in p_graph:
            p_graph[raw_data[1]] = 0
        p_graph[raw_data[1]] += 1
        if raw_data[0] not in o_graph:
            o_graph[raw_data[0]] = set()
        o_graph[raw_data[0]].add(raw_data[1])

# if len(o_graph[node]) > 1 that means it has multi-parent candidates

print("adj list generated")

# Do BFS on graph

bfs_start = random.choice(o_graph.keys()) if start_node is -1 else start_node

if start_node not in o_graph:
    print("starting point does not exist!\n")
    exit()

frontier = set()
visited = set()
frontier.add(start_node)
current_height = 0

fout = open(output_file, "w+")
while len(frontier) is not 0:
    # output graph
    old_frontier = frontier
    visited |= old_frontier
    new_frontier = set()
    for item in old_frontier:
        if item in o_graph:
            t = set([x for x in o_graph[item] if x not in visited])
            new_frontier |= t
            visited |= t
            if len(t) > 0:
                for x in t:
                    if p_graph[x] > 1:
                        fout.write("\t".join([str(item), str(current_height), str(x), "\n"]))
#                fout.write("\t".join([str(item), str(current_height), " ".join([str(x) for x in t if p_graph[x] > 1]), "\n"]))
    current_height += 1
    frontier = new_frontier
print(current_height)
fout.close()
