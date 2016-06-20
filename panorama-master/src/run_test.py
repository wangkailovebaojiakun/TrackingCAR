#!/usr/bin/env python2
# -*- coding: UTF-8 -*-
# File: run_test.py
# Author: Yuxin Wu <ppwwyyxx@gmail.com>

import os
import glob
import subprocess
import re

EXEC = './image-stitching'
THRESHOLD = 0.9

def good_size(x_test, x_truth):
    ratio = x_test * 1.0 / x_truth
    if ratio > 1:
        ratio = 1.0 / ratio
    return ratio > THRESHOLD

def test_final_size(image_globs, w, h):
    print "Testing with {}".format(image_globs)
    images = sorted(glob.glob(image_globs))
    cmd = [EXEC] + images
    outputs = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
    outputs = outputs.split('\n')
    for line in outputs:
        if 'Final Image Size' in line:
            m = re.match(r'.*\(([0-9]+), ([0-9]+)\)', line)
            ww, hh = map(int, m.group(1, 2))
            if good_size(ww, w) and good_size(hh, h):
                return
            else:
                print "Test Failed!"
                sys.exit(1)

if __name__ == '__main__':
    #test_final_size('../data/myself/small*', 5220, 853)
    test_final_size('../data/apartment/small*', 3442, 572)
    print "Tests Passed"
