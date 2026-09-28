# Linux Epoll TCP Server

一个基于 C++17 和 Linux 的模块化 TCP 服务器项目，用于学习和实践 Linux 系统编程、非阻塞网络 I/O、epoll 事件驱动、多线程任务处理和服务器工程化开发。

## 项目定位

这是一个学习型、工程实践型 Linux TCP 服务器。

项目从底层 Socket 开始实现，逐步加入非阻塞 I/O、epoll ET、客户端连接管理、协议解析、输入输出缓冲、线程池、eventfd、连接超时、任务队列背压和优雅退出等功能。

项目重点不在于实现复杂业务，而在于理解 Linux TCP 服务器从连接建立到请求处理、响应发送和资源清理的完整流程。

## 主要功能

* 非阻塞 TCP Socket
* epoll ET 边沿触发模式
* 多客户端连接
* `accept()` 循环处理到 `EAGAIN`
* `read()` 循环处理到 `EAGAIN`
* `write()` 循环处理到 `EAGAIN`
* TCP 半包和粘包处理
* 输入缓冲区
* 输出缓冲区
* TCP 部分写入处理
* `EINTR`、`EAGAIN` 和 `EWOULDBLOCK` 处理
* `PING/PONG` 心跳响应
* 普通文本消息响应
* `CALC` 异步计算任务
* 固定大小线程池
* 有界任务队列
* 任务队列满时返回 `ERROR server busy`
* 使用 `eventfd` 通知 epoll 线程处理异步结果
* 使用 `connection\_id` 防止 fd 复用导致结果误投递
* 客户端空闲超时处理
* 客户端超时响应
* `SIGINT` 和 `SIGTERM` 优雅退出
* CMake 构建
* CTest 单元测试
* 单连接和多连接性能测试

## 技术栈

* C++17
* Linux
* TCP Socket
* 非阻塞 I/O
* epoll ET
* `std::thread`
* `std::mutex`
* `std::condition\_variable`
* `std::atomic`
* ThreadPool
* eventfd
* CMake
* CTest

## 项目结构

```text
.
├── .vscode/
│   └── VS Code 配置文件
├── include/
│   ├── client\_session.hpp
│   ├── connection\_manager.hpp
│   ├── epoll\_utils.hpp
│   ├── listen\_socket.hpp
│   ├── protocol.hpp
│   ├── server.hpp
│   ├── socket\_utils.hpp
│   └── thread\_pool.hpp
├── src/
│   ├── connection\_manager.cpp
│   ├── epoll\_utils.cpp
│   ├── listen\_socket.cpp
│   ├── main.cpp
│   ├── protocol.cpp
│   ├── server.cpp
│   ├── socket\_utils.cpp
│   └── thread\_pool.cpp
├── tests/
│   ├── connection\_manager\_test.cpp
│   ├── epoll\_utils\_test.cpp
│   ├── listen\_socket\_test.cpp
│   ├── protocol\_test.cpp
│   ├── socket\_utils\_test.cpp
│   └── thread\_pool\_test.cpp
├── CMakeLists.txt
├── LICENSE
└── README.md
```



## 环境要求

* Linux 或 WSL Ubuntu
* g++，支持 C++17
* CMake 3.16 或更高版本
* pthread
* netcat，可选，用于手动连接服务器
* 安装依赖：
* sudo apt update
* sudo apt install g++ cmake netcat-openbsd

## 编译项目

* 在项目根目录执行：
* cmake -S . -B build -DCMAKE\_BUILD\_TYPE=Debug
* 该命令会读取 CMakeLists.txt，并在 build 目录中生成构建文件。
* 然后执行：
* cmake --build build
* 编译完成后，服务器程序位于：
* build/epoll\_server

## 运行服务器

* 启动服务器：
* ./build/epoll\_server
* 服务器默认监听：
* 127.0.0.1:9000
* 查看监听端口：
* ss -ltnp | grep 9000
* 另开一个终端连接服务器：
* nc 127.0.0.1 9000

## 协议示例

* 每条请求以换行符结束。
* 发送：
* PING
* 返回：
* PONG
* 发送：
* hello
* 返回：
* server received: hello
* 发送：
* CALC 10
* 返回：
* RESULT 55
* 发送：
* CALC 10 100
* 表示计算结果并模拟 100 毫秒业务处理时间。

## 运行测试

* 编译完成后执行全部测试：
* ctest --test-dir build --output-on-failure
* 也可以单独运行测试：
* ./build/protocol\_test
* ./build/connection\_manager\_test
* ./build/thread\_pool\_test
* ./build/socket\_utils\_test
* ./build/listen\_socket\_test
* ./build/epoll\_utils\_test
* 测试覆盖：
* 协议解析和响应
* 连接管理
* connection\_id 分配
* 非阻塞 Socket
* 监听 Socket
* epoll fd 注册和删除
* 线程池任务执行
* 线程池队列满时拒绝任务

## 优雅退出

* 服务器运行时按：
* Ctrl+C
* 服务器会：
* 停止事件循环
* 关闭客户端连接
* 清理连接管理器
* 关闭监听 Socket
* 关闭 epoll fd
* 停止线程池
* 等待工作线程退出
* 退出后检查端口：
* ss -ltnp | grep 9000
* 如果没有输出，说明端口已经释放。

## 架构说明

* 客户端
* |
* v
* 非阻塞 TCP Socket
* |
* v
* epoll I/O 线程
* |
* +-- accept
* +-- read
* +-- write
* +-- ClientSession
* +-- ConnectionManager
* |
* +-- CALC 请求
* &#x20;     |
* &#x20;     v
* &#x20; ThreadPool
* &#x20;     |
* &#x20;     v
* &#x20; 结果队列
* &#x20;     |
* &#x20;     v
* &#x20;   eventfd
* &#x20;     |
* &#x20;     v
* epoll I/O 线程
* &#x20;     |
* &#x20;     v
* &#x20; 返回客户端

* epoll 线程负责 Socket I/O 和客户端状态管理，线程池负责执行耗时的 CALC 业务。工作线程通过结果队列和 eventfd 将结果- 交回 epoll 线程。

## 性能数据

* 以下数据来自 WSL Ubuntu 本机回环测试，服务器逐请求日志已关闭。
* 
* 测试模式	    连接数	     总请求数	     QPS	    平均延迟	  P99 延迟
* PING 单连接	1	        10000	       12919	   77 us	    120 us
* PING 多连接	10	        10000	       40137	   247 us	    306 us
* PING 多连接	50	        50000	       39799	   1250 us	    1425 us
* PING 多连接	100	        100000	       39220	   2493 us	    2791 us
* CALC 单连接	1	        10000	       6702	       149 us	    213 us
* CALC 多连接	10	        10000	       24575	   405 us	    516 us
* CALC 多连接	50	        50000	       24422	   2040 us	    2422 us
* CALC 多连接	100	        100000	       23770	   4188 us	    5134 us
* 以上是本机 WSL 环境下的学习和对比数据，不代表生产环境性能。

## 项目限制

* 协议功能较简单
* 暂不支持 HTTP
* 暂不支持 TLS
* 暂无配置文件
* 暂无生产级结构化日志
* 未进行真实 ARM 设备部署
* 未进行长时间稳定性测试
* 暂不支持任务取消
* 暂不支持复杂业务路由

## 后续计划

* 增加 HTTP/1.1 基础协议
* 增加简单路由
* 增加配置文件
* 增加结构化日志
* 增加连接和任务统计
* 增加任务超时与取消
* 增加长期稳定性测试

