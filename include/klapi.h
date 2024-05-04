#ifndef KEVCC_KLANG_INCLUDE_VM_KLAPI_H
#define KEVCC_KLANG_INCLUDE_VM_KLAPI_H

#include "include/value/klcfunc.h"
#include "include/value/klvalue.h"
#include "include/vm/klexception.h"
#include "include/vm/klexec.h"
#include "include/value/klstate.h"
#include "include/value/klarray.h"
#include "include/misc/klutils.h"


#define KLAPI_VARIABLE_RESULTS    (KLEXEC_VARIABLE_RESULTS)


#define klapi_pushobj(state, obj, type)           klapi_pushgcobj((state), (KlGCObject*)(obj), (type))
#define klapi_setobj(state, index, obj, type)     klapi_setgcobj((state), (index), (KlGCObject*)(obj), (type))

#define klapi_scall(state, callable, narg, nret)  klapi_call((state), (callable), (narg), (nret), klstate_stktop((state)) - (narg));

KlState* klapi_new_state(KlMM* klmm);

static inline KlException klapi_call(KlState* state, KlValue* callable, size_t narg, size_t nret, KlValue* respos);
static inline bool klapi_checkrange(KlState* state, int index);

static inline KlValue* klapi_access(KlState* state, int index);
static inline KlType klapi_gettype(KlState* state, int index);
static inline void klapi_pop(KlState* state, size_t count);

/* push method */
static inline KlException klapi_pushcfunc(KlState* state, KlCFunction* cfunc);
static inline KlException klapi_pushint(KlState* state, KlInt val);
static inline KlException klapi_pushfloat(KlState* state, KlFloat val);
static inline KlException klapi_pushnil(KlState* state, size_t count);
static inline KlException klapi_pushbool(KlState* state, KlBool val);
static inline KlException klapi_pushstring(KlState* state, const char* str);
static inline KlException klapi_pushmap(KlState* state, size_t capacity);
static inline KlException klapi_pusharray(KlState* state, size_t capacity);
static inline KlException klapi_pushgcobj(KlState* state, KlGCObject* gcobj, KlType type);
static inline KlException klapi_pushvalue(KlState* state, KlValue* val);

/* set method */
static inline KlException klapi_setcfunc(KlState* state, int index, KlCFunction* cfunc);
static inline KlException klapi_setint(KlState* state, int index, KlInt val);
static inline KlException klapi_setnil(KlState* state, int index);
static inline KlException klapi_setbool(KlState* state, int index, KlBool val);
static inline KlException klapi_setstring(KlState* state, int index, const char* str);
static inline KlException klapi_setmap(KlState* state, int index, size_t capacity);
static inline KlException klapi_setarray(KlState* state, int index, size_t capacity);
static inline KlException klapi_setgcobj(KlState* state, int index, KlGCObject* gcobj, KlType type);
static inline KlException klapi_setvalue(KlState* state, int index, KlValue* val);

/* get method */
static inline KlInt klapi_getint(KlState* state, int index);
static inline KlFloat klapi_getfloat(KlState* state, int index);
static inline KlBool klapi_getbool(KlState* state, int index);
static inline KlString* klapi_getstring(KlState* state, int index);
static inline KlMap* klapi_getmap(KlState* state, int index);
static inline KlArray* klapi_getarray(KlState* state, int index);

/* to method */
KlException klapi_toint(KlState* state, int to, int from);
KlException klapi_tofloat(KlState* state, int to, int from);
static inline KlBool klapi_tobool(KlState* state, int index);
KlString* klapi_tostring(KlState* state, int index);

/* access to global variable */
KlException klapi_loadglobal(KlState* state);
KlException klapi_storeglobal(KlState* state, KlString* varname);

/* library loader */

/* load a library by given 'libpath',
 * call the 'entryfunction'.
 * If 'entryfunction' is NULL, a default entry name "kllib_entrypoint" would be used.
 */
KlException klapi_loadlib(KlState* state, const char* libpath, const char* entryfunction);



static inline bool klapi_checktype(KlState* state, int index, KlType type);

static inline KlException klapi_call(KlState* state, KlValue* callable, size_t narg, size_t nret, KlValue* respos) {
  return klexec_call(state, callable, narg, nret, respos);
}


static inline bool klapi_checkrange(KlState* state, int index) {
  return klstack_check_index(klstate_stack(state), index);
}

