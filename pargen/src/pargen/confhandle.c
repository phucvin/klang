#include "pargen/include/pargen/confhandle.h"
#include "kevlr/include/table.h"
#include "pargen/include/pargen/error.h"
#include "utils/include/string/kev_string.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define KEV_PARGEN_CONFHANDLE_PRINT_LOG   (1 << 0)
#define KEV_PARGEN_CONFHANDLE_SHIFTING    (1 << 1)
#define KEV_PARGEN_CONFHANDLE_REDUCING    (1 << 2)
#define KEV_PARGEN_CONFHANDLE_INTERACTIVE (1 << 3)
#define KEV_PARGEN_CONFHANDLE_PRIORITY    (1 << 4)


typedef struct tagKevHandlerConfig {
  uint32_t options;
  KevLRConflictHandler* priority;
  KevLRConflictHandler* interactive;
  FILE* logstream;
} KevHandlerConfig;

static bool kev_pargen_confhandler_interactive_callback(void* object, KevLRConflict* conflict, KevLRCollection* collec);
static bool kev_pargen_confhandler_callback(void* object, KevLRConflict* conflict, KevLRCollection* collec);

static void kev_pargen_confhandle_make_option(KevHandlerConfig* config, KevPParserState* parser_state);

KevLRConflictHandler* kev_pargen_confhandle_get_handler(KevPParserState* parser_state) {
  KevHandlerConfig* config = (KevHandlerConfig*)malloc(sizeof (KevHandlerConfig));
  if (!config) {
    kev_throw_error("confhandler:", "out of memory", NULL);
  }
  kev_pargen_confhandle_make_option(config, parser_state);
  config->priority = kev_lr_conflict_handler_create(parser_state->priorities, kev_lr_confhandler_priority_callback);
  config->interactive = kev_lr_conflict_handler_create(NULL, kev_pargen_confhandler_interactive_callback);
  KevLRConflictHandler* handler = kev_lr_conflict_handler_create(config, kev_pargen_confhandler_callback);
  if (!config->priority || !config->interactive || !handler) {
    kev_throw_error("confhandler:", "out of memory", NULL);
  }
  return handler;
}

void kev_pargen_confhandle_delete(KevLRConflictHandler* handler) {
  KevHandlerConfig* config = (KevHandlerConfig*)handler->object;
  kev_lr_conflict_handler_delete(config->priority);
  kev_lr_conflict_handler_delete(config->interactive);
  if ((config->options & KEV_PARGEN_CONFHANDLE_PRINT_LOG) && config->logstream != stderr)
    fclose(config->logstream);
  kev_lr_conflict_handler_delete(handler);
}

static bool kev_pargen_confhandler_interactive_callback(void* object, KevLRConflict* conflict, KevLRCollection* collec) {
  printf("conflict in itemset %d when handling symbol '%s'(id = %d).",
         (int)kev_lr_itemset_get_id(kev_lr_conflict_get_itemset(conflict)),
         kev_lr_symbol_get_name(kev_lr_conflict_get_symbol(conflict)),
         (int)kev_lr_symbol_get_id(kev_lr_conflict_get_symbol(conflict)));
  printf("conflict items:\n");
  KevItemSet* conflict_items = kev_lr_conflict_get_conflict_items(conflict);
  kev_lr_print_itemset(stdout, collec, conflict_items, false);
  printf("type number to choose a rule to reduce, 's' to choose shifting or 'q' to abort:");
  char buf[24];
  while (true) {
    fgets(buf, (sizeof buf / sizeof buf[0]), stdin);
    if (buf[0] == 's') {
      kev_lr_conflict_set_shifting(conflict);
      return true;
    } else if (buf[0] == 'q') {
      return false;
    } else {
      char* endptr = NULL;
      int num = (size_t)strtoll(buf, &endptr, 10);
      if (*endptr != '\0') {
        printf("\"%s\" is not a number, try again:", buf);
        continue;
      }
      KevItem* item = kev_lr_itemset_iter_begin(conflict_items);
      for (size_t i = 0; i < num - 1; ++i) {
        item = item ? kev_lr_itemset_iter_next(item) : item;
      }
      if (!item) {
        printf("%d is too big, try again: ", (int)num);
        continue;
      }
      kev_lr_conflict_set_reducing(conflict, collec, item);
      return true;
    }
  }
}

