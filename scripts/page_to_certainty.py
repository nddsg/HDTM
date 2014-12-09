#!/usr/bin/env python3
# Add corresponding certainty to data

import sys
import random
import operator
from collections import Counter

if (len(sys.argv) > 1 and sys.argv[1] == "-h") or len(sys.argv) is not 4:
    print("Usage nchange_file input_file output_file")
    exit(0)

certainty_map = dict()
# load category -> level map
with open(sys.argv[1]) as fin:
    for line in fin:
        raw_data = line.split()
        certainty_map[int(raw_data[0])] = {"certainty":float(raw_data[1]),"nparent":int(raw_data[2])}

fout = open(sys.argv[3], "w+")
with open(sys.argv[2]) as fin:
    for line in fin:
        raw_data = line.split("\t")
        dst = int(raw_data[1])
        if int(raw_data[1]) in certainty_map:
            fout.write("\t".join([str(certainty_map[dst]["certainty"]), str(certainty_map[dst]["nparent"])] + raw_data))
fout.close()
