#!/bin/bash
#
# The same integration test of the whole darknetxx executable as the
# ITEST_digits one, driven by this script alone with no test framework. The
# train command trains a tiny network from scratch to detect the digit glyphs
# of the misc/data/labels images, then the recognize command runs the darknetxx
# detection on every glyph of a digit and expects that digit to be recognized:
#
#   ITEST_digits_bash.sh train <darknetxx> <test dir> <labels dir>
#   ITEST_digits_bash.sh recognize <darknetxx> <test dir> <digit>
#
# The ITEST_digits_bash tests run it for the darknetxx executable of the build
# (see the CMakeLists.txt file next to it), while any other darknetxx
# executable, an installed one included, may be checked by hand the same way.
# Every file of the test stays inside the <test dir>.

set -e -u -o pipefail

# the label images hold every glyph in eight sizes
GLYPH_SIZES=8

# the output of a run is too long to be shown in full, while its end tells the
# most
OUTPUT_TAIL_LINES=100

usage()
{
  echo "Usage: $0 train <darknetxx> <test dir> <labels dir>" >&2
  echo "       $0 recognize <darknetxx> <test dir> <digit>" >&2
  exit 1
}

fail()
{
  echo "FAIL: $*"
  exit 1
}

# The runs change their working directories, so every path given is made
# absolute beforehand.
absolute_path()
{
  if [[ $1 == /* ]] ; then
    echo "$1"
  else
    echo "${PWD}/$1"
  fi
}

print_output_tail()
{
  tail -n "${OUTPUT_TAIL_LINES}" -- "$1"
  echo "The whole output is inside the $1 file"
}

# Prints the "<class id> <confidence>" pair of the most confident object of the
# darknetxx detection JSON file, which holds every object on a line of its own.
# Prints nothing when no object is detected.
most_confident_object()
{
  awk '
    /"class_id":/ && /"confidence":/ {
      class_id = $0
      sub(/.*"class_id":[ \t]*/, "", class_id)
      sub(/[^-0-9].*/, "", class_id)

      confidence = $0
      sub(/.*"confidence":[ \t]*/, "", confidence)
      sub(/[^-+.0-9eE].*/, "", confidence)

      if (best_confidence == "" || confidence + 0 > best_confidence + 0) {
        best_class_id = class_id
        best_confidence = confidence
      }
    }

    END {
      if (best_confidence != "") {
        print best_class_id, best_confidence
      }
    }
  ' "$1"
}

# Trains the network from scratch by the darknetxx executable, so the recognize
# command always checks fresh weights.
train()
{
  local labels_dir
  local output_log="${TRAINING_DIR}/output.log"
  local status=0
  local digit size image

  labels_dir=$(absolute_path "$1")

  # Neither the weights nor the log of a previous run may be mixed up with the
  # fresh ones. The darknetxx appends its log to the file of the working
  # directory and saves the training chart there as well.
  rm -rf -- "${TRAINING_DIR}"
  mkdir -p -- "${DATA_DIR}" "${BACKUP_DIR}"

  # The training data set: every size of every digit glyph, labeled with a
  # single box of the whole image, since the image holds the glyph alone.
  for digit in {0..9} ; do
    echo "${digit}" >> "${NAMES}"

    for ((size = 0; size < GLYPH_SIZES; ++size)) ; do
      image="${DATA_DIR}/digit_${digit}_${size}"

      # the label images are named after the ASCII code of the glyph
      cp -- "${labels_dir}/$((48 + digit))_${size}.png" "${image}.png"
      echo "${digit} 0.5 0.5 1 1" > "${image}.txt"
      echo "${image}.png" >> "${TRAIN_LIST}"
    done
  done

  cp -- "${SCRIPT_DIR}/digits.cfg" "${CFG}"

  # the whole output, the sanitizers reports of the standard error included
  (cd -- "${TRAINING_DIR}" && exec "${DARKNETXX}" --train-cfg "${CFG}" \
    --dont-show) > "${output_log}" 2>&1 || status=$?

  if [[ ${status} -ne 0 ]] ; then
    echo "FAIL: the darknetxx training failed with the ${status} result, the end of its output:"
    print_output_tail "${output_log}"
    exit 1
  fi

  if [[ ! -f ${WEIGHTS} ]] ; then
    echo "FAIL: the darknetxx training saved no ${WEIGHTS} file, the end of its output:"
    print_output_tail "${output_log}"
    exit 1
  fi

  echo "OK: the darknetxx training saved the ${WEIGHTS} file"
}

