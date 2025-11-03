# 工作日志
## 2025.11.3
### 已完成
- 从以前的仓库移植了RC和IMU的代码，稍微重构了一下
### Pending（基础框架part）
- 框图绘制，列出所需的TIMER、CAN、DMA等资源并计算频率
- cubeMX配置（CAN（电机）、USART（RC）、DMA（RC）、SPI（IMU）、TIMER、Watchdog）
- PID，电机父类的移植和接口设计，根据用到的电机型号的datasheet和user manual设计电机子类
- RTOS在keil的配置