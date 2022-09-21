
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <cstring>
#include <svdpi.h>

static uint8_t kdb_val;


extern "C" void kdb_read(char dat)
{
  // Assert(in_flash(addr), "Flash address 0x%lx out of bound", addr);
  // *data = *(uint64_t *)(flash + addr);
  kdb_val = dat;
}

extern "C" void ps2_read(uint8_t *dat)
{
  *dat = kdb_val;
}