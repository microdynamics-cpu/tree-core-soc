# SoC集成测试Checklist
**进行SoC集成测试的所有步骤都可以在当前目录下完成**，ysyx的目录结构如下所示：
#BUG: need to modify
```sh
ysyxSoC/ysyx
├── README.md                      # SoC集成步骤说明
├── main.py                        # 测试主脚本
├── program                        # 集成测试程序
├── lint
│   ├── Makefile                   # 代码规范检查脚本
│   └── warning.md                 # Verilator中Warning无法清理说明
├── module-prefix
│   ├── AddModulePrefix.scala      # 为Chisel开发的模块名添加前缀的firrtl transform
│   └── README.md                  # transform使用说明
├── perip
│   ├── spi                        # SPI控制器
│   │   ├── doc
│   │   │   └── spi.pdf            # 文档
│   │   └── rtl
│   │       ├── amba_define.v
│   │       ├── spi_clgen.v
│   │       ├── spi_defines.v
│   │       ├── spi_shift.v
│   │       ├── spi_top.v
│   │       └── spi.v              # 顶层文件(包含flash的XIP模式)
│   ├── spiFlash                   # 支持SPI模式的Flash颗粒简化模型
│   │   ├── spiFlash.cpp
│   │   └── spiFlash.v
│   └── uart16550                  # UART16550控制器
│       ├── doc
│       │   └── UART_spec.pdf      # 文档
│       └── rtl
│           ├── raminfr.v
│           ├── timescale.v
│           ├── uart_apb.v         # 顶层文件
│           ├── uart_defines.v
│           ├── uart_receiver.v
│           ├── uart_regs.v
│           ├── uart_rfifo.v
│           ├── uart_sync_flops.v
│           ├── uart_tfifo.v
│           └── uart_transmitter.v
├── ram
│   ├── README.md                  # RAM端口说明
│   ├── S011HD1P_X32Y2D128_BW.v    # 端口与流片用RAM一致的简化行为模型(带写掩码)
│   └── S011HD1P_X32Y2D128.v       # 端口与流片用RAM一致的简化行为模型(不带写掩码)
└── soc
    ├── interface.md               # CPU端口规范说明
    ├── io-check.py                # CPU命名规范自查脚本
    ├── Makefile                   # 用于将Chisel代码编译成ysyxSoCFull.v，同学无需使用
    └── ysyxSoCFull.v              # SoC的Verilog实现
```

同学们执行SoC集成的所有测试任务都可以运行该目录下的`main.py`完成，我们提供的`main.py`脚本包含有端口命名检查、代码规范检查和Verilator程序编译与仿真测试的全部功能。具体来说，每个同学都需要按照顺序进行：

<br></br>
<center>命名规范检查 -> CPU内部修改 -> 代码规范检查 -> Verilator仿真 -> 提交代码</center>
<br></br>

细心的同学可能会发现，`main.py`其实也是分别调用各个子目录下的Makefile或者python脚本来实现的。另外，该集成任务Checklist是按照**任务先后顺序**排列的，所以同学们要确保前面的任务完成后再进行下一个任务。

> 推荐用`python3`而非`python2`运行`main.py`


## 一些准备工作
为了使用进行测试，需要：
* 将CPU代码合并到一个`.v`文件，文件名为`ysyx_学号后六位.v`，如`ysyx_040228.v`
    * 在Linux上可通过`cat`命令实现:
    ```sh
    $> cat CPU.v ALU.v regs.v ... > ysyx_040228.v
    ```
* 将CPU顶层命名修改为`ysyx_学号后六位.v`，如`ysyx_040228`
* 按照[CPU端口命名规范](./stand/interface.md)修改CPU顶层端口名
* 为CPU内的所有模块名添加前缀`ysyx_学号后六位_`
    * 如`module ALU`修改为`module ysyx_040228_ALU`
    * Chisel福利：我们提供一个[firrtl transform](./utils/AddModulePrefix.scala)来自动添加模块名前缀，使用方法参考[相关说明文档](./utils/README.md)
* 对于手动开发的Verilog代码，目前无法进行模块名前缀的自动添加，请手动进行添加
* **将改好的`ysyx_040228.v`放到的soc/目录下**
* 将`main.py`中的`stud_id`**设置为学号的后六位**，比如学号为22040228的同学，设置`stud_id='040228'`

