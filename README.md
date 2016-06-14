# Temporal-Network-Embedding
The implementation that infers the temporal latent spaces for a sequence of dynamic graph snapshots. For more details, please read our [paper or technical report](#publication).
# Table of Content
- [Publication](#publication)
- [Folder Organization](#fold-organization)
- [Install](#Install)
- [Usage](#Usage)
- [Input and Output](#input-and-output)
- [Example Pipeline](#example-pipeline)

# Publication

1. Zhu, Linhong, Dong Guo, Junming Yin, Greg Ver Steeg, and Aram Galstyan. "Scalable Link Prediction in Dynamic Networks via Non-Negative Matrix Factorization." arXiv preprint arXiv:1411.3675 (2014).
2.  

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
## Compile
        - Enter into each directory, and type make to compile the soure codes to obtain the executable files.
        - Start with test scripts is a good option to make sure everything is well configured.
        
## Usage
   
    
    
# Input and Output
# Example Pipeline

