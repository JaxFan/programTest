# programTest
In my worklife ,I meet many fantastic frame in C，so I create this repo to record it.
This will keep update

* here's what i will update
* and i will use Chinese to descripe all comments next days.^^


### 目前计划实现的框架：
* 任务执行队列调度机制
* 定时器执行队列
* 批量函数执行处理逻辑


使用方法：
1. 需要cmake3.10及以上
3. 确保当前在根目录programTest下
5. mkdir build  //创建一个build文件夹 进入该build路径
6. cmake ..
7. ./ProgramTest // 进行测试。

支持gdb调试，使用方法：在build路径下
执行：gdb ./ProgramTest即可。

### 框架说明：
* 定时器：专门开辟了一个线程去处理超时下链工作，下链之后进入任务调度机中排队执行，十分优雅可控。
</br>优点：查找、增加删除时间复杂度：O(1) 
</br>避免到处开辟线程去执行定时器，
</br>避免大量相同超时时间的任务同一时刻下链，堆积在一堆。
</br>双向队列链表，支持 O(1) 时间复杂度的情况下找到前驱结点，效率更高（当设备能支持成百上千万个定时器的时候，就能体现出它的威力了。）


### 个人感悟：
做C语言开发应用层，有些东西是更古不变的技能，军工，通信，iot，汽车行业都干过之后，渐渐在吸收公司前辈的技能精华的过程中
不断发现一套产品代码之中一定要衡量一些技术的实现，越早越好。今后将这些技能 一 一 罗列，
虽世殊事异，所以兴怀，其志一也,后之览者，亦将有感于斯文。

* 任务调度框架
* 编译框架设计
* 多产品如何融合在同一套代码中（C++的设计模式）
* 通信模式设计（片内 片外）
* 片外通信协议的制定
* 评估哪些技术能不能用 要不要用
