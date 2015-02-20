# RCA XPCC ROS Package

This is a package to use functionality from the
* cross platform communication library (XPCC) with the 
* Robot Operating System (ROS)

## Setup
xpcc source code is linked as a submodule so do:

    git submodule init
    git submodule update
    
This will checkout the tested xpcc version to `ext/xpcc`.

Then you need to build the xpcc library as a static library

    cd ext/xpcc/src
    scons library

