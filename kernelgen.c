/*
 *  KernelGen
 *  Copyright (C) 2002-2021 Neowine, Co., Ltd.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the MIT License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later version.
 */
#include "include/kernelgen.h"
#include "include/simpleconv.h"

static void print_usage() {
    puts("usage: kernelgen [--help]");
    puts("                 (--type TYPE | --dtype TYPE | --width WIDTH | --height HEIGHT | --channel CHANNEL | --count COUNT | --min MIN | --max MAX )");
    puts("                 (--exam EXAM | --input FILE_PATH | --kernel KERNEL_PATH | --group GROUP | --pads [] ) ");
    puts("                 (--convert FILE_PATH");
    puts("");
    puts(" help");
    puts("  --help                show this help message and exit");
    puts("");
    puts(" optional arguments - for kernel generation : ");
    puts("  --type TYPE           set output file type [string] -- (it can be \"INPUT\", \"KERNEL\", \"CONNX\" )");
    puts("  --dtype TYPE          set data type of kernel [string] -- (default : FLOAT32)");
    puts("  --width WIDTH         set width of kernel [int] -- (default : 128)");
    puts("  --height HEIGHT       set height of kernel [int] -- (default : 128)");
    puts("  --channel CHANNEL     set channel of kernel [int] -- (default : 1)");
    puts("  --count COUNT         number of kernels to be created [int] -- (default : 512)");
    puts("  --min MIN             set min value in random number generator [float] -- (range : [min, max], default 0.0)");
    puts("  --max MAX             set max value in random number generator [float] -- (range : [min, max], default 1.0)");
    puts(" optional arguments - for running example : ");
    puts("  --exam                select example [string] -- (implemented conv2d only)");
    puts("  --input               set input file path [string] -- (binary image or feature map)");
    puts("  --kernel              set kernel directory [string] -- (selected all kernel files in given directory path)");
    puts("  --group               set param group for convolution [int] -- (default : 1)");
    puts("  --pads                set param pads for convolution. [int] -- (default : 0)");

}

//generate float [min, max] range
float _randomgen(float min, float max){
    float rand_f = min + (float) (rand()) / ((float) (RAND_MAX / (max - min)));
    return rand_f;
}

int _filefilter(const char* f_name) { 
    char *ext; 
    ext = strrchr(f_name, '.'); 
    if(ext == NULL) { 
        return 0; // no ext 
    } 
    if(strcmp(ext, ".input") == 0 || strcmp(ext, ".kernel")) { 
        return 1; 
    } 
    else { 
        return 0; 
    } 
}

int _getflist(const char* path, char** f_list){
    DIR *dir = opendir(path);
    struct stat filestat;
    int count = 0;
    if(dir == NULL)
    {
        printf("failed open\n");
        return -1;
    }
 
    struct dirent *de=NULL;
 
    while((de = readdir(dir))!=NULL)
    {
        stat(de->d_name,&filestat);
        if( S_ISDIR(filestat.st_mode) ){
            //printf("%4s: %s\n","Dir",de->d_name);
        }
        else{
            if(_filefilter(de->d_name) == 1) {
                count++;
            }
        }
    }
    closedir(dir);
    return count;
}


char* _basename(char const *path)
{
    srand((unsigned int)time(NULL));
    char *s = strrchr(path, '/');
    if (!s)
        return strtok(strdup(path), ".");
    else
        return strtok(strdup(s + 1), ".");
}

int _getfloatcount(char* fname, int* shape) {
    int count = 1;
    int idx = 0;
    char *temp = strtok(fname,"_");
    while (temp != NULL) { 
        int val = atoi(temp);
        if(idx < 3){
            count *= val;
            shape[idx] = val;
            idx++;
        }
        temp = strtok(NULL, "_");
    }
    return count;
}

void _readbinary(const char *path, float* data, int count) {
    FILE *file = fopen(path, "rb"); 
    if (file == NULL) { 
        printf("파일을 읽기 모드로 열 수 없습니다.\n"); 
        return; 
    } 
    
    fread(data, sizeof(float), count, file); 
    fclose(file);
}

