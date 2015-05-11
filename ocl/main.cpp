#include "./common_ocl.hpp"
#include "./traffic_statistics.hpp"

int main(int argc, char** argv)
{
    // Initialize OpenCL environment
    cl_platform_id  platform = NULL;
    cl_device_id    device = NULL;
    cl_context      context = NULL;
    cl_command_queue    cmd_queue;
    cl_int          status;
    InitOpenCLEnv(platform, device, context, cmd_queue);

    // Create kernel
    char *program_source = NULL;
    ReadSource("./packet_index_calculation.cl", program_source);
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&program_source, NULL, &status);
    CHECK_CLSTATUS(status, "clCreateProgramWithSource()");
    status = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (status == CL_BUILD_PROGRAM_FAILURE)
    {
       size_t log_size;
       clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
       char *log = (char*) malloc(log_size);
       clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
       printf("%s\n", log);
       free(log);
    }
    CHECK_CLSTATUS(status, "clBuildProgram()");
    
    cl_kernel kernel = clCreateKernel(program, "PacketIndexCalculation", &status);
    CHECK_CLSTATUS(status, "clCreateKernel()");

    // Construct data
    const cl_uint kPacketNumber = 1024;
    const cl_uint kFlowTableSize = 1024;
    packet_info_t *packets_info_buffer_data = NULL;
    ConstructPacketsInfo(packets_info_buffer_data, kPacketNumber);
    flow_table_entry_t **flow_table = (flow_table_entry_t**) malloc(
                                            kFlowTableSize * sizeof(flow_table_entry_t*) );
    for (int i=0; i<kFlowTableSize; ++i) { flow_table[i] = NULL; }

    cl_mem packets_info_buffer = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, 
                                                kPacketNumber*sizeof(packet_info_t), 
                                                (void*)packets_info_buffer_data, &status );
    CHECK_CLSTATUS(status, "clCreateBuffer()");

    // Set arguments
    status  = clSetKernelArg(kernel, 0, sizeof(cl_uint), (void*)&kPacketNumber);
    status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&packets_info_buffer);
    status |= clSetKernelArg(kernel, 2, sizeof(cl_uint), (void*)&kFlowTableSize);
    CHECK_CLSTATUS(status, "clSetKernelArg()");

    // Run the kernel
    size_t global_work_size[] = {kPacketNumber};
    status = clEnqueueNDRangeKernel(cmd_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
    CHECK_CLSTATUS(status, "clEnqueueNDRangeKernel()");

    // Get result
    printf("clEnqueueMapBuffer() ...\t");
    void *packets_info_buffer_mapptr = clEnqueueMapBuffer(cmd_queue, packets_info_buffer, CL_TRUE, 
                                            CL_MAP_READ, 0, 1024*sizeof(int32_t), 0, NULL, NULL, NULL);
    CHECK_CLSTATUS(status, "clEnqueueMapBuffer()");
    if (packets_info_buffer_mapptr == packets_info_buffer_data) 
        printf("Finished\n");
    else
        printf("Wrong: unequal map\n"); 

    printf("TrafficStatistics() ...\t");
    TrafficStatistics(kFlowTableSize, flow_table, kPacketNumber, (packet_info_t*)packets_info_buffer_mapptr);
    printf("Finished\n");
    clEnqueueUnmapMemObject(cmd_queue, packets_info_buffer, packets_info_buffer_mapptr, 0, NULL, NULL);

    // Release sources
    printf("clReleaseMemObject() ...\t");
    clReleaseMemObject(packets_info_buffer);
    printf("Finish\n");
    printf("clReleaseKernel() ...\t");
    clReleaseKernel(kernel);
    printf("Finish\n");
    printf("clReleaseProgram() ...\t");
    clReleaseProgram(program);
    printf("Finish\n");
    printf("clReleaseCommandQueue() ...\t");
    clReleaseCommandQueue(cmd_queue);
    printf("Finish\n");
    printf("clReleaseContext() ... \t");
    clReleaseContext(context);
    printf("Finish\n");
    
    // Free memory
    free(program_source);
    free(packets_info_buffer_data);
    for (int i=0; i<kFlowTableSize; ++i)
    {
        flow_table_entry_t *pp = NULL;
        flow_table_entry_t *p = flow_table[i];
        while(NULL != p)
        {
            pp = p;
            p = p->next;
            free(pp);
        }
    }
    free(flow_table);
}
