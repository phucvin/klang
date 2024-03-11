#include "kevlr/include/lr_utils.h"
#include "kevlr/include/itemset_def.h"
#include "utils/include/set/hashset.h"

#include <stdlib.h>


static void klr_util_compute_first(KBitSet** firsts, KlrSymbol* symbol, size_t epsilon);

static inline bool klr_util_symbol_is_in_array(KlrSymbol* symbol, KArray* array);

bool klr_util_generate_transition(KlrItemPoolCollec* pool, KlrItemSet* itemset, KlrItemSetClosure* closure, KlrTransSet* transitions) {
  klr_transset_make_empty(transitions);
  KArray* symbols = closure->symbols;
  KBitSet** las = closure->lookaheads;
  /* for kernel item */
  KlrItem* kitem = itemset->items;
  for (; kitem; kitem = kitem->next) {
    KlrRule* rule = kitem->rule;
    if (rule->bodylen == kitem->dot) continue;
    KlrSymbol* symbol = rule->body[kitem->dot];
    KlrItem* item = klr_item_create(&pool->itempool, rule, kitem->dot + 1);
    if (k_unlikely(!item)) return false;
    if (k_unlikely(!(item->lookahead = kbitset_create_copy(kitem->lookahead)))) {
      klr_item_delete(&pool->itempool, item);
      return false;
    }
    KlrItemSet* target = klr_transset_search(transitions, symbol);
    if (!target) {
      if (k_unlikely(!(target = klr_itemset_create(&pool->itemsetpool)))) {
        klr_item_delete(&pool->itempool, item);
        return false;
      }
      if (k_unlikely(!klr_itemset_goto(&pool->itemsettranspool, itemset, symbol, target) ||
          !klr_transset_insert(transitions, symbol, target))) {
        klr_item_delete(&pool->itempool, item);
        klr_itemset_delete(pool, target);
        return false;
      }
    }
    klr_itemset_add_item(target, item);
  }

  /* for non-kernel item */
  size_t closure_size = karray_size(symbols);
  for (size_t i = 0; i < closure_size; ++i) {
    KlrSymbol* head = (KlrSymbol*)karray_access(symbols, i);
    KlrRuleNode* rulenode = head->rules;
    for (; rulenode; rulenode = rulenode->next) {
      KlrRule* rule = rulenode->rule;
      if (rule->bodylen == 0) continue;
      KlrSymbol* symbol = rule->body[0];
      KlrItem* item = klr_item_create(&pool->itempool, rule, 1);
      if (k_unlikely(!item)) return false;
      if (k_unlikely(!(item->lookahead = kbitset_create_copy(las[head->index])))) {
        klr_item_delete(&pool->itempool, item);
        return false;
      }
      KlrItemSet* target = klr_transset_search(transitions, symbol);
      if (!target) {
        if (k_unlikely(!(target = klr_itemset_create(&pool->itemsetpool)))) {
          klr_item_delete(&pool->itempool, item);
          return false;
        }
        if (k_unlikely(!klr_itemset_goto(&pool->itemsettranspool, itemset, symbol, target) ||
            !klr_transset_insert(transitions, symbol, target))) {
          klr_item_delete(&pool->itempool, item);
          klr_itemset_delete(pool, target);
          return false;
        }
      }
      klr_itemset_add_item(target, item);
    }
  }
  return true;
}

static void klr_util_compute_first(KBitSet** firsts, KlrSymbol* symbol, size_t epsilon) {
  KBitSet* first = firsts[symbol->index];
    for (KlrRuleNode* node = symbol->rules; node; node = node->next) {
      KlrRule* rule = node->rule;
      size_t bodylen = rule->bodylen;
      KlrSymbol** body = rule->body;
      size_t i = 0;
      bool first_has_epsilon = kbitset_has_element(first, epsilon);
      for (; i < bodylen; ++i) {
        if (body[i]->kind == KLR_TERMINAL) {
          kbitset_set(first, body[i]->index);
          if (!first_has_epsilon) kbitset_clear(first, epsilon);
          break;
        }
        KBitSet* curr = firsts[body[i]->index];
        /* all first sets have same size, so union will never fail */
        kbitset_union(first, curr);
        if (!kbitset_has_element(curr, epsilon)) {
          if (!first_has_epsilon) kbitset_clear(first, epsilon);
          break;
        }
      }
      if (i == bodylen) kbitset_set(first, epsilon);
    }
}

