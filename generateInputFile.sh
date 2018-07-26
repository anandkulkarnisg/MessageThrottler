#!/bin/bash

orderId=100001

getOrderType()
{
if [ "$1" = "N" ]; then
	echo "M"
fi

if [ "$1" = "M" ]; then
	echo "X"
fi

if [ "$1" = "X" ]; then
	echo "N"
fi
}


cat /dev/null > input.txt.full
orderType="N"
for iterator in $(seq 10000)
do
	orderType=$(getOrderType $orderType)
	echo -e "$orderType,$orderId,B,1000,101.50,SINTEXSYMBOL" >> input.txt.full
	orderId=$((orderId+1))
done

echo -e "A,100004,P,100,103.50,SINTEXSYMBOL" >> input.txt.full
echo -e "A,100005,B,0,103.50,SINTEXSYMBOL" >> input.txt.full
echo -e "A,100006,S,100,0,SINTEXSYMBOL" >> input.txt.full 
echo -e "A,100007,S,100,103.40,SINTEX" >> input.txt.full

