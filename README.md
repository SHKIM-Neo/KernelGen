# KernelGen
Binary file generator of kernel or image(feature map) with random float number

## Overview
- Runnble on linux only (if you run on windows, you need mingw)
- It developed on Visual Studio Code (Windows with mingw)

## Build
- Linux : gcc kernelgen.c -o kernelgen
- Windows : gcc kernelgen.c -o kernelge.exe

## Run
### Kernel Generation
```
kernelgen --type TYPE | --width WIDTH | --height HEIGHT | --count COUNT | --channel CHANNEL | --min MIN | --max MAX 
```
- type : "KERNEL" or "INPUT"
- width : width of kernel or feature map - default : 128
- height : height of kernel or feature map - default : 128
- channel : channel of kernel or feature map - default : 1
- count : number of kernel or feature map - default : 4
- min : min value of kernel or feature map (this value use on random float number generation) - default : 0.0
- max : max value of kernel or feature map (this value use on random float number generation) - default : 1.0

### Example : Convolution 2d
```
kernelgen --exam EXAM | --input INPUT_PATH | --kernel KERNEL_PATH | --group GROUP | --pads PAD 
```
- exam : only support "CONV"
- input : input file path
- kernel : kernel file path
- group : (not implemented) - default : 0
- pads : padding of convolution - default : 0
