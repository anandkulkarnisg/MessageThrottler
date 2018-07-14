#!/bin/bash

for iterator in $(seq 1000)
do
	echo -e "A,100001,B,1000,101.50,SINTEXSYMBOL" >> input.txt.full
done

echo -e "A,100004,P,100,103.50,SINTEXSYMBOL" >> input.txt.full
echo -e "A,100005,B,0,103.50,SINTEXSYMBOL" >> input.txt.full
echo -e "A,100006,S,100,0,SINTEXSYMBOL" >> input.txt.full 
echo -e "A,100007,S,100,103.40,SINTEX" >> input.txt.full

