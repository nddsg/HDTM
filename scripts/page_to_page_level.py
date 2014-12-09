#!/usr/bin/env python3
# Add page's level according to page_id(column 1)

import sys

if (len(sys.argv) >= 2 and sys.argv[1] == "-h") or len(sys.argv) is not 4:
    print("Usage page_graph input_file output_file")
    exit(0)

page_graph = dict() # children -> parent
level_graph = dict() # parent -> level
with open(sys.argv[1]) as graph_file:
    cnt = 0
    for line in graph_file:
        dat = line.split("\t") # 0 is parent / 1 is height / 2 is chidren list
        parent = int(dat[0])
        lev = int(dat[1])
        children = [int(x) for x in dat[2].split(" ")[:-1]]
        for child in children:
            page_graph[child] = parent
        level_graph[parent] = lev
        cnt+=1
        if cnt % 1000000 == 0:
            print(cnt)

fout = open(sys.argv[3], "w+")
with open(sys.argv[2]) as dist_file:
    for line in dist_file:
        dat = line.split("\t") # src \t dst \t dist
        parent = page_graph[int(dat[0])]
        lev = level_graph[parent]
        fout.write("\t".join([str(lev)]+dat))
        #fout.write("\t".join([str(lev), str(parent), dat[-1]]))

fout.close()
