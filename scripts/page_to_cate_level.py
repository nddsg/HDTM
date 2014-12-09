#!/usr/bin/env python3
# Add corresponding category level based on page_id(col1)

import sys
import random
import operator
from collections import Counter

if (len(sys.argv) > 1 and sys.argv[1] == "-h") or len(sys.argv) < 6:
    print("Usage category_level page_category input_file output_file [rand|high|low|major]")
    exit(0)

level_map = dict()
# load category -> level map
with open(sys.argv[1]) as fin:
    for line in fin:
        raw_data = line.split("\t")
        level_map[int(raw_data[0])] = int(raw_data[1])

page_cate_level_map = dict()
# load page to category level map
with open(sys.argv[2]) as fin:
    for line in fin:
        raw_data = line.split("\t")
        page_cate_level_map[int(raw_data[0])] = [level_map[int(x)] for x in raw_data[1].split() if int(x) in level_map]

fout = open(sys.argv[4], "w+")
with open(sys.argv[3]) as fin:
    for line in fin:
        raw_data = line.split("\t")
        if len(page_cate_level_map[int(raw_data[0])]) is not 0:
            l = ""
            if sys.argv[5] == "rand":
                l = str(random.choice(page_cate_level_map[int(raw_data[0])]))
            elif sys.argv[5] == "high":
                l = str(sorted(page_cate_level_map[int(raw_data[0])])[0])
            elif sys.argv[5] == "low":
                l = str(sorted(page_cate_level_map[int(raw_data[0])])[-1])
            elif sys.argv[5] == "major":
                l = str(sorted(Counter(page_cate_level_map[int(raw_data[0])]).items(), key=lambda x:x[1])[-1][0])
            else:
                print("undefined level assignment method!")
                exit()
                
            fout.write("\t".join([l]+raw_data))
fout.close()
