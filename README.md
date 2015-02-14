YCSB-C
======

Yahoo! Cloud Serving Benchmark in C++, a C++ version of YCSB (https://github.com/brianfrankcooper/YCSB/wiki)

## Quick Start

### 1. Download YCSB-C

    git clone https://github.com/basicthinker/YCSB-C.git

Please note which branch should be in use.
The "svm" and "undo" branches correspond to branches in the sitevm project.

### 2. Modify makefile variables

Modify the first three lines of YCSB-C/Makefile. Point these variables to dependent project directories.

### 3. Compile

    cd YCSB-C
    make

After successful compilation, there is an executable ycsbc in the top project dirctory.

### 4. Runtime setting

You might want to see the help info by running ycsbc without any option.

    ./ycsbc

Available DB names can be found in db/db\_factory.cc.

If it is used with GCC libitm, the environment variable ITM\_DEFAULT\_METHOD can be set to choose among different transaction systems.

    export ITM_DEFAULT_METHOD=svm

