#!/usr/bin/env python3

import sys

page_cate_file = sys.argv[1]
graph_file = sys.argv[2]
output_file = sys.argv[3]

page_cate_level_map = dict()
with open(page_cate_file) as f:
    for line in f:
        raw_data = line.split("\t")
        page_cate_level_map[int(raw_data[0])] = [int(x) for x in raw_data[1].split()]

result = dict()
with open(graph_file) as f:
    for line in f:
        src, l, siblings = line.split("\t")
        src = int(src)
        siblings = [int(x) for x in siblings.split()]
        if int(src) not in result:
            result[src] = {"intersect":set(), "union":set()}

        for sibling in siblings:
            if sibling in page_cate_level_map:
                if len(result[src]["intersect"]) is 0:
                    result[src]["intersect"] = set(page_cate_level_map[sibling])
                    break

        for sibling in siblings:
            if sibling in page_cate_level_map:
                cate_set = set(page_cate_level_map[sibling])
                result[src]["intersect"] &= set(page_cate_level_map[sibling])
                result[src]["union"] |= set(page_cate_level_map[sibling])

with open(output_file, "w+") as fout:
    for idx in result:
        if len(result[idx]["union"]) is not 0:
            print(result[idx])
            fout.write(str(idx) + "\t" + str(float(len(result[idx]["intersect"])) / float(len(result[idx]["union"])))+"\n")

