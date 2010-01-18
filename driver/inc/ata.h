#ifndef ATA_H
#define ATA_H

extern struct driver_interface ata;

#define ATA_DATA 		0x0000
#define ATA_ERROR 		0x0001
#define ATA_COUNT		0x0002
#define ATA_ADDR0		0x0003
#define ATA_ADDR1		0x0004
#define ATA_ADDR2		0x0005
#define ATA_FLAGS		0x0006
#define ATA_RSTAT		0x0007
#define ATA_ASTAT		0x0206

#define ATA_RSTAT_BSY	0x0080
#define ATA_RSTAT_RDY	0x0040
#define ATA_RSTAT_DF	0x0020
#define ATA_RSTAT_SRV	0x0010
#define ATA_RSTAT_DRV	0x0008
#define ATA_RSTAT_ERR	0x0001

#define ATA_ASTAT_HOB	0x0080
#define ATA_ASTAT_SRST	0x0004
#define ATA_ASTAT_nEIN	0x0002

#endif
