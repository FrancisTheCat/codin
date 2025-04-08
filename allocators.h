#pragma once

#include "codin.h"

[[nodiscard]]
extern Allocator _mmap_allocator();

typedef struct {
  isize used;
  isize allocated;
  byte *data;
} Arena_Allocator;

[[nodiscard]]
extern Allocator arena_allocator_init(Arena_Allocator *arena, Byte_Slice data);
extern void arena_allocator_destroy(Arena_Allocator arena, Allocator allocator);

typedef struct {
  isize used;
  byte *data;
} Growing_Arena_Allocator_Block;

typedef struct {
  Vector(Growing_Arena_Allocator_Block) blocks;
  isize                                 block_size;
  Allocator                             backing;
} Growing_Arena_Allocator;

[[nodiscard]]
extern Allocator growing_arena_allocator_init(
  Growing_Arena_Allocator *ga,
  isize                    block_size,
  Allocator                backing_allocator
);
extern void growing_arena_allocator_destroy(Growing_Arena_Allocator arena);

typedef rawptr Pool_Allocation_Header;

typedef struct {
  // growing: just before the pointer is a pointer to the next block
  rawptr                  data;
  isize                   block_size;
  isize                   chunk_size;
  Pool_Allocation_Header *free;
  b8                      growing;
  Allocator               backing;
} Pool_Allocator;

extern Pool_Allocation_Header *pool_allocator_chunk_init(
  rawptr data,
  isize  chunk_size,
  isize  block_size
);

[[nodiscard]]
extern Allocator pool_allocator(
  Pool_Allocator *pool,
  isize           block_size,
  isize           chunk_size,
  b8              growing,
  Allocator       backing_allocator
);

extern void pool_allocator_destroy(Pool_Allocator pool);

typedef struct {
  Source_Code_Location location;
  isize                size;
  isize                id;
  rawptr               ptr;
  Allocator_Error      error;
  Allocator_Mode       mode;
} Tracked_Allocation;

/*
fmt_printflnc("Failed Allocations: %d", t->failed_allocations.len);
vector_iter(t->failed_allocations, fa, i, {
  fmt_printflnc(
    "Allocation(id: %d):\nError: %S\nMode:  %S\nSize: %d\nPtr:   %x\nLoc:   %L\n",
    fa->id,
    enum_to_string(Allocator_Error, fa->error),
    enum_to_string(Allocator_Mode, fa->mode),
    fa->size,
    fa->ptr,
    fa->location
  );
});
fmt_printflnc("");
fmt_printflnc("Leaked Allocations: %d", t->allocations.len);
hash_map_iter(t->allocations, ptr, fa, {
  fmt_printflnc(
    "Allocation(id: %d):\nError: %S\nMode:  %S\nSize:  %d\nPtr:   %x\nLoc:   %L\n",
    fa->id,
    enum_to_string(Allocator_Error, fa->error),
    enum_to_string(Allocator_Mode, fa->mode),
    fa->size,
    ptr,
    fa->location
  );
});
*/
typedef struct {
  Hash_Map(rawptr, Tracked_Allocation) allocations;
  Vector(Tracked_Allocation)           failed_allocations;
  isize                                current_allocation_index;
  Allocator                            backing;
} Tracking_Allocator;

extern void tracking_allocator_destroy(Tracking_Allocator track);

[[nodiscard]]
extern Allocator tracking_allocator_init(Tracking_Allocator *track, Allocator backing_allocator);

typedef struct {
  byte *data;
  isize size;
  isize current;
} Ring_Buffer_Allocator;

[[nodiscard]]
extern Allocator ring_buffer_allocator(Ring_Buffer_Allocator *r);

[[nodiscard]]
extern Allocator nil_allocator();

[[nodiscard]]
extern Allocator panic_allocator();

typedef struct {
  rawptr next;
  isize  size;
} Default_Allocator_Block_Header;

typedef struct {
  isize  size;
  rawptr page;
} Default_Allocator_Page_Header;

typedef struct {
  rawptr next;
  isize  size;
} Default_Allocator_Pool_Header;

typedef struct {
  // size_of   | Allocator 
  // ----------+-----------
  // ]0 ..8  ] | 0         
  // ]8 ..16 ] | 1         
  // ]16..32 ] | 2         
  // ]32..64 ] | 3         
  // ]64..128] | 4         
  Default_Allocator_Pool_Header  *fixed_allocator_blocks[5];
  rawptr                          fixed_allocator_free  [5];
  Default_Allocator_Block_Header *dynamic_chunks;
} Default_Allocator;

[[nodiscard]]
extern Allocator default_allocator_init(Default_Allocator *a);

#define ARENA_INIT_STACK(ARENA, SIZE) ({                                       \
  Byte_Slice data = alloca_slice(Byte_Slice, (SIZE));                          \
  arena_allocator_init(&(ARENA), data);                                        \
})
