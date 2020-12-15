# OpenNI2

This project contains the low-level USB drivers to drive structured light sensors dervied from and using a protocol similar to the original PrimeSense.
This currently includes:

* PrimeSense Carmine 1.09
* Asus Xtion PRO
* Orbbec Astra series

## Prerequisites

### Linux

#### Ubuntu 20.04

```
sudo apt-get install cmake freeglut3-dev g++ libjpeg-dev libopengl-dev libudev-dev libusb-1.0-0-dev
```

#### Fedora 32

```
sudo dnf install cmake freeglut-devel gcc-c++ libglvnd-devel libjpeg-turbo-devel libgudev-devel libusbx-devel
```

## Building

```
mkdir build
cd build
cmake ..
make -j
```

## History

This code has a long and checkered history.  The original OpenNI (Open Natural Interaction) code was written to
interface with the PrimeSense camera starting around 2010.  At the time, OpenNI was positioned as a non-profit
organization to come up with user interfaces for "Natural Interaction" devices.  These devices were primarily
structured light depth sensors, such as that in the PrimeSense and Kinect.  The original idea behind OpenNI was
to come up with an industry-standard set of APIs for interacting with these devices.  This original code is
still available at https://github.com/OpenNI/OpenNI, but has not been touched since 2013.

In parallel, OpenNI2 (at https://github.com/OpenNI/OpenNI2) was developed by PrimeSense starting in 2012.
OpenNI2 is more of a low-level driver for the USB devices, rather than a high-level "Natural Interaction" API.
PrimeSense continued to be the main developer behind OpenNI2 until the acquisition of PrimeSense by Apple
in late 2013.  At that point, PrimeSense/Apple ceased all development on OpenNI2.

However, there were still players in the market that had licensed PrimeSense technology.  One of those
players was Occipital, which took over development of OpenNI2 in mid-2014 and maintained it until approximately
mid-2015.

Around mid-2015, Orbbec forked off OpenNI2 to add support for their Astra series of depth cameras.  Orbbec
developed the driver just far enough to get their Astra cameras working, and stopped committing to their
OpenNI2 fork around September 2015.

In late 2020, I spent a bunch of time pulling together the disparate forks of development, cleaning up the
code, removing dead code, fixing compiler warnings, fixing memory leaks, rewriting the build system, and
making the whole thing a lot more manageable.
