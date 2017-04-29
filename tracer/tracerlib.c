#include "tracerlib.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BUFLEN 4096

static char *buf;
static char *ptr;

void serialiseUint64(uint64_t val);
void serialiseEvent(event_t val);
void initBuf(void);
void flushBuf(void);

void serialiseUint64(uint64_t val) {
  ptr[0] = (char) (val >> 56);
  ptr[1] = (char) (val >> 48);
  ptr[2] = (char) (val >> 40);
  ptr[3] = (char) (val >> 32);
  ptr[4] = (char) (val >> 24);
  ptr[5] = (char) (val >> 16);
  ptr[6] = (char) (val >> 8);
  ptr[7] = (char) val;
  ptr += 8;
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

  ptr = buf;
}

void flushBuf(void) {
  size_t toWrite = (size_t) (ptr - buf);

  while (toWrite > 0) {
    ssize_t res = write(2, buf, toWrite);

    if (res < 0) {
      printf("Failed to write buffer\n");
      exit(1);
    }

    toWrite -= (size_t) res;
  }

  ptr = buf;
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
  serialiseUint64((uint64_t) addr);

  if (ptr - buf > BUFLEN - 17) {
    flushBuf();
  }
}
