#!/bin/bash
#
# The YOLO4 folder of the original weights converts into the dxxwjz2p project,
# which keeps the annotations of the images beside the network, the weights
# file of the project keeps no annotations and the project converts back into
# the YOLO4 folder of the copies of the images of the same labels and weights.

. "$(dirname "$0")/common.sh"

itest_init "$@"

FOLDER="${WORK_DIR}/yolo4"

mkdir -p "${FOLDER}/cfg" "${FOLDER}/data/obj" "${FOLDER}/backup"
cp "${REFERENCE_DIR}/1.cfg" "${FOLDER}/cfg/net.cfg"
cp "${REFERENCE_DIR}/1.names" "${FOLDER}/data/obj.names"
cp "${REFERENCE_DIR}/1.weights" "${FOLDER}/backup/net_last.weights"

# the 4x2 images, the one of no object
for IMAGE in a b ; do
    printf 'P6\n4 2\n255\n%024d' 0 > "${FOLDER}/data/obj/${IMAGE}.ppm"
done

printf '3 0.5 0.5 0.25 0.5\n' > "${FOLDER}/data/obj/a.txt"
: > "${FOLDER}/data/obj/b.txt"
printf 'data/obj/a.ppm\ndata/obj/b.ppm\n' > "${FOLDER}/data/train.txt"

itest_run "${CONVERTER_BINARY}" \
  --convert-from yolo4 --convert-src "${FOLDER}" \
  --convert-to dxxwjz2p --convert-dst "${WORK_DIR}/net.dxxwjz2p"

TEXT="$(gzip -dc "${WORK_DIR}/net.dxxwjz2p")"

itest_expect_output_contains "${TEXT}" '"format": "dxxwjz2p"'
itest_expect_output_contains "${TEXT}" '"ImagesAnnotator": {"version": "0.12.0"}'
itest_expect_output_contains "${TEXT}" '{"db": {"path": "yolo4/data/obj"}'
itest_expect_output_contains "${TEXT}" '{"rel_path": "a.ppm", "width": 4, "height": 2'
itest_expect_output_contains "${TEXT}" \
  '[{"name": "class_3", "rect": {"x": 1.5, "y": 0.5, "width": 1, "height": 1}}]'

# the weights of the project alone
itest_run "${CONVERTER_BINARY}" \
  --convert-from dxxwjz2p --convert-src "${WORK_DIR}/net.dxxwjz2p" \
  --convert-to dxxwjz2 --convert-dst "${WORK_DIR}/net.dxxwjz2"

if [[ "$(gzip -dc "${WORK_DIR}/net.dxxwjz2")" == *'"annotations"'* ]] ; then
    itest_fail "The weights file of the project keeps the annotations"
fi

itest_run "${CONVERTER_BINARY}" \
  --convert-from dxxwjz2 --convert-src "${WORK_DIR}/net.dxxwjz2" \
  --convert-to weights --convert-dst "${WORK_DIR}/net.weights"

itest_expect_same_files "${REFERENCE_DIR}/1.weights" "${WORK_DIR}/net.weights"

# the folder of the copies of the images
itest_run "${CONVERTER_BINARY}" \
  --convert-from dxxwjz2p --convert-src "${WORK_DIR}/net.dxxwjz2p" \
  --convert-to yolo4 --convert-dst "${WORK_DIR}/back"

BACK="${WORK_DIR}/back"

itest_expect_output_contains "$(cat "${BACK}/cfg/back.cfg")" 'train=data/train.txt'
itest_expect_same_files "${REFERENCE_DIR}/1.names" "${BACK}/data/obj.names"

for FILE in train.txt obj/a.ppm obj/a.txt obj/b.ppm obj/b.txt ; do
    itest_expect_same_files "${FOLDER}/data/${FILE}" "${BACK}/data/${FILE}"
done

itest_run "${CONVERTER_BINARY}" \
  --convert-from dxxwjz3 --convert-src "${BACK}/backup/back_last.dxxwjz3" \
  --convert-to weights --convert-dst "${WORK_DIR}/back.weights"

itest_expect_same_files "${REFERENCE_DIR}/1.weights" "${WORK_DIR}/back.weights"

# the folder of the files is no destination
itest_run_failing "${CONVERTER_BINARY}" \
  --convert-from dxxwjz2p --convert-src "${WORK_DIR}/net.dxxwjz2p" \
  --convert-to yolo4 --convert-dst "${BACK}"
