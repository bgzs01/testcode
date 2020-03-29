# CMakeList.txt: FileTransferServer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)


aux_source_directory(. DIR_SERVER_SRCS)
add_library(Server ${DIR_SERVER_SRCS})

TARGET_LINK_LIBRARIES(Server boost_date_time)
TARGET_LINK_LIBRARIES(Server boost_regex)
TARGET_LINK_LIBRARIES(Server boost_system)
TARGET_LINK_LIBRARIES(Server boost_thread)