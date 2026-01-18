// Boot loader.
//
// Part of the boot block, along with bootasm.S, which calls bootmain().
// bootasm.S has put the processor into protected 32-bit mode.
// bootmain() loads an ELF kernel image from the disk starting at
// sector 1 and then jumps to the kernel entry routine.

#include "types.h"
#include "elf.h"
#include "x86.h"
#include "memlayout.h"
//新增：引入cprintf所需的头文件
#include "defs.h"

#define SECTSIZE  512

void readseg(uchar*, uint, uint);

void
bootmain(void)
{
  struct elfhdr *elf; //ELF文件头指针
  struct proghdr *ph, *eph;//ELF程序段头指针（ph:当前段，eph:尾段）
  void (*entry)(void);//内核入口函数指针
  uchar* pa;//物理内存地址指针
//新增：打印进入bootmain
  cprintf("[BOOT] enter bootmain\n");

//定位内核ELF镜像：将内核加载到物理内存0x10000地址（1MB处）
  elf = (struct elfhdr*)0x10000;  // scratch space

  // 从磁盘读取ELF头部：读取前4096字节（覆盖ELF头和程序段头）
  readseg((uchar*)elf, 4096, 0);
//新增：打印ELF头部加载完成
  cprintf("[BOOT] elf header loaded\n");

  // Is this an ELF executable?
  if(elf->magic != ELF_MAGIC)
    return;  // let bootasm.S handle error魔数错误，启动失败

  // Load each program segment (ignores ph flags).遍历所有ELF程序段，将内核加载到指定物理内存地址
  ph = (struct proghdr*)((uchar*)elf + elf->phoff);
  eph = ph + elf->phnum;
  for(; ph < eph; ph++){
    pa = (uchar*)ph->paddr;
    //读取段的文件内容到物理内存
    readseg(pa, ph->filesz, ph->off);
    // 若段的内存大小 > 文件大小，填充剩余空间为0
    if(ph->memsz > ph->filesz)
      stosb(pa + ph->filesz, 0, ph->memsz - ph->filesz);
  }

  // Call the entry point from the ELF header.
  // Does not return!
  // 新增：打印内核加载完成
  cprintf("[BOOT] kernel loaded\n");
  entry = (void(*)(void))(elf->entry);
  entry();
}

void
waitdisk(void)
{
  // Wait for disk ready.
  while((inb(0x1F7) & 0xC0) != 0x40)
    ;
}

// Read a single sector at offset into dst.
void
readsect(void *dst, uint offset)
{
  // Issue command.
  waitdisk();
  outb(0x1F2, 1);   // count = 1
  outb(0x1F3, offset);
  outb(0x1F4, offset >> 8);
  outb(0x1F5, offset >> 16);
  outb(0x1F6, (offset >> 24) | 0xE0);
  outb(0x1F7, 0x20);  // cmd 0x20 - read sectors

  // Read data.
  waitdisk();
  insl(0x1F0, dst, SECTSIZE/4);
}

// Read 'count' bytes at 'offset' from kernel into physical address 'pa'.
// Might copy more than asked.
void
readseg(uchar* pa, uint count, uint offset)
{
  uchar* epa;

  epa = pa + count;

  // Round down to sector boundary.
  pa -= offset % SECTSIZE;

  // Translate from bytes to sectors; kernel starts at sector 1.
  offset = (offset / SECTSIZE) + 1;

  // If this is too slow, we could read lots of sectors at a time.
  // We'd write more to memory than asked, but it doesn't matter --
  // we load in increasing order.
  for(; pa < epa; pa += SECTSIZE, offset++)
    readsect(pa, offset);
}
