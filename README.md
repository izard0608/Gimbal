# 资源参数
## 时钟树
- HCLK(MHz)：168
- APB1 Timer(MHz): 84
- APB1 peripheral(MHz): 42
- APB2 Timer(MHz): 168
## TIMER(previous)
### Due to structural change, TIMER-interrupt-based parts may be modified and taken into RTOS threads and controlled with ```osDelay()```.
### However, these changes will only be adapted after the fundamental framework is established, so the previous TIMER configs are listed here for reference.
### TIM_RC
- From: APB1 Timer
- Purpose: RC USART3 DMA
- Prescaler: 256 - 1
- Counter Period: 2625 - 1
- Frequency(Hz): 125
### TIM_IMU
- From: APB1 Timer
- Purpose: IMU SPI1
- Prescaler: 4 - 1
- Counter Period: 21000 - 1
- Frequency(Hz): 1000
### TIM_MOTOR
#### This project is using a different MCU from previous projects, therefore recording params is meaningless.
- Purpose: Timer interrupt for CAN
- Frequency(Hz): 1000
## RC
### USART3
- Baud Rate: 100000 Bits/s
- Word Length: 9 Bits (including parity)
- Parity: Even
- Stop Bits: 1
### GPIO
- ```USART3_TX```: PC10
- ```USART3_RX```: PC11
### DMA
- Request: USART3_RX
- Direction: Peripheral to Memory
- Priority: Low
## IMU
### SPI1
- Mode: Full-Duplex Master
#### Basic params
- Frame Format: Motorola
- Data Size: 8 Bits
- First Bit: MSB First
#### Clock params
- Prescaler(for Baud Rate): 256
- Clock Polarity: High
- Clock Phase: 2 Edge
#### Advanced params
- CRC Calculation: Disabled
- NSS Signal Type: Software
### GPIO
- ```SPI1_SCK```: PB3
- ```SPI1_MISO```: PB4
- ```SPI1_MOSI```: PA7
- ```CS1_ACCEL```: PA4
- ```CS1_GYRO```: PB0
## Motor(single motor)
### CAN1
#### Different MCU and therefore params need modifying.
- Time for one Bit: 1000 ns
- Baud Rate: 1000000 Bits/s
### NVIC
- CAN1 RX0 interrupts: ENABLE
### GPIO
- ```CAN1_RX```: PA11
- ```CAN1_TX```: PA12
## IWDG
- Countdown time: 1.5s
- Prescaler: 32
- Reload value: 1500 - 1
# 工作日志
## 2025.11.3
### 已完成  
- 从以前的仓库移植了RC和IMU的代码，稍微重构了一下
- RC相关参数cubeMX已配完（USART3，DMA）
- IMU相关参数cubeMX配完（SPI1，GPIO）
- ```PID```类已移植
- ```Motor```父类完成构建，需要检查
- 配了一个电机的CAN，剩下的需要看到硬件再配
### Pending（基础框架part）
- 框图绘制，列出所需的TIMER、CAN、DMA等资源
- cubeMX配置（TIMER、Watchdog）
- 根据用到的电机型号的datasheet和user manual设计电机子类
- RTOS在keil的配置
## 2025.11.4
### 已完成
- 检查了```Motor```抽象类，修改了一些封装和接口
- 看门狗配完
### Pending（基础框架）
- 框图绘制，列出所需的TIMER、CAN、DMA等资源
- TIMER的cubeMX配置
- 设计电机子类，重写CAN读写函数
- keil里配RTOS
## 2025.11.5
### 已完成
- 修改了```Motor```类部分成员的可见性，给```stop_flag_```加了内联修饰
- 完成了```M3508```和```M6020```电机子类的CAN收发接口重写
- 写了Utils库，目前包含线性映射和卡尔曼滤波，使用```utils```命名空间
### Pending
- 框图
- RTOS的keil配置
## 2025.11.7
### 已完成
- keil的RTOS路径配完
- 修bug，keil里编译能过了，ARM C++版本调整为C++17
### Pending
- 框图
- 准备进入下一阶段，写线程
## 2025.11.9
### 已完成
- 大致分了一下线程，内部逻辑还没写
- RC DMA中断完成
- 优化了部分代码
### Pending（线程Part）
- 写各个线程的内部逻辑
- 写CAN recv的中断
- 看门狗和定时器
## 2025.11.10
### 已完成
- 除了主控线程其他线程都写完了
- 把CAN recv的代码封装到class ```Motor```里
### Pending
- 主控线程
- CAN中断，可能得去学创照着硬件写
- 看门狗和定时器
- 在考虑要不要把```Motor``` class里前馈的纯虚函数改成函数指针，不然没办法给每个对象写不同的前馈，破坏封装性和可读性
- 除了主控以外写完了之后先在keil跑通然后merge到dev里
## 2025.11.15
### 已完成
- RTOS_Frame跑通，合并到dev，RTOS_Frame closed
- 主控大部分写完
### Pending
- 主控细节调整
- CAN2中断
- 看门狗和定时器
- pitch机械限位角度
- 有个铸币到现在才意识到接线要自己接（），CAN2配置
- RTOS没考虑线程安全问题，得加锁或者消息队列
## 2025.11.17
### 已完成
- 实际硬件是两个M6020，都用can1通讯，改了下通讯的代码，把中断写好了
- 硬件原因不得不把前馈函数改成了函数指针
- 看门狗完成，TIM7中断喂狗
### Pending
- pitch机械限位角度
- RTOS线程安全
## 2025.11.19
### 已完成
- 修复了部分bug
## 2025.11.20
### 已完成
- 修复了CAN中断bug（时钟树配置问题，修改input frequency后解决）
- 修复了IMU无法读取加速度计数据的bug（初始化中加入延时）
### Pending
- 调PID和前馈
## 2025.11.24
### 已完成
- yaw部分全部完成，可以正常工作
### Pending
- pitch前馈