#include "../include/simpleconv.h"

//TODO : depthwise conv implementation
int conv2d(struct tensor* input, struct tensor* output, struct tensor* kernel, int group, int pads) {
    int output_idx = 0;
    int kernel_idx = 0;
    for(int idx1 = 0; idx1 < output->shape[1]; idx1++){
        for(int idx2 = 0; idx2 < output->shape[2]; idx2++) {
            for(int idx3 = 0; idx3 < kernel->shape[1]; idx3++) {
                for(int idx4 = 0; idx4 < kernel->shape[2]; idx4++) {
                    float val1 = input->data[(input->shape[2] * idx1)+ idx2 +(input->shape[2] * idx3) + idx4];
                    float val2 = kernel->data[(kernel->shape[2] * idx3) + idx4];
                    output->data[output_idx] += val1 * val2;
                }   
            }
            output_idx++;
        }
    }
}

int getcount(struct tensor* t) {
    int count = 1;
    for (int i = 0; i < t->n_dim; i++){
        count = count * t->shape[i];
    }
    return count;
}

int getcountshape(int* shape, int dim){
    int count = 1;
    for (int i = 0; i < dim; i++){
        count = count * shape[i];
    }
    return count;
}

void getresultshape(struct tensor* input, struct tensor* kernel, struct tensor* output){
    for(int idx = 0; idx < output->n_dim; idx++) {
         output->shape[idx] = 1;
    }

    for(int idx = output->n_dim - 1; idx > 0; idx--) {
         output->shape[idx] = ((input->shape[idx] - kernel->shape[idx]) / 1 ) + 1;
     }
}

void printtensor(struct tensor* t, const char* text){
    int dim = t->n_dim;
    int count = getcount(t);

    printf("tensor info ---------------- %s\n", text);
    printf("dim : %d\n", t->n_dim);
    printf("shape : ");
    for(int i = dim-1; i >= 0; i--){
        printf("%d ", t->shape[i]);
    }
    printf("\n");
    printf("data : \n");
    for(int i = 0; i < count; i++){
        if(i != count - 1)
            printf("%f, ", t->data[i]);
        else
            printf("%f", t->data[i]);
    }
    printf("\n----------------------------\n");
}