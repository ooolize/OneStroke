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

```c++

 void resume() {
    if (_coroutine_handle.done()) {
      return;
    }
    _coroutine_handle.resume();
  }

  Task test_sleep() {
  // co_await co_sleep(std::chrono::seconds(2));
  co_return 42;
  std::cout << "Inside coroutine\n";
  co_return 43;
}

int main() {
  // GetSchedule::get_instance().Start();
  // std::this_thread::sleep_for(std::chrono::seconds(1));
  // func1();
  Task p = test_sleep();
  auto promise = p._coroutine_handle.promise();
  std::cout << "before resume\n";
  // std::this_thread::sleep_for(std::chrono::seconds(1));
  p.resume();
  std::cout << "ret = " << promise.get_return_value() << std::endl;
  std::cout << "after resume\n";

  p.resume();
  std::cout << "ret = " << promise.get_return_value() << std::endl;
  std::cout << "after resume\n";
  // p._coroutine_handle.destroy();
  // std::this_thread::sleep_for(std::chrono::seconds(10));
  return 0;
}

```