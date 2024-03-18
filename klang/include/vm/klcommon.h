#ifndef KEVCC_KLANG_INCLUDE_VM_KLSTRS_H
#define KEVCC_KLANG_INCLUDE_VM_KLSTRS_H

#include "klang/include/value/klclass.h"
#include "klang/include/value/klmap.h"
#include "klang/include/value/klstring.h"

typedef struct tagKlCommon {
  struct {
    KlString* neg;
    KlString* add;
    KlString* sub;
    KlString* mul;
    KlString* div;
    KlString* mod;
    KlString* call;
    KlString* concat;
    KlString* index;
    KlString* indexas;
    KlString* eq;
    KlString* neq;
    KlString* lt;
    KlString* gt;
    KlString* le;
    KlString* ge;
    KlString* hash;
    KlString* arrpush;
  } string;
  struct {
    KlClass* map;
    KlClass* array;
    KlClass* phony[KL_NTYPE];
  } klclass;
  size_t ref_count;
} KlCommon;

KlCommon* klcommon_create(KlStrPool* strpool, KlMapNodePool* mapnodepool);
static inline void klcommon_delete(KlCommon* common, KlMM* klmm);
static inline void klcommon_pin(KlCommon* common);
static inline void klcommon_unpin(KlCommon* common, KlMM* klmm);
KlGCObject* klcommon_propagate(KlCommon* common, KlGCObject* gclist);

static inline void klcommon_delete(KlCommon* common, KlMM* klmm) {
  klmm_free(klmm, common, sizeof (KlCommon));
}

static inline void klcommon_pin(KlCommon* common) {
  ++common->ref_count;
}

static inline void klcommon_unpin(KlCommon* common, KlMM* klmm) {
  if (--common->ref_count)
    return;
  klcommon_delete(common, klmm);
}


#endif
