#!/bin/bash
for i in `seq 1 20`;
do
	`python gen.py > t$i.txt`
done
