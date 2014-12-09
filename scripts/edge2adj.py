#!/usr/bin/env python3

import sys

input_file = sys.argv[1]
output_file = sys.argv[2]
nchange_file = None
max_nchange = None
nchange_list = dict()
if len(sys.argv) is 5:
    nchange_file = sys.argv[3]
    max_nchange = int(sys.argv[4])
    with open(nchange_file) as f:
        for line in f:
            raw_data = line.split()
            src = int(raw_data[0])
            nchange = float(raw_data[1])
            nparent = int(raw_data[2])
            nchange_list[src] = {"nchange":nchange, "nparent":nparent}


adj_list = dict()
lev_list = dict()
ignored = 0
with open(input_file) as f:
    cnt = 0
    for line in f:
        if len(line.split()) is 3:
            src, lev, dst = [int(x) for x in line.split()]
        else:
            src, dst = [int(x) for x in line.split()]

        # ignore 
        if max_nchange is not None:
            if src not in nchange_list:
                ignored += 1
                continue
            if nchange_list[src]["nchange"] > max_nchange:
                ignored += 1
                continue

        if src not in adj_list:
            adj_list[src] = set()
        adj_list[src].add(dst)
        cnt += 1

        # Log src node's graph level
        if src not in lev_list:
            lev_list[src] = lev

        if (cnt % 50000) == 0:
            print(cnt)

with open(output_file, "w+") as f:
    for src in adj_list:
        f.write(str(src)+"\t"+ str(lev_list[src]) +"\t"+" ".join([str(dst) for dst in adj_list[src]])+"\n")
    f.flush()

print("ignore ", ignored, "nodes")
