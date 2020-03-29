# CMakeList.txt: FileTransferClient 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)

# 将源代码添加到此项目的可执行文件。
add_executable (FileTransferClient "FileTransferClient.cpp" "FileTransferClient.h")

# TODO: 如有需要，请添加测试并安装目标。

include_directories(/usr/local/include)
LINK_DIRECTORIES(/usr/local/lib)
target_link_libraries(FileTransferClient zmq)
target_link_libraries(FileTransferClient event)

find_package(Threads)
target_link_libraries(FileTransferClient ${CMAKE_THREAD_LIBS_INIT})

include_directories(./interface)
target_link_libraries(FileTransferClient interface)
ADD_SUBDIRECTORY(interface)

include_directories(./protocol)
target_link_libraries(FileTransferClient protocol)
ADD_SUBDIRECTORY(protocol)

include_directories(./client)
target_link_libraries(FileTransferClient client)
ADD_SUBDIRECTORY(client)

target_link_libraries(FileTransferClient boost_filesystem)
TARGET_LINK_LIBRARIES(FileTransferClient boost_date_time)
TARGET_LINK_LIBRARIES(FileTransferClient boost_regex)
TARGET_LINK_LIBRARIES(FileTransferClient boost_system)