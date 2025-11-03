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
- ```CAN1_RX```: PD0
- ```CAN1_TX```: PD1
# 工作日志
## 2025.11.3
### 已完成  
- 从以前的仓库移植了RC和IMU的代码，稍微重构了一下
- RC相关参数cubeMX已配完（USART3，DMA）
- IMU相关参数cubeMX配完（SPI1，GPIO）
- PID类已移植
- Motor父类完成构建，需要检查
- 配了一个电机的CAN，剩下的需要看到硬件再配
### Pending（基础框架part）
- 框图绘制，列出所需的TIMER、CAN、DMA等资源并计算频率
- cubeMX配置（TIMER、Watchdog）
- 根据用到的电机型号的datasheet和user manual设计电机子类
- RTOS在keil的配置