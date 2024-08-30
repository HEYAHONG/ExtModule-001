# 工具

主要使用的操作系统:

- Windows 10及更新版本:Keil MDK5开发Core程序。

注意:由于PY32F002B的Flash空间及SRAM空间有限,因此主要使用MDK作为开发环境。

## Keil MDK5

MDK5为集成开发环境，可对MCU的程序进行编译、调试等

MDK5的最低版本为5.29。

### 开发环境配置

安装好Keil MDK5并成功激活后，还需要安装设备支持包[Puya.PY32F0xx_DFP.1.2.0.pack](3rdparty/PY32F002B_Firmware_V1.1.2/Packs/MDK/Puya.PY32F0xx_DFP.1.2.0.pack)

# 目录说明

- [数字]-[名称]：数字为固件编号，名称为固件名称。
- [3rdparty](3rdparty):第三方源代码。

# 固件列表

- [001-base_mdk](001-base_mdk):基础模板，采用Keil MDK5编译。