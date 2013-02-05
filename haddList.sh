#!/bin/bash
hadd -O $1.root `cat $1 | xargs`