## 命名规范检查(北京时间 2022/10/07 23:59:59前完成)
运行脚本执行命名规范检查，该脚本会检测设计的.v文件是否符合命名规范，并会生成日志文件`check.log`。可执行的测试环境为`Debian10`、`Ubuntu 20.04`、 `WSL2-Ubuntu 20.04`和`Windows10`。
* 在当前目录下运行`./main.py stand`
* 最后可以在终端看到检测结果，如果检查通过，则会在终端打印出：
    ```sh
    $> Your core is fine in module name and signal interface
    ```
* 同时，在该目录下会生成日志文件`check.log`。如果检测未通过，则会给出错误信息，并提示是`module name`错误还是`signal interface`错误。也可以打开⽬录下⽣成的log日志⽂件查看报错原因和提示。
* 注意：处理器核的端口定义要严格按照[CPU端口命名规范](./stand/interface.md)来修改，不能在端口中有多余的注释、`wire`和其他的自定义信号，否则可能会导致后续的任务出现问题。

## CPU内部修改(北京时间 2022/10/07 23:59:59前完成)
* 所有触发器都需要带复位端，使其复位后带初值
    * **Chisel福利：可以通过以下命令对编译生成的`.fir`文件进行扫描，找出不带复位端的寄存器：**
    ```sh
    $> grep -rn "^ *reg " xxx.fir | grep -v "reset =>"
    ```
    其中`xxx.fir`的文件名与顶层模块名相关，通常位于`build/`目录下。若上述命令无输出，说明所有寄存器已经带上复位端。
* 另外Cache还需要：
    * 确认ICache和DCache的data array的大小均不大于4KB，总和不大于8KB
    * 确认ICache和DCache的data array均采用单口RAM
    * 对data array进行端口替换：我们提供端口与流片用RAM一致的简化行为模型，采用的是[带写掩码的单口RAM模型](./ram/S011HD1P_X32Y2D128_BW.v)。`ysyxSoCFull.v`已经在顶层集成了8个带写掩码的RAM，每个RAM的规格是128X64bits，共8KB。**同学们需要将Cache替换成访问核顶层的SRAM端口**。
    * 注意：实现大于8KB Cache的核要进行删减，小于8KB的仍要保留端口。实现小于8KB的核将核顶层不用的SRAM端口置常数(输入悬空，输出置`0`)。需要使用的SRAM中若不用写掩码，则需置`1`。另外tag array无需替换，**而且不允许在核内自行例化SRAM进行其他设计**，同时需要同学们自行维护程序加载时的Cache的一致性。具体RAM的端口定义请见[这里](./ram/README.md)。
* 若采用Verilog开发，则需要：
    * 确认代码中的锁存器(Latch)已经去除
        * **Chisel福利：Chisel不会生成锁存器**
    * 确认代码中的异步复位触发器已经去除，或已经实现同步撤离
        * **Chisel福利：Chisel默认生成同步复位触发器**
* 对于除了SRAM口之外，其他不使用的核顶层端口(io_interrupt和AXI4 slave)，需要将输出端口置`0`，输入端口悬空。


