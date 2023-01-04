#include <am.h>
#include <klib-macros.h>
#include <klib.h>

#define MEM_ADDR 0x80200000
#define SDRAM_ADDR 0xFC200000
#define ARR_SIZE 0x2000
uint16_t mem_data[ARR_SIZE];
uint16_t sdram_data[ARR_SIZE];

int main(const char *args)
{
    putstr("start test...\n");
    uint16_t *mem_data = (void *)(MEM_ADDR);
    uint16_t *sdram_data = (void *)(SDRAM_ADDR);
    uint16_t len = ARR_SIZE / sizeof(uint16_t);
    for (int i = 0; i < len; ++i)
    {
        mem_data[i] = i;
    }
    putstr("mem tests prepared\n");

    for (int i = 0; i < len; ++i)
    {
        assert(mem_data[i] == i);
    }
    putstr("mem tests passed!!\n");

    for (int i = 0; i < len; ++i)
    {
        sdram_data[i] = mem_data[i];
    }
    putstr("sdram tests prepared\n");

    for (int i = 0; i < len; ++i)
    {
        assert(sdram_data[i] == i);
    }

    putstr("sdram tests passed!!\n");
    putstr("all tests passed!!\n");
    return 0;
}
