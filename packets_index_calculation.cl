//#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
//#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable 
//#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable 
//#pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable 

/*
 * Identifier of a flow.
 */
struct tuple5
{
	unsigned int 	src_ip; 	//Source IP
	unsigned int	dst_ip;		//Destination IP
	unsigned short 	src_port;	//Source Port
	unsigned short	dst_port;	//Destination Port
	unsigned short	protocol;	//Protocol Number
} __attribute__ ((aligned(4)));
typedef struct tuple5 tuple5_t;

/*
 * Packet information which needs to be processed.
 */
struct packet_info
{
	struct tuple5	flow_id;	//Flow identifier this packet belongs to
	unsigned int	packet_size;//Size of packet
	unsigned int 	index;		//Index in flow table
} __attribute__ ((aligned(4)));
typedef struct packet_info packet_info_t;

/*
 * Entry of a flow table which calculates throughput 
 * of every flow.
 */
/*struct flow_table_entry
{
	struct tuple5	flow_id;		//Identifier of flow
	unsigned int	throughput; 	//unit is byte
	bool			invalid;		//used to judge whether an entry exists
	int				mutex;
} __attribute__ ((aligned(4)));
typedef struct flow_table_entry flow_table_entry_t;
*/

/*
 * Calculate hash index of packet.
 */
__kernel void PacketsIndexCalculation(
							const unsigned int 	packets_info_size,
							__global packet_info_t	*packets_info,
							const unsigned int		flow_table_size )
{
	int gid = get_global_id(0);
	__global packet_info_t *p = packets_info + gid;

	//calculate hash value
	p->index = ( ((p->flow_id).src_ip & 0xff) | (((p->flow_id).dst_ip & 0xff)<<8) |
				 (((p->flow_id).src_port & 0xff)<<16) | (((p->flow_id).dst_port & 0xff)<<24) )%flow_table_size;
	printf("gid=%d\tindex:=%d\n", gid, p->index);
}
