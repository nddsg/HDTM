#!/usr/bin/env python3

# Convert adjacent list to edge list

import sys

input_file = sys.argv[1]
output_file = sys.argv[2]

with open(output_file, "w+") as fout:
    with open(input_file) as fin:
        for line in fin:
            raw_data = [int(x) for x in line.split()]
            for dst in raw_data[1:]:
                fout.write(str(raw_data[0])+"\t"+str(dst)+"\n")
