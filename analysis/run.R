setwd("/data/bshi/hdtm_result/report/certainty/")
source("./DeltaCon.R")

# Category links (This is used for all graphs)
#if(file.exists("cate_cate.rds")) {
#  cate_cate <- readRDS("cate_cate.rds") 
#} else {
  #cate_cate <- read.csv("../../misc/cate_tree.edgelist", header=FALSE, sep="\t")
  cate_cate <- read.csv("../../misc/cate_to_subcat_no_hidden_cate.edgelist", header=FALSE, sep="\t")
  cate_cate <- subset(cate_cate, select=c(V1,V2))
#  saveRDS(cate_cate, "cate_cate.rds")
#}

# Category document links (This is used for all graphs)
#if(file.exists("cate_doc.rds")) {
#  cate_doc <- readRDS("cate_doc.rds")
#} else {
  #cate_doc <- read.csv("../../misc/page_to_category_tree.edgelist", header=FALSE, sep="\t")
  cate_doc <- read.csv("../../misc/page_to_category_no_redirection_no_hidden_cate.edgelist", header=FALSE, sep="\t")
#  saveRDS(cate_doc, "cate_doc.rds")
#}

# Max id for category-category graph and category-document graph
cc_max <- max(cate_cate)
cd_max <- max(cate_doc)

# HDTM graph rooted by 26700
#if(file.exists("26700_hdtm.rds")) {
#  g_hdtm <- readRDS("26700_hdtm.rds")
#} else {
  g_hdtm <- read.csv("../../misc/26700_page_graph.txt", sep="\t", header=FALSE, colClasses=c(NA, "NULL", NA))
#  saveRDS(g_hdtm, "26700_hdtm.rds")
#}

#if(file.exists("26700_0.05.rds")) {
#  g_hdtm_05 <- readRDS("26700_0.05.rds")
#} else {
  g_hdtm_05 <- read.csv("../../misc/26700_0.05_page_graph.txt", sep="\t", header=FALSE, colClasses=c(NA, "NULL", NA))
#  saveRDS(g_hdtm_05, "26700_0.05.rds")
#}

#if(file.exists("26700_0.95.rds")) {
#  g_hdtm_95 <- readRDS("26700_0.95.rds")
#} else {
  g_hdtm_95 <- read.csv("../../misc/26700_0.95_page_graph.txt", sep="\t", header=FALSE, colClasses=c(NA, "NULL", NA))
#  saveRDS(g_hdtm_95, "26700_0.95.rds")
#}

#if(file.exists("obama.rds")) {
#  g_obama <- readRDS("obama.rds")
#} else {
  g_obama <- read.csv("../../misc/534366_page_graph.txt", sep="\t", header=FALSE, colClasses=c(NA, "NULL", NA))
#  saveRDS(g_obama, "obama.rds")
#}

#if(file.exists("obama_0.05.rds")) {
#  g_obama_05 <- readRDS("obama_0.05.rds")
#} else {
  g_obama_05 <- read.csv("../../misc/534366_0.05_page_graph.txt", sep="\t", header=FALSE, colClasses=c(NA, "NULL", NA))
#  saveRDS(g_obama_05, "obama_0.05.rds")
#}

#if(file.exists("obama_0.95.rds")) {
#  g_obama_95 <- readRDS("obama_0.95.rds")
#} else {
  g_obama_95 <- read.csv("../../misc/534366_0.95_page_graph.txt", sep="\t", header=FALSE, colClasses=c(NA, "NULL", NA))
#  saveRDS(g_obama_95, "obama_0.95.rds")
#}



# Original document graph
#if(file.exists("original.rds")) {
#  g_doc <- readRDS("original.rds")
#} else {
  g_doc <- read.csv("../../misc/26700_original_page_graph.txt", sep="\t", header=FALSE)
#  saveRDS(g_doc, "original.rds")
#}

#if(file.exists("bfs.rds")) {
#  g_bfs <- readRDS("bfs.rds")
#} else {
  g_bfs <- read.csv("../../26700_rand.txt", sep="\t", header = FALSE, colClasses=c(NA, "NULL", NA, "NULL"))
#  saveRDS(g_bfs, "bfs.rds")
#}

#if(file.exists("rand.rds")) {
#  g_trand <- readRDS("rand.rds")
#} else {
  g_trand <- read.csv("../../26700_randd.txt", sep="\t", header = FALSE)
#  saveRDS(g_trand, "rand.rds")
#}

#if(file.exists("lda_topic_500.rds")) {
#  g_lda500 <- readRDS("lda_topic_500.rds")
#} else {
  g_lda500 <- read.csv("../../misc/topic_500", sep="\t", header = FALSE)
#  saveRDS(g_lda500, "lda_topic_500.rds")
#}

#if(file.exists("lda_topic_10.rds")) {
#  g_lda10 <- readRDS("lda_topic_10.rds")
#} else {
  g_lda10 <- read.csv("../../misc/topic_10", sep="\t", header = FALSE)
#  saveRDS(g_lda10, "lda_topic_10.rds")
#}

#if(file.exists("lda_topic_30.rds")) {
#  g_lda30 <- readRDS("lda_topic_30.rds")
#} else {
  g_lda30 <- read.csv("../../misc/topic_30", sep="\t", header = FALSE)
