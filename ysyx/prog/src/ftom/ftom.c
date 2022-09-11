#include <am.h>
#include <klib-macros.h>

int main(){
  putstr("jump-to-sdram\n");
  asm("jr %0" : : "r"(0xfc000000));
}

// int register eax asm("eax") = 0x80000000; // addr is a memory address
// asm("jr *%eax");
// asm volatile("jr a5, %0" : : "r" (0x80000000));
// asm volatile("jr a0, a5" : :"r"(0x80000000));
