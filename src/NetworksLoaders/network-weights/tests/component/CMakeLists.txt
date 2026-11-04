cmake_minimum_required(VERSION 3.13)

# The darknet networks of the network weights files formats of the
# darknetxx-datadrivers library: the files the library reads into the darknet
# networks and writes of them through the NetworkWeights view, the very bytes
# the original darknet functions read and wrote.
add_subdirectory(NetworkWeights)
add_subdirectory(OrigWeightsFile)
add_subdirectory(WeightsModel)
add_subdirectory(Dxxwjz1)
add_subdirectory(Dxxwjz2)
add_subdirectory(Dxxwjz2p)
add_subdirectory(Dxxwjz3)
add_subdirectory(Dxxwjz3p)
add_subdirectory(Dxxwjz4)
add_subdirectory(Dxxwjz4p)
add_subdirectory(Yolo4FolderFormat)
add_subdirectory(WeightsConverter)
