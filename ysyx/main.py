#!/bin/python

import os
import sys

stud_id = '040228'  # the last six digits of the student ID

app_type = ['flash', 'loader']
app = [('hello', 20), ('memtest', 50), ('rtthread', 350)]


def run_stand_check():
    os.chdir('stand')
    os.system('./check.py ' + stud_id)
    os.chdir('..')


def run_lint_check(tgt):
    os.system('make -C lint ID=' + stud_id + ' ' + tgt)


def run_comp():
    os.system('make -C sim ID=' + stud_id + ' build')


def run_test():
    for i in app_type:
        for j in app:
            os.system('make -C sim SOC_APP_TYPE=' + i + ' SOC_APP_NAME=' +
                      j[0] + ' SOC_SIM_TIME=' + str(j[1]) + ' test')


def modify_flash_mode(mode):
    if mode == 'fst-test':
        os.system(
            "sed -i 's/^\/\/\(`define FAST_FLASH\)/\1/g' ./perip/spi/rtl/spi.v"
        )
    else:
        os.system(
            "sed -i 's/^\(`define FAST_FLASH\)/\/\/\1/g' ./perip/spi/rtl/spi.v"
        )

def run_soc_comp():
    os.system('make -C soc all')


# NOTE: only allow one param
param = sys.argv[1:2][0]
if param == 'stand':
    run_stand_check()
elif param == 'lint' or param == 'lint-unused':
    run_lint_check(param)
elif param == 'test' or param == 'fst-test':
    modify_flash_mode(param)
    run_comp()
    run_test()
else:
    print('error param')
