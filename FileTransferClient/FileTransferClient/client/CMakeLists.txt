# CMakeList.txt: FileTransferServer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)


aux_source_directory(. DIR_CLIENT_SRCS)
add_library(client ${DIR_CLIENT_SRCS})

TARGET_LINK_LIBRARIES(client boost_date_time)
TARGET_LINK_LIBRARIES(client boost_regex)
TARGET_LINK_LIBRARIES(client boost_system)
TARGET_LINK_LIBRARIES(client boost_thread)

target_link_libraries(client boost_filesystem)
TARGET_LINK_LIBRARIES(client boost_date_time)
TARGET_LINK_LIBRARIES(client boost_regex)
TARGET_LINK_LIBRARIES(client boost_system)