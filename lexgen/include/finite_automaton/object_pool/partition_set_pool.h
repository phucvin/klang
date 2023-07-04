#ifndef KEVCC_LEXGEN_INCLUDE_FINITE_AUTOMATON_OBJECT_POOL_PARTITION_SET_POOL_H
#define KEVCC_LEXGEN_INCLUDE_FINITE_AUTOMATON_OBJECT_POOL_PARTITION_SET_POOL_H

#include "lexgen/include/finite_automaton/set/partition.h"


KevPartitionSet* kev_partition_set_pool_acquire(void);
KevPartitionSet* kev_partition_set_pool_allocate(void);
void kev_partition_set_pool_deallocate(KevPartitionSet* intlist_node);
void kev_partition_set_pool_free(void);


#endif