#ifndef OCL_TRAFFIC_STATISTICS_HPP_
#define OCL_TRAFFIC_STATISTICS_HPP_

#include <cstdio>
#include <cstdlib>
#include <CL/cl.h>

/*
 * Identifier of a flow.
 */
struct tuple5
{
	cl_uint 	src_ip; 	//Source IP
	cl_uint		dst_ip;		//Destination IP
	cl_ushort 	src_port;	//Source Port
	cl_ushort	dst_port;	//Destination Port
	cl_ushort	protocol;	//Protocol Number
} __attribute__ ((aligned(4)));
typedef struct tuple5 tuple5_t;

/*
 * Packet information which needs to be processed.
 */
struct packet_info
{
	struct tuple5	flow_id;	//Flow identifier this packet belongs to
	cl_uint			packet_size;//Size of packet
	cl_uint			index;		//Index in flow table
} __attribute__ ((aligned(4)));
typedef struct packet_info packet_info_t;

/*
 * Entry of a flow table which calculates throughput 
 * of every flow.
 */
struct flow_table_entry
{
	struct tuple5	flow_id;		//Identifier of flow
	cl_uint			throughput; 	//unit is byte
	struct flow_table_entry *next;
} __attribute__ ((aligned(4)));
typedef struct flow_table_entry flow_table_entry_t;

void ConstructPacketsInfo(packet_info_t *&packets_info, const size_t num);
void TrafficStatistics(const unsigned int &flow_table_size, flow_table_entry_t **flow_table,
                    const unsigned int &packets_num, packet_info_t *packets);
bool Tuple5Equal(const tuple5_t &v1, const tuple5_t &v2);


#endif // OCL_TRAFFIC_STATISTICS_HPP_
