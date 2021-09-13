# KernelGen
Binary file generator of kernel or image(feature map) with random float number

## Overview
- Runnble on linux only (if you run on windows, you need mingw)
- It developed on Visual Studio Code (Windows with mingw)

## Build
- Linux : gcc kernelgen.c -o kernelgen
- Windows : gcc kernelgen.c -o kernelge.exe

## Run
```
kernelgen [--help] (--type TYPE | --width WIDTH | --height HEIGHT | --count COUNT | --min MIN | --max MAX )
```
- type : only FLOAT32 (other type not support)
- width : width of kernel or feature map - default : 128
- height : height of kernel or feature map - default : 128
- count : number of kernel or feature map - default : 512
- min : min value of kernel or feature map (this value use on random float number generation) - default : 0.0
- max : max value of kernel or feature map (this value use on random float number generation) - default : 1.0