static bool kev_pargen_confhandler_callback(void* object, KevLRConflict* conflict, KevLRCollection* collec) {
  KevHandlerConfig* config = (KevHandlerConfig*)object;
  uint32_t options = config->options;
  bool succeed = false;
  const char* which_stage = NULL;
  if (options & KEV_PARGEN_CONFHANDLE_PRIORITY) {
    succeed = kev_lr_conflict_handle(config->priority, conflict, collec);
    which_stage = "priority";
  }
  if (!succeed && (options & KEV_PARGEN_CONFHANDLE_INTERACTIVE)) {
    succeed = kev_lr_conflict_handle(config->interactive, conflict, collec);
    which_stage = "interactive";
  }
  if (!succeed && (options & KEV_PARGEN_CONFHANDLE_SHIFTING)) {
    succeed = kev_lr_conflict_handle(kev_lr_confhandler_shifting, conflict, collec);
    which_stage = "shifting";
  }
  if (!succeed && (options & KEV_PARGEN_CONFHANDLE_REDUCING)) {
    succeed = kev_lr_conflict_handle(kev_lr_confhandler_reducing, conflict, collec);
    which_stage = "reducing";
  }
  if (options & KEV_PARGEN_CONFHANDLE_PRINT_LOG) {
    fprintf(config->logstream, "conflict occurred on itemset %d when handling symbol '%s'(id = %d)\n",
            (int)kev_lr_itemset_get_id(kev_lr_conflict_get_itemset(conflict)),
            kev_lr_symbol_get_name(kev_lr_conflict_get_symbol(conflict)),
            (int)kev_lr_symbol_get_id(kev_lr_conflict_get_symbol(conflict)));
    const char* status = NULL;
    if (!succeed) {
      status = "unresolved";
    } else if (kev_lr_conflict_get_status(conflict) == KEV_LR_ACTION_RED) {
      status = "reduce";
    } else if (kev_lr_conflict_get_status(conflict) == KEV_LR_ACTION_ACC) {
      status = "aacept";
    } else if (kev_lr_conflict_get_status(conflict) == KEV_LR_ACTION_SHI) {
      status = "shift";
    }
    fprintf(config->logstream, "status: %s\n", status);
    fprintf(config->logstream, "resolved stage: %s\n", succeed ? which_stage : "");
    fprintf(config->logstream, "conflict item(s):\n");
    kev_lr_print_itemset(config->logstream, collec, kev_lr_conflict_get_conflict_items(conflict), false);
    fputc('\n', config->logstream);
  }
  return succeed;
}

static void kev_pargen_confhandle_make_option(KevHandlerConfig* config, KevPParserState* parser_state) {
  config->options = 0;
  for (size_t i = 0; i < kev_addrarray_size(parser_state->confhandlers); ++i) {
    KevConfHandler* handler = (KevConfHandler*)kev_addrarray_visit(parser_state->confhandlers, i);
    if (kev_str_is_prefix(handler->handler_name, "log")) {
      config->options |= KEV_PARGEN_CONFHANDLE_PRINT_LOG;
      FILE* stream = handler->attribute ? fopen(handler->attribute, "w") : stderr;
      config->logstream = stream;
      if (!config->logstream) {
        kev_throw_error("confhandler:", "can not open log file: ", handler->attribute);
      }
    } else if (kev_str_is_prefix(handler->handler_name, "priority")) {
      config->options |= KEV_PARGEN_CONFHANDLE_PRIORITY;
    } else if (kev_str_is_prefix(handler->handler_name, "interactive")) {
      config->options |= KEV_PARGEN_CONFHANDLE_INTERACTIVE;
    } else if (kev_str_is_prefix(handler->handler_name, "shifting")) {
      config->options |= KEV_PARGEN_CONFHANDLE_SHIFTING;
    } else if (kev_str_is_prefix(handler->handler_name, "reducing")) {
      config->options |= KEV_PARGEN_CONFHANDLE_REDUCING;
    }
  }
}