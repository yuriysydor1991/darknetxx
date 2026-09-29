# Trains the network of the digits integration test from scratch by the
# darknetxx executable, so the ITEST_digits tests always check fresh weights.
# The ITEST_digits_training test runs the script in the `cmake -P` mode with
# the next variables given:
#
#   DARKNETXX  - the darknetxx executable
#   CFG        - the network configuration file
#   WORK_DIR   - the directory of the training run, recreated by every run
#   BACKUP_DIR - the directory the training saves the weights into
#   WEIGHTS    - the final weights file the training is to produce

cmake_minimum_required(VERSION 3.13)

# an empty WORK_DIR would make the erasing below remove the current directory
# for the CMake versions older than the 3.15
foreach(var DARKNETXX CFG WORK_DIR BACKUP_DIR WEIGHTS)
  if ("${${var}}" STREQUAL "")
    message(FATAL_ERROR "No ${var} variable given")
  endif()
endforeach()

# Neither the weights nor the log of a previous run may be mixed up with the
# fresh ones. The darknetxx appends its log to the file of the working
# directory and saves the training chart there as well.
file(REMOVE_RECURSE "${WORK_DIR}")
file(MAKE_DIRECTORY "${BACKUP_DIR}")

# the whole output, the sanitizers reports of the standard error included
set(output_log "${WORK_DIR}/output.log")

execute_process(
  COMMAND "${DARKNETXX}" --train-cfg "${CFG}" --dont-show
  WORKING_DIRECTORY "${WORK_DIR}"
  RESULT_VARIABLE train_result
  OUTPUT_FILE "${output_log}"
  ERROR_FILE "${output_log}"
)

if (NOT train_result EQUAL 0)
  set(failure "The darknetxx training failed with the ${train_result} result")
elseif (NOT EXISTS "${WEIGHTS}")
  set(failure "The darknetxx training saved no ${WEIGHTS} file")
endif()

if (DEFINED failure)
  # the output is too long to be shown in full, while its end tells the most
  set(output_tail_length 8000)
  file(READ "${output_log}" train_output)
  string(LENGTH "${train_output}" train_output_length)

  if (train_output_length GREATER output_tail_length)
    math(EXPR output_tail_start "${train_output_length} - ${output_tail_length}")
    string(SUBSTRING "${train_output}" ${output_tail_start} -1 train_output)
  endif()

  message(FATAL_ERROR "${failure}, the end of its output:\n${train_output}\nThe whole output is inside the ${output_log} file")
endif()
