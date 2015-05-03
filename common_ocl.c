#include "./common_ocl.h"

#define CHECK_CLSTATUS(status, func) \
		if (status != CL_SUCCESS) { \
				printf("%s status = %d, failed at line %d, exit\n", func, status, __LINE__); \
				exit(1); \
		}

void InitOpenCLEnv(cl_platform_id *&platforms, cl_device_id *&devices, cl_uint &num_devices, 
				cl_context &context, cl_command_queue &cmd_queue)
{
	// Get platforms
	cl_int status;
	cl_uint num_platforms = 0;

	status = clGetPlatformIDs(0, NULL, &num_platforms);
	CHECK_CLSTATUS(status, "clGetPlatformIDs()");
	printf("Number of Platforms: %d\n", num_platforms);

	free(platforms);
	platforms = (cl_platform_id*) malloc(num_platforms*sizeof(cl_platform_id));
	status = clGetPlatformIDs(num_platforms, platforms, NULL);
	CHECK_CLSTATUS(status, "clGetPlatformIDs()");

	// Get devices
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
	CHECK_CLSTATUS(status, "clGetDeviceIDs()");
	printf("Number of Devices on the 1st Platform: %d\n", num_devices);

	free(devices);
	devices = (cl_device_id*) malloc(num_devices*sizeof(cl_device_id));
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, num_devices, devices, NULL);
	CHECK_CLSTATUS(status, "clGetDeviceIDs()");

	// Create context
	context = clCreateContext(NULL, num_devices, devices, NULL, NULL, &status);
	CHECK_CLSTATUS(status, "clCreateContext()");

	// Create command queue
	cmd_queue = clCreateCommandQueue(context, devices[0], 0, &status);
	CHECK_CLSTATUS(status, "clCreateCommandQueue()");
}

void ReadSourse(const char *kernel_path, char *&program_source)
{
	cl_int status;
	FILE *fp;
	long int size;

	printf("Program file is: %s\n", kernel_path);

	fp = fopen(kernel_path, "rb");
	if(!fp) 
	{
		printf("Could not open kernel file\n");
		exit(-1);
	}
 	status = fseek(fp, 0, SEEK_END);
    if(status != 0) 
	{
        printf("Error seeking to end of file\n");
    	exit(-1);
    }
    size = ftell(fp);
    if(size < 0) 
	{
    	printf("Error getting file position\n");
    	exit(-1);
    }

    rewind(fp);

	free(program_source);
    program_source = (char*) malloc(size + 1);

 	for (int i = 0; i < size+1; i++) 
	{
		program_source[i]='\0';
  	}

	if(program_source == NULL) 
	{
  		printf("Error allocating space for the kernel source\n");
  		exit(-1);
  	}

 	fread(program_source, 1, size, fp);
  	program_source[size] = '\0';

  	return;
}




