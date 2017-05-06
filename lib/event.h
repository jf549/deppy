#ifndef EVENT_H
#define EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum event {
  LOOP_ENTRY,
  LOOP_ITER,
  LOOP_EXIT,
  LOAD,
  STORE,
  SENTINAL
} event_t;

#ifdef __cplusplus
}
#endif

#endif // EVENT_H
