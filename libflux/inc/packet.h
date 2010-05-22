#ifndef FLUX_PACKET_H
#define FLUX_PACKET_H

#include <flux/arch.h>

/*** Flux Packet Protocol (FPP) ***/

#define PACKET_MAXDATA		(PAGESZ - 64)

#define PACKET_PROTOCOL		1	/* protocol version */
#define PACKET_SOFTWARE		1	/* software version */

#define PACKET_ENC_UNK		0
#define PACKET_ENC_ASCII	1
#define PACKET_ENC_UTF8		2
#define PACKET_ENC_UTF32	3
#define PACKET_ENC_GRAPH	4

#define PACKET_FLAG_BENDIAN	0x01
#define PACKET_FLAG_EXTOFF	0x02

struct packet {

	/* general information */
	uint32_t identity;
	uint16_t reserved;
	uint16_t protocol;
	uint16_t software;
	uint8_t  encoding;
	uint8_t  flags;

	/* fragment information */
	uint16_t fragment_index;
	uint16_t fragment_count;

	/* data buffer information */
	uint32_t data_length;
	uint32_t data_offset;

	/* source routing information */
	uint32_t source_pid;
	uint64_t source_inode;

	/* target routing information */
	uint32_t target_pid;
	uint64_t target_inode;

	/* target file offset */
	uint64_t offset;
	uint64_t offset_ext;

} __attribute__ ((packed));

bool  packet_setbuf(struct packet **packet, uint32_t length);
void *packet_getbuf(struct packet *packet);

void *packet_alloc(uint32_t size);
void  packet_free (struct packet *packet);

#endif/*FLUX_PACKET_H*/
