cmake_minimum_required(VERSION 3.13)

include(template-project-sanitizers-regular)

# The ThreadSanitizer can't see the synchronization inside the uninstrumented
# OpenMP runtime (libgomp), so it reports the OpenMP worker threads of every
# parallel loop as the data races. The threads sanitizer build therefore turns
# the OpenMP parallelisation off, which leaves the application threads (the
# stop watcher, the data loading and the training ones) to check.
if (ENABLE_OPENMP)
  message(STATUS "The threads sanitizer build turns the ENABLE_OPENMP option off")
  set(ENABLE_OPENMP OFF)
endif()

set(SANITIZERS_LIST_LINK
  ${SANITIZERS_LIST_LINK}
  -fsanitize=thread 
  -fnon-call-exceptions
)

set(EXTRA_COMPILE_OPTIONS 
  ${EXTRA_COMPILE_OPTIONS}
  -fsanitize=thread 
  -fnon-call-exceptions
)

# The Darknet C core resets the directory compile options and takes the
# SANITIZERS_LIST ones instead, while the data loading and the training threads
# live there.
set(SANITIZERS_LIST
  ${SANITIZERS_LIST}
  -fsanitize=thread
  -fnon-call-exceptions
)

set(SANITIZERS_LIST_LINK
  ${SANITIZERS_LIST_LINK}
  -fsanitize=thread
)

set(
  EXTRA_LINK_OPTIONS 
  ${EXTRA_LINK_OPTIONS} 
  -fsanitize=thread
)
