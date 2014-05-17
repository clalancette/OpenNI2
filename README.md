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

- JDK 6.0

    - Download and install from: http://www.oracle.com/technetwork/java/javase/downloads/jdk-6u32-downloads-1594644.html

    You must also define an environment variable called `JAVA_HOME` that points to the JDK installation directory. For example:

    	set JAVA_HOME=c:\Program Files (x86)\Java\jdk1.6.0_32

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

- JDK 6.0

    - Download and install from: http://www.oracle.com/technetwork/java/javase/downloads/jdk-6u32-downloads-1594644.html

    - Or use `apt`:

    	- On Ubuntu 10.x:

				sudo add-apt-repository "deb http://archive.canonical.com/ lucid partner"
				sudo apt-get update
				sudo apt-get install sun-java6-jdk

    	- On Ubuntu 12.x:

				sudo apt-get install openjdk-6-jdk

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

### Android

- Download and install the Android NDK from http://developer.android.com/tools/sdk/ndk/index.html#Downloads
- You must also define an environment variable called "NDK_HOME" that points to the NDK installation directory.
- Download and install the Android SDK from http://developer.android.com/sdk/index.html#download
  - Currently requires installing SDK for API level 14 and 18 (via Android SDK Manager)
- You must also define an environment variable called "ANDROID_HOME" that points to the SDK installation directory.
- Download and install Ant:
  - Ubuntu: sudo apt-get install ant
  - Windows: https://code.google.com/p/winant
- Download and install JDK 6.0
  - Ubuntu: sudo apt-get install openjdk-6-jdk
  - Windows: http://www.oracle.com/technetwork/java/javase/downloads/jdk-6u32-downloads-1594644.html
  - You must also define an environment variable called "JAVA_HOME" that points to the JDK installation directory.

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

		ReleaseVersion.py [x86|x64|Arm|Android]

  - The installer will be placed in the `Final` directory
