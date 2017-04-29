#include "tracerlib.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BUFLEN 4096
static char *buf;
static char *ptr;

void serialise_uint64(uint64_t val);
void serialise_event(event_t val);
void init_buf(void);
void flush_buf(void);

void serialise_uint64(uint64_t val) {
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

void serialise_event(event_t val) {
  ptr[0] = (char) val;
  ++ptr;
}

void init_buf(void) {
  buf = (char *) malloc(BUFLEN);
  ptr = buf;
  if (!buf) {
    printf("Failed to allocate buffer!");
    exit(1);
  }
}

void flush_buf(void) {
  write(2, buf, ptr - buf);
  ptr = buf;
}

void loopEvent(event_t event) {
  if (!buf) init_buf();
  serialise_event(event);
  if (ptr - buf > BUFLEN - 17) flush_buf();
}

void memoryEvent(event_t event, void *addr, uint64_t pc) {
  if (!buf) init_buf();
  serialise_event(event);
  serialise_uint64(pc);
  serialise_uint64((uint64_t)addr);
  if (ptr - buf > BUFLEN - 17) flush_buf();
}
