<!--
 * @Description: 
 * @Author: lize
 * @Date: 2024-09-23
 * @LastEditors: lize
-->

# OneStroke

<p align="center">
  <a>
  <img src="asset//OneStroke.svg" alt="OneStroke" width="100" height="100">
  </a>
</p>

## Introduce

一个基于协程的网络库

## Primary Goal

+ 熟悉并实践协程在c++的使用

## secondary Goal

+ 复习concept及模板元编程的应用
+ 复习Linux网络编程(待总结)

## Minimal Result

+ echo例程

## Feature

+ 协同调度器
+ 使用静态多态(标签派发)的技术实现init_suspend的行为多态
+ 实用协程frame_stack 用来打印协程的调用关系
+ 实用协程run 实现在普通函数中调用协程
+ 实用协程wait_for  实现超时等待的逻辑
+ 实用协程sleep  实现sleep逻辑
+ 实用协程task_schedule 用来并发的开启多个协程
+ 实用协程echo 实现echo网络通信

## TODO

+ loop改造
+ waitqueue数据优化
+ 封装socket
+ 测试
+ 优化代码结构
