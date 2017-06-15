#include <assert.h>
#include <dlfcn.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "c_hook.h"

static bool rr_it(void *ctx, void *it)
{
   char name[DNS_MAX_HOSTNAME_LEN + 1];
   
   FnTable *fn_table = ctx;
   fn_table->name(it, name);
   printf("- found RR [%s] with type: %" PRIu32 "\n",
	  name, fn_table->rr_type(it));
   return 0;
}

void hook(const FnTable *fn_table, ParsedPacket *parsed_packet)
{
    uint32_t flags;
    
    assert(fn_table->abi_version == ABI_VERSION);
    flags = fn_table->flags(parsed_packet);
    printf("flags as seen by the C hook: %" PRIx32 "\n", flags);
    fn_table->set_flags(parsed_packet, flags | 0x10);
    puts("Answer section");	 
    fn_table->iter_answer(parsed_packet, rr_it, fn_table);
    puts("Nameservers section");
    fn_table->iter_nameservers(parsed_packet, rr_it, fn_table);
    puts("Additional section");	 
    fn_table->iter_additional(parsed_packet, rr_it, fn_table);
}