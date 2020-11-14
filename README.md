# OpenNI

**Structure Core customers:** OpenNI does not currently support Structure Core. To start developing with Structure Core download *Structure SDK (Cross-Platform)* on the [Developer Portal](https://developer.structure.io/sdk).


OpenNI2 homepage: http://structure.io/openni

## Develop branch ##

The latest ongoing development is currently being done in the develop branch.  Refer to README and ReleasesNotes in the develop branch for up to date build instructions.

## Contributing ##

Pull requests that do not apply cleanly on top of the [`develop` branch head](http://github.com/occipital/OpenNI2/tree/develop) will be rejected.

Other than that, sensible and meaningful contributions are very welcome!

## Building Prerequisites

### Windows

- Microsoft Visual Studio 2010

    - Download and install from: http://msdn.microsoft.com/en-us/vstudio/bb984878.aspx

- Microsoft Kinect SDK v1.6

    - Download and install from: http://go.microsoft.com/fwlink/?LinkID=262831

- Python 2.6+/3.x

    - Download and install from: http://www.python.org/download/

- PyWin32

    - Download and install from: http://sourceforge.net/projects/pywin32/files/pywin32/

    Please make sure you download the version that matches your exact python version.

- WIX 3.5

    - Download and install from: http://wix.codeplex.com/releases/view/60102

- Doxygen

    - Download and install from: http://www.stack.nl/~dimitri/doxygen/download.html#latestsrc

- GraphViz

    - Download and install from: http://www.graphviz.org/Download_windows.php

### Linux

- GCC 4.x

	- Download and install from: http://gcc.gnu.org/releases.html

    - Or use `apt`:

	    	sudo apt-get install g++

- Python 2.6+/3.x

    - Download and install from: http://www.python.org/download/

    - Or use `apt`:

	    	sudo apt-get install python

- LibUSB 1.0.x

    - Download and install from: http://sourceforge.net/projects/libusb/files/libusb-1.0/

    - Or use `apt`:

	    	sudo apt-get install libusb-1.0-0-dev

- LibUDEV

		sudo apt-get install libudev-dev

- FreeGLUT3

    - Download and install from: http://freeglut.sourceforge.net/index.php#download

    - Or use `apt`:

	    	sudo apt-get install freeglut3-dev

- Doxygen

    - Download and install from: http://www.stack.nl/~dimitri/doxygen/download.html#latestsrc

    - Or use `apt`:

    		sudo apt-get install doxygen

- GraphViz

    - Download and install from: http://www.graphviz.org/Download_linux_ubuntu.php

    - Or use `apt`:

    		sudo apt-get install graphviz

### macOS
- Libusb 1.0
  - Install Homebrew: ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"
  - brew install libusb --universal


## Building

### Building on Windows:

  Open the solution `OpenNI.sln`

### Building on Linux / macOS:

  Run:

	make

### Cross-Compiling for ARM on Linux

  The following environment variables should be defined:

- `ARM_CXX=path-to-cross-compilation-g++`
- `ARM_STAGING=path-to-cross-compilation-staging-dir`

Then, run:

	PLATFORM=Arm make

### Creating OpenNI2 packages

  - Go into the directory `Packaging`
  - Run:

		ReleaseVersion.py [x86|x64|Arm]

  - The installer will be placed in the `Final` directory

### Building in an armhf qemu-enabled docker container

- Note: we're building from the osrf fork which has a couple of minor tweaks compared to the orbbec upstream repo (https://github.com/orbbec/OpenNI2/compare/orbbec-dev...osrf:orbbec-dev?expand=1):
  - don't force soft-float in third-party software (because we're building with hard-float)
  - disable documentation generation (because we don't need it, and also because doxygen seems to just hang in this environment).

- Check out the code (because git doesn't work inside qemu):
  - git clone https://github.com/osrf/OpenNI2
  - cd OpenNI2
  - git checkout orbbec-dev
  - cd ..
- Start docker, mounting in the git repo:
  - docker run -ti -v `pwd`/OpenNI2:/tmp/OpenNI2 osrf/ubuntu_armhf:trusty bash
- Now inside docker, install prereqs (we're removing udev because it somehow conflicts with libudev-dev; we're leaving out doxygen and graphviz because we're not going to build docs):
  - apt-get update
  - apt-get remove -y --force-yes udev
  - apt-get install -y build-essential python libusb-1.0-0-dev libudev-dev openjdk-6-jdk freeglut3-dev
- Still inside docker, build the release package
  - cd /tmp/OpenNI2/Packaging
  - ./ReleaseVersion.py Arm
- If all goes well, there will be a file in the docker container called something like `/tmp/OpenNI2/Packaging/Final/OpenNI-Linux-Arm-2.3.tar.bz2`.  It's also in the corresponding place in the host filesystem, so it will survive exiting the container. That file contains the compiled libraries (importantly, `OpenNI-Linux-Arm-2.3/Redist/libOpenNI2.so`).
