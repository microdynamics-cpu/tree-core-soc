#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#define VGA_REG_CTRL *((volatile uint32_t *)0x10002000)
#define VGA_REG_BASE *((volatile uint32_t *)0x10002004)
#define VGA_REG_OFST *((volatile uint32_t *)0x10002008)

int main()
{
    VGA_REG_CTRL = 0;
    putstr("Hello World!\n");
    return 0;
}
