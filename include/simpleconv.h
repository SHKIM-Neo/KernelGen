#include "kernelgen.h"

#define 

struct tensor{
    int n_dim; //always 3
    int *shape; //[W, H, C]
    float *data;
};

int conv2d(struct tensor* input, struct tensor* output, struct tensor* kernel, int group, int* pads);
int getresultshape(struct tensor* input, struct tensor* kernel, int* pads);