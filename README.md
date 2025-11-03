# 资源参数
## 时钟树
- HCLK：168MHz
- APB1 Timer: 84MHz
- APB2 Timer: 168MHz
## TIMER
### TIM7
- From: APB1 Timer
- Purpose: RC USART3 DMA
- Prescaler: 256 - 1
- Counter Mode: Up
- Counter Period: 2625 - 1
- Frequency: 125Hz
## RC串口
### USART3
- Baud Rate: 100000 Bits/s
- Word Length: 9 Bits (including parity)
- Parity: Even
- Stop Bits: 1
### GPIO
- USART3_TX: PC10
- USART3_RX: PC11
### DMA
- Request: USART3_RX
- Direction: Peripheral to Memory
- Priority: Low
# 工作日志
## 2025.11.3
### 已完成
- 从以前的仓库移植了RC和IMU的代码，稍微重构了一下
- RC相关参数cubeMX已配完（USART3，DMA）
### Pending（基础框架part）
- 框图绘制，列出所需的TIMER、CAN、DMA等资源并计算频率
- cubeMX配置（CAN（电机）、SPI（IMU）、TIMER、Watchdog）
- PID，电机父类的移植和接口设计，根据用到的电机型号的datasheet和user manual设计电机子类
- RTOS在keil的配置