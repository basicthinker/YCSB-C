YCSB-C
======

Yahoo! Cloud Serving Benchmark in C++, a C++ version of YCSB (https://github.com/brianfrankcooper/YCSB/wiki)

## Quick Start

### 1. Download YCSB-C

    git clone https://github.com/basicthinker/YCSB-C.git

Please note which branch should be in use.
The "svm" and "undo" branches correspond to branches in the sitevm project.

### 2. Set up environment variables

To compile YCSB-C, we need the following two environment variables:

Variable name | Note | Example
--------------|------|--------
**SITEVM\_HOME** | Top directory of the sitevm project. | SITEVM\_HOME=$HOME/Projects/sitevm\_dune
**LIBITM\_HOME** | Output path of the libitm project. This directory should inlucde libitm.a. | LIBITM\_HOME=$HOME/Projects/gcc/x86\_64-unknown-linux-gnu/libitm
**PLIB\_HOME**(optional) | If the sitevm project is using persistence, this variable has to be set to the plib directory of vm-persistence (https://github.com/basicthinker/vm-persistence) | PLIB\_HOME=$HOME/Projects/vm-persistence/plib

### 3. Prerequisites

Target databases or their dependencies should be installed properly before compiling or running YCSB-C.

#### 3.1. Intel Threading Building Blocks

Please refer to https://www.threadingbuildingblocks.org/.

On Ubuntu/Debian, you may only need to run the following command.

    $ sudo apt-get install libtbb-dev

#### 3.2. Aerospike

You have to install both Aerospike server and client dev packages.

For server installation, please refer to http://www.aerospike.com/docs/operations/install/.

For client installation, please refer to http://www.aerospike.com/docs/client/c/install/.

Don't forget to start the Aerospike service afterwards.

### 4. Compile

    cd YCSB-C
    make

After successful compilation, there is an executable ycsbc in the top project directory.

### 5. Runtime setting

You might want to see the help info by running ycsbc without any option.

    ./ycsbc

A typical command line can be:

    ./ycsbc -db itm_slib -threads 8 -P workloads/workloada.spec

Available DB names can be found in db/db\_factory.cc.

If it is used with GCC libitm, the environment variable ITM\_DEFAULT\_METHOD can be set to choose among different transaction systems.

    export ITM_DEFAULT_METHOD=svm

