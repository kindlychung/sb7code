#!/usr/bin/env bash

cd build
time cmake .. && time make  && ../bin/$1

while true; do 
    # inotifywait -e modify,create,delete -r source/ && dub build --combined --build=debug  --parallel  && dub run
    # inotifywait -e modify,create,delete -r source/ && time dub build --build=debug   && dub run
    inotifywait -e modify,create,delete -r ../src/examples/$1 && time cmake &&  time make && ../bin/$1
done