static inline KlValue* klapi_access(KlState* state, int index) {
  kl_assert(klapi_checkrange(state, index), "index out of range");
  return klstate_getval(state, index);
}

static inline KlType klapi_gettype(KlState* state, int index) {
  return klvalue_gettype(klapi_access(state, index));
}

static inline void klapi_pop(KlState* state, size_t count) {
  kl_assert(count <= klstack_size(klstate_stack(state)), "pop too many values");
  KlValue* bound = klstate_stktop(state) - count;
  klreflist_close(&state->reflist, bound, klstate_getmm(state));
  klstack_set_top(klstate_stack(state), bound);
}

static inline KlException klapi_pushcfunc(KlState* state, KlCFunction* cfunc) {
  if (!klstack_expand_if_full(&state->stack, klstate_getmm(state)))
    return klstate_throw(state, KL_E_OOM, "out of momery");
  klstack_pushcfunc(klstate_stack(state), cfunc);
  return KL_E_NONE;
}

static inline KlException klapi_pushint(KlState* state, KlInt val) {
  if (!klstack_expand_if_full(klstate_stack(state), klstate_getmm(state)))
    return klstate_throw(state, KL_E_OOM, "out of momery");
  klstack_pushint(klstate_stack(state), val);
  return KL_E_NONE;
}

static inline KlException klapi_pushfloat(KlState* state, KlFloat val) {
  if (!klstack_expand_if_full(klstate_stack(state), klstate_getmm(state)))
    return klstate_throw(state, KL_E_OOM, "out of momery");
  klstack_pushfloat(klstate_stack(state), val);
  return KL_E_NONE;
}

static inline KlException klapi_pushnil(KlState* state, size_t count) {
  if (kl_unlikely(klstate_checkframe(state, count)))
    return klstate_throw(state, KL_E_OOM, "out of momery");
  klstack_pushnil(klstate_stack(state), count);
  return KL_E_NONE;
}

static inline KlException klapi_pushbool(KlState* state, KlBool val) {
  if (!klstack_expand_if_full(klstate_stack(state), klstate_getmm(state)))
    return klstate_throw(state, KL_E_OOM, "out of momery");
  klstack_pushbool(klstate_stack(state), val);
  return KL_E_NONE;
}

static inline KlException klapi_pushstring(KlState* state, const char* str) {
  if (!klstack_expand_if_full(klstate_stack(state), klstate_getmm(state)))
    return klstate_throw(state, KL_E_OOM, "out of momery");
  KlString* klstr = klstrpool_new_string(state->strpool, str);
  if (!klstr) return klstate_throw(state, KL_E_OOM, "out of momery");
  klstack_pushgcobj(klstate_stack(state), (KlGCObject*)klstr, KL_STRING);
  return KL_E_NONE;
}

static inline KlException klapi_pushmap(KlState* state, size_t capacity) {
  if (!klstack_expand_if_full(klstate_stack(state), klstate_getmm(state)))
    return klstate_throw(state, KL_E_OOM, "out of momery");
  KlMap* map = klmap_create(state->common->klclass.map, capacity, state->mapnodepool);
  if (!map) return klstate_throw(state, KL_E_OOM, "out of momery");
  klstack_pushgcobj(klstate_stack(state), (KlGCObject*)map, KL_MAP);
  return KL_E_NONE;
}

static inline KlException klapi_pusharray(KlState* state, size_t capacity) {
  if (!klstack_expand_if_full(klstate_stack(state), klstate_getmm(state)))
    return klstate_throw(state, KL_E_OOM, "out of momery");
  KlArray* array = klarray_create(state->common->klclass.array, klstate_getmm(state), capacity);
  if (!array) return klstate_throw(state, KL_E_OOM, "out of momery");
  klstack_pushgcobj(klstate_stack(state), (KlGCObject*)array, KL_ARRAY);
  return KL_E_NONE;
}

static inline KlException klapi_pushgcobj(KlState* state, KlGCObject* gcobj, KlType type) {
  if (!klstack_expand_if_full(klstate_stack(state), klstate_getmm(state)))
    return klstate_throw(state, KL_E_OOM, "out of momery");
  klstack_pushgcobj(&state->stack, gcobj, type);
  return KL_E_NONE;
}

static inline KlException klapi_pushvalue(KlState* state, KlValue* val) {
  if (!klstack_expand_if_full(klstate_stack(state), klstate_getmm(state)))
    return klstate_throw(state, KL_E_OOM, "out of momery");
  klstack_pushvalue(&state->stack, val);
  return KL_E_NONE;
}

