#!/bin/bash
for dir in ./*.out; do
    for var in $@; do
        num=0
        while read line
        do
            array=($line)
            if [[ ${array[0]} == *"$var" ]];
            then num=$(( num + ${array[1]} )) 
            fi
        done <$dir
        echo $var " " $num  
    done
done