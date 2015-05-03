#include "./common_ocl.h"
#include "./flow_statistics.h"

#define CHECK_CLSTATUS(status, func) \
		if (status != CL_SUCCESS) { \
				printf("%s status = %d, failed at line %d, exit\n", func, status, __LINE__); \
				exit(1); \
		}

int main(int argc, char** argv)
{
	const cl_uint kPacketNumber = 1024;
	const cl_uint kFlowTableSize = 1024;

	// Initialize OpenCL environment
	cl_platform_id *platforms = NULL;
	cl_device_id *devices = NULL;
	cl_uint num_devices = 0;
	cl_context context = NULL;
	cl_command_queue cmd_queue;
	InitOpenCLEnv(platforms, devices, num_devices, context, cmd_queue);

	// Construct packet data and flow table
	packet_info_t *packets_info = NULL;
	ConstructPacketInfo(packets_info, kPacketNumber);
	flow_table_entry_t **flow_table = (flow_table_entry_t**)malloc(kFlowTableSize*sizeof(flow_table_entry_t*));
	for (int i=0; i<kFlowTableSize; ++i)	flow_table[i] == NULL;
	//ConstructFlowTable(flow_table, kFlowTableSize);

	// Calculate by GPU
	int status;
	cl_mem packets_info_buffer = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, 
										kPacketNumber*sizeof(packet_info_t), (void*)packets_info, &status);
	CHECK_CLSTATUS(status, "clCreateBuffer()");

	char *program_source = NULL;
	ReadSourse("./packets_index_calculation.cl", program_source);
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&program_source, NULL, &status);
	CHECK_CLSTATUS(status, "clCreateProgramWithSource()");

	status = clBuildProgram(program, num_devices, devices, NULL, NULL, NULL);
	if(status == CL_BUILD_PROGRAM_FAILURE) 
	{
	    size_t log_size;
		clGetProgramBuildInfo(program, devices[0],CL_PROGRAM_BUILD_LOG,0,NULL,&log_size);
		char *log = (char*) malloc(log_size);
		clGetProgramBuildInfo(program,devices[0],CL_PROGRAM_BUILD_LOG,log_size,log,NULL);
		printf("%s\n", log);
		free(log);
	}
	CHECK_CLSTATUS(status, "clBuildProgram()");

	cl_kernel kernel = clCreateKernel(program, "PacketsIndexCalculation", &status);
	CHECK_CLSTATUS(status, "clCreateKernel()");

	status |= clSetKernelArg(kernel, 0, sizeof(cl_uint), (void*)&kPacketNumber);
	status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&packets_info_buffer);
	status |= clSetKernelArg(kernel, 2, sizeof(cl_uint), (void*)&kFlowTableSize);
	CHECK_CLSTATUS(status, "clSetKernelArg()");

	size_t global_worksize[] = {kPacketNumber};
	status = clEnqueueNDRangeKernel(cmd_queue, kernel, 1, NULL, global_worksize, NULL, 0, NULL, NULL);
	CHECK_CLSTATUS(status, "clEnqueueNDRangeKernel()");

	status = clFinish(cmd_queue);
	CHECK_CLSTATUS(status, "clFinish()");
	printf("Have finished calculation!\n");

	FlowStatistics(kFlowTableSize, flow_table, kPacketNumber, packets_info);
	printf("Have finished FlowStatistics()\n");

	// Release opencl resources
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmd_queue);
	clReleaseMemObject(packets_info_buffer);
	clReleaseContext(context);

	// Free memory resources
	free(platforms);
	free(devices);
	free(packets_info);
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
	free(program_source);
	
	return 0;
}