#  saveRDS(g_lda10, "lda_topic_30.rds")
#}

#if(file.exists("lda_topic_50.rds")) {
#  g_lda50 <- readRDS("lda_topic_50.rds")
#} else {
  g_lda50 <- read.csv("../../misc/topic_50", sep="\t", header = FALSE)
#  saveRDS(g_lda10, "lda_topic_50.rds")
#}

#if(file.exists("lda_topic_100.rds")) {
#  g_lda100 <- readRDS("lda_topic_100.rds")
#} else {
  g_lda100 <- read.csv("../../misc/topic_100", sep="\t", header = FALSE)
#  saveRDS(g_lda10, "lda_topic_100.rds")
#}

# Max id of graph
ghdtm_max <- max(g_hdtm)
ghdtm05_max <- max(g_hdtm_05)
ghdtm95_max <- max(g_hdtm_95)
gobama_max <- max(g_obama)
gobama05_max <- max(g_obama_05)
gobama95_max <- max(g_obama_95)
gdoc_max <- max(g_doc)
gbfs_max <- max(g_bfs)
gtrand_max <- max(g_trand)
glda500_max <- max(g_lda500$V1)

max_id <- max(ghdtm_max, gdoc_max, gbfs_max, gtrand_max)
g_lda500 <- transform(g_lda500, V2 = V2 + max_id)
g_lda10 <- transform(g_lda10, V2 = V2 + max_id)
g_lda30 <- transform(g_lda30, V2 = V2 + max_id)
g_lda50 <- transform(g_lda50, V2 = V2 + max_id)
g_lda100 <- transform(g_lda100, V2 = V2 + max_id)

colnames(g_hdtm) <- c("V1","V2")
colnames(g_hdtm_05) <- c("V1","V2")
colnames(g_hdtm_95) <- c("V1","V2")
colnames(g_obama) <- c("V1", "V2")
colnames(g_obama_05) <- c("V1", "V2")
colnames(g_obama_95) <- c("V1", "V2")

#g_hdtm <- rbind(g_hdtm, cate_cate, cate_doc)
#g_doc <- rbind(g_doc, cate_cate, cate_doc)
g_origin <- rbind(cate_cate, cate_doc)

colnames(g_bfs) <- c("V1", "V2")
#g_bfs <- rbind(cate_cate, cate_doc, g_bfs)
#g_trand <- rbind(cate_cate, cate_doc, g_trand)

# Compare original graph(category graph) and document graph
nnodes <- max(cc_max, cd_max, gdoc_max)
odscore <- delta_con(g_origin, g_doc, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and document graph similarity score is ", odscore))
# 
# # Compare category graph and hdtm graph
nnodes <- max(cc_max, cd_max, ghdtm_max)
ohscore <- delta_con(g_origin, g_hdtm, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and hdtm graph similarity score is ", ohscore))

nnodes <- max(cc_max, cd_max, ghdtm05_max)
oh05score <- delta_con(g_origin, g_hdtm_05, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and hdtm 0.05 graph similarity score is ", oh05score))

nnodes <- max(cc_max, cd_max, ghdtm95_max)
oh95score <- delta_con(g_origin, g_hdtm_95, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and hdtm 0.95 graph similarity score is ", oh95score))

nnodes <- max(cc_max, cd_max, gobama_max)
oobamascore <- delta_con(g_origin, g_obama, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and obama graph similarity score is ", oobamascore))

nnodes <- max(cc_max, cd_max, gobama05_max)
oobamascore05 <- delta_con(g_origin, g_obama_05, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and 05 obama graph similarity score is ", oobamascore05))

nnodes <- max(cc_max, cd_max, gobama95_max)
oobamascore95 <- delta_con(g_origin, g_obama_95, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and 95 obama graph similarity score is ", oobamascore95))

# # Compare category graph and bfs graph
nnodes <- max(cc_max, cd_max, gbfs_max)
orscore <- delta_con(g_origin, g_bfs, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and bfs graph similarity score is ", orscore))
# 
# 
# # Compare category graph and random graph
nnodes <- max(cc_max, cd_max, ghdtm_max, gtrand_max)
otscore <- delta_con(g_origin, g_trand, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and random graph similarity score is ", otscore))
# 
# # Compare LDA500 with category graph
 nnodes <- max(g_lda500, cc_max, cd_max)
 ol500score <- delta_con(g_origin, g_lda500, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
 print(paste("category graph and LDA 500 similarity score is ", ol500score))

# Compare LDA500 with category graph
nnodes <- max(g_lda10, cc_max, cd_max)
ol10score <- delta_con(g_origin, g_lda10, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and LDA 10 similarity score is ", ol10score))

nnodes <- max(g_lda30, cc_max, cd_max)
ol30score <- delta_con(g_origin, g_lda30, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and LDA 30 similarity score is ", ol30score))

nnodes <- max(g_lda50, cc_max, cd_max)
ol50score <- delta_con(g_origin, g_lda50, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and LDA 50 similarity score is ", ol50score))

nnodes <- max(g_lda100, cc_max, cd_max)
ol100score <- delta_con(g_origin, g_lda100, nnodes, method = "fast", percent = 0.1, symmetrical = TRUE)
print(paste("category graph and LDA 100 similarity score is ", ol100score))
