#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>



#define ACTION_SHIFT      (1)
#define ACTION_REDUCE     (2)
#define ACTION_ACCEPT     (3)
#define ACTION_ERROR      (0)

typedef union { int intval; float floatval; char* str; } Attr;

typedef struct tagSymbol {
  Attr attr;
  int16_t state;
} Symbol;

typedef struct tagActionEntry {
  uint8_t action;
  uint16_t info;
} ActionEntry;

typedef struct tagRuleInfo {
  uint8_t head_id;
  uint8_t bodylen;
} RuleInfo;

typedef void LRCallback(Symbol* stk);

static ActionEntry action_tbl[57][7];
static int16_t goto_tbl[57][13];
static RuleInfo rules_info[10];
static LRCallback* callbacks[10];
static const char* symbol_name[13];
static int16_t start_state;
static int state_symbol_mapping[57];


Attr parse(FILE* input);
static Symbol* symstk_expand(Symbol* symstk, Symbol* symstk_end);

Attr parse(FILE* input) {
  

  uint8_t _token = ;
  int16_t _state = start_state;

  /* symbol stack definition */
  Symbol* _symstk = (Symbol*)malloc(16 * sizeof (Symbol*));
  if (!_symstk) {
    fprintf(stderr, "out of memory", NULL);
    exit(EXIT_FAILURE);
  }
  /* ensure the cell above the top of stack is always available, so the capacity of
   * the stack is set to the actual capacity minus 1.
   */
  Symbol* _symstk_end = _symstk + 15;
  Symbol* _symstk_curr = _symstk;

  _symstk_curr++->state = _state;

  while (true) {
    ActionEntry action = action_tbl[_state][_token];
    switch (action.action) {
      case ACTION_SHIFT: {
        if (_symstk_curr == _symstk_end) {
          size_t old_capacity = _symstk_end - _symstk + 1;
          _symstk = symstk_expand(_symstk, _symstk_end);
          _symstk_end = _symstk + old_capacity * 2 - 1;
          _symstk_curr = _symstk + old_capacity - 1;
        }
        _state = action.info;
        Attr attr;
        
        _symstk_curr->attr = attr;
        _symstk_curr++->state = _state;
        
        _token = ;
        break;
      }
      case ACTION_REDUCE: {
        RuleInfo ruleinfo = rules_info[action.info];
        size_t rulelen = ruleinfo.bodylen;
        _symstk_curr -= rulelen;
        if (callbacks[action.info])
          callbacks[action.info](_symstk_curr);
        if (_symstk_curr == _symstk_end) {
          size_t old_capacity = _symstk_end - _symstk + 1;
          _symstk = symstk_expand(_symstk, _symstk_end);
          _symstk_end = _symstk + old_capacity * 2 - 1;
          _symstk_curr = _symstk + old_capacity - 1;
        }
        _state = goto_tbl[(_symstk_curr - 1)->state][ruleinfo.head_id];
        _symstk_curr++->state = _state;
        break;
      }
      case ACTION_ACCEPT: {
        Attr ret = (_symstk_curr - 1)->attr;
        free(_symstk);
        
        return ret;
      }
      case ACTION_ERROR: {
        fprintf(stderr, "error occurred!\n");
        exit(EXIT_FAILURE);
        free(_symstk);
        break;
      }
      default: {
        fprintf(stderr, "impossible state\n");
        exit(EXIT_FAILURE);
        break;
      }
    }
  }
}


