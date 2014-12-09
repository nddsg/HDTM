# Hierarchical Document Topic Model

## Install

1. Clone this repo

		git clone https://github.com/nddsg/HDTM.git

2. Configure `GraphLab` project

		cd HDTM/src/graphlab/
		./configure

3. Compile `HDTM`

		cd ./release/apps/hdtm
		make -j2

4. `HDTM` arguments

		--vertices	The file contains article vertices
		--edges	The file contains article edges
		--root	Id of root article
	  --gamma	Gamma for RWR calculation, default 0.25
		--alpha	Alpha, default 10
		--eta	Eta, default 0.1
		--iter	Iter, default 5
		--burn	Burn-in, default 100
		--sample	Sample interval, default 10
		--token	Number of tokens, run wc -l on dictionary file
		--prefix	Prefix of binary graph files
		--load	Load graph from binary files

Example:

		./hdtm --vertices /data/bshi/wikipedia/vertices.txt --edges /data/bshi/wikipedia/edges.txt \
		       --root 26700 --token 6064216 --iter 400 --burn 100 --sample 20 \
					 --prefix /data/bshi/output/test

## Results
