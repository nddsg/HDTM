#!/usr/bin/env python3

import sys
import random

if len(sys.argv) is not 4:
    print(" input_graph output_graph start_node\n")
    exit()

input_file = sys.argv[1]
output_file = sys.argv[2]
start_node = int(sys.argv[3])

g = dict()

with open(input_file) as f:
    for line in f:
        raw_data = [int(x) for x in line.split()]
        for x in raw_data[1:]:
            if x not in g:
                g[x] = set()
            g[x].add(raw_data[0])

print("adj list generated")

# Do BFS on graph

bfs_start = random.choice(g.keys()) if start_node is -1 else start_node

if start_node not in g:
    print("starting point does not exist!\n")
    exit()

frontier = set()
visited = set()
frontier.add(start_node)

fout = open(output_file, "w+")
while len(frontier) is not 0:
    # output graph
    old_frontier = frontier
    visited |= old_frontier
    new_frontier = set()
    for item in old_frontier:
        if item in g:
            current_height_accu = 0
            t = set([x for x in g[item] if x not in visited])
            new_frontier |= t
            visited |= t
            for x in t:
                fout.write("\t".join([str(item), str(x), "\n"]))
    frontier = new_frontier

fout.close()
