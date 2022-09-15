.section entry, "ax"
.globl _start
.type _start, @function

_start:
  mv s0, zero
  la sp, _stack_pointer
  jal _trm_init


##### sdram #####
  # "li sp, 0xfc008000" 替换 "la sp, _stack_pointer"
  
#### chenlu
# "li sp, 0xfc008000ul" 替换 "la sp, _stack_pointer"