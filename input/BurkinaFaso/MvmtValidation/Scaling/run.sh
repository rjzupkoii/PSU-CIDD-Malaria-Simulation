#!/bin/bash

for i in 0.01 0.05 0.10 0.15 0.20 0.25 0.50 0.75 10;
do
  sed 's/#SCALING#/'"$i"'/g' template.yml > scaling-$i.yml
  sed 's/#SCALING#/'"$i"'/g' template.job > $i.pbs
  qsub $i.pbs
done