# Runs the darknetxx detection on a single glyph image and expects the most
# confident object detected to be the digit of the glyph.
check_glyph()
{
  local digit=$1
  local size=$2
  local work_dir=$3
  local name="digit_${digit}_${size}"
  local image="${DATA_DIR}/${name}.png"
  local json="${work_dir}/${name}.json"
  local log="${work_dir}/${name}.log"
  local status=0
  local recognized class_id confidence

  (cd -- "${work_dir}" && exec "${DARKNETXX}" --detect-cfg "${CFG}" \
    --names "${NAMES}" --weights "${WEIGHTS}" --image "${image}" \
    --dont-show --out "${json}") > "${log}" 2>&1 || status=$?

  if [[ ${status} -ne 0 ]] ; then
    echo "FAIL ${image}: the darknetxx detection failed with the ${status} result, the end of its output:"
    print_output_tail "${log}"
    return 1
  fi

  if [[ ! -f ${json} ]] ; then
    echo "FAIL ${image}: the darknetxx detection saved no ${json} file"
    return 1
  fi

  recognized=$(most_confident_object "${json}")

  if [[ -z ${recognized} ]] ; then
    echo "FAIL ${image}: no object detected, see the ${json} file"
    return 1
  fi

  class_id=${recognized% *}
  confidence=${recognized#* }

  if [[ ${class_id} != "${digit}" ]] ; then
    echo "FAIL ${image}: recognized as ${class_id} with the ${confidence} confidence, see the ${json} file"
    return 1
  fi

  echo "OK ${image}: recognized with the ${confidence} confidence"
}

# Checks the trained network on every glyph of a single digit.
recognize()
{
  local digit=$1
  local work_dir="${DETECTIONS_DIR}/${digit}"
  local failures=0
  local size

  if [[ ! ${digit} =~ ^[0-9]$ ]] ; then
    usage
  fi

  if [[ ! -f ${WEIGHTS} ]] ; then
    fail "no trained ${WEIGHTS} file, the train command is to be run first"
  fi

  # The darknetxx saves the predictions image into the current directory, so
  # every digit gets a directory of its own for the parallel ctest runs.
  # Nothing of a previous run may pass for the results of this one, and the
  # darknetxx appends its log to the file of the working directory.
  rm -rf -- "${work_dir}"
  mkdir -p -- "${work_dir}"

  for ((size = 0; size < GLYPH_SIZES; ++size)) ; do
    if ! check_glyph "${digit}" "${size}" "${work_dir}" ; then
      failures=$((failures + 1))
    fi
  done

  if [[ ${failures} -ne 0 ]] ; then
    fail "${failures} of the ${GLYPH_SIZES} glyphs of the ${digit} digit are not recognized"
  fi

  echo "OK: every glyph of the ${digit} digit is recognized"
}

if [[ $# -ne 4 || ($1 != train && $1 != recognize) || -z $2 || -z $3 ||
  -z $4 ]] ; then
  usage
fi

COMMAND=$1
DARKNETXX=$(absolute_path "$2")
TEST_DIR=$(absolute_path "$3")
SCRIPT_DIR=$(dirname "$(absolute_path "${BASH_SOURCE[0]}")")

# The training runs inside a directory of its own, where the network
# configuration finds the data set files by their relative paths.
TRAINING_DIR="${TEST_DIR}/training"
DATA_DIR="${TRAINING_DIR}/data"
CFG="${TRAINING_DIR}/digits.cfg"
NAMES="${TRAINING_DIR}/digits.names"
TRAIN_LIST="${TRAINING_DIR}/train.txt"
BACKUP_DIR="${TRAINING_DIR}/backup"
# the training names the final weights after the cfg file
WEIGHTS="${BACKUP_DIR}/digits_final.dxxwjz3"
# the working directories of the detection runs
DETECTIONS_DIR="${TEST_DIR}/detections"

if [[ ! -f ${DARKNETXX} || ! -x ${DARKNETXX} ]] ; then
  fail "no darknetxx executable at ${DARKNETXX}"
fi

if [[ ${COMMAND} == train ]] ; then
  train "$4"
else
  recognize "$4"
fi
