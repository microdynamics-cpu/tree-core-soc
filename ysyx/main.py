#!/bin/python

import os
import sys
import argparse

stud_id = '040228'  # the last six digits of the student ID
app_type = ['flash', 'loader']
app = [('hello', 20), ('memtest', 50), ('rtthread', 350)]


def run_stand_check():
    os.chdir('stand')
    os.system('./check.py ' + stud_id)
    os.chdir('..')


def run_lint_check(tgt):
    os.system('make -C lint ID=' + stud_id + ' ' + tgt)


def modify_flash_mode(mode):
    if mode == 'fast':
        os.system(
            "sed -i 's/^\/\/\(`define FAST_FLASH\)/\1/g' ./perip/spi/rtl/spi.v"
        )
    else:
        os.system(
            "sed -i 's/^\(`define FAST_FLASH\)/\/\/\1/g' ./perip/spi/rtl/spi.v"
        )


def run_comp(mode):
    modify_flash_mode(mode)
    os.system('make -C sim ID=' + stud_id + ' build')


def run_test(val):
    print(val)
    for i in app_type:
        for j in app:
            if val[0] == i and val[1] == j[0]:
                os.system('make -C sim SOC_APP_TYPE=' + i + ' SOC_APP_NAME=' +
                          j[0] + ' SOC_SIM_TIME=' + str(j[1]) + ' test')


def run_reg_test():
    for i in app_type:
        for j in app:
            os.system('make -C sim SOC_APP_TYPE=' + i + ' SOC_APP_NAME=' +
                      j[0] + ' SOC_SIM_TIME=' + str(j[1]) + ' test')


def submit_code():
    pass


def run_soc_comp():
    os.system('make -C soc all')


parser = argparse.ArgumentParser(description='OSCPU Season 4 SoC Test')
parser.add_argument('-s',
                    '--stand',
                    help='run interface standard check',
                    action='store_true')
parser.add_argument('-l',
                    '--lint',
                    help='run code lint check',
                    action='store_true')
parser.add_argument('-lu',
                    '--lint_unused',
                    help='run code lint with unused check',
                    action='store_true')

parser.add_argument('-c',
                    '--comp',
                    help='compile core with SoC in normal flash mode',
                    action='store_true')

parser.add_argument('-fc',
                    '--fst_comp',
                    help='compile core with SoC in fast flash mode',
                    action='store_true')

parser.add_argument(
    '-t',
    '--test',
    help=
    'Example: ./main.py -t [flash|loader] [hello|memtest|rttread|keyboard|vga|pal]',
    nargs=2)

parser.add_argument('-r',
                    '--regress',
                    help='run all test in normal flash mode',
                    action='store_true')

parser.add_argument('-fr',
                    '--fst_regress',
                    help='run all test in fast flash mode',
                    action='store_true')

parser.add_argument('-su',
                    '--submit',
                    help='submit code and spec to CICD',
                    action='store_true')

parser.add_argument('-y',
                    '--ysyx',
                    help='compile ysyxSoCFull framework[NOT REQUIRED]',
                    action='store_true')

args = parser.parse_args()
if args.stand:
    run_stand_check()
elif args.lint or args.lint_unused:
    run_lint_check('lint' if args.lint else 'lint-unused')
elif args.comp or args.fst_comp:
    run_comp('normal' if args.comp else 'fast')
elif args.regress or args.fst_regress:
    run_comp('normal' if args.regress else 'fast')
    run_reg_test()
elif args.submit:
    submit_code()
elif args.ysyx:
    run_soc_comp()
else:
    run_test(args.test)
