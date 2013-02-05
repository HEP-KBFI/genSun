#!/bin/bash

for f in split/*
do
    echo "hadding file "$f
    hadd $f_out.root `cat $f | xargs` >> log &
done
