# CMakeList.txt: FileTransferServer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

aux_source_directory(. DIR_INTERFACE_SRCS)
add_library(db ${DIR_INTERFACE_SRCS})

TARGET_LINK_LIBRARIES(db hiredis)
TARGET_LINK_LIBRARIES(db boost_filesystem)
TARGET_LINK_LIBRARIES(db boost_system)
