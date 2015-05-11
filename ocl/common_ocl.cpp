#include "common_ocl.hpp"
#define DEBUG 1

void InitOpenCLEnv(cl_platform_id &platform, 
                    cl_device_id &device, 
                    cl_context &context, 
                    cl_command_queue &cmd_queue)
{
    cl_int status;

    // Get the 1st platfomr
#if DEBUG
    printf("clGetPlatformIDs() ...\t");
#endif
    status = clGetPlatformIDs(1, &platform, NULL);
    CHECK_CLSTATUS(status, "clGetPlatformIDs()");
#if DEBUG
    printf("Finish\n");
#endif

    // Get the 1st device of the 1st platform
#if DEBUG
    printf("clGetDeviceIDs() ...\t");
#endif
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    CHECK_CLSTATUS(status, "clGetDeviceIDs()");
#if DEBUG
    printf("Finish\n");
#endif

    // Create context
#if DEBUG
    printf("clCreateContext() ...\t");
#endif
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);
    CHECK_CLSTATUS(status, "clCreateContext()");
#if DEBUG
    printf("Finish\n");
#endif

    // Create command queue
#if DEBUG
    printf("clCreateCommandQueue() ...\t");
#endif
    cmd_queue = clCreateCommandQueue(context, device, 0, &status);
    CHECK_CLSTATUS(status, "clCreateCommandQueue()");
#if DEBUG
    printf("Finish\n");
#endif
}

void ReadSource(const char *kernel_path, char *&program_source)
{
    cl_int status;
    FILE *fp;
    long int size;

    printf("Program file is: %s\n", kernel_path);

    fp = fopen(kernel_path, "rb");
    if (!fp) { printf("Could not open kernel file.\n"); exit(-1); }

    status = fseek(fp, 0, SEEK_END);
    if (0 != status) { printf("Error seeking to end of file.\n"); exit(-1); }

    size = ftell(fp);
    if (size < 0) { printf("Error getting file position\n"); exit(-1); }

    rewind(fp);

    free(program_source);
    program_source = (char*) malloc(size+1);

    for (int i=1; i < size+1; i++)
    {
        program_source[i] = '\0';
    }

    if (NULL == program_source) { printf("Error allocating sapce for the kernel source\n"); exit(-1); }

    fread(program_source, 1, size, fp);
    program_source[size] = '\0';

    return;
}
