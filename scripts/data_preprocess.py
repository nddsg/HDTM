#!/usr/bin/env python3

import sys
from subprocess import call

if len(sys.argv) is not 3:
    print("usage: graph_lab_graph_file root_node \n")
    exit(255)

original_graphlab_graph = sys.argv[1][:-5] # Remove 0.bin
data_prefix = sys.argv[2] # Will be added in all filenames

# generate original_graphlab_graph

raw_original_graph = "../misc/"+data_prefix+"_original_page_graph.txt"
raw_hdtm_graph = "../misc/"+data_prefix+"_page_graph.txt"
nchange_file = "../misc/"+data_prefix+"_nchange.txt"

rtn = call(["./bin/hdtm_ana", original_graphlab_graph,
            raw_original_graph, raw_hdtm_graph, nchange_file])

if rtn is not 0:
    exit(rtn)

if rtn is not 0:
    exit(rtn)

# Convert original graph to bfs graph
#raw_rand_graph = "../misc/" + data_prefix+"_rand_page_graph.txt"

#rtn = call(["./bfs_generator.py", raw_original_graph, raw_rand_graph, data_prefix, "-1"])

#if rtn is not 0:
#    exit(rtn)

# generate adj-list based on generated edge_list

hdtm_graph = "../misc/"+data_prefix+"_adj_page_graph.txt"
#rand_graph = "../misc/"+data_prefix+"_rand_adj_page_graph.txt"

#rtn = call(["./edge2adj.py", raw_hdtm_graph, hdtm_graph, nchange_file, max_nchange])
rtn = call(["./edge2adj.py", raw_hdtm_graph, hdtm_graph])

if rtn is not 0:
    exit(rtn)

#rtn = call(["./edge2adj.py", raw_rand_graph, rand_graph])

#if rtn is not 0:
#    exit(rtn)

# Run cate_cover
page_to_category = "../misc/page_to_category_no_redirection_no_hidden_cate.adj"
subcate_to_cate = "../misc/cate_to_subcat_no_hidden_cate.adj"
#hdtm_cover = "../coverage/raw_data/"+data_prefix+"_variable_cate_cover.tsv"
#rand_cover = "../coverage/raw_data/"+data_prefix+"_rand_variable_cate_cover.tsv"

#rtn = call(["./bin/cate_cover", page_to_category, subcate_to_cate, 
#            hdtm_graph, hdtm_cover])

#if rtn is not 0:
#    exit(rtn)

#rtn = call(["./bin/cate_cover", page_to_category, subcate_to_cate, 
#            rand_graph, rand_cover])

#if rtn is not 0:
#    exit(rtn)

# Run cate_hier

hdtm_hier = "../coverage/raw_data/"+data_prefix+"_variable_cate_hier.tsv"
#rand_hier = "../coverage/raw_data/"+data_prefix+"_rand_variable_cate_hier.tsv"
rtn = call(["./bin/cate_hier", page_to_category, subcate_to_cate,
             hdtm_graph, hdtm_hier])

if rtn is not 0:
    exit(rtn)

#rtn = call(["./bin/cate_hier", page_to_category, subcate_to_cate,
#             rand_graph, rand_hier])

#if rtn is not 0:
#    exit(rtn)

# Convert result into category based

#cate_level = "../misc/category_level.tsv"

#for order_type in ["low", "high", "major", "rand"]:


    
#    rtn = call(["./page_to_cate_level.py", cate_level, page_to_category,
#                hdtm_cover, "../coverage/"+data_prefix+"_variable_cate_cover_by_"+order_type+"_cate_level.tsv",
#                order_type])
#    rtn = call(["./page_to_cate_level.py", cate_level, page_to_category,
#                rand_cover, "../coverage/"+data_prefix+"_rand_variable_cate_cover_by_"+order_type+"_cate_level.tsv",
#                order_type])
#    rtn = call(["./page_to_cate_level.py", cate_level, page_to_category,
#                hdtm_hier, "../coverage/"+data_prefix+"_variable_cate_hier_by_"+order_type+"_cate_level.tsv",
#                order_type])
#    
#    rtn = call(["./page_to_cate_level.py", cate_level, page_to_category,
#                rand_hier, "../coverage/"+data_prefix+"_rand_variable_cate_hier_by_"+order_type+"_cate_level.tsv",
#                order_type])

rtn = call(["./page_to_certainty.py", nchange_file, hdtm_hier, "../coverage/"+data_prefix+"_cate_hier_by_certainty.tsv"])
