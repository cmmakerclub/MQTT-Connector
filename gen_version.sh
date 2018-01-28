#!/bin/bash

(echo '/* Generated file, do not edit. */'; echo  '#define APP_VERSION "'`git describe`'"';   echo  '#define APP_VERSION_DATE "'`git log -n 1 --format=%ai`'"'; ) > src/version.h
