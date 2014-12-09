#!/usr/bin/env python3

import sys
import random

if len(sys.argv) is not 6:
    print(" input_graph nchange_file output_graph start_node threshold\n")
    exit()

input_file = sys.argv[1]
nchange_file = sys.argv[2]
output_file = sys.argv[3]
start_node = int(sys.argv[4])
th = int(sys.argv[5])

o_graph = dict()
p_graph = dict()
# Construct adj list for original graph
with open(input_file) as f:
    for line in f:
        raw_data = [int(x) for x in line.split()]
        for x in raw_data[2:]:
            if x not in p_graph:
                p_graph[x] = 0
            p_graph[x] += 1
            if raw_data[0] not in o_graph:
                o_graph[raw_data[0]] = set()
            o_graph[raw_data[0]].add(x)

# if len(o_graph[node]) > 1 that means it has multi-parent candidates

print("adj list generated")

nchange = list()

with open(nchange_file) as f:
    for line in f:
        nid, prob, nparent = line.split()
        nid = int(nid)
        prob = float(prob)
        nparent = int(nparent)
        nchange.append({'id':nid, 'prob':prob, 'nparent':nparent})

print("nchange loaded")

nchange.sort(key=lambda dic: dic['prob'], reverse=True)

print("sort okay")

# Do BFS on graph

bfs_start = random.choice(o_graph.keys()) if start_node is -1 else start_node

if start_node not in o_graph:
    print("starting point does not exist!\n")
    exit()

frontier = set()
visited = set()
frontier.add(start_node)
current_height = 0
current_height_accu = 0
cnt = 0

fout = open(output_file, "w+")
while len(frontier) is not 0:
    # output graph
    old_frontier = frontier
    visited |= old_frontier
    print(current_height, old_frontier)
    new_frontier = set()
    for item in old_frontier:
        if item in o_graph:
            current_height_accu = 0
            t = set([x for x in o_graph[item] if x not in visited])
            new_t = set()
            for cand in nchange:
                if cand['id'] in t and len(new_t) < 2 * current_height + 5 and len(t) > 0:
                #if cand['id'] in t and cand['prob'] > (float(1) / cand['nparent'])  and len(new_t) < pow(2, current_height + 1) + 5 and len(t) > 0:
                    new_t.add(cand['id'])
                    t -= new_t

            new_frontier |= new_t
            visited |= t
            if len(t) > 0:
                for x in new_t:
                    current_height_accu += 1
                    cnt += 1
                    fout.write("\t".join([str(item), str(x), "\n"]))
    frontier = new_frontier
    current_height += 1

fout.close()
