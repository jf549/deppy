#include <stdio.h>

#define LOOP_ENTRY 0
#define LOOP_ITER 1
#define LOOP_EXIT 2
#define LOAD 0
#define STORE 1

void print(int isLoad, void *addr) {
  printf("%s: %p\n", isLoad ? "Load" : "Store", addr);
}

void loopEvent(int type) {
  switch (type) {
    case LOOP_ENTRY: printf("LoopEntry\n"); break;
    case LOOP_ITER: printf("LoopIter\n"); break;
    case LOOP_EXIT: printf("LoopExit\n"); break;
    default: perror("Invalid LoopEvent\n"); break;
  }
}

void memoryEvent(int type, void *addr, int pc) {
  switch (type) {
    case LOAD: printf("Load: %i %p\n", pc, addr); break;
    case STORE: printf("Store: %i %p\n", pc, addr); break;
    default: perror("Invalid MemoryEvent\n"); break;
  }
}
