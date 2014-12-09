#!/usr/bin/env python3

import sys

cate_set = set()
with open(sys.argv[1]) as f:
    for line in f:
        dat = [int(y) for y in line.split()]
        for x in dat:
            cate_set.add(x)

print("load finish")
with open(sys.argv[3], "w+") as fout:
    with open(sys.argv[2]) as f:
        for line in f:
            dat = [int(x) for x in line.split()]
            page = dat[0]
            cats = dat[1:]
            for x in cats:
                if x in cate_set:
                    fout.write(str(page)+"\t"+str(x)+"\n")
    
