#include "klang/include/error/klerror.h"
#include "utils/include/kio/ki.h"
#include <stdbool.h>


static size_t klerror_helper_locateline(Ki* input, KlFileOffset offset);
static bool klerror_helper_showline_withcurl(KlError* klerror, Ki* input, KlFileOffset begin, KlFileOffset end);

void klerror_error(KlError* klerror, Ki* input, const char* inputname, KlFileOffset begin, KlFileOffset end, const char* format, ...) {
  va_list args;
  va_start(args, format);
  klerror_errorv(klerror, input, inputname, begin, end, format, args);
  va_end(args);
}

void klerror_errorv(KlError* klerror, Ki* input, const char* inputname, KlFileOffset begin, KlFileOffset end, const char* format, va_list args) {
  ++klerror->errcount;
  Ko* err = klerror->err;
  size_t orioffset = ki_tell(input);
  size_t line = klerror_helper_locateline(input, begin);
  size_t linebegin = ki_tell(input);

  unsigned int col = begin - linebegin + 1;
  ko_printf(err, "%s:%4u:%4u: ", inputname, line, col);
  ko_vprintf(err, format, args);
  ko_putc(err, '\n');

  while (klerror_helper_showline_withcurl(klerror, input, begin, end))
    continue;
  ki_seek(input, orioffset);
  ko_putc(err, '\n');
  ko_flush(err);
}

#define kl_isnl(ch)       ((ch) == '\n' || (ch) == '\r')

static size_t klerror_helper_locateline(Ki* input, KlFileOffset offset) {
  ki_seek(input, 0);
  size_t currline = 1;
  size_t lineoff = 0;
  while (ki_tell(input) < offset) {
    int ch = ki_getc(input);
    if (ch == KOF) break;
    if (kl_isnl(ch)) {
      if ((ch = ki_getc(input)) != '\r' && ch != KOF)
        ki_ungetc(input);
      ++currline;
      lineoff = ki_tell(input);
    }
  }
  ki_seek(input, lineoff);
  return currline;
}

static bool klerror_helper_showline_withcurl(KlError* klerror, Ki* input, KlFileOffset begin, KlFileOffset end) {
  Ko* err = klerror->err;
  size_t curroffset = ki_tell(input);
  if (curroffset >= end) return false;
  int ch = ki_getc(input);
  while (!kl_isnl(ch) && ch != KOF) {
    ko_putc(err, ch);
    ch = ki_getc(input);
  }
  ko_putc(err, '\n');
  ki_seek(input, curroffset);
  ch = ki_getc(input);
  while (!kl_isnl(ch)) {
    if (curroffset == begin && curroffset == end) {
      ko_putc(err, klerror->config.zerocurl);
    } else if (curroffset >= begin && curroffset < end) {
      if (ch == '\t') {
        for (size_t i = 0; i < klerror->config.tabstop; ++i)
          ko_putc(err, klerror->config.curl);
      } else {
        ko_putc(err, klerror->config.curl);
      }
    } else {
      ko_putc(err, ch == '\t' ? '\t' : ' ');
    }
    if (ch == KOF) break;
    ch = ki_getc(input);
    ++curroffset;
  }
  ko_putc(err, '\n');
  if (ch != KOF && (ch = ki_getc(input)) != '\r' && ch != KOF)
    ki_ungetc(input);
  return true;
}