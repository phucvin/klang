#ifndef KEVCC_LEXGEN_INCLUDE_FINITE_AUTOMATON_OBJECT_POOL_NFA_POOL_H
#define KEVCC_LEXGEN_INCLUDE_FINITE_AUTOMATON_OBJECT_POOL_NFA_POOL_H
#include "lexgen/include/finite_automaton/finite_automaton.h"

KevFA* kev_fa_pool_acquire(void);
KevFA* kev_fa_pool_allocate(void);
void kev_fa_pool_deallocate(KevFA* fa);
void kev_fa_pool_free(void);

#endif