void do_conv(struct kernel_options* option) {

    //file exist check
    if( access(option->value_input, F_OK) == -1 ) {
        puts("input file not exist!");
        return;
    }

    if( access(option->value_kernel, F_OK) == -1 ) {
        puts("kernel file not exist!");
        return;
    }

    char **f_list;
    //get float count from filename
    int input_fcount, kernel_fcount;
    int dim = 3;
    int input_shape[dim];
    int input_shape_pad[dim];
    int kernel_shape[dim];
    int output_shape[dim];
    int group = atoi(option->value_group);
    int pads = atoi(option->value_pads);

    char* input_f_name, *kernel_f_name;

    input_f_name = _basename(option->value_input);
    kernel_f_name = _basename(option->value_kernel);

    printf("input fname : %s\n", input_f_name);
    printf("kernel fname : %s\n", kernel_f_name);

    input_fcount = _getfloatcount(input_f_name, input_shape);
    kernel_fcount = _getfloatcount(kernel_f_name, kernel_shape);

    printf("input fcount : %d\n", input_fcount);
    printf("kernel fcount : %d\n", kernel_fcount);

    if(input_shape[0] != kernel_shape[0]) {
        printf("input channel and kernel channel are must be the same!\n");
        return;
    }

    if(input_shape[0] % group != 0 || kernel_shape[0] % group != 0) {
        printf("input channel and kernel channel must be divisible by groups! -- input channel : %d, kernel channel : %d, group : %d\n", input_shape[0], kernel_shape[0], group);
        return;
    }

    //malloc using float count
    float* input_data_pad;
    float* input_data = (float*) malloc(sizeof(float) * input_fcount);
    float* kernel_data = (float*) malloc(sizeof(float) * kernel_fcount);

    //read binary file
    _readbinary(option->value_input, input_data, input_fcount);
    _readbinary(option->value_kernel, kernel_data, kernel_fcount);


    //make input and kernel tensor
    struct tensor* input_tensor = malloc(sizeof(struct tensor));
    input_tensor->data = input_data;
    input_tensor->n_dim = dim;
    input_tensor->shape = input_shape;

    for(int i = dim - 1; i >= 0; i--){
        if(i != 0) {
            input_shape_pad[i] = input_tensor->shape[i] + (2 * pads);
        } else {
            input_shape_pad[i] = input_tensor->shape[i];
        }
    }

    int input_count_pad = getcountshape(input_shape_pad, dim);
    input_data_pad = (float*)malloc(sizeof(float) * input_count_pad);
    memset(input_data_pad, 0, sizeof(float) * input_count_pad);
    
    int first_pad = input_shape_pad[2] + pads;
    int pad_step = (2 * pads) + input_tensor->shape[2];
    if(pads != 0) {
        for(int i=0; i<input_tensor->shape[1]; i++){
            memcpy(input_data_pad + first_pad + (i * pad_step), input_tensor->data + (i * input_tensor->shape[2]), sizeof(float) * input_tensor->shape[2]);
        }
        input_tensor->data = input_data_pad;
        input_tensor->shape = input_shape_pad;
    }

    struct tensor* kernel_tensor = malloc(sizeof(struct tensor));
    kernel_tensor->data = kernel_data;
    kernel_tensor->n_dim = 3;
    kernel_tensor->shape = kernel_shape;

    //make output tensor
    struct tensor* output_tensor = malloc(sizeof(struct tensor));
    output_tensor->n_dim = input_tensor->n_dim;
    output_tensor->shape = output_shape;
    getresultshape(input_tensor, kernel_tensor, output_tensor, group);
    int output_count = getcount(output_tensor);
    float output_data[output_count];
    for(int i = 0; i < output_count; i++){
        output_data[i] = 0;
    }
    output_tensor->data = output_data;

    printtensor(input_tensor, "input");
    printtensor(kernel_tensor, "kernel");

    //run conv
    conv2d(input_tensor, output_tensor, kernel_tensor, group, pads);
    printtensor(output_tensor, "output");
}

void do_gen(struct kernel_options* option) {
    int width  = atoi(option->value_width);
    int height = atoi(option->value_height);
    int channel = atoi(option->value_channel);
    int count  = atoi(option->value_count);
    float min = atof(option->value_min);
    float max = atof(option->value_max);
    char* ext;

    printf("data type : %s\n", option->value_type);
    if(strcmp(option->value_type, "INPUT") == 0) {
        ext = "input";
    }
    
    else if(strcmp(option->value_type, "KERNEL") == 0) {
        ext = "kernel";
    }

    else if(strcmp(option->value_type, "CONNX") == 0) {
        ext = "data";
    } 
    else {
        printf("Cannot support type : %s\n", option->value_type);
        return;
    }

    for(int c = 0; c < count; c++)
    {
        char filename[1024];
        sprintf(filename, "%d_%s_%s_%d.%s", channel, option->value_height, option->value_width, c, ext);

        if( access(filename, F_OK) != -1 )
            remove(filename);

        FILE *f = fopen(filename, "wb");

        if(strcmp(ext, "data") == 0){ //connx gen. header
            unsigned int dtype = 1;
            unsigned int dim = 3;
            printf("size of unsigned int : %d\n", sizeof(unsigned int));
            fwrite(&dtype, sizeof(unsigned int), 1, f);
            fwrite(&dim, sizeof(unsigned int), 1, f);
            fwrite(&channel, sizeof(unsigned int), 1, f);
            fwrite(&height, sizeof(unsigned int), 1, f);
            fwrite(&width, sizeof(unsigned int), 1, f);
        }

        srand(time(NULL));
        for(int ch = 0; ch < channel; ch++) {
            float arr[width * height];
            for(int i = 0; i < width * height; i++) {
                arr[i] = _randomgen(min, max);
            }

            for(int i = 0; i < width * height; i++) {
                float f_data = arr[i];
                fwrite(&f_data, sizeof(float), 1, f);
            }
        }
        fclose(f);
    }
    puts("Generation Complete.");
}