KBitSet** klr_util_compute_firsts(KlrSymbol** symbols, size_t nsymbol, size_t nterminal) {
  KBitSet** firsts = (KBitSet**)malloc(sizeof (KBitSet*) * nsymbol);
  if (k_unlikely(!firsts)) return NULL;
  KBitSet backup;
  if (k_unlikely(!kbitset_init(&backup, nterminal + 1))) {
    free (firsts);
    return NULL;
  }
  /* initialize */
  for (size_t i = 0; i < nterminal; ++i)
    firsts[i] = NULL;
  for (size_t i = nterminal; i < nsymbol; ++i) {
    if (k_unlikely(!(firsts[i] = kbitset_create(nterminal + 1)))) {
      for (size_t j = nterminal; j < i; ++j)
        kbitset_delete(firsts[j]);
      free(firsts);
      kbitset_destroy(&backup);
      return NULL;
    }
  }
  /* compute firsts */
  bool loop = true;
  while (loop) {
    loop = false;
    for (size_t i = nterminal; i < nsymbol; ++i) {
      if (!loop) kbitset_assign(&backup, firsts[i]);
      klr_util_compute_first(firsts, symbols[i], nterminal);
      if (!loop && !kbitset_equal(&backup, firsts[i]))
        loop = true;
    }
  }
  kbitset_destroy(&backup);
  return firsts;
}

KlrSymbol* klr_util_augment(KlrSymbol* start) {
  KlrSymbol* new_start = klr_symbol_create(KLR_NONTERMINAL, "");
  if (k_unlikely(!new_start)) return NULL;
  KlrRule* start_rule = klr_rule_create(new_start, &start, 1);
  if (k_unlikely(!start_rule)) {
    klr_symbol_delete(new_start);
    return NULL;
  }
  return new_start;
}

KBitSet* klr_util_symbols_to_bitset(KlrSymbol** symbols, size_t length) {
  KBitSet* set = kbitset_create(1);
  if (k_unlikely(!set)) return NULL;
  for (size_t i = 0; i < length; ++i) {
    if (k_unlikely(!kbitset_set(set, symbols[i]->index))) {
      kbitset_delete(set);
      return NULL;
    }
  }
  return set;
}

KlrItemSet* klr_util_get_start_itemset(KlrItemPoolCollec* pool, KlrSymbol* start, KlrSymbol** lookahead, size_t length) {
  KBitSet* la = klr_util_symbols_to_bitset(lookahead, length);
  KlrItemSet* iset = klr_itemset_create(&pool->itemsetpool);
  if (k_unlikely(!iset || !la)) {
    kbitset_delete(la);
    klr_itemset_delete(pool, iset);
    return NULL;
  }
  for (KlrRuleNode* node = start->rules; node; node = node->next) {
    KlrItem* item = klr_item_create(&pool->itempool, node->rule, 0);
    if (k_unlikely(!item || !(item->lookahead = kbitset_create_copy(la)))) {
      klr_itemset_delete(pool, iset);
      kbitset_delete(la);
      return NULL;
    }
    klr_itemset_add_item(iset, item);
  }
  kbitset_delete(la);
  return iset;
}

void klr_util_destroy_terminal_set_array(KBitSet** firsts, size_t size) {
  for (size_t i = 0; i < size; ++i)
    kbitset_delete(firsts[i]);
  free(firsts);
}

static inline bool klr_util_symbol_is_in_array(KlrSymbol* symbol, KArray* array) {
  return symbol->index < karray_size(array) && karray_access(array, symbol->index) == symbol;
}