static ActionEntry action_tbl[57][7] = {
  {
    { 0,   -1 }, { 1,    1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 1,    5 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    0 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 2,    0 }, 
  },
  {
    { 0,   -1 }, { 2,    2 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 2,    2 }, 
  },
  {
    { 0,   -1 }, { 1,    1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 3,   -1 }, 
  },
  {
    { 0,   -1 }, { 0,   -1 }, { 2,    9 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 0,   -1 }, { 1,    8 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 2,    1 }, 
  },
  {
    { 0,   -1 }, { 1,   10 }, { 0,   -1 }, { 1,   13 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    8 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 1,   14 }, { 2,    8 }, 
  },
  {
    { 1,    5 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    0 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 2,    0 }, { 2,    0 }, 
  },
  {
    { 0,   -1 }, { 2,    4 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 2,    4 }, { 2,    4 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,    1 }, { 0,   -1 }, { 1,   22 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    3 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 2,    3 }, 
  },
  {
    { 0,   -1 }, { 0,   -1 }, { 1,    8 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 1,    5 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    0 }, { 0,   -1 }, { 0,   -1 }, { 2,    0 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    2 }, { 0,   -1 }, { 0,   -1 }, { 2,    2 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 0,   -1 }, { 1,   25 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    6 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 2,    6 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    8 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 1,   28 }, { 2,    8 }, 
  },
  {
    { 0,   -1 }, { 0,   -1 }, { 1,   30 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    5 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 2,    5 }, { 2,    5 }, 
  },
  {
    { 0,   -1 }, { 2,    1 }, { 0,   -1 }, { 0,   -1 }, { 2,    1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 0,   -1 }, { 1,   32 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,   10 }, { 0,   -1 }, { 1,   33 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    3 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 2,    3 }, { 2,    3 }, 
  },
  {
    { 0,   -1 }, { 1,   35 }, { 0,   -1 }, { 1,   38 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    8 }, { 0,   -1 }, { 0,   -1 }, { 2,    8 }, { 1,   39 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    7 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 2,    7 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    6 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 2,    6 }, { 2,    6 }, 
  },
  {
    { 1,    5 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    0 }, { 0,   -1 }, { 0,   -1 }, { 2,    0 }, { 2,    0 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    4 }, { 0,   -1 }, { 0,   -1 }, { 2,    4 }, { 2,    4 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 1,   44 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    3 }, { 0,   -1 }, { 0,   -1 }, { 2,    3 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 0,   -1 }, { 1,   46 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 0,   -1 }, { 1,   30 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 0,   -1 }, { 1,   48 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    6 }, { 0,   -1 }, { 0,   -1 }, { 2,    6 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    7 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 2,    7 }, { 2,    7 }, 
  },
  {
    { 0,   -1 }, { 2,    8 }, { 0,   -1 }, { 0,   -1 }, { 2,    8 }, { 1,   50 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    5 }, { 0,   -1 }, { 0,   -1 }, { 2,    5 }, { 2,    5 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 0,   -1 }, { 1,   52 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,   35 }, { 0,   -1 }, { 1,   54 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    3 }, { 0,   -1 }, { 0,   -1 }, { 2,    3 }, { 2,    3 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    7 }, { 0,   -1 }, { 0,   -1 }, { 2,    7 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    6 }, { 0,   -1 }, { 0,   -1 }, { 2,    6 }, { 2,    6 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 1,   17 }, { 0,   -1 }, { 0,   -1 }, { 1,   56 }, { 0,   -1 }, { 0,   -1 }, 
  },
  {
    { 0,   -1 }, { 2,    7 }, { 0,   -1 }, { 0,   -1 }, { 2,    7 }, { 2,    7 }, { 0,   -1 }, 
  },
};



