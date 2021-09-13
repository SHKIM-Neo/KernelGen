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
    puts("usage: kernelgen [--help] (--exam EXAM | --input FILE_PATH | --kernel KERNEL_DIR ) (--type TYPE | --dtype TYPE | --width WIDTH | --height HEIGHT | --count COUNT | --min MIN | --max MAX )");
    puts("");
    puts("kernelgen frontend");
    puts("");
    puts("optional arguments:");
    puts("  --help                show this help message and exit");
    puts("  --exam                select example (implemented conv2d only)");
    puts("  --input               set input file path (binary image or feature map)");
    puts("  --kernel              set kernel directory (selected all kernel files in given directory path)");
    puts("  --type TYPE           set output file type (it can be \"INPUT\" or \"KERNEL\")");
    puts("  --dtype TYPE          set kernel data type [default : FLOAT32]");
    puts("  --width WIDTH         set kernel width [default : 128]");
    puts("  --height HEIGHT       set kernel height [default : 128]");
    puts("  --count COUNT         number of kernels to be created [default : 512]");
    puts("  --min MIN             set min value in random number generator (range : [min, max], default 0.0)");
    puts("  --max MAX             set max value in random number generator (range : [min, max], default 1.0)");
}

//generate float [min, max] range
float randomgen(float min, float max){
    float rand_f = min + (float) (rand()) /( (float) (RAND_MAX/(max - min)));
    //printf("rand : %f\n", rand_f);
    return rand_f;
}

void do_gen(struct kernel_options* option){
    int width  = atoi(option->value_width);
    int height = atoi(option->value_height);
    int count  = atoi(option->value_count);
    float min = atof(option->value_min);
    float max = atof(option->value_max);

    for(int c = 0; c < count; c++){
        float arr[width * height];
        for(int i = 0; i < width * height; i++){
            arr[i] = randomgen(min, max);
        }
        char filename[1024];
        sprintf(filename, "%s_%s_%s_%d.data", option->value_type, option->value_width, option->value_height, c);

        if( access(filename, F_OK) != -1 ) {
            remove(filename);
        }

        FILE *f = fopen(filename, "wb");
        
        for(int i = 0; i < width * height; i++){
            float f_data = arr[i];
            fwrite(&f_data, sizeof(float), 1, f);
        }

        fclose(f);
    }
}

int main(int argc, char** argv) {
    const char* short_form = "h:txyc";
    const struct option long_form[] = {
        { "help",      no_argument,       NULL, CHOICE_H },
        { "type",      required_argument, NULL, CHOICE_T },
        { "width",     required_argument, NULL, CHOICE_WIDTH },
        { "height",    required_argument, NULL, CHOICE_HEIGHT },
        { "count",     required_argument, NULL, CHOICE_COUNT },
        { "min",       required_argument, NULL, CHOICE_MIN},
        { "max",       required_argument, NULL, CHOICE_MAX},
        { "exam",      required_argument, NULL, CHOICE_EXAM},
        { "input",     required_argument, NULL, CHOICE_INPUT},
        { "kernel",    required_argument, NULL, CHOICE_KERNEL},
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
                if(options.value_type == NULL)
                    options.value_type = "FLOAT32";
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
                    options.value_count = "512";
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
        }
    }
    //init default value
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
        options.value_count = "12";
    }
    if(!options.flag_min) {
        options.flag_min = true;
        options.value_min = "0.0";
    }
    if(!options.flag_max) {
        options.flag_max = true;
        options.value_max = "1.0";
    }

    if(options.flag_type)
        do_gen(&options);

    if(options.flag_exam){
        if(options.flag_input == false || options.value_input == NULL || options.flag_kernel == false || options.value_kernel == NULL){
            puts("argument [--input FILE_PATH] or [--kernel DIR] was not given.");
            return 0;
        }
        conv2d(&options);
    }

    if(options.flag_h)
        print_usage();
    return 0;
}