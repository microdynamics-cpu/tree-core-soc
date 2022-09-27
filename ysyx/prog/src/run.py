#!/bin/python

import os

APP_NAME = 'rtthread'
APP_TYPE = 'flash'  # flash, mem, sdram
APP_ARCH = 'riscv64-mycpu'
APP_ORG_BIN = APP_NAME + '-' + APP_ARCH + '.bin'
APP_ORG_ELF = APP_NAME + '-' + APP_ARCH + '.elf'
APP_LOD_BIN = 'loader-' + APP_ARCH + '.bin'
APP_LOD_ELF = 'loader-' + APP_ARCH + '.elf'
APP_STD_BIN = APP_NAME + '-' + APP_TYPE + '.bin'
APP_STD_ELF = APP_NAME + '-' + APP_TYPE + '.elf'

HOME_DIR = os.getcwd()


def chg_ld_script(app_type):
    os.system("sed -i 's/core_[a-z]\+/core_" + app_type +
              "/' $AM_HOME/scripts/riscv64-mycpu.mk")


def chg_ld_addr(addr):
    os.system("sed -i 's/\(pmem_start=\)0x[0-9]\+/\\1" + addr +
              "/' $AM_HOME/scripts/" + APP_ARCH + ".mk")


def copy_oper(app_type):
    if app_type == 'flash':
        os.system('mv build/' + APP_ORG_BIN + ' build/' + APP_STD_BIN)
        os.system('mv build/' + APP_ORG_ELF + ' build/' + APP_STD_ELF)

    elif app_type == 'mem':
        os.system('mv build/' + APP_LOD_BIN + ' build/' + APP_STD_BIN)
        os.system('mv build/' + APP_LOD_ELF + ' build/' + APP_STD_ELF)

    os.system('cp build/' + APP_STD_BIN + ' build/' + APP_STD_ELF + ' ' +
              HOME_DIR + '/../bin/' + APP_TYPE + '/')


if APP_TYPE == 'flash':
    chg_ld_script(APP_TYPE)
    chg_ld_addr('0x30000000')
    os.chdir(APP_NAME)
    if APP_NAME != 'rtthread':
        os.system('make ARCH=' + APP_ARCH)
    else:
        os.system('cp main.c rt-thread/bsp/qemu-riscv-virt64/applications/')
        os.chdir('rt-thread/bsp/qemu-riscv-virt64/')
        os.system("sed -i 's/^FLASH = 0/FLASH = 1/' rtconfig.py")
        os.system('scons')
        os.system('cp rtthread.bin ' + APP_STD_BIN)
        os.system('cp rtthread.elf ' + APP_STD_ELF)
        os.system('mv ' + APP_STD_BIN + ' ' + APP_STD_ELF + ' ' + HOME_DIR +
                  '/../bin/' + APP_TYPE + '/')

elif APP_TYPE == 'mem':
    chg_ld_script(APP_TYPE)
    chg_ld_addr('0x80000000')
    os.chdir(APP_NAME)
    os.system('make ARCH=' + APP_ARCH)
    os.system('cp build/' + APP_ORG_BIN + ' ' + HOME_DIR + '/loader')

    chg_ld_script('flash')
    chg_ld_addr('0x30000000')
    os.chdir(HOME_DIR + '/loader')
    os.system("sed -i 's/^\(BIN_PATH\s\+=\s\+\)\(.\+\)/\\1" + APP_NAME + "-" +
              APP_ARCH + "\.bin/' " + "Makefile")
    os.system('make ARCH=' + APP_ARCH)

if APP_NAME != 'rtthread':
    copy_oper(APP_TYPE)
