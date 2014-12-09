#!/usr/bin/env python3

import sys
import random

if len(sys.argv) is not 5:
    print(" input_graph output_graph ntotal nedge\n")
    exit()

input_file = sys.argv[1]
output_file = sys.argv[2]
ntotal = int(sys.argv[3])
nedge = int(sys.argv[4])

random.seed()

# Construct adj list for original graph
with open(output_file, "w+") as fout:
    with open(input_file) as f:
        for line in f:
            if random.randint(0, ntotal) < nedge:
                fout.write(line)