KlrSymbol** klr_util_get_symbol_array(KlrSymbol* start, KlrSymbol** ends, size_t nend, size_t* p_size) {
  KArray array;
  if (k_unlikely(!karray_init(&array)))
    return NULL;
  if (k_unlikely(!karray_push_back(&array, start))) {
    karray_destroy(&array);
    return NULL;
  }
  start->index = 0;

  for (size_t curr = 0; curr != karray_size(&array); ++curr) {
    KlrSymbol* symbol = (KlrSymbol*)karray_access(&array, curr);
    for (KlrRuleNode* node = symbol->rules; node; node = node->next) {
      KlrRule* rule = node->rule;
      size_t bodylen = klr_rule_get_bodylen(rule);
      KlrSymbol** body = klr_rule_get_body(rule);
      for (size_t i = 0; i < bodylen; ++i) {
        if (klr_util_symbol_is_in_array(body[i], &array))
          continue;
        body[i]->index = karray_size(&array);
        if (k_unlikely(!karray_push_back(&array, body[i]))) {
          karray_destroy(&array);
          return NULL;
        }
      }
    }
  }

  for (size_t i = 0; i < nend; ++i) {
    if (klr_util_symbol_is_in_array(ends[i], &array))
      continue;
    ends[i]->index = karray_size(&array);
    if (k_unlikely(!karray_push_back(&array, ends[i]))) {
      karray_destroy(&array);
      return NULL;
    }
  }

  *p_size = karray_size(&array);
  KlrSymbol** symbol_array = (KlrSymbol**)karray_steal(&array);
  karray_destroy(&array);
  return symbol_array;
}

KlrSymbol** klr_util_get_symbol_array_with_index_unchanged(KlrSymbol* start, KlrSymbol** ends, size_t nend, size_t* p_size) {
  KArray array;
  if (!karray_init(&array))
    return NULL;

  KevHashSet set;
  if (!khashset_init(&set, 64)) {
    khashset_destroy(&set);
    return NULL;
  }
  if (!khashset_insert(&set, start) ||
      !karray_push_back(&array, start)) {
    khashset_destroy(&set);
    karray_destroy(&array);
    return NULL;
  }
  size_t curr = 0;

  while (curr != karray_size(&array)) {
    KlrSymbol* symbol = (KlrSymbol*)karray_access(&array, curr++);
    KlrRuleNode* rule = symbol->rules;
    for (; rule; rule = rule->next) {
      KlrSymbol** rule_body = rule->rule->body;
      size_t len = rule->rule->bodylen;
      for (size_t i = 0; i < len; ++i) {
        if (khashset_has(&set, rule_body[i]))
          continue;
        if (!khashset_insert(&set, rule_body[i]) ||
            !karray_push_back(&array, rule_body[i])) {
          khashset_destroy(&set);
          karray_destroy(&array);
          return NULL;
        }
      }
    }
  }

  for (size_t i = 0; i < nend; ++i) {
    if (khashset_has(&set, ends[i]))
      continue;
    if (!khashset_insert(&set, ends[i]) ||
        !karray_push_back(&array, ends[i])) {
      khashset_destroy(&set);
      karray_destroy(&array);
      return NULL;
    }
  }

  *p_size = karray_size(&array);
  KlrSymbol** symbol_array = (KlrSymbol**)karray_steal(&array);
  karray_destroy(&array);
  khashset_destroy(&set);
  return symbol_array;
}

size_t klr_util_symbol_array_partition(KlrSymbol** array, size_t size) {
  KlrSymbol** left = array;
  KlrSymbol** right = array + size - 1;
  while (true) {
    while (left < right && (*left)->kind == KLR_TERMINAL)
      ++left;
    while (left < right && (*right)->kind == KLR_NONTERMINAL)
      --right;
    if (left >= right) break;
    KlrSymbol* tmp = *left;
    *left = *right;
    *right = tmp;
  }
  return (*left)->kind == KLR_TERMINAL ? left - array + 1 : left - array;
}

