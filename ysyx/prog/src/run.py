#!/bin/python

import os

APP_NAME = 'muldiv'
APP_TYPE = 'mem'  # flash, mem
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
              "/' $AM_HOME/scripts/" + APP_ARCH + ".mk")


def chg_ld_addr(addr):
    os.system("sed -i 's/\(pmem_start=\)0x[0-9A-Z]\+/\\1" + addr +
              "/' $AM_HOME/scripts/" + APP_ARCH + ".mk")


def chg_ld_sp(addr):
    os.system("sed -i 's/\(_stack_top = ALIGN\)(0x[0-9A-Z]\+)/\\1(" + addr +
              ")/' $AM_HOME/scripts/platform/core_flash.ld")


def copy_oper(app_type):
    if app_type == 'flash':
        os.system('mv build/' + APP_ORG_BIN + ' build/' + APP_STD_BIN)
        os.system('mv build/' + APP_ORG_ELF + ' build/' + APP_STD_ELF)

    else:
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
    if APP_NAME != 'rtthread':
        os.system('make ARCH=' + APP_ARCH + ' LOAD_TYPE=-DMEM_LOAD')
        os.system('cp build/' + APP_ORG_BIN + ' ' + HOME_DIR + '/loader')

    else:
        os.system('cp main.c rt-thread/bsp/qemu-riscv-virt64/applications/')
        os.chdir('rt-thread/bsp/qemu-riscv-virt64/')
        os.system("sed -i 's/^FLASH = 1/FLASH = 0/' rtconfig.py")
        os.system('scons')
        os.system('cp rtthread.bin ' + HOME_DIR + '/loader/' + APP_ORG_BIN)

    chg_ld_script('flash')
    chg_ld_addr('0x30000000')
    os.chdir(HOME_DIR + '/loader')
    os.system("sed -i 's/^\(BIN_PATH\s\+=\s\+\)\(.\+\)/\\1" + APP_NAME + "-" +
              APP_ARCH + "\.bin/' " + "Makefile")
    os.system('make ARCH=' + APP_ARCH)

if APP_NAME != 'rtthread' and APP_TYPE == 'flash':
    copy_oper(APP_TYPE)
elif APP_TYPE == 'mem':
    copy_oper(APP_TYPE)
