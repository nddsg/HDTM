#!/usr/bin/env python3

# Check difference between two files despite the order

import sys

file_1 = dict()
with open(sys.argv[1]) as f:
    for line in f:
        line = line.split()
        line = " ".join([line[0], line[2]])
        if line not in file_1:
            file_1[line] = 0
        file_1[line] += 1

file_2 = dict()
with open(sys.argv[2]) as f:
    for line in f:
        line = line.split()
        line = " ".join([line[0], line[2]])
        if line not in file_2:
            file_2[line] = 0
        file_2[line] += 1

diff = 0
for key in file_2:
    if key not in file_1:
        print(key)
        diff += 1

print(diff)
