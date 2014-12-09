#!/usr/bin/env python2

import pydot
import sys
import re

if len(sys.argv) is not 4:
    print("Usage name_to_id_file graph_adj_list output")
    exit(0)

id2name = dict()

pattern = re.compile("[^\w']|_")

with open(sys.argv[1]) as f:
    for line in f:
        name, vid = line.split()
        id2name[int(vid)] = pattern.sub(" ", name)#.replace("_", " ")

graph = pydot.Dot(graph_type="graph")

glist = set()
with open(sys.argv[2]) as f:
    for line in f:
        dat = [int(x) for x in line.split()]
        src = dat[0]
        dest_list = dat[1:]
        for dst in dest_list:
            edge = pydot.Edge(id2name[src], id2name[dst])
            graph.add_edge(edge)

    graph.write_png('test.png')
graph.write('doc.dot')
