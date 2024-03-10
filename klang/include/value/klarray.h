#ifndef KEVCC_KLANG_INCLUDE_VALUE_KLARRAY_H
#define KEVCC_KLANG_INCLUDE_VALUE_KLARRAY_H

#include "klang/include/mm/klmm.h"
#include "klang/include/value/klclass.h"
#include "klang/include/value/klvalue.h"

#include <stddef.h>
#include <stdbool.h>

typedef struct tagKlArray {
  KlObject objbase;
  KlValue* begin;
  KlValue* end;
  KlValue* current;
  klobject_tail;
} KlArray;

typedef KlValue* KlArrayIter;

KlClass* klarray_class(KlMM* klmm);
KlArray* klarray_create(KlClass* arrayclass, size_t capacity);

bool klarray_check_capacity(KlArray* array, size_t capacity);

static inline size_t klarray_size(KlArray* array);
static inline size_t klarray_capacity(KlArray* array);

static inline bool klarray_expand(KlArray* array);

static inline bool klarray_push_back(KlArray* array, KlValue* value);
static inline void klarray_pop_back(KlArray* array);
static inline void klarray_multipop(KlArray* array, size_t count);
static inline KlValue* klarray_top(KlArray* array);

static inline void klarray_make_empty(KlArray* array);
static inline KlValue* klarray_access(KlArray* array, size_t index);
static inline void klarray_index(KlArray* array, size_t index, KlValue* val);
static inline KlException klarray_indexas(KlArray* array, size_t index, KlValue* val);
static inline KlValue* klarray_access_from_top(KlArray* array, size_t index);
//static inline KlValue* klarray_set_from_top(KlArray* array, size_t index);
static inline KlValue* klarray_raw(KlArray* array);

static inline KlArrayIter klarray_iter_begin(KlArray* array);
static inline KlArrayIter klarray_iter_end(KlArray* array);
static inline KlArrayIter klarray_iter_next(KlArrayIter itr);


static inline size_t klarray_size(KlArray* array) {
  return array->current - array->begin;
}

static inline size_t klarray_capacity(KlArray* array) {
  return array->end - array->begin;
}


static inline bool klarray_expand(KlArray* array) {
  return klarray_check_capacity(array, klarray_size(array) * 2);
}

static inline bool klarray_push_back(KlArray* array, KlValue* value) {
  if (kl_unlikely(array->current == array->end && !klarray_expand(array))) {
    return false;
  }
  klvalue_setvalue(array->current++, value);
  return true;
}

static inline void klarray_multipop(KlArray* array, size_t count) {
  array->current -= count;
}

static inline void klarray_pop_back(KlArray* array) {
  klarray_multipop(array, 1);
}

static inline KlValue* klarray_top(KlArray* array) {
  return array->current - 1;
}


static inline KlValue* klarray_access(KlArray* array, size_t index) {
  return &array->begin[index];
}

static inline void klarray_index(KlArray* array, size_t index, KlValue* val) {
  if (index < klarray_size(array)) {
    klvalue_setvalue(val, klarray_access(array, index));
    return;
  }
  klvalue_setnil(val);
}

static inline KlException klarray_indexas(KlArray* array, size_t index, KlValue* val) {
  if (index < klarray_size(array)) {
    klvalue_setvalue(klarray_access(array, index), val);
    return KL_E_NONE;
  } else if (index == klarray_size(array)) {
    if (kl_unlikely(!klarray_push_back(array, val)))
      return KL_E_OOM;
    return KL_E_NONE;
  }
  return KL_E_RANGE;
}

static inline KlValue* klarray_access_from_top(KlArray* array, size_t index) {
  return &array->current[-index];
}

static inline void klarray_make_empty(KlArray* array) {
  array->current = array->begin;
}

static inline KlValue* klarray_raw(KlArray* array) {
  return array->begin;
}

static inline KlArrayIter klarray_iter_begin(KlArray* array) {
  return array->begin;
}

static inline KlArrayIter klarray_iter_end(KlArray* array) {
  return array->current;
}

static inline KlArrayIter klarray_iter_next(KlArrayIter itr) {
  return itr + 1;
}


#endif
