#!/usr/bin/env python3

import os
from subprocess import call
import platform
import argparse

def is_unix():
    system = platform.system()
    return system == "Darwin" or system == "Linux"

parser = argparse.ArgumentParser()
parser.add_argument('--root', help='Install into /usr/local/bin', action='store_true')
parser.add_argument('--compiler', help='C++ compiler path')
args = vars(parser.parse_args())

octopus_dir = os.path.dirname(os.path.realpath(__file__))

root_cmake = octopus_dir + "/CMakeLists.txt"

if not os.path.exists(root_cmake):
    print("octopus source directory corrupted: root CMakeLists.txt is missing. Please redownload source code.")
    exit()

octopus_build_dir = octopus_dir + "/build"

if not os.path.exists(octopus_build_dir):
    print("octopus source directory corrupted: build directory is missing. Please redownload source code.")
    exit()

bin_dir = octopus_dir + "/bin"

if not os.path.exists(bin_dir):
    print("No bin directory found, making one")
    os.makedirs(bin_dir)

os.chdir(octopus_build_dir) # so cmake doesn't pollute root directory

ret = 0

cmake_options = []

if args["root"]:
    cmake_options.append(["-DINSTALL_ROOT=ON", octopus_dir])

if args["compiler"]:
    cmake_options.append("-DCMAKE_CXX_COMPILER=" + args["compiler"])

ret = call(["cmake"] + cmake_options + [".."])

if ret == 0:
    if is_unix():
        if args["root"]:
            call(["sudo", "make", "install"])
        else:
            call(["make", "install"])
    else:
        print("TODO: make for Windows!")
