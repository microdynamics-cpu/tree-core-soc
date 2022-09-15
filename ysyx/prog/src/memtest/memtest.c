#include <am.h>
#include <klib-macros.h>
#include <klib.h>

#define ARR_SIZE 0x2000
uint16_t data[ARR_SIZE];

int main(const char *args)
{
    putstr("start test...\n");
    uint16_t *data = (void *)(0x80200000);
    //  uint16_t* data = (void*)(0xfc200000);
    for (int i = 0; i < ARR_SIZE / sizeof(uint16_t); i++)
    {
        data[i] = i;
    }
    putstr("all tests prepared\n");
    for (int i = 0; i < ARR_SIZE / sizeof(uint16_t); i++)
    {
        assert(data[i] == i);
    }
    putstr("all tests passed!!\n");
    return 0;
}
