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
#!/usr/bin/python2

import os
import sys
import platform
import shutil
import subprocess

def create_page(orig_path, page_name, page_header):
    orig = open(orig_path)
    dest = open("Temp/" + os.path.split(orig_path)[1] + ".txt", "w")
    dest.write("/** @page " + page_name + " " + page_header + "\n")
    dest.write(orig.read())
    dest.write("\n*/")
    orig.close()
    dest.close()

beforeDir = os.getcwd()
scriptDir = os.path.dirname(sys.argv[0])
os.chdir(scriptDir)

# Start with C++ reference

# create Legal page
if os.path.isdir("Temp"):
    shutil.rmtree("Temp")
os.mkdir("Temp")
create_page("../../NOTICE", "legal", "Legal Stuff & Acknowledgments")
create_page("../../ReleaseNotes.txt", "release_notes", "Release Notes")

errfile = "Temp/doxygen_error"
subprocess.check_call(["doxygen", "Doxyfile"], stdout=open(os.devnull,"w"), stderr=open(errfile,"w"))

os.chdir(beforeDir)
