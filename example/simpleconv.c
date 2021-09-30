#include "../include/simpleconv.h"

//TODO : depthwise conv implementation
int conv2d(struct tensor* input, struct tensor* output, struct tensor* kernel, int group, int pads) {
    int output_idx = 0;
    int kernel_idx = 0;

    int output_w = output->shape[2];
    int output_h = output->shape[1];
    int output_c = output->shape[0];

    for(int g = 0; g < group; g++) {
        int start_channel = g * (kernel->shape[0] / group);
        int end_channel = (g * (kernel->shape[0] / group)) + (kernel->shape[0] / group);
        for(int idx_o_h = 0; idx_o_h < output_h; idx_o_h++) {
            for(int idx_o_w = 0; idx_o_w < output_w; idx_o_w++) {
                for(int idx_k_c = start_channel; idx_k_c < end_channel; idx_k_c++) {
                    for(int idx_k_h = 0; idx_k_h < kernel->shape[1]; idx_k_h++) {
                        for(int idx_k_w = 0; idx_k_w < kernel->shape[2]; idx_k_w++) {
                            float val1 = input->data[ (input->shape[1] * input->shape[2] * idx_k_c) + (input->shape[2] * idx_o_h) + idx_o_w + (input->shape[2] * idx_k_h) + idx_k_w];
                            float val2 = kernel->data[ (kernel->shape[1] * kernel->shape[2] * idx_k_c) + (kernel->shape[2] * idx_k_h) + idx_k_w];
                            output->data[output_idx] += val1 * val2;
                        }
                    }
                }
                output_idx++;
            }
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

void getresultshape(struct tensor* input, struct tensor* kernel, struct tensor* output, int group){
    //init
    //printf("output ndim : %d\n", output->n_dim);
    for(int idx = 0; idx < output->n_dim; idx++) {
         output->shape[idx] = 1;
    }
    for(int idx = output->n_dim - 1; idx >= 0; idx--) {
        if(idx != 0) {//H, W
            int val = ((input->shape[idx] - kernel->shape[idx]) / 1 ) + 1;
            //printf("shape[%d] : %d\n", idx, val);
            output->shape[idx] = val;
        }
        else {
            int val = (((input->shape[idx] - kernel->shape[idx]) / 1 ) + 1) * group;
            //printf("shape[%d] : %d\n", idx, val);
            output->shape[idx] = val;
        }
    }
}

void printtensor(struct tensor* t, const char* text){
    int dim = t->n_dim;
    int count = getcount(t);
    const char* caption[10] = {"channel", "height", "width"};
    printf("tensor info ---------------- %s\n", text);
    printf("dim : %d\n", t->n_dim);
    printf("shape -- ");
    for(int i = dim-1; i >= 0; i--){
        printf("%s : %d ", caption[i], t->shape[i]);
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