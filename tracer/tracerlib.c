#include "tracerlib.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFLEN (32 * BUFSIZ)

#ifdef EXTBENCH
#define OUT_FD STDERR_FILENO
#else
#define OUT_FD STDOUT_FILENO
#endif

static char *buf;
static char *ptr;

void serialiseUint64(uint64_t val);
void serialiseEvent(event_t val);
void initBuf(void);
void flushBuf(void);

void serialiseUint64(uint64_t val) {
  memcpy(ptr, &val, sizeof(val));
  ptr += sizeof(val);
}

void serialiseEvent(event_t val) {
  ptr[0] = (char) val;
  ++ptr;
}

void initBuf(void) {
  buf = (char *) malloc(BUFLEN);

  if (!buf) {
    printf("Failed to allocate buffer\n");
    exit(1);
  }

  atexit(flushBuf); // Flush the buffer on program exit
  ptr = buf;
}

void flushBuf(void) {
  if (buf) {
    ptrdiff_t toWrite = ptr - buf;

    while (toWrite > 0) {
      ssize_t res = write(OUT_FD, buf, (size_t) toWrite);

      if (res < 0) {
        printf("Failed to write buffer\n");
        exit(1);
      }

      toWrite -= res;
    }

    ptr = buf;
  }
}

void loopEvent(event_t event) {
  if (!buf) {
    initBuf();
  }

  serialiseEvent(event);

  if (ptr - buf > BUFLEN - 17) {
    flushBuf();
  }
}

void memoryEvent(event_t event, void *addr, uint64_t pc) {
  if (!buf) {
    initBuf();
  }

  serialiseEvent(event);
  serialiseUint64(pc);
  serialiseUint64(((uint64_t) addr) | 0x3);

  if (ptr - buf > BUFLEN - 17) {
    flushBuf();
  }
}
