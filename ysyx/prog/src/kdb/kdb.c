#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#define PS2_REG_RB *((volatile uint8_t *)0x10003000)

int main(){
    while(1)
    {
        uint8_t tmp = PS2_REG_RB;
        if(tmp) {
            char s[16] = "";
            putstr(strcat(itoa(tmp, s, 16), "\n"));
        }
    }
    
    putstr("all tests passed!!\n");
    return 0;
}
