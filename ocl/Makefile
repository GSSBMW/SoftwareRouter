traffic_statistics : main.cpp common_ocl.o traffic_statistics.o
	g++ -o traffic_statistics main.cpp common_ocl.o traffic_statistics.o -lOpenCL
	rm common_ocl.o traffic_statistics.o *.gch

common_ocl.o : common_ocl.hpp common_ocl.cpp
	g++ -c common_ocl.hpp common_ocl.cpp -lOpenCL

traffic_statistics.o : traffic_statistics.hpp traffic_statistics.cpp
	g++ -c traffic_statistics.hpp traffic_statistics.cpp -lOpenCL

clean:
	rm -f traffic_statistics
