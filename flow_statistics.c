#include "./flow_statistics.h"
#include <malloc.h>

void ConstructPacketInfo(packet_info_t *&packets_info, size_t num)
{
	free(packets_info);
	packets_info = (packet_info_t*) malloc(num*sizeof(packet_info_t));
	for (int i=0; i<num; ++i)
	{
		packets_info[i].flow_id.src_ip = 0xC0A80304; //192.168.3.4
		packets_info[i].flow_id.dst_ip = 0xC0A80403; //192.168.4.3
		packets_info[i].flow_id.src_port = 5000;
		packets_info[i].flow_id.dst_port = 4000;
		packets_info[i].flow_id.protocol = 3;
		packets_info[i].packet_size = 1024;
		packets_info[i].index = -1;
	}
}

/*void ConstructFlowTable(flow_table_entry_t **&flow_table, size_t flow_table_size)
{
	free(flow_table);
	flow_table = (flow_table_entry_t*) malloc(flow_table_size*sizeof(flow_table_entry_t));
	for (int i=0; i<flow_table_size; ++i)
	{
		flow_table[i].throughput = 0;
		flow_table[i].invalid = true;
		flow_table[i].p_next = NULL;	
	}
}
*/
void FlowStatistics(const unsigned int &flow_table_size, 
					flow_table_entry_t **flow_table,
					const unsigned int &packets_num, 
					packet_info_t *packets )
{
	unsigned int index;
	flow_table_entry_t *pp, *p;

	for (int i=0; i<packets_num; ++i)
	{
		index = packets[i].index;
		printf("i: %d\tindex: %d\n", i, index);
		if (NULL == flow_table[index])
		{
			flow_table[index] = (flow_table_entry_t*)malloc(sizeof(flow_table_entry_t));
			flow_table[index]->flow_id.src_ip   = packets[i].flow_id.src_ip;   
			flow_table[index]->flow_id.dst_ip   = packets[i].flow_id.dst_ip;  
			flow_table[index]->flow_id.src_port = packets[i].flow_id.src_port;
			flow_table[index]->flow_id.dst_port = packets[i].flow_id.dst_port;
			flow_table[index]->flow_id.protocol = packets[i].flow_id.protocol;
			flow_table[index]->throughput = packets[i].packet_size;
			flow_table[index]->next = NULL;
			continue;
		}

		pp = NULL;
		p = flow_table[index];
		while (NULL != p)
		{
			if ( Tuple5Equal(packets[i].flow_id, p->flow_id) )
			{
				p->throughput += packets[i].packet_size;
				break;
			}
			else
			{
				pp = p;
				p = p->next;
			}
		}

		// New flow with conflict
		if (NULL == p)
		{
			pp->next = (flow_table_entry_t*)malloc(sizeof(flow_table_entry_t));
			pp->next->flow_id.src_ip   = packets[i].flow_id.src_ip;   
			pp->next->flow_id.dst_ip   = packets[i].flow_id.dst_ip;  
			pp->next->flow_id.src_port = packets[i].flow_id.src_port;
			pp->next->flow_id.dst_port = packets[i].flow_id.dst_port;
			pp->next->flow_id.protocol = packets[i].flow_id.protocol;
			pp->next->throughput = packets[i].packet_size;
			pp->next->next = NULL;
			continue;
		}
	}
}


bool Tuple5Equal(const tuple5_t &v1, const tuple5_t &v2)
{
	if ( (v1.src_ip   == v2.src_ip  ) &&
		 (v1.dst_ip   == v2.dst_ip  ) &&
		 (v1.src_port == v2.src_port) &&
		 (v1.dst_port == v2.dst_port) &&
		 (v1.protocol == v2.protocol) )
		return true;
	else
		return false;
}

