#include <am.h>
#include <klib.h>
#include <klib-macros.h>

int main(){
    int a = 1 * 3;
    printf("%d\n", a);
    assert(a == 3);
    putstr("all tests passed!!\n");
    return 0;
}
