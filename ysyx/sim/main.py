#!/bin/python

import os

app = ['hello', 'memtest', 'rtthread']


def run_test():
    for v in app:
        print(v)
    os.system('make test')


run_test()