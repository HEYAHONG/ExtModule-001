# 说明

本工程作为基础模板，可使用以下方式编译:

- [MDK](MDK-ARM):安装好MDK环境后打开目录下的工程文件即可编译。
- [GCC](GCC)：采用[CMake](https://cmake.org)管理源代码，一般用于各种IDE打开编译。注意:由于内存资源较少，其编译的二进制可能不能工作。

# 引脚分配

| 引脚 | 默认功能 | 说明                         |
| ---- | -------- | ---------------------------- |
| PC0  | NRST     | 复位，选项字节开启后有效     |
| PB6  | SWDIO    | 调试及烧录引脚，不可用作它用 |
| PB5  | O0       |                              |
| PB4  | I2C_SDA  |                              |
| PB3  | I2C_SCL  |                              |
| PB2  | I0       |                              |
| PB1  | ADC_CH0  |                              |
| PB0  | ADC_CH7  |                              |
| PA0  | I1       |                              |
| PA1  | I2       |                              |
| PA2  | SWCLK    | 调试及烧录引脚，不可用作它用 |
| PA3  | UART_TX  | 主串口                       |
| PA4  | UART_RX  | 主串口                       |
| PA5  | I3       |                              |
| PA6  | ADC_CH3  |                              |
| PA7  | ADC_CH4  |                              |

# 接口说明

## UART

默认情况下运行[modbus](https://modbus.org) rtu从机。串口参数115200 8N1,modbus rtu从机地址默认为1.