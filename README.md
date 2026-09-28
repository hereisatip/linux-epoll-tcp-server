# Linux Epoll TCP Server

一个基于 C++17 和 Linux 的模块化 TCP 服务器，用于学习和实践 Linux 系统编程、网络 I/O 和并发服务器设计。

## Features

- 非阻塞 TCP Socket
- epoll ET 事件驱动
- 多客户端连接
- TCP 半包、粘包处理
- 部分写入处理
- PING/PONG 心跳
- 普通文本消息响应
- CALC 异步计算任务
- 固定大小线程池
- 有界任务队列和过载拒绝
- eventfd 线程间通知
- connection_id 防止 fd 复用导致结果误投递
- 客户端空闲超时处理
- SIGINT/SIGTERM 优雅退出
- CMake 构建
- CTest 单元测试
- 单连接和多连接性能测试

## Technology Stack

- C++17
- Linux
- TCP Socket
- Non-blocking I/O
- epoll ET
- ThreadPool
- eventfd
- CMake
- CTest

## Project Structure

```text
include/    头文件
src/        服务器源码
tests/      单元测试
.vscode/    VS Code 配置
