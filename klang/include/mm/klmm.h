/* klang memory manager and garbage collector */

#ifndef KEVCC_INCLUDE_MM_KLMM_H
#define KEVCC_INCLUDE_MM_KLMM_H

#include "klang/include/misc/klutils.h"

#include <stdint.h>
#include <stdlib.h>

/* Mark a object accessible and link it to gclist. */
#define klmm_gcobj_mark_accessible(obj, gclist) {               \
  if ((obj)->created.gc_state == KL_GC_INACCESSIBLE) {          \
    KlGCObject* gcobj = (obj);                                  \
    gcobj->created.gc_state = KL_GC_ACCESSIBLE;                 \
    gcobj->created.next_reachable = (gclist);                   \
    (gclist) = gcobj;                                           \
  }                                                             \
}

#define klmm_to_gcobj(obj)  (klcast(KlGCObject*, (obj)))

typedef enum tagKlGCStat { KL_GC_INACCESSIBLE, KL_GC_ACCESSIBLE } KlGCStat;


struct tagKlGCObject;
struct tagKlMM;

typedef void (*KlGCDestructor)(struct tagKlGCObject* gcobj);
typedef struct tagKlGCObject* (*KlGCProp)(struct tagKlGCObject* gcobj, struct tagKlGCObject* gclist);

typedef struct tagKlGCVirtualFunc {
  KlGCDestructor destructor;
  KlGCProp propagate;
} KlGCVirtualFunc;

/* this serves as the base class of collectable object */
typedef struct tagKlGCObject {
  struct tagKlGCObject* next;               /* link all object in the same level */
  union {
    struct {
      struct tagKlGCObject* next_reachable; /* link all accessible object in the same level */
      struct tagKlMM* klmm;
      KlGCVirtualFunc* virtualfunc;
      KlGCStat gc_state;
    } created;
    struct {
      struct tagKlGCObject* next_level;
      struct tagKlGCObject* tail;           /* point to the list tail of current level */
    } creating;
  };                                        /* anonymous union */
} KlGCObject;


typedef struct tagKlMM {
  KlGCObject allgc;                         /* top level */
  KlGCObject* currlevel;                    /* current level */
  KlGCObject* root;
  size_t mem_used;
  /* if mem_used exceeds this limit, the garbage collection will start.
   * the value of limit will dynamically change. */
  size_t limit; 
} KlMM;

static inline void klmm_init(KlMM* klmm, size_t limit);
void klmm_destroy(KlMM* klmm);

static inline void klmm_register_root(KlMM* klmm, KlGCObject* root);
static inline KlGCObject* klmm_get_root(KlMM* klmm);

static inline void* klmm_alloc(KlMM* klmm, size_t size);
static inline void* klmm_realloc(KlMM* klmm, void* blk, size_t new_size, size_t old_size);
static inline void klmm_free(KlMM* klmm, void* blk, size_t size);

static inline void klmm_gcobj_enable(KlMM* klmm, KlGCObject* gcobj, KlGCVirtualFunc* vfunc);
static inline void klmm_gcobj_resetvfunc(KlGCObject* gcobj, KlGCVirtualFunc* vfunc);
static inline void klmm_newlevel(KlMM* klmm, KlGCObject* gcobj);
static inline void klmm_newlevel_abort(KlMM* klmm);
static inline void klmm_newlevel_done(KlMM* klmm, KlGCVirtualFunc* vfunc);

void klmm_gc_clean_all(KlGCObject* list);
void klmm_do_gc(KlMM* klmm);
static inline void klmm_try_gc(KlMM* klmm);

static inline KlMM* klmm_gcobj_getmm(KlGCObject* gcobj);


static inline void klmm_init(KlMM* klmm, size_t limit) {
  klmm->allgc.next = NULL;
  klmm->allgc.creating.tail = &klmm->allgc;
  klmm->allgc.creating.next_level = NULL;
  klmm->currlevel = &klmm->allgc;
  klmm->mem_used = 0;
  klmm->limit = limit;
  klmm->root = NULL;
}

static inline void klmm_try_gc(KlMM* klmm) {
  //if (klmm->mem_used >= klmm->limit)
    klmm_do_gc(klmm);
}

static inline void klmm_register_root(KlMM* klmm, KlGCObject* root) {
  klmm->root = root;
}

static inline KlGCObject* klmm_get_root(KlMM* klmm) {
  return klmm->root;
}

static inline void* klmm_alloc(KlMM* klmm, size_t size) {
  klmm_try_gc(klmm);

  void* blk = malloc(size);
  if (blk) {
    klmm->mem_used += size;
    return blk;
  }
  /* second try */
  klmm_do_gc(klmm);
  blk = malloc(size);
  if (blk)
    klmm->mem_used += size;
  return blk;
}

static inline void* klmm_realloc(KlMM* klmm, void* blk, size_t new_size, size_t old_size) {
  klmm_try_gc(klmm);

  void* new_blk = realloc(blk, new_size);
  if (new_blk) {
    klmm->mem_used += new_size - old_size;
    return new_blk;
  }
  /* second try */
  klmm_do_gc(klmm);
  new_blk = realloc(blk, new_size);
  if (new_blk)
    klmm->mem_used += new_size - old_size;
  return new_blk;
}

static inline void klmm_free(KlMM* klmm, void* blk, size_t size) {
  free(blk);
  klmm->mem_used -= size;
}

static inline void klmm_gcobj_enable(KlMM* klmm, KlGCObject* gcobj, KlGCVirtualFunc* vfunc) {
  gcobj->created.virtualfunc = vfunc;
  gcobj->created.gc_state = KL_GC_INACCESSIBLE;
  gcobj->created.klmm = klmm;
  KlGCObject* delegate = klmm->currlevel;
  delegate->creating.tail->next = gcobj;
  delegate->creating.tail = gcobj;
}

static inline void klmm_gcobj_resetvfunc(KlGCObject* gcobj, KlGCVirtualFunc* vfunc) {
  gcobj->created.virtualfunc = vfunc;
}

static inline void klmm_newlevel(KlMM* klmm, KlGCObject* gcobj) {
  gcobj->creating.next_level = klmm->currlevel;
  gcobj->creating.tail = gcobj;
  klmm->currlevel = gcobj;
}

static inline void klmm_newlevel_abort(KlMM* klmm) {
  klmm_gc_clean_all(klmm->currlevel->next);
  klmm->currlevel = klmm->currlevel->creating.next_level;
}

static inline void klmm_newlevel_done(KlMM* klmm, KlGCVirtualFunc* vfunc) {
  KlGCObject* gcobj = klmm->currlevel;
  KlGCObject* tail = gcobj->creating.tail;
  KlGCObject* next_level = gcobj->creating.next_level;
  klmm->currlevel = next_level;

  gcobj->created.virtualfunc = vfunc;
  gcobj->created.gc_state = KL_GC_INACCESSIBLE;
  gcobj->created.klmm = klmm;
  next_level->creating.tail->next = gcobj;
  next_level->creating.tail = tail;
}

static inline KlMM* klmm_gcobj_getmm(KlGCObject* gcobj) {
  return gcobj->created.klmm;
}

#endif