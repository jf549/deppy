#ifndef TRACERLIB_H
#define TRACERLIB_H

#include <stdint.h>

void loopEvent(uint8_t type);
void memoryEvent(uint8_t type, void *addr, uint64_t pc);

#endif // TRACERLIB_H
