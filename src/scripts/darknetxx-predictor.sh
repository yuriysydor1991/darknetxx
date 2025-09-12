#!/bin/bash

yoloDataRoot="/usr/share/yolo4-data"

if [[ -z $(which darknetxx) ]] ; then
  echo "No darknetxx application found!"
  exit 1
fi

#randImage=$(find /var/share/images -type f \( -name '*.png' -o -name "*.jpg" -o -name "*.jpeg" \) | shuf | tail -n 1)
#
#echo "Trying with image: $randImage"
#
#if [[ -z "$randImage" ]]
#then
#  echo "No random image found!"
#  exit 1
#fi

cfgFile="${yoloDataRoot}/cfg/yolo4-tiniest.cfg"
dataFile="${yoloDataRoot}/data/obj.data"

if [[ ! -f "$cfgFile" ]] ; then
  cfgFile=$(find "$dataDir/cfg/" -name '*.cfg' | head -n 1) 
  
  if [[ ! -f "$cfgFile" ]] ; then
    echo "Absent cfg file at $cfgFile"
    exit 1
  fi
fi

if [[ ! -f "${dataFile}" ]]
then
  echo "No obj.data file provided in the ${yoloDataRoot}/data subdirectory"
  exit 1
fi

latestSavedWeights=$(find backup -name '*_last.weights' -exec realpath {} \;)
#latestSavedWeights=$(find backup -name 'yolo4-tiniest_296270.weights' -exec realpath {} \;)

if [[ ! -z "$latestSavedWeights" ]]
then
  echo "Found and using latest saved weights file: $latestSavedWeights"
else
  echo "No weights file found!"
  exit 1
fi

declare -i cnt=0
declare -i maxTries=10

for randImage in $(find /var/share/images -type f \( -name '*.png' -o -name "*.jpg" -o -name "*.jpeg" \) | shuf)
do
  echo "Trying with image: $randImage"

  if [[ -z "$randImage" ]]
  then
    echo "No random image found!"
    exit 1
  fi

  if [[ ! -z "${latestSavedWeights}" ]]
  then
    echo "Trying command: time darknetxx --detect-cfg ${cfgFile} --weights ${latestSavedWeights} --image $randImage"

    if ! time darknetxx --detect-cfg "${cfgFile}" --weights "${latestSavedWeights}" --image $randImage
    then
      echo "Failure to train!"
      exit 1
    fi
  fi
  
  cnt=cnt+1
  
  if [[ $cnt -ge $maxTries ]] ; then break; fi
done

exit 0 

if [[ ! -z "${latestSavedWeights}" ]]
then
  echo "Trying command: valgrind --tool=callgrind darknetxx --detect-cfg ${cfgFile} --weights ${latestSavedWeights} --image $randImage"

  if ! valgrind --tool=callgrind darknetxx --detect-cfg "${cfgFile}" --weights "${latestSavedWeights}" --image $randImage
  then
    echo "Failure to train!"
    exit 1
  fi
fi

