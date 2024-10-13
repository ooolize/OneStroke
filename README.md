<!--
 * @Description: 
 * @Author: lize
 * @Date: 2024-09-23
 * @LastEditors: lize
-->

# ZhouboTong

<p align="center">
  <a>
  <img src="asset//ruler.svg" alt="Planck" width="100" height="100">
  </a>
</p>




## Introduce

基于协程的网络库

## Feature


## 问题

为什么要记录调用者的continue_handle 协程调用完后不会回到调用处继续执行吗，为什么还要主动恢复调用者


问题1： 为什么会segmentation fault ，堆栈是在destory中

协程帧在suspend_final处提供一个选择，我们是否要在这里挂起？ 
如果是 后续需要手动destory  即我们使用Task类型 托管协程帧
如果不是 那就不要挂起，协程会自动销毁并返回调用者


问题2： 为什么 resume中 必须判断handle.done() 否则就算没结束也会报错

因为没有到挂起点 co_await co_yield
```c++

 void resume() {
    if (_coroutine_handle.done()) {
      return;
    }
    _coroutine_handle.resume();
  }

Task test_sleep() {
  co_return 42;
  std::cout << "Inside coroutine\n";
  co_return 43;
}
```

问题3: 为什么注释是错误?
```c++
int main() {

  Task p = test_sleep();
  // auto promise = p._coroutine_handle.promise();
  std::cout << "before resume\n";
  p.resume();
  std::cout << "after resume\n";
  // std::cout << "ret = " << promise.get_return_value() << std::endl;
  std::cout << "ret = " << p._coroutine_handle.promise().get_return_value()
            << std::endl;
  return 0;
}


问题4： 如果在协程中调用return 会发生什么

导致未定义行为，应使用co_return 。

在 C++ 协程中，如果你不显式使用 co_return，协程仍然会正常结束。C++ 协程有默认的行为来处理这种情况。根据返回类型和promise类型

问题5: 进程A开启一个jthread死循环线程B A退出了 线程B还能共享静态变量吗

不能，线程共享进程除栈空间以外的空间，当进程退出后，地址映射失效，很可能拿到的是错误。
内存损坏？ 变量？ 进程？ 


问题5: 为什么这是错误的
```c++
int main() {
  GetSchedule::get_instance().Start();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // 这是错误的 为什么
  // test_sleep();

  // 这才是正确的
  auto task1 = test_sleep();

  fmt::print("do some thing\n");

  std::this_thread::sleep_for(std::chrono::seconds(10));
  return 0;
}
```
问题6： 子协程退出会自动返回到父协程吗 那为什么还需要子协程保存父协程的句柄并恢复

子协程退出是会自动返回给父协程，那是final_suspend是suspend_never的情况下。 如果是suspend_always,就需要在task中RAII的回收句柄，这也意味着需要手动恢复父协程，所以才有了fianlawaiter



问题7: run中 为什么sleep_resume在后面
```c++
before test3
before sleep
await_resume
Inside coroutine
coroutine done
sleep_resume
after test3
```

问题8: 为什么会死锁

在恢复(run)deep1后（注意run动作现在是持有锁的） 
此时又co_await deep2(await_suspend里要把任务放入队列，此时又获取一遍锁)
最终的效果是获取的两次锁 所以死锁了

解决方法 改变锁的粒度

问题9: 当调用co_await时候 是先创建并执行deep2 还是进入deep2的await_ready

这也是符合直觉的不是吗 先deep3()生成Task对象 再调用co_await运算符


```c++
==597729== Thread #2: Exiting thread still holds 1 lock
==597729==    at 0x4C99F60: futex_wait (futex-internal.h:146)
==597729==    by 0x4C99F60: __lll_lock_wait (lowlevellock.c:49)
==597729==    by 0x4CA10F0: lll_mutex_lock_optimized (pthread_mutex_lock.c:48)
==597729==    by 0x4CA10F0: pthread_mutex_lock@@GLIBC_2.2.5 (pthread_mutex_lock.c:93)
==597729==    by 0x4851274: ??? (in /usr/libexec/valgrind/vgpreload_helgrind-amd64-linux.so)
==597729==    by 0x118A52: __gthread_mutex_lock(pthread_mutex_t*) (gthr-default.h:749)
==597729==    by 0x11A4F4: std::mutex::lock() (std_mutex.h:113)
==597729==    by 0x118BC2: std::lock_guard<std::mutex>::lock_guard(std::mutex&) (std_mutex.h:249)
==597729==    by 0x1184B4: lz::ZhouBoTong::Schedule::schedule_now(lz::ZhouBoTong::HandleInfo) (src/schedule.cpp:16)
==597729==    by 0x11488E: lz::ZhouBoTong::Task<void>::await_suspend(std::__n4861::coroutine_handle<lz::ZhouBoTong::Task<void>::Promise>) (include/task.h:146)
==597729==    by 0x112950: deep1(lz::ZhouBoTong::WaitForInit) [clone .resume] (src/main.cpp:61)
==597729==    by 0x1155C7: std::__n4861::coroutine_handle<void>::resume() const (coroutine:135)
==597729==    by 0x11545B: lz::ZhouBoTong::CoRoutineHandler::run() (include/interface/handle.h:47)
==597729==    by 0x1189D8: lz::ZhouBoTong::Schedule::loop() (src/schedule.cpp:72)
```
由于选择了RAII 所以增加手动在final_suspend恢复父协程
在

co_await让出父协程 并在子协程(awaiter)的await_suspend中有机会恢复 或者保存下来择机恢复
co_await是否让出父协程取决于awaiter的实现 

+ 为何cancel会失效

// co_await task await_suspend
// final_suspend
恢复父协程两次 一次cancel

## TODO

+ loop改造
+ final恢复调用者 以及 构造时保存调用者
+ run
+ wait_for
+ frame_stack
+ read




+ promise类型的构造 标签派发 使得init_suspend不再阻塞
+ task_schedule的参数传递 

想好数据怎么存放 再确定关系