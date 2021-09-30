#pragma once
#include <stdio.h>

struct tensor{
    int n_dim; //always 3
    int *shape; //[C, H, W]
    float *data;
};

void printtensor(struct tensor* t, const char* text);
int getcount(struct tensor* t);
int getcountshape(int* shape, int dim);
int conv2d(struct tensor* input, struct tensor* output, struct tensor* kernel, int group, int pads);
void getresultshape(struct tensor* input, struct tensor* kernel, struct tensor* output, int group);