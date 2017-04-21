#ifndef TRACERLIB_H
#define TRACERLIB_H

#include "tracerdefs.h"

#include <stdint.h>

void loopEvent(event_t type);
void memoryEvent(event_t type, void *addr, uint64_t pc);

#endif // TRACERLIB_H
