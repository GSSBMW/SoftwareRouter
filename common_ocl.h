#ifndef COMMON_OCL_H 
#define COMMON_OCL_H

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <malloc.h>
#include <math.h>
#include <CL/cl.h>

void InitOpenCLEnv(cl_platform_id *&platforms, cl_device_id *&devices, cl_uint &num_devices, cl_context& context, cl_command_queue& cmd_queue);

void ReadSourse(const char *kernel_path, char *&protram_source);

#endif // COMMON_OCL_H
