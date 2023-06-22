#ifndef KEVCC_TOKENIZER_GENERATOR_INCLUDE_FINITE_AUTOMATON_OBJECT_POOL_INTSETMAP_NODE_POOL_H
#define KEVCC_TOKENIZER_GENERATOR_INCLUDE_FINITE_AUTOMATON_OBJECT_POOL_INTSETMAP_NODE_POOL_H
#include "tokenizer_generator/include/finite_automaton/hashmap/intset_map.h"

KevIntSetMapNode* kev_intsetmap_node_pool_acquire(void);
KevIntSetMapNode* kev_intsetmap_node_pool_allocate(void);
void kev_intsetmap_node_pool_deallocate(KevIntSetMapNode* intsetmap_node);
void kev_intsetmap_node_pool_free(void);

#endif
