# Temporal-Network-Embedding (Node2Vec)
The implementation that infers the temporal latent spaces for a sequence of dynamic graph snapshots. For more details, please read our [paper or technical report](#publication).
# Table of Content
- [Publication](#publication)
- [Folder Organization](#fold-organization)
- [Install](#Install)
- [Usage](#Usage)
- [Input and Output](#input-and-output)
- [Example Pipeline](#example-pipeline)

# Publication

1. Linhong Zhu, Dong Guo, Junming Yin, Greg Ver Steeg, and Aram Galstyan. "Scalable Link Prediction in Dynamic Networks via Non-Negative Matrix Factorization." arXiv preprint arXiv:1411.3675 (2014).
2. Linhong Zhu, Dong Guo, Junming Yin, Greg Ver Steeg, and Aram Galstyan. "Scalable Temporal Latent Space Inference for Link Prediction in Dynamic Social Networks." To be appeared in TKDE (2016).
3. Dingxiong Deng, Cyrus Shahabi, Ugur Demiryurek, Linhong Zhu, Rose Yu, Yan Liu. "Latent Space Model for Road Networks to Predict Time-Varying Traffic." To be appeared in ACM SIGKDD Conferences on Knowledge Discovery and Data Mining (SIGKDD), 2016.

# Folder organization
/source_code/

      /document2vector/
  
      an example pipeline that apply the temporal network embedding to perform document to vector embedding on document to word bipartite graphs
      
      /evaluation/
  
      scripts that evaluate the link prediction performance for latent space approach and weighted common neighbore approach AA [1]
      
      /format/
      
      scripts that transform between different input formats
      
      /generator/
      
      scripts that generate pairs of vertices for testing link prediction performance
      
      /main/
      
      the main source code that implements the temporal network embedding. 
      
      /platform_dependent
      
      source codes that are depedent on platforms. Bascially, we have different running time measures for Linux and Windows
      
      /test scripts
      
      unit test scripts 
  
# Install
## Pre-configuration
        - make sure that g++ compiler is pre-installed
        
        - if it is a Linux/Mac Machine, please copy and paste \platform_dependent\linux\Runtimecounter.h to the \main\ directory. If it is a Windows Machine, please copy and paste \platform_dependent\windows\Runtimecounter.h to the \main\ directory.
        
        - Enter into each directory, and open the makefile file with any Text Editor, and change the path to g++ comiler into your local machine g++ compiler path
        
## Compile
        - Enter into each directory, and type make to compile the soure codes to obtain the executable files.
        - Start with test scripts is a good option to make sure everything is well configured.
        
## Usage
   
   ### Temporal NetWork Embedding Usage
   executable file location: source_code/main/BCGDEmbed
   
   Usage: BCGDEmbed graphdir [options]
   graphdir is a String
   
   Options: 
   -t: type of algorithms default t=4
   
   -t:=1 global,=2 global auto, =3 local, =4 local auto, =5 incremental, =6 incremental auto
   
   -c: number_of_dimensions [int default 20]
   
   -m: memorybound (MB) [int default 1024]
   
   -a : alpha>0 [double default 0.00001]
   
   -l : lambda [double default 0.01]
   
   -p : prefix [string]
   
   -i : iteration number [int default 300]
   
   -e : print out step size defalt=100, print out every 100 iters
   
   -b : size of output buffer (>4MB), default 60MB\n"
   
   -z : zeta value (default sqrt(1/nodenum)
   
   -d : delta value (default zeta*2/k\n")
    
   
   
# Input and Output

## Input

Input is a directory of graph files. Each graph file is formatted as follows:

The first line is number of nodes, and starting from the second lins is the adjacence list of each node formated as follows:

node_id,degree_d:neighboreid1,weight1:neighborid2,weight2:...neighboridd,weightd

Note that the node_id is within the range [0,n-1], where n is number of nodes, and the list of neighbors are sorted in ascending order too.

An example of input graph file is as follows:

3

0,2:1,1.0:2,1.0

1,2:0,1.0:2,1.0

2,2:0,1.0:1,1.0

where this graph is a triangle with three vertices, and each edge is with weight 1.0.

Another example is shown in file Infection_8.txt

## Output

Output is a directory of embedding files. Each embedding file is formatted as follows:

The first line is number of nodes, and starting from the seond line is the sparse encoding of latent positions of each node:
node_id,number_non-zero:index1,weight1:index2,weight2:...index_d,weightd

Each index gives the non-zero index of each dimension, and each weight gives the non-zero position for that dimension.

Note that the node_id is within the range [0,n-1], where n is number of nodes, and the indexes are sorted in ascending order too.

## Format Script Usage
   
   To facilitate the usage of the code, we provide a set of format script that transforms different graph formats into input formats of program:
   
### edge2wstandard
   
   Transforms the edge format into the input weighted standard format of program
   
   For the edge format， each line encodes an edge of graph with format:
   
   source_id[]target_id, it can be sepearted by either whitespace or tab
   
   Both the source_id and target_id is using C-index (i.e., Integer starts from 0)
   
   Usage: edge2wstandard [edgegraphfile] [number_of_nodes]
   
   
### matlab2wstandard
   
   Transforms the matlab format into the input weighted standard format of program
   
   For the matlab format， each line encodes an edge of graph with format:
   
   source_id[]target_id[]value, it can be sepearted by either whitespace or tab
   
   Both the source_id and target_id is using matlab-index (i.e., Integer starts from 1)
   
   Usage: matlab2wstandard [edgegraphfile] [number_of_nodes]
   
### TSV2wstandard
   
   Transforms the TSV format into the input weighted standard format of program
   
   For the TSV format， each line encodes an edge of graph with format:
   
   source_id[]target_id[]value, it can be sepearted by either whitespace or tab
   
   Both the source_id and target_id is using C-index (i.e., Integer starts from 0)

# Example Pipeline
Here we will walk through an example of applying the temporal network embedding to learn the vector represetation for both documents and workds simultaneously.

Assume that we have 31 documents located in a directory "document".

     1. we need to go to the directory of /source_code/document2vector/src, using another Java IDE (e.g., eclipse, Neatbean) to compile the source code and generate a jar file (e.g., D2V.jar). Here we require two libraries: lucene-core, kstem.
     
     2. run the Matrix class with the parameter directory name of documents and prun frequence threshold, in this example,
     ``java -cp D2V.jar process/Matrix "document" 2``
     this will prune the word with frequence less than or equal to 2 
     After running this script, it will automatically generate two outputs: words.txt, and Doc2Word.txt
     In the Doc2Word.txt (a bipartite graph between Documents and words), each line is a weighted edge between a document and a word, using the TSV format
     An example of Doc2Word.txt can be found [here](https://github.com/linhongseba/Temporal-Network-Embedding/blob/master/source_code/document2vector/Doc2word.txt)
     In the words.txt, each line is the word integer id and a word string, separated by tab. An example of words.txt can be found [here](https://github.com/linhongseba/Temporal-Network-Embedding/blob/master/source_code/document2vector/words.txt)
     
     3. run the format script:
      ``TSV2wstandard Doc2Word.txt #nodes``
      where #nodes is equal to number of documents plus number of words
      After running the format scrript, it will generate the a new bipartite graph file with name " Doc2Word.txt_new.txt"
      
     4. run the embedding following the instruction specified in Section [Usage](##Usage)
     
        After running the embedding, it will generate the vector representation matrix for words and documents.
        An example of 20-dimension embedding is shown in [Zmatrix0](https://github.com/linhongseba/Temporal-Network-Embedding/blob/master/source_code/document2vector/Zmatrix0), while another example of 50-dimension embedding is shown in [50Zmatrix0](https://github.com/linhongseba/Temporal-Network-Embedding/blob/master/source_code/document2vector/50Zmatrix0).
        
     5. Anaysis using embedding (e.g., clustering/classifying documents)
        Here we provide an example of printing the top-k words of each embedding dimension, like topic modeing
        run the following script:
         ``java -jar D2V.jar process/ReadTopwords [K] [D] [#doc] [#words] [word-file] [embed-file]``
         Here 
              K (integer): is the top K words to be printed for each dimension
         
              D (integer): number of embedding dimensions
              
              #doc (integer): number of documents
              
              #words (integer: number of words
              
              word-file (string): the filename of words (e.g., words.txt)
              
              embed-file (string); the filenmae of embedding results (e.g., Zmatrix0)
              
         The output is the top-K words for each dimension per line
         
              
     
     
    
    




