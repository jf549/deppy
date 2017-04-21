#ifndef TRACERDEFS_H
#define TRACERDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum event {
  LOOP_ENTRY,
  LOOP_ITER,
  LOOP_EXIT,
  LOAD,
  STORE
} event_t;

typedef struct memory_event {
  uint64_t pc, addr;
} mem_event_t;

#ifdef __cplusplus
}
#endif

#endif // TRACERDEFS_H
