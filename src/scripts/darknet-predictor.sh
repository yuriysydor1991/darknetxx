#!/bin/bash

yoloDataRoot="/usr/share/yolo4-data"

if [[ -z $(which darknet) ]] ; then
  echo "No darknet application found!"
  exit 1
fi

randImage=$(find /var/share/images -type f \( -name '*.png' -o -name "*.jpg" -o -name "*.jpeg" \) | shuf | tail -n 1)

echo "Trying with image: $randImage"

cfgFile="${yoloDataRoot}/cfg/yolov3-tiny_3l.cfg"
dataFile="${yoloDataRoot}/data/obj.data"

if [[ ! -f "${cfgFile}" ]]
then
  echo "No cfg file available in the ${yoloDataRoot}/cfg/ subdirectory"
  exit 1
fi

if [[ ! -f "${dataFile}" ]]
then
  echo "No obj.data file provided in the ${yoloDataRoot}/data subdirectory"
  exit 1
fi

#weightsFile=$(find "${yoloDataRoot}/backup" -name '*_last.weights' -exec realpath {} \;)
weightsFile=$(find "${yoloDataRoot}/backup" -name 'yolov3-tiny_3l_last.weights' -exec realpath {} \;)

if [[ -z "$weightsFile" ]]
then
  echo "Not found and using latest saved weights file in ${yoloDataRoot}/backup"
  exit 1
fi

echo "Command: time darknet detector test ${dataFile} ${cfgFile} ${weightsFile} $randImage"

if ! time darknet detector test "${dataFile}" "${cfgFile}" "${weightsFile}" "$randImage"
then
  echo "Failure to test ${randImage}"
  exit 1
fi
