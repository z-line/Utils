#include "RingBuffer.h"

#include "memory.h"
#include "stddef.h"

bool ring_buffer_init(ring_buffer_context_t* context, u32 item_size,
                      u32 item_capacity) {
  if (context->buffer != NULL && !ring_buffer_deinit(context)) {
    return false;
  }
  context->item_size = item_size;
  context->item_capacity = item_capacity;
  context->item_count = 0;
  context->head = 0;
  context->tail = 0;
  context->buffer = malloc(item_size * item_capacity);
  if (context->buffer != NULL) {
    return true;
  }
  return false;
}

bool ring_buffer_deinit(ring_buffer_context_t* context) {
  if (context->buffer != NULL) {
    free(context->buffer);
  }
  memset(context, 0, sizeof(*context));
  return true;
}

bool ring_buffer_empty(ring_buffer_context_t* context) {
  return context->item_count == 0;
}

bool ring_buffer_full(ring_buffer_context_t* context) {
  return context->item_count == context->item_capacity;
}

bool ring_buffer_push(ring_buffer_context_t* context, void* item) {
  if (ring_buffer_full(context)) {
    return false;
  }
  memcpy(context->buffer + context->head * context->item_size, item,
         context->item_size);
  context->head = (context->head + 1) % context->item_capacity;
  context->item_count++;
  return true;
}

bool ring_buffer_pop(ring_buffer_context_t* context, void* item) {
  if (ring_buffer_empty(context)) {
    return false;
  }
  memcpy(item, context->buffer + context->tail * context->item_size,
         context->item_size);
  context->tail = (context->tail + 1) % context->item_size;
  context->item_count--;
  return true;
}

void ring_buffer_clear(ring_buffer_context_t* context) {
  context->item_count = 0;
  context->head = 0;
  context->tail = 0;
}

u32 ring_buffer_item_count(ring_buffer_context_t* context) {
  return context->item_count;
}