KBitSet** klr_util_compute_follows(KlrSymbol** symbols, KBitSet** firsts, size_t nsymbol, size_t nterminal, KlrSymbol* start, KlrSymbol** ends, size_t nend) {
  KBitSet curr_follow;
  if (!kbitset_init(&curr_follow, nterminal + 1))
    return NULL;
  KBitSet** follows = (KBitSet**)malloc(sizeof (KBitSet*) * nsymbol);
  if (k_unlikely(!follows)) {
    kbitset_destroy(&curr_follow);
    return NULL;
  }
  /* initialize */
  for (size_t i = 0; i < nterminal; ++i)
    follows[i] = NULL;
  for (size_t i = nterminal; i < nsymbol; ++i) {
    if (k_unlikely(!(follows[i] = kbitset_create(nterminal + 1)))) {
      for (size_t j = nterminal; j < i; ++j)
        kbitset_delete(follows[j]);
      free(follows);
      kbitset_destroy(&curr_follow);
      return NULL;
    }
  }
  for (size_t i = 0; i < nend; ++i)
    kbitset_set(follows[start->index], ends[i]->index);

  bool loop = true;
  size_t epsilon = nterminal;
  while (loop) {
    loop = false;
    for (size_t i = 0; i < nsymbol; ++i) {
      KlrSymbol* head = symbols[i];
      KBitSet* head_follow = follows[head->index];
      for (KlrRuleNode* rulenode = head->rules; rulenode; rulenode = rulenode->next) {
        KlrSymbol** body = rulenode->rule->body;
        size_t len = rulenode->rule->bodylen;
        if (len == 0) continue;
        size_t i = len;
        kbitset_assign(&curr_follow, head_follow);
        do {
          --i;
          KlrSymbol* symbol = body[i];
          if (symbol->kind == KLR_TERMINAL) {
            kbitset_make_empty(&curr_follow);
            kbitset_set(&curr_follow, symbol->index);
          } else {
            if (kbitset_changed_after_shrinking_union(follows[symbol->index], &curr_follow))
              loop = true;
            if (kbitset_has_element(firsts[symbol->index], epsilon)) {
              kbitset_union(&curr_follow, firsts[symbol->index]);
              kbitset_clear(&curr_follow, epsilon);
            } else {
              kbitset_assign(&curr_follow, firsts[symbol->index]);
            }
          }
        } while (i != 0);
      }
    }
  }

  kbitset_destroy(&curr_follow);
  return follows;
}

size_t klr_util_user_symbol_max_id(KlrCollection* collec) {
  size_t max_id = 0;
  KlrSymbol** symbols = klr_collection_get_symbols(collec);
  size_t nsymbol = collec->nsymbol;
  for (size_t i = 0; i < nsymbol; ++i) {
    /* collec->start is not defined by user */
    if (symbols[i] != collec->start && max_id < symbols[i]->id)
      max_id = symbols[i]->id;
  }
  return max_id;
}

size_t klr_util_user_terminal_max_id(KlrCollection* collec) {
  size_t max_id = 0;
  KlrSymbol** symbols = klr_collection_get_symbols(collec);
  size_t nterminal = collec->nterminal;
  for (size_t i = 0; i < nterminal; ++i) {
    /* collec->start is non-terminal */
    if (max_id < symbols[i]->id)
      max_id = symbols[i]->id;
  }
  return max_id;
}

size_t klr_util_user_nonterminal_max_id(KlrCollection* collec) {
  size_t max_id = 0;
  KlrSymbol** symbols = klr_collection_get_symbols(collec);
  size_t nsymbol = collec->nsymbol;
  for (size_t i = collec->nterminal; i < nsymbol; ++i) {
    /* collec->start is not defined by user */
    if (symbols[i] != collec->start && max_id < symbols[i]->id)
      max_id = symbols[i]->id;
  }
  return max_id;
}
