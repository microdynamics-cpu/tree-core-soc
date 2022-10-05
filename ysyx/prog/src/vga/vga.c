#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#define FB_ADDR 0x1c000000
#define VGA_CTRL_ADDR 0x10002000
#define VGA_FB_ADDR 0x81000000
#define VGA_REG_CTRL *((volatile uint32_t *)(VGA_CTRL_ADDR))
#define VGA_REG_BASE *((volatile uint32_t *)(VGA_CTRL_ADDR + 4))
#define VGA_REG_OFST *((volatile uint32_t *)(VGA_CTRL_ADDR + 8))

int main()
{
    putstr("config VGA module\n");
    // VGA_REG_CTRL = 0;
    // VGA_REG_BASE = VGA_FB_ADDR;
    // VGA_REG_OFST = VGA_FB_ADDR - FB_ADDR;

    // uint32_t write_mode = VGA_REG_CTRL;
    // uint32_t write_base = VGA_REG_BASE;
    // uint32_t write_ofst = VGA_REG_OFST;
    printf("mode: %x base: %x offset: %x\n", 0, 0, 0);

    return 0;
}
