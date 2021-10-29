# KernelGen
- Binary file generator of kernel or feature map with random float number
- Simple convolution implementation in C

## Overview
- Runnble on linux only (if you run on windows, you need mingw)
- It developed on Visual Studio Code (Windows with mingw)

## Build
- Linux : gcc -o kernelgen kernelgen.c .\example\simpleconv.c
- Windows : gcc -o kernelgen.exe kernelgen.c .\example\simpleconv.c

## Run
### Kernel Generation
```
kernelgen --type TYPE | --dtype TYPE | --width WIDTH | --height HEIGHT | --count COUNT | --channel CHANNEL | --min MIN | --max MAX 
```
- type TYPE[string] : "KERNEL", "INPUT", "CONNX"
- dtype TYPE[string] : data type of data (only support "FLOAT32") - default : "FLOAT32"
- width WIDTH[int] : width of kernel or feature map - default : 128
- height HEIGHT[int] : height of kernel or feature map - default : 128
- channel CHANNEL[int] : channel of kernel or feature map - default : 1
- count COUNT[int] : number of kernel or feature map - default : 5
- min MIN[float] : min value of kernel or feature map (this value use on random float number generation) - default : 0.0
- max MAX[float] : max value of kernel or feature map (this value use on random float number generation) - default : 1.0

### Example : Convolution 2d
```
kernelgen --exam EXAM | --input INPUT_PATH | --kernel KERNEL_PATH | --group GROUP | --pads PAD 
```
- exam : only support "CONV"
- input : input file path
- kernel : kernel file path
- group : group for depthwise convolution - default : 1
- pads : padding of convolution - default : 0
