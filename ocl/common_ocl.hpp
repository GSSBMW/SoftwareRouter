#ifndef OCL_COMMON_OCL_HPP_
#define OCL_COMMON_OCL_HPP_

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <malloc.h>
#include <CL/cl.h>
#include <CL/cl_intel.h>

#define CHECK_CLSTATUS(status, func) \
    if (status != CL_SUCCESS) { \
        printf("%s status = %d, failed at line %d, exit\n", func, status, __LINE__); \
        exit(1); \
    }

void InitOpenCLEnv(cl_platform_id &platform, 
                    cl_device_id &device, 
                    cl_context &context, 
                    cl_command_queue &cmd_queue);

void ReadSource(const char *kernel_path, char *&program_source);


#endif // OCL_COMMON_OCL_HPP_
