main : main.c flow_statistics.o common_ocl.o
	g++ -o main main.c flow_statistics.o common_ocl.o -lOpenCL
	rm flow_statistics.o common_ocl.o
flow_statistics.o : flow_statistics.h flow_statistics.c
	g++ -c flow_statistics.c -lOpenCL
common_ocl.o : common_ocl.h common_ocl.c
	g++ -c common_ocl.c -lOpenCL

clean:
	rm main	
