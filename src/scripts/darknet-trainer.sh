#!/bin/bash

yoloDataRoot="/usr/share/yolo4-data"
pytorchYolo="/src/pytorch-YOLOv4"
pyvenv=/opt/venv/bin/activate

dataDir="${yoloDataRoot}"

if [[ -z "$dataDir" ]] ; then
  echo "No root YOLO4 data dir path provided!"
  exit 1
fi

if [[ ! -d "$dataDir" ]] ; then
  echo "No ${dataDir} mounted data dir found"
  exit 1
fi

installPrefix=/usr
darknetB="/src/darknetmod"
darknetBD="$darknetB/cmake-build"

if [[ -z $(which darknet) ]] ;
then
  rm -fr "$darknetB/cmake-build"

  if ! cmake -B $darknetBD -S $darknetB -DCMAKE_INSTALL_PREFIX=$installPrefix -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_CUDA=OFF \
    -DINSTALL_BIN_DIR=$installPrefix/bin \
    -DINSTALL_LIB_DIR=$installPrefix/lib
  then 
    echo "Fail to configure custom darknet"
    exit 1
  fi

  if ! cmake --build $darknetBD -j$(nproc)
  then 
    echo "Fail to build custom darknet"
    exit 1
  fi

  if ! cmake --install $darknetBD --prefix $installPrefix
  then
    echo "Fail to install custom darknet"
    exit 1
  fi
fi

subdataDir="$dataDir/data"
dataFile="$subdataDir/obj.data"
#cfgFile="$dataDir/cfg/yolov4-obj.cfg"
cfgFile="$dataDir/cfg/yolov3-tiny_3l.cfg"
yolo4conv137Preset="/usr/share/yolov4.conv.137"
yolo4tinyPreset="/usr/share/yolov4-tiny.conv.29"

if [[ ! -f "$dataFile" ]] ; then
  echo "Absent data file at $dataFile"
  exit 1
fi

if [[ ! -f "$cfgFile" ]] ; then
  echo "Absent data file at $cfgFile"
  exit 1
fi

if [[ ! -f "$yolo4conv137Preset" ]] ; then
  echo "Absent data file at $yolo4conv137Preset"
  exit 1
fi

if [[ ! -f "$yolo4tinyPreset" ]] ; then
  echo "Absent data file at $yolo4tinyPreset"
  exit 1
fi

yoloDataBackupD="${yoloDataRoot}/backup"

if [[ ! -d "$yoloDataBackupD" ]] ; then
  mkdir -vp "$yoloDataBackupD"
  
  if [[ ! -d "$yoloDataBackupD" ]] ; then
    echo "Failure while creating dir: $yoloDataBackupD"
    exit 1
  fi
fi

if [[ -z $(which darknet) ]] ; then
  echo "No darknet application found!"
  exit 1
fi

yoloDST="$yoloDataBackupD/yolov4.pth"

if [[ $(pwd) != "${yoloDataRoot}" ]]
then
  cd "${yoloDataRoot}"

  if [[ $(pwd) != "${yoloDataRoot}" ]]
  then
    echo "$yoloDataRoot is not cwd!"
    exit 1
  fi
fi

latestSavedWeights=$(find backup -name '*_last.weights' -exec realpath {} \;)

if [[ ! -z "$latestSavedWeights" ]]
then
  echo "Found and using latest saved weights file: $latestSavedWeights"
fi

echo "Trying command: time darknet detector train ${dataFile} ${cfgFile} ${latestSavedWeights} -draw-precision"

if ! time darknet detector train "${dataFile}" "${cfgFile}" "${latestSavedWeights}" -draw-precision
then
  echo "Failure to train!"
  exit 1
fi
