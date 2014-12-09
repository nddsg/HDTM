#!/usr/bin/env python3

import sys

hidden_cate_id = 15961454

if len(sys.argv) is not 5:
    print("cate_to_cate_file page_to_cate_file output_cate_to_cate output_page_to_cate")
    exit(255)

input_path = sys.argv[1]
input_page_path = sys.argv[2]
output_path = sys.argv[3]
output_page_path = sys.argv[4]

graph = dict()
with open(input_path) as f:
    for line in f:
        raw_data = line.split()
        src = int(raw_data[0])
        dst_list = [int(x) for x in raw_data[1:]]
        graph[src] = dst_list

if hidden_cate_id not in graph:
    print("hidden_categories does not exists in the graph")
    exit(255)

removed_set = set()
current_point = 0
next_frontier = set()
next_frontier.add(hidden_cate_id)
while len(next_frontier) is not 0:
    current_point = next_frontier.pop()
    if current_point in graph:
        removed_set.add(current_point)
        [removed_set.add(x) for x in graph[current_point]]
        del graph[current_point]

print("remove", len(removed_set), "nodes")

# Save new category_to_category map without hidden_categories
with open(output_path, "w+") as fout:
    for key in graph:
        fout.write(str(key) + "\t" + " ".join([str(x) for x in graph[key]])+"\n")

# Save new page_to_category by filtering out hidden_categories
with open(input_page_path) as f:
    with open(output_page_path, "w+") as fout:
        for line in f:
            raw_data = line.split()
            dst_list = [x for x in raw_data[1:] if int(x) not in removed_set]
            fout.write(raw_data[0]+"\t"+ " ".join(dst_list)+"\n")