## 代码规范检查(北京时间 2022/10/07 23:59:59前完成)
对代码进行规范检查，并清除报告中的Warning。具体步骤如下：
* 运行`./main.py lint`，Verilator将会报告除`DECLFILENAME`和`UNUSED`之外所有类别的Warning，你需要修改代码来清理它们。Warning的含义可以参考[Verilator手册的说明](https://veripool.org/guide/latest/warnings.html#list-of-warnings)。
* 运行`./main.py lint-unused`，Verilator将会额外报告`UNUSED`类别的Warning，你需要修改代码来尽最大可能清理它们。
* 若某些`UNUSED`类别的Warning无法清理，需要填写`./lint`目录中的[warning.md](./lint/warning.md)并给出原因，用于向SoC团队和后端设计团队提供参考。其中[warning.md](./lint/warning.md)中已经给出了格式范例，同学们填写时可以删除。

## Verilator仿真(北京时间 2022/10/07 23:59:59前完成)
对代码使用verilator进行集成测试与仿真，其中SoC的地址空间分配如下：
| 设备 | 地址空间 |
| --- | --- |
| Reserve           | `0x0000_0000~0x01ff_ffff`|
| CLINT             | `0x0200_0000~0x0200_ffff`|
| Reserve           | `0x0201_0000~0x0fff_ffff`|
| UART16550         | `0x1000_0000~0x1000_0fff`|
| SPI               | `0x1000_1000~0x1000_1fff`|
| VGA               | `0x1000_2000~0x1000_2fff`|
| PS2               | `0x1000_3000~0x1000_3fff`|
| Reserve           | `0x1000_4000~0x2fff_ffff`|
| SPI-flash XIP Mode| `0x3000_0000~0x3fff_ffff`|
| ChipLink MMIO     | `0x4000_0000~0x7fff_ffff`|
| MEM               | `0x8000_0000~0xfbff_ffff`|
| SDRAM             | `0xfc00_0000~0xffff_ffff`|

其中:
* 处理器的复位PC需设置为`0x3000_0000`，第一条指令从flash中取出。
* CLINT模块位于处理器核内部，SoC不提供，需要同学们自行实现。
* 接入外部中断需要同学们**自行设计仲裁逻辑**。(核的top层已经预留有io_interrupt接口， 该口会从SoC引出并通过ChipLink接入到FPGA中。同学们需要自行在FPGA上实现PLIC。核在接收到中断会jump到异常处理程序，之后通过读ChipLink MMIO的相关寄存器来查看中断源信息并响应。异常处理完后可以通过写ChipLink MMIO的相关寄存器来清除中断源，外部中断功能是可选实现的，但不实现的话仍需保留io_interrupt接口)。
* 接入同学们自行设计的设备需要核内实现并将设备寄存器分配到**Reserve地址范围内**。
* 注意：地址空间中内没有设置与SoC时钟和管脚相关的功能寄存器，**即不支持通过软件访问某个确定地址来设置SoC相关参数**。

Verilator仿真测试的具体要求和步骤如下：
* 
* 确认清除Warning后的代码可以成功启动RT-Thread
* 通过快速模式(跳过SPI传输，不可综合，适合快速调试和迭代)对flash进行模拟，运行并通过本框架提供的测试程序。为了打开flash的快速模式，你需要在`./perip/spi/rtl/spi.v`的开头定义宏`FAST_FLASH`:
  ```verilog
  // define this macro to enable fast behavior simulation
  // for flash by skipping SPI transfers
  `define FAST_FLASH
  ```
  * 直接在flash上运行的程序(位于`./prog/bin/flash`目录下)：
    * hello-flash.bin
    * memtest-flash.bin
    * rtthread-flash.bin
  * 通过loader把程序加载到memory，然后跳转运行(位于`./prog/bin/loader`目录下)。**注意需要额外实现`fence.i`指令**
    * hello-loader.bin
    * memtest-loader.bin
    * rtthread-loader.bin
* 通过正常模式(不跳过SPI传输，仿真速度慢，用于最终的系统测试)对flash进行模拟，重新运行上述测试程序。你需要在`./perip/spi/rtl/spi.v`的开头取消对宏`FAST_FLASH`的定义:
  ```verilog
  // define this macro to enable fast behavior simulation
  // for flash by skipping SPI transfers
  // `define FAST_FLASH
  ```
    * 然后再重新运行上面提到的3个flash测试程序和3个loader测试程序：
        * hello-flash.bin
        * memtest-flash.bin
        * rtthread-flash.bin
        * hello-loader.bin
        * memtest-loader.bin
        * rtthread-loader.bin
* 为了方便同学们进行Verilator测试，我们**已经在`main.py`中实现了回归测试**。同学们只需要运行`./main.py fst-test`和`./main.py test`就可以依次执行编译、和快速模式与正常模式下的所有测试。
* 注意：若为了正确运行测试程序而对处理器核进行了修改，**需要重新按照上述流程从头开始依次执行并通过**。

## 提交代码(北京时间 2022/10/07 23:59:59前完成)
注意：此处提交是为了尽快运行综合流程并发现新问题，此后可以继续调试处理器的实现。

提交方式参考[这里](https://github.com/OSCPU/oscpu-framework/blob/2021/README.md)的"代码上传"小节.

## 协助SoC团队在流片仿真环境中启动RT-Thread(北京时间 2022/11/07 23:59:59前完成)
提交代码后，具体请关注SoC团队的反馈。

需要注意的是， **本项目中的SoC只用于在verilator中验证， 不参与流片环节!
此外本项目与流片SoC仿真环境仍然有少数不同，
在本项目中通过测试， 不代表也能通过流片SoC仿真环境的测试，
在流片SoC仿真环境中的运行结果， 以SoC团队的反馈为准， 因此请大家务必重视SoC团队的反馈.**
具体地， 两者的不同之处包括:
* 没有不定态(x态)信号传播的问题
* 没有跨时钟域和异步桥
* 没有PLL

**注意：以下内容不是同学们必须要完成的任务，而是给那些对我们提供的SoC仿真框架有定制化需求的同学们提供的。下面分别介绍了生成自己的verilator仿真程序所需要的必要步骤和使用chisel生成ysyxSoCFull框架的过程和注意要点。**
## ysyxSoCFull定制集成步骤
1. 将`ysyxSoC/ysyx/peripheral`目录及其子目录下的所有`.v`文件加入verilator的Verilog文件列表
2. 将`ysyxSoC/ysyx/soc/ysyxSoCFull.v`文件加入verilator的Verilog文件列表
3. 将处理器Verilog文件加入verilator的Verilog文件列表
4. 将`ysyxSoC/ysyx/peripheral/uart16550/rtl`和`ysyxSoC/ysyx/peripheral/spi/rtl`两个目录加入包含路径中(使用verilator的`-I`选项)
5. 将`ysyxSoC/ysyx/peripheral/spiFlash/spiFlash.cpp`文件加入verilator的C++文件列表
6. 将处理器的复位PC设置为`0x3000_0000`
7. 在verilator编译选项中添加`--timescale "1ns/1ns"`
8. 在verilator初始化时对flash进行初始化， 有以下两种方式:
   * 调用`spiFlash.cpp`中的`flash_init(img)`函数， 用于将bin文件中的指令序列放置在flash中，
     其中参数`img`是bin文件的路径， 在`ysyxSoC/ysyx/program/bin/flash`和
     `ysyxSoC/ysyx/program/bin/loader`目录下提供了一些示例
   * 调用`spiFlash.cpp`中的`flash_memcpy(src， len)`函数， 用于将已经读入内存的指令序列放置在flash中，
     其中参数`src`是指令序列的地址， `len`是指令序列的长度
9.  将`ysyxSoCFull`模块(在`ysyxSoC/ysyx/soc/ysyxSoCFull.v`中定义)设置为verilator仿真的顶层
10. 将`ysyxSoC/ysyx/soc/ysyxSoCFull.v`中的`ysyx_000000`模块名修改为自己的处理器模块名
11. 通过verilator进行仿真即可


## 自己编译并生成ysyxSoCFull.v步骤
1. 更新并拉取当前仓库的子模块
    ```sh
    $> git submodule update --init
    ```
2. 指定RISCV环境变量为工具链的安装目录，为`riscv64-unknown-elf-xxx`开发版的根目录
    ```sh
    $> export RISCV=/path/to/riscv/toolchain/installation
    ```
3. 进入到/ysyx目录下执行`./main.py soc`编译SoC框架

    ```sh
    ysyxSoC/src/main/scala/ysyx
    ├── chiplink
    │   └── ...                        # ChipLink的实现
    └── ysyx
        ├── AXI4ToAPB.scala            # AXI4-APB的转接桥， 不支持burst， 且只支持4字节以下的访问
        ├── ChipLinkBridge.scala       # ChipLink-AXI4的转接桥
        ├── CPU.scala                  # CPU wrapper， 将会按照SoC端口规范实例化一个CPU实例
        ├── SoC.scala                  # SoC顶层
        ├── SPI.scala                  # SPI wrapper， 将会实例化verilog版本的SPI控制器
        └── Uart16550.scala            # UART16550 wrapper， 将会实例化verilog版本的UART16550控制器
    ```

注意：编译时需要使用Java 11，高版本的Java会抛出异常