int main(int argc, char** argv) {
    const char* short_form = "h:txyc";
    const struct option long_form[] = {
        { "help",      no_argument,       NULL, CHOICE_H },
        { "type",      required_argument, NULL, CHOICE_T },
        { "width",     required_argument, NULL, CHOICE_WIDTH },
        { "height",    required_argument, NULL, CHOICE_HEIGHT },
        { "count",     required_argument, NULL, CHOICE_COUNT },
        { "channel",   required_argument, NULL, CHOICE_CHANNEL },
        { "min",       required_argument, NULL, CHOICE_MIN},
        { "max",       required_argument, NULL, CHOICE_MAX},
        { "exam",      required_argument, NULL, CHOICE_EXAM},
        { "input",     required_argument, NULL, CHOICE_INPUT},
        { "kernel",    required_argument, NULL, CHOICE_KERNEL},
        { "group",     required_argument, NULL, CHOICE_GROUP},
        { "pads",      required_argument, NULL, CHOICE_PADS},
        { NULL, 0, NULL, 0 },
    };

    struct kernel_options options = {};
    int longindex = 0;
    int choice = -1;
    while((choice = getopt_long(argc, argv, short_form, long_form, &longindex)) >= 0) {
        switch(choice) {
            case CHOICE_H:
                options.flag_h = true;
                break;
            case CHOICE_T:
                options.flag_type = true;
                options.value_type = optarg;
                break;
            case CHOICE_DTYPE:
                options.flag_dtype = true;
                options.value_dtype = optarg;
                if(options.value_dtype == NULL)
                    options.value_dtype = "INPUT";
                break;
            case CHOICE_WIDTH:
                options.flag_width = true;
                options.value_width = optarg;
                if(options.value_width == NULL)
                    options.value_width = "128";
                break;
            case CHOICE_HEIGHT:
                options.flag_height = true;
                options.value_height = optarg;
                if(options.value_height == NULL)
                    options.value_height = "128";
                break;
            case CHOICE_COUNT:
                options.flag_count = true;
                options.value_count = optarg;
                if(options.value_count == NULL)
                    options.value_count = "4";
                break;
            case CHOICE_CHANNEL:
                options.flag_channel = true;
                options.value_channel = optarg;
                if(options.value_channel == NULL)
                    options.value_channel = "1";
                break;
            case CHOICE_MIN:
                options.flag_min = true;
                options.value_min = optarg;
                if(options.value_min == NULL)
                    options.value_min = "0.0";
                break;
            case CHOICE_MAX:
                options.flag_max = true;
                options.value_max = optarg;
                if(options.value_max == NULL)
                    options.value_max = "1.0";
                break;
            case CHOICE_EXAM:
                options.flag_exam = true;
                options.value_exam = optarg;
                if(options.value_exam == NULL)
                    options.value_exam = "CONV2D";
                break;
            case CHOICE_INPUT:
                options.flag_input = true;
                options.value_input = optarg;
                break;
            case CHOICE_KERNEL:
                options.flag_kernel = true;
                options.value_kernel = optarg;
                break;
            case CHOICE_GROUP:
                options.flag_group = true;
                options.value_group = optarg;
                break;
            case CHOICE_PADS:
                options.flag_pads = true;
                options.value_pads = optarg;
                break;
        }
    }
    
    if(options.flag_type) {
        if(!options.flag_dtype) {
            options.flag_dtype = true;
            options.value_dtype = "FLOAT32";
        }
        if(!options.flag_width) {
            options.flag_width = true;
            options.value_width = "128";
        }
        if(!options.flag_height) {
            options.flag_height = true;
            options.value_height = "128";
        }
        if(!options.flag_count) {
            options.flag_count = true;
            options.value_count = "4";
        }
        if(!options.flag_min) {
            options.flag_min = true;
            options.value_min = "0.0";
        }
        if(!options.flag_max) {
            options.flag_max = true;
            options.value_max = "1.0";
        }
        if(!options.flag_channel){
            options.flag_channel = true;
            options.value_channel = "1";
        }
        do_gen(&options);
    }

    if(options.flag_exam) {
        if(!options.flag_group) {
            options.flag_group = true;
            options.value_group = "1";
        }

        if(!options.flag_pads) {
            options.flag_pads = true;
            options.value_pads = "0";
        }

        if(options.flag_input == false || options.value_input == NULL || options.flag_kernel == false || options.value_kernel == NULL) {
            puts("argument [--input FILE_PATH] or [--kernel DIR] was not given.");
            return 0;
        }
        do_conv(&options);
    }

    if(options.flag_h)
        print_usage();

    return 0;
}