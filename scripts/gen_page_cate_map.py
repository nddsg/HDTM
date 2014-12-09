#!/usr/bin/env python3

import sys

file_path = sys.argv[1]
output_path = sys.argv[2]

# ID -> Title
page_dict = dict()
cate_dict = dict()

# page_ID -> cate_ID
page_cate_map = dict()
with open(file_path) as f:
    for line in f:
        try:
            page_id, ns, title = line.split()
            page_id = int(page_id)
            ns = int(ns)
            if ns is 0:
                page_dict[title] = page_id
                if title in cate_dict:
                    page_cate_map[page_id] = cate_dict[title] 
            elif ns is 14:
                cate_dict[title] = page_id
                if title in page_dict:
                    page_cate_map[page_dict[title]] = page_id
        except Exception:
            print(line.split())

with open(output_path, "w+") as fout:
    for k in page_cate_map:
        fout.write(str(k) + "\t" +str(page_cate_map[k]) + "\n")
