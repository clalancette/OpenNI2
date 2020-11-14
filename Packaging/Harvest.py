#!/usr/bin/python2.7

#/****************************************************************************
#*                                                                           *
#*  OpenNI 2.x Alpha                                                         *
#*  Copyright (C) 2012 PrimeSense Ltd.                                       *
#*                                                                           *
#*  This file is part of OpenNI.                                             *
#*                                                                           *
#*  Licensed under the Apache License, Version 2.0 (the "License");          *
#*  you may not use this file except in compliance with the License.         *
#*  You may obtain a copy of the License at                                  *
#*                                                                           *
#*      http://www.apache.org/licenses/LICENSE-2.0                           *
#*                                                                           *
#*  Unless required by applicable law or agreed to in writing, software      *
#*  distributed under the License is distributed on an "AS IS" BASIS,        *
#*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
#*  See the License for the specific language governing permissions and      *
#*  limitations under the License.                                           *
#*                                                                           *
#****************************************************************************/
import os
import glob
import re
import sys
import shutil
import platform
import stat
import xml.dom.minidom

class Harvest:
    def __init__(self, rootDir, outDir, arch, osName):
        self.rootDir = rootDir
        self.outDir = outDir
        self.arch = arch
        self.osName = osName
        self.platformSuffix = ''
        self.glutSuffix = '32'
        self.binDir = os.path.join(rootDir, 'Bin', arch + '-Release')

        # override some defaults
        if self.osName == 'Windows':
            if arch == 'x86':
                self.binDir = os.path.join(rootDir, 'Bin', 'Win32-Release')
            elif arch == 'x64':
                self.platformSuffix = '64'
                self.glutSuffix = '64'
    def shcopy(self, src, targetDir):
        if os.path.exists(src):
            shutil.copy(src, targetDir)

    def shcopytree(self, srcDir, targetDir):
        if os.path.exists(srcDir):
            shutil.copytree(srcDir, targetDir)

    def copySharedObject(self, sourceDir, name, targetDir):
        if self.osName == 'Windows':
            self.shcopy(os.path.join(sourceDir, name + '.dll'), targetDir)
            self.shcopy(os.path.join(sourceDir, name + '.pdb'), targetDir)
        elif self.osName == 'Linux':
            self.shcopy(os.path.join(sourceDir, 'lib' + name + '.so'), targetDir)
        elif self.osName == 'Darwin':
            self.shcopy(os.path.join(sourceDir, 'lib' + name + '.dylib'), targetDir)
        else:
            raise 'Unsupported platform!'

    def copyExecutable(self, sourceDir, name, targetDir):
        if self.osName == 'Windows':
            self.shcopy(os.path.join(sourceDir, name + '.exe'), targetDir)
            self.shcopy(os.path.join(sourceDir, name + '.pdb'), targetDir)
        else:
            self.shcopy(os.path.join(sourceDir, name), targetDir)

    def regxReplace(self, findStr, repStr, filePath):
        "replaces all findStr by repStr in file filePath using regular expression"
        findStrRegx = re.compile(findStr)
        tempName = filePath+'~~~'
        fileMode = os.stat(filePath).st_mode
        os.chmod(filePath, fileMode | stat.S_IWRITE)
        input = open(filePath)
        output = open(tempName, 'w')
        for s in input:
            output.write(findStrRegx.sub(repStr, s))
        output.close()
        input.close()
        os.remove(filePath)
        os.rename(tempName, filePath)

    def copyRedistFiles(self, targetDir):
        os.makedirs(targetDir)
        # start with OpenNI itself
        self.copySharedObject(self.binDir, 'OpenNI2', targetDir)
        self.copySharedObject(self.binDir, 'OpenNI2.jni', targetDir)

        self.shcopy(os.path.join(self.binDir, 'org.openni.jar'), targetDir)
        self.shcopy(os.path.join(self.rootDir, 'Config', 'OpenNI.ini'), targetDir)

        # and now all drivers
        binDriversDir = os.path.join(self.binDir, 'OpenNI2', 'Drivers')
        targetDriversDir = os.path.join(targetDir, 'OpenNI2', 'Drivers')
        os.makedirs(targetDriversDir)

        self.copySharedObject(binDriversDir, 'OniFile', targetDriversDir)
        self.copySharedObject(binDriversDir, 'PS1080', targetDriversDir)
        self.copySharedObject(binDriversDir, 'orbbec', targetDriversDir)
        self.copySharedObject(binDriversDir, 'PSLink', targetDriversDir)

        self.shcopy(os.path.join(self.rootDir, 'Config', 'OpenNI2', 'Drivers', 'PS1080.ini'), targetDriversDir)
        self.shcopy(os.path.join(self.rootDir, 'Config', 'OpenNI2', 'Drivers', 'orbbec.ini'), targetDriversDir)
        self.shcopy(os.path.join(self.rootDir, 'Config', 'OpenNI2', 'Drivers', 'PSLink.ini'), targetDriversDir)
        self.shcopy(os.path.join(self.rootDir, 'Config', 'OpenNI2', 'Drivers', 'OniFile.ini'), targetDriversDir)

        if self.osName == 'Windows':
            self.copySharedObject(binDriversDir, 'Kinect', targetDriversDir)

    def copySample(self, samplesDir, targetBinDir, name, isLibrary = False, isGL = False, sourceSamplesDir = None):
        if self.arch == 'Arm' and isGL:
            return

        if sourceSamplesDir is None:
            sourceSamplesDir = os.path.join(self.rootDir, 'Samples')

        sampleTargetDir = os.path.join(samplesDir, name)
        sampleSourceDir = os.path.join(sourceSamplesDir, name)

        for root, dirs, files in os.walk(sampleSourceDir):
            # take dir name without 'root' and append to target
            dst = os.path.join(sampleTargetDir, os.path.relpath(root, sampleSourceDir))
            for file in files:
                if file.endswith('.h') or file.endswith('.cpp'):
                    if not os.path.exists(dst):
                        os.makedirs(dst)
                    self.shcopy(os.path.join(root, file), dst)

        # copy common header
        if not isLibrary:
            self.shcopy(os.path.join(self.rootDir, 'Samples', 'Common', 'OniSampleUtilities.h'), sampleTargetDir)

        # copy GL headers
        if self.osName == 'Windows' and isGL:
            self.shcopytree(os.path.join(self.rootDir, 'ThirdParty', 'GL', 'GL'), os.path.join(sampleTargetDir, 'GL'))
            # and lib
            self.shcopy(os.path.join(self.rootDir, 'ThirdParty', 'GL', 'glut32.lib'), sampleTargetDir)
            self.shcopy(os.path.join(self.rootDir, 'ThirdParty', 'GL', 'glut64.lib'), sampleTargetDir)
            self.shcopy(os.path.join(self.rootDir, 'ThirdParty', 'GL', 'glut32.dll'), sampleTargetDir)
            self.shcopy(os.path.join(self.rootDir, 'ThirdParty', 'GL', 'glut64.dll'), sampleTargetDir)

        # and project file / makefile
        if self.osName == 'Windows':
            self.shcopy(os.path.join(sampleSourceDir, name + '.vcxproj'), sampleTargetDir)
            projFile = os.path.join(sampleTargetDir, name + '.vcxproj')
            #ET.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')
            doc = xml.dom.minidom.parse(projFile)

            # remove OutDir and IntDir (make them default)
            for propertyGroup in doc.getElementsByTagName("PropertyGroup"):
                if len(propertyGroup.getElementsByTagName("OutDir")) > 0:
                    propertyGroup.parentNode.removeChild(propertyGroup)

            for group in doc.getElementsByTagName("ItemDefinitionGroup"):
                condAttr = group.getAttribute('Condition')
                if condAttr.find('x64') != -1:
                    postfix = '64'
                    glPostfix = '64'
                else:
                    postfix = ''
                    glPostfix = '32'

                incDirs = group.getElementsByTagName('ClCompile')[0].getElementsByTagName('AdditionalIncludeDirectories')[0]
                val = incDirs.firstChild.data

                # fix GL include dir
                val = re.sub('..\\\\..\\\\ThirdParty\\\\GL', r'.', val)
                # fix Common include dir
                val = re.sub('..\\\\Common', r'.', val)
                # fix OpenNI include dir
                val = re.sub('..\\\\..\\\\Include', '$(OPENNI2_INCLUDE' + postfix + ')', val)

                incDirs.firstChild.data = val

                # fix additional library directories
                libDirs = group.getElementsByTagName('Link')[0].getElementsByTagName('AdditionalLibraryDirectories')[0]
                val = libDirs.firstChild.data
                val = re.sub('\$\(OutDir\)', '$(OutDir);$(OPENNI2_LIB' + postfix + ')', val)
                libDirs.firstChild.data = val

                # add post-build event to copy OpenNI redist
                post = doc.createElement('PostBuildEvent')
                cmd = 'xcopy /D /S /F /Y "$(OPENNI2_REDIST' + postfix + ')\*" "$(OutDir)"\n'
                if isGL:
                    cmd += 'xcopy /D /F /Y "$(ProjectDir)\\glut' + glPostfix + '.dll" "$(OutDir)"\n'

                cmdNode = doc.createElement('Command')
                cmdNode.appendChild(doc.createTextNode(cmd))
                post.appendChild(cmdNode)
                group.appendChild(post)

                proj = open(projFile, 'w')
                proj.write(doc.toxml())
                proj.close()

        elif self.osName == 'Linux' or self.osName == 'Darwin':
            self.shcopy(os.path.join(sampleSourceDir, 'Makefile'), sampleTargetDir)
            self.shcopy(os.path.join(rootDir, 'ThirdParty', 'PSCommon', 'BuildSystem', 'CommonDefs.mak'), sampleTargetDir)
            self.shcopy(os.path.join(rootDir, 'ThirdParty', 'PSCommon', 'BuildSystem', 'CommonTargets.mak'), sampleTargetDir)
            self.shcopy(os.path.join(rootDir, 'ThirdParty', 'PSCommon', 'BuildSystem', 'Platform.x86'), sampleTargetDir)
            self.shcopy(os.path.join(rootDir, 'ThirdParty', 'PSCommon', 'BuildSystem', 'Platform.x64'), sampleTargetDir)
            self.shcopy(os.path.join(rootDir, 'ThirdParty', 'PSCommon', 'BuildSystem', 'Platform.Arm'), sampleTargetDir)
            self.shcopy(os.path.join(rootDir, 'ThirdParty', 'PSCommon', 'BuildSystem', 'CommonCppMakefile'), sampleTargetDir)

            # fix common makefiles path
            self.regxReplace('../../ThirdParty/PSCommon/BuildSystem/', '', os.path.join(sampleTargetDir, 'Makefile'))

            # fix BIN dir
            self.regxReplace('BIN_DIR = ../../Bin', 'BIN_DIR = Bin', os.path.join(sampleTargetDir, 'Makefile'))

            # fix include dirs and copy openni_redist
            add = r'''
ifndef OPENNI2_INCLUDE
    $(error OPENNI2_INCLUDE is not defined. Please define it or 'source' the OpenNIDevEnvironment file from the installation)
else ifndef OPENNI2_REDIST
    $(error OPENNI2_REDIST is not defined. Please define it or 'source' the OpenNIDevEnvironment file from the installation)
endif

INC_DIRS += $(OPENNI2_INCLUDE)

include \1

.PHONY: copy-redist
copy-redist:
    cp -R $(OPENNI2_REDIST)/* $(OUT_DIR)

$(OUTPUT_FILE): copy-redist
'''
            self.regxReplace(r'include (Common.*Makefile)', add, os.path.join(sampleTargetDir, 'Makefile'))

        # and executable
        if isLibrary:
            self.copySharedObject(self.binDir, name, targetBinDir)
            if self.osName == 'Windows':
                self.shcopy(os.path.join(self.binDir, name + '.lib'), targetBinDir)
        else: # regular executable
            self.copyExecutable(self.binDir, name, targetBinDir)

    def copyTool(self, toolsDir, name, isGL = False):
        if self.arch == 'Arm' and isGL:
            return

        self.copyExecutable(self.binDir, name, toolsDir)

    def copyDocumentation(self, docDir):
        os.makedirs(docDir)
        if self.osName == 'Windows':
            self.shcopy(os.path.join(self.rootDir, 'Source', 'Documentation', 'cpp', 'OpenNI.chm'), docDir)
        else:
            self.shcopytree(os.path.join(self.rootDir, 'Source', 'Documentation', 'cpp'), os.path.join(docDir, 'cpp'))

    def copyGLUT(self, targetDir):
        if self.osName == 'Windows':
            self.shcopy(os.path.join(self.rootDir, 'ThirdParty', 'GL', 'glut' + self.glutSuffix + '.dll'), targetDir)

    def copyAssets(self, targetDir):
        os.makedirs(targetDir)
        self.shcopy(os.path.join(self.rootDir, 'Config', 'OpenNI.ini'), targetDir)
        self.shcopy(os.path.join(self.rootDir, 'Config', 'OpenNI2', 'Drivers', 'PS1080.ini'), targetDir)
        self.shcopy(os.path.join(self.rootDir, 'Config', 'OpenNI2', 'Drivers', 'orbbec.ini'), targetDir)
        self.shcopy(os.path.join(self.rootDir, 'Config', 'OpenNI2', 'Drivers', 'PSLink.ini'), targetDir)
        self.shcopy(os.path.join(self.rootDir, 'Config', 'OpenNI2', 'Drivers', 'OniFile.ini'), targetDir)

    def run(self):
        if os.path.exists(self.outDir):
            shutil.rmtree(self.outDir)
        os.makedirs(self.outDir)

        # Redist
        redistDir = os.path.join(self.outDir, 'Redist')
        self.copyRedistFiles(redistDir)

        # Samples
        samplesDir = os.path.join(self.outDir, 'Samples')
        samplesBinDir = os.path.join(samplesDir, 'Bin')
        self.copyRedistFiles(samplesBinDir)
        self.copyGLUT(samplesBinDir)
        self.copySample(samplesDir, samplesBinDir, 'SimpleRead')
        self.copySample(samplesDir, samplesBinDir, 'SimpleViewer', isGL = True)
        self.copySample(samplesDir, samplesBinDir, 'EventBasedRead')
        self.copySample(samplesDir, samplesBinDir, 'MultiDepthViewer', isGL = True)
        self.copySample(samplesDir, samplesBinDir, 'MultipleStreamRead')
        self.copySample(samplesDir, samplesBinDir, 'MWClosestPoint', isLibrary = True)
        self.copySample(samplesDir, samplesBinDir, 'MWClosestPointApp')
        self.copySample(samplesDir, samplesBinDir, 'ClosestPointViewer', isGL = True)

        # Tools
        toolsDir = os.path.join(self.outDir, 'Tools')
        self.copyRedistFiles(toolsDir)
        self.copyGLUT(toolsDir)
        self.copyTool(toolsDir, 'NiViewer', isGL = True)
        self.copyTool(toolsDir, 'PS1080Console')
        self.copyTool(toolsDir, 'PSLinkConsole')

        # Documentation
        docDir = os.path.join(self.outDir, 'Documentation')
        self.copyDocumentation(docDir)

        # Include
        incDir = os.path.join(self.outDir, 'Include')
        self.shcopytree(os.path.join(self.rootDir, 'Include'), incDir)

        # Release notes and change log
        self.shcopy(os.path.join(self.rootDir, 'ReleaseNotes.txt'), self.outDir)
        self.shcopy(os.path.join(self.rootDir, 'CHANGES.txt'), self.outDir)

        # Licenses
        self.shcopy(os.path.join(self.rootDir, 'NOTICE'), self.outDir)
        self.shcopy(os.path.join(self.rootDir, 'LICENSE'), self.outDir)

        if self.osName == 'Windows':
            # Driver
            self.shcopytree(os.path.join(self.rootDir, 'ThirdParty', 'PSCommon', 'XnLib', 'Driver', 'Win32', 'Bin'), os.path.join(self.outDir, 'Driver'))

            # Library
            libDir = os.path.join(self.outDir, 'Lib')
            os.makedirs(libDir)
            self.shcopy(os.path.join(self.binDir, 'OpenNI2.lib'), libDir)
        else:
            # install script
            self.shcopy(os.path.join(self.rootDir, 'Packaging', 'Linux', 'install.sh'), self.outDir)
            self.shcopy(os.path.join(self.rootDir, 'Packaging', 'Linux', 'primesense-usb.rules'), self.outDir)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print('Usage: ' + sys.argv[0] + ' <OutDir> <x86|x64|Arm>')
        exit(1)

    rootDir = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '..'))
    arch = sys.argv[2]
    osName = platform.system()
    harvest = Harvest(rootDir, sys.argv[1], arch, osName)
    harvest.run()
