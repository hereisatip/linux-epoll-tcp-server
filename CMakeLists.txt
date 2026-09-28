cmake_minimum_required(VERSION 3.16)

# 定义项目名称和使用的语言。
project(epoll_server_rewrite LANGUAGES CXX)

# 使用 C++17。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找线程库。
# ThreadPool 使用 std::thread、std::mutex 和 condition_variable。
find_package(Threads REQUIRED)

# 统一设置编译警告。
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
)

# =========================
# 主服务器程序
# =========================

# 编译服务器需要的所有源文件。
add_executable(epoll_server
    src/main.cpp
    src/server.cpp
    src/socket_utils.cpp
    src/listen_socket.cpp
    src/epoll_utils.cpp
    src/connection_manager.cpp
    src/protocol.cpp
    src/thread_pool.cpp
)

# 告诉编译器去 include 目录查找头文件。
target_include_directories(epoll_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 链接线程库。
target_link_libraries(epoll_server PRIVATE
    Threads::Threads
)

# =========================
# 协议测试
# =========================

add_executable(protocol_test
    src/protocol.cpp
    tests/protocol_test.cpp
)

target_include_directories(protocol_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# =========================
# 连接管理测试
# =========================

add_executable(connection_manager_test
    src/connection_manager.cpp
    tests/connection_manager_test.cpp
)

target_include_directories(connection_manager_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# =========================
# 线程池测试
# =========================

add_executable(thread_pool_test
    src/thread_pool.cpp
    tests/thread_pool_test.cpp
)

target_include_directories(thread_pool_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(thread_pool_test PRIVATE
    Threads::Threads
)

# =========================
# Socket 工具测试
# =========================

add_executable(socket_utils_test
    src/socket_utils.cpp
    tests/socket_utils_test.cpp
)

target_include_directories(socket_utils_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# =========================
# 监听 Socket 测试
# =========================

add_executable(listen_socket_test
    src/socket_utils.cpp
    src/listen_socket.cpp
    tests/listen_socket_test.cpp
)

target_include_directories(listen_socket_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# =========================
# epoll 工具测试
# =========================

add_executable(epoll_utils_test
    src/epoll_utils.cpp
    src/socket_utils.cpp
    src/listen_socket.cpp
    tests/epoll_utils_test.cpp
)

target_include_directories(epoll_utils_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# =========================
# 启用 CTest
# =========================

enable_testing()

# 注册协议测试。
add_test(
    NAME protocol_test
    COMMAND protocol_test
)

# 注册连接管理测试。
add_test(
    NAME connection_manager_test
    COMMAND connection_manager_test
)

# 注册线程池测试。
add_test(
    NAME thread_pool_test
    COMMAND thread_pool_test
)

# 注册 Socket 工具测试。
add_test(
    NAME socket_utils_test
    COMMAND socket_utils_test
)

# 注册监听 Socket 测试。
add_test(
    NAME listen_socket_test
    COMMAND listen_socket_test
)

# 注册 epoll 工具测试。
add_test(
    NAME epoll_utils_test
    COMMAND epoll_utils_test
)