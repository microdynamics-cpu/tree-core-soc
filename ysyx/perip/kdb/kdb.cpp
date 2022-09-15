
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <cstring>
#include <svdpi.h>
#include <iostream>

extern "C" void ps2_read(char dat)
{
  // Assert(in_flash(addr), "Flash address 0x%lx out of bound", addr);
  // *data = *(uint64_t *)(flash + addr);
  // std::cout << std::hex << "0x" << (int)dat << std::oct << std::endl;
}