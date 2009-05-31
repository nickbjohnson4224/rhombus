#include <lib.h>
#include <call.h>

void *call_table[32] = {
fork_call, 
exit_call,
sint_call,
sret_call,
mmap_call,
umap_call,
rmap_call,
fmap_call,
eout,
rirq,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
NULL, NULL,
};
