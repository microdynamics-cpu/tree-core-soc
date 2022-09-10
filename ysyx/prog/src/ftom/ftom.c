#include <am.h>
#include <klib-macros.h>

int main(){
//  uart_set_baud(115200,50000000);     //晶振为50MHz
//  uart_set_baud(115200,100000000);
//   uart_set_baud(115200,25000000);
//   uart_tr_init();
  putstr("jump-to-sdram\n");
  asm("jr %0" : : "r"(0xfc000000));
}

// int register eax asm("eax") = 0x80000000; // addr is a memory address
// asm("jr *%eax");
// asm volatile("jr a5, %0" : : "r" (0x80000000));
// asm volatile("jr a0, a5" : :"r"(0x80000000));