static inline KlException klapi_setcfunc(KlState* state, int index, KlCFunction* cfunc) {
  KlValue* val = klapi_access(state, index);
  klvalue_setcfunc(val, cfunc);
  return KL_E_NONE;
}

static inline KlException klapi_setint(KlState* state, int index, KlInt intval) {
  KlValue* val = klapi_access(state, index);
  klvalue_setint(val, intval);
  return KL_E_NONE;
}

static inline KlException klapi_setnil(KlState* state, int index) {
  KlValue* val = klapi_access(state, index);
  klvalue_setnil(val);
  return KL_E_NONE;
}

static inline KlException klapi_setbool(KlState* state, int index, KlBool boolval) {
  KlValue* val = klapi_access(state, index);
  klvalue_setbool(val, boolval);
  return KL_E_NONE;
}

static inline KlException klapi_setstring(KlState* state, int index, const char* str) {
  KlValue* val = klapi_access(state, index);
  KlString* klstr = klstrpool_new_string(state->strpool, str);
  if (!klstr) return klstate_throw(state, KL_E_OOM, "out of momery");
  klvalue_setobj(val, klstr, KL_STRING);
  return KL_E_NONE;
}

static inline KlException klapi_setmap(KlState* state, int index, size_t capacity) {
  KlValue* val = klapi_access(state, index);
  KlMap* map = klmap_create(state->common->klclass.map, capacity, state->mapnodepool);
  if (!map) return klstate_throw(state, KL_E_OOM, "out of momery");
  klvalue_setobj(val, map, KL_MAP);
  return KL_E_NONE;
}

static inline KlException klapi_setarray(KlState* state, int index, size_t capacity) {
  KlValue* val = klapi_access(state, index);
  KlArray* array = klarray_create(state->common->klclass.array, klstate_getmm(state), capacity);
  if (!array) return klstate_throw(state, KL_E_OOM, "out of momery");
  klvalue_setobj(val, array, KL_ARRAY);
  return KL_E_NONE;
}

static inline KlException klapi_setgcobj(KlState* state, int index, KlGCObject* gcobj, KlType type) {
  KlValue* val = klapi_access(state, index);
  klvalue_setgcobj(val, gcobj, type);
  return KL_E_NONE;
}

static inline KlException klapi_setvalue(KlState* state, int index, KlValue* other) {
  KlValue* val = klapi_access(state, index);
  klvalue_setvalue(val, other);
  return KL_E_NONE;
}

static inline KlInt klapi_getint(KlState* state, int index) {
  kl_assert(klvalue_checktype(klapi_access(state, index), KL_INT), "expected type KL_INT");
  return klvalue_getint(klapi_access(state, index));
}

static inline KlFloat klapi_getfloat(KlState* state, int index) {
  kl_assert(klvalue_checktype(klapi_access(state, index), KL_FLOAT), "expected type KL_FLOAT");
  return klvalue_getfloat(klapi_access(state, index));
}

static inline KlBool klapi_getbool(KlState* state, int index) {
  kl_assert(klvalue_checktype(klapi_access(state, index), KL_BOOL), "expected type KL_BOOL");
  return klvalue_getbool(klapi_access(state, index));
}

static inline KlString* klapi_getstring(KlState* state, int index) {
  kl_assert(klvalue_checktype(klapi_access(state, index), KL_STRING), "expected type KL_STRING");
  return klvalue_getobj(klapi_access(state, index), KlString*);
}

static inline KlMap* klapi_getmap(KlState* state, int index) {
  kl_assert(klvalue_checktype(klapi_access(state, index), KL_MAP), "expected type KL_MAP");
  return klvalue_getobj(klapi_access(state, index), KlMap*);
}

static inline KlArray* klapi_getarray(KlState* state, int index) {
  kl_assert(klvalue_checktype(klapi_access(state, index), KL_ARRAY), "expected type KL_ARRAY");
  return klvalue_getobj(klapi_access(state, index), KlArray*);
}

static inline KlBool klapi_tobool(KlState* state, int index) {
  KlValue* val = klapi_access(state, index);
  return klexec_satisfy(val, KL_TRUE);
}


static inline bool klapi_checktype(KlState* state, int index, KlType type) {
  return klapi_gettype(state, index) == type;
}

#endif
