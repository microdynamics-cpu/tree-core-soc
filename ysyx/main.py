#!/bin/python

import os
import sys

stud_id = '000000'  # the last six digits of the student ID

app_type = ['flash', 'loader']
app = [('hello', 20), ('memtest', 50), ('rtthread', 350)]


def run_stand_check():
    os.system('python stand/check.py ' + stud_id)


def run_lint_check():
    os.system('make -C lint ')


def run_test():
    for i in app_type:
        for j in app:
            os.system('make -C sim SOC_APP_TYPE=' + i + ' SOC_APP_NAME=' +
                      j[0] + ' SOC_SIM_TIME=' + str(j[1]) + ' test')


def run_soc_comp():
    os.system('make -C soc all')


# NOTE: only allow one param
param = sys.argv[1:2][0]

if param == 'stand':
    run_stand_check()
elif param == 'lint':
    run_lint_check()
elif param == 'test':
    run_test()
else:
    print('error param')
