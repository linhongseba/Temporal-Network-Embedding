all: edge2big edge2matlab edge2standard edge2wstandard matlab2wstandard TSV2wstandard TSV2list standard2edge wstandard2edge

edge2big: 
	g++ edge2big.cpp -O3 -o edge2big

edge2matlab: 
	g++ edge2matlab.cpp -O3 -o edge2matlab
	

edge2standard: 
	g++ edge2standard.cpp -O3 -o edge2standard

edge2wstandard:
	g++ edge2weightstandard.cpp -O3 -o edge2wstandard
	
matlab2wstandard: 
	g++ matlab2weightstandard.cpp -O3 -o matlab2wstandard
	
TSV2wstandard: 
	g++ tsv2weightstandard.cpp -O3 -o TSV2wstandard


TSV2list: 
	g++ ReadpredictTSV2list.cpp -O3 -o TSV2list

standard2edge: 
	g++ unweightstandard2edge.cpp -O3 -o standard2edge

wstandard2edge: 
	g++ weightstandard2edge.cpp -O3 -o wstandard2edge

clean:
	rm -f $(all)
