#!/bin/python

import os

stud_id = '000000'
app_type = ['flash', 'loader']
app = [('hello', 20), ('memtest', 50), ('rtthread', 350)]


def run_soc_comp():
    os.system('make -C soc all')


def run_lint():
    os.system('make -C lint ')


def run_test():
    for i in app_type:
        for j in app:
            os.system('make -C sim SOC_APP_TYPE=' + i + ' SOC_APP_NAME=' +
                      j[0] + ' SOC_SIM_TIME=' + str(j[1]) + ' test')


run_test()
