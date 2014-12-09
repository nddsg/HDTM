#!/usr/bin/env python3

import sys

category_level_file = sys.argv[1]
page_cate_file = sys.argv[2]
page_graph_file = sys.argv[3]
output_file = sys.argv[4]

cate_map = dict()
with open(category_level_file) as f:
    for line in f:
        cate, lev = [int(x) for x in line.split()]
        cate_map[cate] = lev 

page_cate_map = dict()
with open(page_cate_file) as f:
    for line in f:
        page, cate = [int(x) for x in line.split()]
        page_cate_map[page] = cate

visited_nodes = set()
cp_node = set()
with open(output_file, "w+") as fout:
    with open(page_graph_file) as f:
        for line in f:
            src, level, dst = [int(x) for x in line.split()]
            if src in page_cate_map:
                if page_cate_map[src] in cate_map:
                    visited_nodes.add(src)
            if dst in page_cate_map:
                if page_cate_map[dst] in cate_map:
                    cp_node.add(dst)
                    visited_nodes.add(dst)
                    fout.write("\t".join([str(dst), str(level), str(cate_map[page_cate_map[dst]]), "\n"]))

cnt = 0
for node in cp_node:
    if node not in visited_nodes:
        cnt += 1

print("there are", cnt, "non-parent nodes")
