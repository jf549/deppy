#include "tracerlib.h"

#include <stdio.h>

void loopEvent(event_t type) {
  switch (type) {
    case LOOP_ENTRY:
    case LOOP_ITER:
    case LOOP_EXIT:
      fwrite(&type, sizeof(type), 1, stdout);
      break;

    case LOAD:
    case STORE:
      perror("Invalid LoopEvent\n");
      break;
  }
}

void memoryEvent(event_t type, void *addr, uint64_t pc) {
  mem_event_t me = {pc, (uint64_t) addr};

  switch (type) {
    case LOAD:
    case STORE:
      fwrite(&type, sizeof(type), 1, stdout);
      fwrite(&me, sizeof(me), 1, stdout);
      break;

    case LOOP_ENTRY:
    case LOOP_ITER:
    case LOOP_EXIT:
      perror("Invalid MemoryEvent\n");
      break;
  }
}
