#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

#include "../BaseType.h"
#include "stdbool.h"

typedef struct {
  u32 head;
  u32 tail;
  u32 item_capacity;
  u32 item_size;
  u32 item_count;
  void* buffer;
} ring_buffer_context_t;

bool ring_buffer_init(ring_buffer_context_t* context, u32 item_size,
                      u32 item_capacity);
bool ring_buffer_deinit(ring_buffer_context_t* context);
bool ring_buffer_empty(ring_buffer_context_t* context);
bool ring_buffer_full(ring_buffer_context_t* context);
bool ring_buffer_push(ring_buffer_context_t* context, void* item);
bool ring_buffer_pop(ring_buffer_context_t* context, void* item);
void ring_buffer_clear(ring_buffer_context_t* context);
u32 ring_buffer_item_count(ring_buffer_context_t* context);

#endif
