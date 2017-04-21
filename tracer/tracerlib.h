#ifndef TRACERLIB_H
#define TRACERLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lib/event.h>

#include <stdint.h>

void loopEvent(event_t type);
void memoryEvent(event_t type, void *addr, uint64_t pc);

#ifdef __cplusplus
}
#endif

#endif // TRACERLIB_H
