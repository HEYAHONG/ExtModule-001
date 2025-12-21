# 工具

主要使用的操作系统:

- Windows 10及更新版本:Keil MDK5开发Core程序。

注意:由于PY32F002B的Flash空间及SRAM空间有限,因此主要使用MDK作为开发环境。

## Keil MDK5

MDK5为集成开发环境，可对MCU的程序进行编译、调试等

MDK5的最低版本为5.29。

### 开发环境配置

安装好Keil MDK5并成功激活后，还需要安装设备支持包[Puya.PY32F0xx_DFP.1.2.0.pack](3rdparty/PY32F002B_Firmware_V1.1.2/Packs/MDK/Puya.PY32F0xx_DFP.1.2.0.pack)

## arm-none-eabi-gcc

arm-none-eabi-gcc主要使用采用newlib-nano作为C运行库的工具，本工程中一般配合CMake使用.

由于PY32的资源有限，一般不推荐直接使用此工具作为生产环境(编译出的bin文件较大),本工程中一般作为测试用(如使用一些IDE作为代码编辑器)。

# 目录说明

- [数字]-[名称]：数字为固件编号，名称为固件名称。
- [3rdparty](3rdparty):第三方源代码。

# 固件列表

- [001-base_mdk](001-base_mdk):基础模板，采用Keil MDK5编译。
- [002-softplc](002-softplc)：软件PLC模板。