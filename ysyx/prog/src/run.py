#!/bin/python

import os

APP_NAME = 'memtest'
APP_TYPE = 'mem'  # flash, mem, sdram
APP_ARCH = 'riscv64-mycpu'
APP_ORG_BIN = APP_NAME + '-' + APP_ARCH + '.bin'
APP_ORG_ELF = APP_NAME + '-' + APP_ARCH + '.elf'
APP_LOD_BIN = 'loader-' + APP_ARCH + '.bin'
APP_LOD_ELF = 'loader-' + APP_ARCH + '.elf'
APP_STD_BIN = APP_NAME + '-' + APP_TYPE + '.bin'
APP_STD_ELF = APP_NAME + '-' + APP_TYPE + '.elf'

HOME_DIR = os.getcwd()


def chg_ld_script(type):
    os.system("sed -i 's/core_[a-z]\+/core_" + type +
              "/' $AM_HOME/scripts/riscv64-mycpu.mk")


def chg_ld_addr(addr):
    os.system("sed -i 's/\(pmem_start=\)0x[0-9]\+/\\1" + addr +
              "/' $AM_HOME/scripts/" + APP_ARCH + ".mk")


def copy_oper(type):
    if type == 'flash':
        os.system('mv build/' + APP_ORG_BIN + ' build/' + APP_STD_BIN)
        os.system('mv build/' + APP_ORG_ELF + ' build/' + APP_STD_ELF)

    elif type == 'mem':
        os.system('mv build/' + APP_LOD_BIN + ' build/' + APP_STD_BIN)
        os.system('mv build/' + APP_LOD_ELF + ' build/' + APP_STD_ELF)

    os.system('cp build/' + APP_STD_BIN + ' build/' + APP_STD_ELF + ' ' +
              HOME_DIR + '/../bin/' + APP_TYPE + '/')


if APP_TYPE == 'flash':
    chg_ld_script(APP_TYPE)
    chg_ld_addr('0x30000000')
    os.chdir(APP_NAME)
    os.system('make ARCH=' + APP_ARCH)

elif APP_TYPE == 'mem':
    chg_ld_script(APP_TYPE)
    chg_ld_addr('0x80000000')
    # os.system('cat $AM_HOME/scripts/' + APP_ARCH + '.mk')
    os.chdir(APP_NAME)
    os.system('make ARCH=' + APP_ARCH)
    os.system('cp build/' + APP_ORG_BIN + ' ' + HOME_DIR + '/loader')

    chg_ld_script('flash')
    chg_ld_addr('0x30000000')
    # os.system('cat $AM_HOME/scripts/' + APP_ARCH + '.mk')
    os.chdir(HOME_DIR + '/loader')
    os.system("sed -i 's/^\(BIN_PATH\s\+=\s\+\)\(.\+\)/\\1" + APP_NAME + "-" +
              APP_ARCH + "\.bin/' " + "Makefile")
    os.system('make ARCH=' + APP_ARCH)

copy_oper(APP_TYPE)
