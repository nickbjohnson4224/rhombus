// Copyright 2009 Nick Johnson

#include <lib.h>
#include <init.h>

/***** CPUID FEATURE FLAGS *****/
#define CF_FPU 	0x00000001	// x87 FPU
#define CF_VME 	0x00000002	// Virtual 8086
#define CF_DE  	0x00000004	// I/O Breakpoints
#define CF_PSE 	0x00000008	// 4 MB Pages
#define CF_TSC 	0x00000010	// Timestamps (rdtsc)
#define CF_MSR 	0x00000020	// Model specific registers
#define CF_PAE 	0x00000040	// PAE support
#define CF_MCE 	0x00000080	// Machine check exception
#define CF_CX8 	0x00000100	// 64 bit compare/exchange
#define CF_APIC	0x00000200	// APIC available
#define CF_SEP	0x00000800	// SYSTENTER/EXIT
#define CF_MTRR	0x00001000  // Memory type range registers
#define CF_PGE	0x00002000	// TLB flush uneeded b/w processes
#define CF_MCA	0x00004000	// Machine check architecture
#define CF_CMOV	0x00008000	// Conditional move instructions
#define CF_PAT	0x00010000	// Page attribute table
#define CF_PSEE	0x00020000	// 4 MB pages for PAE
#define CF_PSN	0x00040000	// Has serial number
#define CF_CLFL	0x00080000	// CLFLUSH instruction
#define CF_DS	0x00200000	// Debug store
#define CF_ACPI	0x00400000	// ACPI support
#define CF_MMX	0x00800000	// MMX
#define CF_FXSR	0x01000000	// FXSAVE FXRSTOR
#define CF_SSE	0x02000000	// SSE
#define CF_SSE2	0x04000000	// SSE2
#define CF_SS	0x08000000	// Self Snoop
#define CF_HTT	0x10000000	// Hyperthreading support
#define CF_TM	0x20000000	// Thermal monitor
#define CF_PBE	0x80000000	// Pending break enabled

__attribute__ ((section(".ttext"))) 
void init_detect() {
	u32int i;

	colork(0xC);
	printk("Memory: ");

	// This is quite lazy - it finds the free block directly above 0x100000
	struct memory_map *mem_map = (void*) mboot->mmap_addr;
	u32int nmem_map = mboot->mmap_length / sizeof(struct memory_map);
	for (i = 0; i < nmem_map; i++) {
		if (mem_map[i].base_addr_low == 0x100000) {
			memsize = mem_map[i].length_low + 0x100000;
			printk("%d MB", memsize >> 20);
			break;
		}
	}
	cursek(36, -1);
	printk("done\n");

	colork(0x9);
	printk("CPU: ");

	extern void cpuid_name(char*);

	char manufacturer[13];
	cpuid_name(manufacturer);
	manufacturer[12] = '\0';
	printk("%s ", manufacturer);

	cursek(36, -1);
	printk("done\n");

	colork(0xA);
	printk("Initrd: ");

	init_kload();
	
	cursek(36, -1);
	printk("done\n");

	colork(0xF);

}
