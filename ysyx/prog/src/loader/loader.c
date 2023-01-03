#include <am.h>
#include <klib.h>
#include <klib-macros.h>

extern uint32_t app_start;
extern uint32_t app_end;
extern uint32_t _appmem_start;
#define PROG_SIZE ((&app_end) - (&app_start))
#define APP_PMEM 0x80000000

int main()
{
    uint32_t *prog = (uint32_t *)&app_start;
    uint32_t *pmem = (uint32_t *)&(_appmem_start);
    putstr("Loading program of size: ");
    printf("%d bytes, expect 128 \'#\'\n", (uint32_t)PROG_SIZE * sizeof(uint32_t));
    putstr("Loading.....\n");
    uint32_t step = (uint32_t)(&app_end - &app_start) / 128;
    uint32_t *pre = prog;

    while (prog < &app_end)
    {
        *pmem++ = *prog++;
        if ((uint32_t)(prog - pre) >= step)
        {
            putch('#');
            pre = prog;
        }
    }
    putstr("\nLoad finished\nExec app...\n");
    asm volatile("fence.i");
    int (*f)() = (int (*)())(APP_PMEM);
    f();
    return 0;
}
