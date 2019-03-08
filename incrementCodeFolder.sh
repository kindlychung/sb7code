#!/usr/bin/env bash

if [[ $1 == "-h" ]]; then
echo "Usage:"
echo "incrementCodeFolder.sh "
echo "incrementCodeFolder.sh newchapter"
exit 0
fi
# find the lastest code list folder
lastlist=$(ls -1d src/list* | sort | tail -n1)
# remove prefix and find out list number
listNumber=${lastlist#src/list0}

if [[ $1 == "newchapter" ]]; then
    newChapterNumber=$((listNumber + 100))
    newChapterNumber=$((newChapterNumber / 100))
    echo "Start code list for a new chapter: $newChapterNumber"
    newListName="list0${newChapterNumber}01"
else
    echo "Latest code list number: $listNumber"
    # increment
    let "listNumber++"
    echo "New    code list number: $listNumber"
    newListName="list0$listNumber"
fi


echo "Copying...."
cp -av "$lastlist" "src/$newListName"
mv  -v src/$newListName/*.cpp src/$newListName/$newListName.cpp