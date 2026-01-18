# xv6操作系统实践：启动流程优化与可视化
## 项目概述
基于MIT xv6操作系统，完成三大核心任务：修复编译递归错误实现系统启动、为启动关键文件添加注释、植入启动可视化打印日志。通过实践掌握x86架构下系统启动链路与Git版本控制、QEMU仿真调试技能。

## 环境准备
- 依赖工具：Git、QEMU、GCC、Make、Vim
- 初始化步骤：Fork MIT xv6仓库，克隆至本地：
  ```bash
  git clone https://github.com/wangqiyu123/xv6-public.git ~/xv6-boot-practice/xv6-public
  cd ~/xv6-boot-practice/xv6-public
  ```
  ![克隆](image-5.png)
  ![正确启动xv6](image-6.png)

## 启动流程说明
1. **BIOS初始化**：上电后BIOS执行硬件自检，加载Bootloader至内存`0x7c00`并移交控制权。
2. **Bootloader引导**：通过`bootasm.S`初始化实模式环境，`bootmain.c`读取解析内核ELF镜像并加载至内存。
3. **内核初始化**：`entry.S`配置内核态环境，`main.c`完成内存管理、中断控制等核心子系统初始化。
4. **用户进程启动**：创建首个用户进程，启动进程调度，终端显示`$`提示符。
[bootasm.S添加注释](image-7.png)
![bootmain.c添加注释](image-8.png)
![entity.s添加注释](image-9.png)
## 运行结果
### 核心命令
```bash
make clean && make qemu  # 编译启动；按Ctrl+A后按X退出
```
### 可视化日志
```
[BOOT] enter bootmain
[BOOT] elf header loaded
[BOOT] kernel loaded
[BOOT] enter kernel entry
[KERNEL] main() started
xv6 kernel is booting
$
```
![可视化日志](image-4.png)
### Git提交记录
- `feat: 修复编译错误，成功启动xv6系统`（修改sh.c、Makefile）
![第一次commit提交](image-1.png)
- `docs: 给启动流程文件添加注释`（修改bootasm.S、bootmain.c、entry.S）
![第二次commit提交](image-2.png)
- `feat: 实现启动可视化`（添加5处打印日志）
![第三次commit提交](image-3.png)
![提交过程](image-10.png)
## 个人总结
实践收获：深入理解x86系统启动链路，熟练运用Git与QEMU工具，掌握汇编与C语言混合编程技巧。解决Git配置、Vim交换文件残留、编译递归错误等问题。后续可扩展调试信息、集成GDB联调，优化日志打印机制。