#include <stdint.h>
#include <stdio.h>

#define LOOP_ENTRY 0
#define LOOP_ITER 1
#define LOOP_EXIT 2
#define LOAD 0
#define STORE 1

void loopEvent(uint8_t type);
void memoryEvent(uint8_t type, void *addr, uint64_t pc);

void loopEvent(uint8_t type) {
  switch (type) {
    case LOOP_ENTRY: printf("LoopEntry\n"); break;
    case LOOP_ITER: printf("LoopIter\n"); break;
    case LOOP_EXIT: printf("LoopExit\n"); break;
    default: perror("Invalid LoopEvent\n"); break;
  }
}

void memoryEvent(uint8_t type, void *addr, uint64_t pc) {
  switch (type) {
    case LOAD: printf("Load %llu %p\n", pc, addr); break;
    case STORE: printf("Store %llu %p\n", pc, addr); break;
    default: perror("Invalid MemoryEvent\n"); break;
  }
}
