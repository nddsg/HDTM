#!/usr/bin/env python3

# Edge list to adjlist

import sys

with open(sys.argv[1]) as fin:
    dat = dict()
    with open(sys.argv[2], "w+") as fout:
        for line in fin:
            raw = [int(x) for x in line.split()]
            src = raw[0]
            raw = raw[1:]
            if src not in dat:
                dat[src] = set()
            for x in raw:
                dat[src].add(x)

        for k in dat:
            fout.write(str(k) + "\t" + " ".join([str(x) for x in dat[k]])+"\n")
