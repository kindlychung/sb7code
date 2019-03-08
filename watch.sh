#!/usr/bin/env bash

cd build
time make $1 && ../bin/$1

while true; do 
    # inotifywait -e modify,create,delete -r source/ && dub build --combined --build=debug  --parallel  && dub run
    # inotifywait -e modify,create,delete -r source/ && time dub build --build=debug   && dub run
    inotifywait -e modify,create,delete -r ../src/$1/$1.cpp && time make $1 && ../bin/$1
done