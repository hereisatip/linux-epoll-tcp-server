# CMake generated Testfile for 
# Source directory: /home/curry/linux-learning/epoll_server_rewrite
# Build directory: /home/curry/linux-learning/epoll_server_rewrite/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(protocol_test "/home/curry/linux-learning/epoll_server_rewrite/build/protocol_test")
set_tests_properties(protocol_test PROPERTIES  _BACKTRACE_TRIPLES "/home/curry/linux-learning/epoll_server_rewrite/CMakeLists.txt;140;add_test;/home/curry/linux-learning/epoll_server_rewrite/CMakeLists.txt;0;")
add_test(connection_manager_test "/home/curry/linux-learning/epoll_server_rewrite/build/connection_manager_test")
set_tests_properties(connection_manager_test PROPERTIES  _BACKTRACE_TRIPLES "/home/curry/linux-learning/epoll_server_rewrite/CMakeLists.txt;146;add_test;/home/curry/linux-learning/epoll_server_rewrite/CMakeLists.txt;0;")
add_test(thread_pool_test "/home/curry/linux-learning/epoll_server_rewrite/build/thread_pool_test")
set_tests_properties(thread_pool_test PROPERTIES  _BACKTRACE_TRIPLES "/home/curry/linux-learning/epoll_server_rewrite/CMakeLists.txt;152;add_test;/home/curry/linux-learning/epoll_server_rewrite/CMakeLists.txt;0;")
add_test(socket_utils_test "/home/curry/linux-learning/epoll_server_rewrite/build/socket_utils_test")
set_tests_properties(socket_utils_test PROPERTIES  _BACKTRACE_TRIPLES "/home/curry/linux-learning/epoll_server_rewrite/CMakeLists.txt;158;add_test;/home/curry/linux-learning/epoll_server_rewrite/CMakeLists.txt;0;")
add_test(listen_socket_test "/home/curry/linux-learning/epoll_server_rewrite/build/listen_socket_test")
set_tests_properties(listen_socket_test PROPERTIES  _BACKTRACE_TRIPLES "/home/curry/linux-learning/epoll_server_rewrite/CMakeLists.txt;164;add_test;/home/curry/linux-learning/epoll_server_rewrite/CMakeLists.txt;0;")
add_test(epoll_utils_test "/home/curry/linux-learning/epoll_server_rewrite/build/epoll_utils_test")
set_tests_properties(epoll_utils_test PROPERTIES  _BACKTRACE_TRIPLES "/home/curry/linux-learning/epoll_server_rewrite/CMakeLists.txt;170;add_test;/home/curry/linux-learning/epoll_server_rewrite/CMakeLists.txt;0;")
