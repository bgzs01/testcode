# CMakeList.txt: FileTransferServer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)

# 将源代码添加到此项目的可执行文件。
add_executable (FileTransferServer "FileTransferServer.cpp" "FileTransferServer.h")

# TODO: 如有需要，请添加测试并安装目标。

include_directories(/usr/local/include)
LINK_DIRECTORIES(/usr/local/lib)
target_link_libraries(FileTransferServer zmq)
target_link_libraries(FileTransferServer event)

find_package(Threads)
target_link_libraries(FileTransferServer ${CMAKE_THREAD_LIBS_INIT})


include_directories(./interface)
target_link_libraries(FileTransferServer interface)

include_directories(./db)
target_link_libraries(FileTransferServer db)

include_directories(./Server)
target_link_libraries(FileTransferServer Server)

include_directories(./protocol)
target_link_libraries(FileTransferServer protocol)

# sub dir
ADD_SUBDIRECTORY(interface)
ADD_SUBDIRECTORY(db)
ADD_SUBDIRECTORY(Server)
ADD_SUBDIRECTORY(protocol)
