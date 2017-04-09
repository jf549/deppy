#include "tracerlib.h"
#include "tracerdefs.h"

#include <stdio.h>

void loopEvent(uint8_t type) {
  switch (type) {
    case LOOP_ENTRY:
    case LOOP_ITER:
    case LOOP_EXIT:
      fwrite(&type, sizeof(type), 1, stdout);
      break;

    default:
      perror("Invalid LoopEvent\n");
      break;
  }
}

void memoryEvent(uint8_t type, void *addr, uint64_t pc) {
  switch (type) {
    case LOAD:
    case STORE:
      fwrite(&type, sizeof(type), 1, stdout);
      fwrite(&pc, sizeof(pc), 1, stdout);
      fwrite(&addr, sizeof(addr), 1, stdout);
      break;

    default:
      perror("Invalid MemoryEvent\n");
      break;
  }
}
