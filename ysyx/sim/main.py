#!/bin/python

import os

app_type = ['flash', 'loader']
app = [('hello', 20), ('memtest', 50), ('rtthread', 350)]

def run_test():
    for i in app_type:
        for j in app:
            os.system('make SOC_APP_TYPE=' + i + ' SOC_APP_NAME=' + j[0] + ' SOC_SIM_TIME=' + str(j[1]) + ' test')


run_test()