static int16_t goto_tbl[57][13] = {
  {
      -1,   1,  -1,  -1,  -1,  -1,  -1,  -1,   4,   3,   2,  -1,  -1,
  }, {
       5,  -1,  -1,  -1,  -1,  -1,  -1,   6,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   7,   2,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   9,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  10,  -1,  13,  -1,  -1,  -1,  -1,  -1,  12,  11,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  14,  -1,  -1,  -1,  -1,  -1,  -1,  15,
  }, {
       5,  -1,  -1,  -1,  -1,  -1,  -1,  16,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  17,  -1,  -1,  -1,  -1,  -1,  -1,  20,  19,  18,  -1,  -1,
  }, {
      -1,   1,  -1,  22,  -1,  -1,  -1,  -1,  -1,  21,   2,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,   8,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  23,  -1,
  }, {
       5,  -1,  -1,  -1,  -1,  -1,  -1,  24,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  17,  -1,  -1,  25,  -1,  -1,  -1,  -1,  26,  18,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  17,  -1,  -1,  -1,  -1,  -1,  -1,  27,  19,  18,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  28,  -1,  -1,  -1,  -1,  -1,  -1,  29,
  }, {
      -1,  -1,  30,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  31,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  17,  -1,  -1,  32,  -1,  -1,  -1,  -1,  26,  18,  -1,  -1,
  }, {
      -1,  10,  -1,  33,  -1,  -1,  -1,  -1,  -1,  34,  11,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  35,  -1,  38,  -1,  -1,  -1,  -1,  -1,  37,  36,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  39,  -1,  -1,  -1,  -1,  -1,  -1,  40,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  17,  -1,  -1,  -1,  -1,  -1,  -1,  41,  19,  18,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
       5,  -1,  -1,  -1,  -1,  -1,  -1,  42,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  17,  -1,  -1,  -1,  -1,  -1,  -1,  43,  19,  18,  -1,  -1,
  }, {
      -1,  17,  -1,  44,  -1,  -1,  -1,  -1,  -1,  45,  18,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  17,  -1,  -1,  46,  -1,  -1,  -1,  -1,  26,  18,  -1,  -1,
  }, {
      -1,  -1,  30,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  47,  -1,
  }, {
      -1,  17,  -1,  -1,  48,  -1,  -1,  -1,  -1,  26,  18,  -1,  -1,
  }, {
      -1,  17,  -1,  -1,  -1,  -1,  -1,  -1,  49,  19,  18,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  50,  -1,  -1,  -1,  -1,  -1,  -1,  51,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  17,  -1,  -1,  52,  -1,  -1,  -1,  -1,  26,  18,  -1,  -1,
  }, {
      -1,  35,  -1,  54,  -1,  -1,  -1,  -1,  -1,  53,  36,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  }, {
      -1,  17,  -1,  -1,  -1,  -1,  -1,  -1,  55,  19,  18,  -1,  -1,
  }, {
      -1,  17,  -1,  -1,  56,  -1,  -1,  -1,  -1,  26,  18,  -1,  -1,
  }, {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  },
};



static RuleInfo rules_info[10] = {
  {    9,    1 }, {    8,    2 }, {    8,    1 }, {   10,    4 }, {   11,    2 }, {   11,    4 }, {   12,    2 }, {   12,    4 }, 
  {   12,    0 }, {    7,    1 }, 
};



static LRCallback* callbacks[10] = {
   ~:�,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};



static const char* symbol_name[13] = {
  "num",
  "if",
  "then",
  "{",
  "}",
  "else",
  "$",
  "expr",
  "stmts",
  "stmt",
  "stmt_if",
  "then_blk",
  "else_blk",
};



static int16_t start_state = 0;



static int state_symbol_mapping[57] = {
    -1,   1,  10,   9,   8,   0,   7,   9,   2,  11,   1,  10,   9,   3,   5,  12,
     7,   1,  10,   9,   8,   9,   3,  11,   7,   4,   9,   8,   5,  12,   2,  11,
     4,   3,   9,   1,  10,   9,   3,   5,  12,   8,   7,   8,   3,   9,   4,  11,
     4,   8,   5,  12,   4,   9,   3,   8,   4,
};




static Symbol* symstk_expand(Symbol* symstk, Symbol* symstk_end) {
  size_t old_size = symstk_end - symstk + 1;
  size_t new_size = old_size * 2;
  Symbol* newstk = (Symbol*)realloc(symstk, new_size * sizeof (Symbol*));
  if (!newstk) {
    fprintf(stderr, "out of memory", NULL);
    exit(EXIT_FAILURE);
  }
  symstk = newstk;
  return symstk;
}

