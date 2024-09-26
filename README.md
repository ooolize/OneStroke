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

## TODO

+ loop改造
+ promise类型的构造 标签派发 使得init_suspend不再阻塞
+ task_schedule的参数传递 
+ final恢复调用者 以及 构造时保存调用者