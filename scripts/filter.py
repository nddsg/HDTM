#!/usr/bin/env python3

import sys

with open(sys.argv[3], "w+") as fout:
	node_set = set()
	with open(sys.argv[1]) as f:
		for line in f:
			raw_data = [int(x) for x in line.split()]
			raw_data.pop(1)
			for node in raw_data:
				node_set.add(node)
	print(len(node_set))

	with open(sys.argv[2]) as f:
		cnt = 0
		for line in f:
			node = line.split()[0]
			if int(node) in node_set:
				fout.write(line)
				cnt += 1
	print(cnt)
