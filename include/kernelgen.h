#pragma once
#define _GNU_SOURCE
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

enum {
    CHOICE_H = 'h',
    CHOICE_T = 't',
    CHOICE_DTYPE,
    CHOICE_WIDTH,
    CHOICE_HEIGHT,
    CHOICE_COUNT,
    CHOICE_CHANNEL,
    CHOICE_MIN,
    CHOICE_MAX,
    CHOICE_EXAM = 'e',
    CHOICE_INPUT,
    CHOICE_KERNEL
};

struct kernel_options{
    bool flag_h;
    bool flag_type;
    bool flag_dtype;
    bool flag_width;
    bool flag_height;
    bool flag_count;
    bool flag_channel;
    bool flag_min;
    bool flag_max;
    bool flag_exam;
    bool flag_input;
    bool flag_kernel;

    char* value_type;
    char* value_dtype;
    char* value_width;
    char* value_height;
    char* value_count;
    char* value_channel;
    char* value_min;
    char* value_max;
    char* value_exam;
    char* value_input;
    char* value_kernel;
};