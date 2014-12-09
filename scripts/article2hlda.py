#!/usr/bin/env python3

# This program convert raw document-word file to Graphlab's LDA format

import sys

with open(sys.argv[2], "w+") as fout:
    with open(sys.argv[1]) as f:
        for line in f:
            raw_data = [int(x) for x in line.split()]
            word_set = dict()
            for x in raw_data[1:]:
                if x not in word_set:
                    word_set[x] = 0
                word_set[x] += 1
            fout.write(str(raw_data[0])+" ")
            for k in word_set:
                fout.write(str())
