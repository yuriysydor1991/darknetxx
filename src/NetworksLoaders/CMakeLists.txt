cmake_minimum_required(VERSION 3.13)

# The network weights files and the training projects formats are the ones of
# the darknetxx-datadrivers library (see the
# cmake/enablers/darknetxx-DarknetxxDataDrivers-enabler.cmake). The darknet
# networks of them stay here: the loader of the networks of the cfg files, the
# view of the library over the darknet networks and the rewrite of the edited
# network into the files.
add_subdirectory(OrigDefaultLoader)
add_subdirectory(network-weights)
add_subdirectory(network-rewriter)
