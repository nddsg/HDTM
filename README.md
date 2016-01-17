# Hierarchical Document Topic Model

## Install

1. Clone this repo

		git clone https://github.com/nddsg/HDTM.git

2. Configure `GraphLab` project

		cd HDTM/src/graphlab/
		./configure

3. Compile `HDTM`

		cd ./release/apps
		make -j2

## Execution

### `HDTM` arguments

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
		       --root 26700 --token 6064216 --iter 400 --burn 100 --sample 50 \
					 --prefix /data/bshi/wikipedia/graphlab/result/


 HDTM requires:

 - a vertices file with a vertex id, a tab and a list of words id space separated

 	```
 	1	3 4 5 6 7
 	2	3 4 5 7 8
 	....
 	```
 - edges file with vertex id, tab and vertex id

  ```
1	2
1	4
	```

Actually the algorithm requires that the words of the articles should be converted as int.


When `hdtm` is finished, you will get a set of results that looks like this

		bshi@dsg1:/data/bshi/wikipedia/graphlab/result$ ls
		0_0_0.bin    140_0_0.bin  190_0_0.bin  240_0_0.bin  290_0_0.bin  340_0_0.bin  390_-1.89962e+14_0.bin
		100_0_0.bin  150_0_0.bin  200_0_0.bin  250_0_0.bin  300_0_0.bin  350_0_0.bin
		110_0_0.bin  160_0_0.bin  210_0_0.bin  260_0_0.bin  310_0_0.bin  360_0_0.bin
		120_0_0.bin  170_0_0.bin  220_0_0.bin  270_0_0.bin  320_0_0.bin  370_0_0.bin
		130_0_0.bin  180_0_0.bin  230_0_0.bin  280_0_0.bin  330_0_0.bin  380_0_0.bin

The first number in file names is the iteration number.

### Convert `HDTM` binary graph to text file

		./hdtm_ana BINARY_FILE.bin ORIGINAL_GRAPH_OUT HIERARCHY_OUT NODE_CHANGES_OUT

* `ORIGINAL_GRAPH_OUT` is the original graph without modification.
* `HIERARCHY_OUT` is the result hierarchy.
* `NODE_CHANGES_OUT` is the change log of every node in original graph

## Data analysis

All scripts used in this project is under `scripts` folder. You can use `data_preprocess.py` to reproduce the result.

Graph generation is done in `R`, and you can find the code under `analysis` folder. You can find R package `rdsg` at http://github.com/bxshi/rdsg.
