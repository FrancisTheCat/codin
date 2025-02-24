#include "codin.h"

internal rawptr os_allocate_pages(isize n);
internal b8 os_deallocate_pages(rawptr p, isize n);
#define OS_PAGE_SIZE (4 * Kibibyte)

internal Allocator_Result _mmap_allocator_proc(
  rawptr               data,
  Allocator_Mode       mode,
  isize                size,
  isize                align,
  rawptr               old_memory,
  Source_Code_Location location
) {
  (void)data;
  (void)align;
  (void)location;

  Allocator_Result result = {0};
  isize end_padding;
  switch (mode) {
  case AM_Alloc:
    if (!size) {
      result.err = AE_None;
      return result;
    }
    end_padding = align - size % align;
    if (end_padding) {
      size += end_padding;
    }
    result.value = os_allocate_pages((size + OS_PAGE_SIZE - 1) / OS_PAGE_SIZE);
    if (!result.value) {
      result.err = AE_Out_Of_Memory;
    } else {
      mem_zero(result.value, size);
      result.err = AE_None;
    }
    return result;
  case AM_Free:
    os_deallocate_pages(old_memory, (size + OS_PAGE_SIZE - 1) / OS_PAGE_SIZE);
    result.err = AE_None;
    return result;
  case AM_Free_All:
    result.err = AE_Unimplemented;
    return result;
  }

  unreachable();
}

[[nodiscard]]
internal Allocator _mmap_allocator() {
  return (Allocator) {
      _mmap_allocator_proc,
      nil,
  };
}

typedef struct {
  isize used;
  isize allocated;
  byte *data;
} Arena_Allocator;

internal Allocator_Result arena_allocator_proc(
  rawptr data,
  Allocator_Mode mode,
  isize size,
  isize align,
  rawptr old_memory,
  Source_Code_Location location
) {
  (void)location;
  (void)old_memory;

  Allocator_Result result = {.value = nil, .err = AE_None};
  assert(data);
  Arena_Allocator *a = (Arena_Allocator *)data;
  isize align_offset;
  switch (mode) {
  case AM_Alloc:
    if (!size) {
      return result;
    }
    align_offset = required_align_offset((uintptr)&a->data[a->used], align);
    if (a->used + size + align_offset > a->allocated) {
      result.err = AE_Out_Of_Memory;
      return result;
    } else {
      result.value = (rawptr)&a->data[a->used + align_offset];
      a->used += size + align_offset;
      result.err = AE_None;
      return result;
    }
  case AM_Free:
    result.err = AE_Unimplemented;
    return result;
  case AM_Free_All:
    a->used = 0;
    result.err = AE_None;
    mem_zero(a->data, a->allocated);
    return result;
  }
  unreachable();
}

[[nodiscard]]
internal Allocator arena_allocator_init(Arena_Allocator *arena, Byte_Slice data) {
  arena->data = data.data;
  mem_zero(data.data, data.len);
  arena->allocated = data.len;
  arena->used = 0;
  return (Allocator) {
      arena_allocator_proc,
      arena,
  };
}

internal void arena_allocator_destroy(Arena_Allocator arena,
                                      Allocator allocator) {
  mem_free(arena.data, arena.allocated, allocator);
}

typedef struct {
  isize used;
  byte *data;
} Growing_Arena_Allocator_Block;

typedef struct {
  Vector(Growing_Arena_Allocator_Block) blocks;
  isize block_size;
  Allocator backing;
} Growing_Arena_Allocator;

internal Allocator_Result growing_arena_allocator_proc(
  rawptr data,
  Allocator_Mode mode,
  isize size,
  isize align,
  rawptr old_memory,
  Source_Code_Location location
) {
  (void)location;
  (void)old_memory;

  Allocator_Result result = {0};

  assert(data);
  Growing_Arena_Allocator *a = (Growing_Arena_Allocator *)data;
  assert(a->blocks.len);
  isize align_offset = 0;
  Growing_Arena_Allocator_Block *b = nil;
  Growing_Arena_Allocator_Block new_block = {};
  switch (mode) {
  case AM_Alloc:
    if (!size) {
      result.value = nil;
      result.err   = AE_None;
      return result;
    }
    b = &a->blocks.data[a->blocks.len - 1];
    align_offset = required_align_offset((uintptr)b->data[b->used], align);
    if (b->used + size + align_offset > a->block_size) {
      result.err = AE_None;
      if (size > a->block_size) {
        new_block.used = size;
        new_block.data = (byte *)or_return_err(mem_alloc_aligned(size, max(MAX_ALIGN, align), a->backing));
        vector_append(&a->blocks, new_block);
        result.value = new_block.data;
        return result;
      } else {
        new_block.used = size;
        new_block.data = (byte *)or_return_err(mem_alloc_aligned(a->block_size, max(MAX_ALIGN, align), a->backing));
        vector_append(&a->blocks, new_block);
        result.value = new_block.data;
        return result;
      }
    } else {
      result.value = (rawptr)&b->data[b->used + align_offset];
      b->used += size + align_offset;
      result.err = AE_None;
      return result;
    }
  case AM_Free:
    result.err = AE_Unimplemented;
    return result;
  case AM_Free_All:
    while (a->blocks.len > 1) {
      Growing_Arena_Allocator_Block b = vector_pop(&a->blocks);
      mem_free(b.data, max(a->block_size, b.used), a->backing);
    }
    b = &a->blocks.data[0];
    b->used = 0;
    mem_zero(b->data, a->block_size);
    result.err = AE_None;
    return result;
  }
  unreachable();
}

internal void growing_arena_allocator_destroy(Growing_Arena_Allocator arena) {
  vector_iter(arena.blocks, block, _i, {
    mem_free(block->data, max(arena.block_size, block->used), arena.backing);
  });
  vector_delete(arena.blocks);
}

[[nodiscard]]
internal Allocator growing_arena_allocator_init(
  Growing_Arena_Allocator *ga,
  isize                    block_size,
  Allocator                backing_allocator
) {
  *ga = (Growing_Arena_Allocator){0};
  vector_init(&ga->blocks, 1, 8, backing_allocator);
  IDX(ga->blocks, 0)->used = 0;
  IDX(ga->blocks, 0)->data = (byte *)unwrap_err(mem_alloc(block_size, backing_allocator));
  ga->backing    = backing_allocator;
  ga->block_size = block_size;
  return (Allocator) {
      .proc = growing_arena_allocator_proc,
      .data = ga,
  };
}

typedef rawptr Pool_Allocation_Header;

typedef struct {
  // growing: just before the pointer is a pointer to the next block
  rawptr data;
  isize block_size;
  isize chunk_size;
  Pool_Allocation_Header *free;
  bsize growing;
  Allocator backing;
} Pool_Allocator;

internal Pool_Allocation_Header *pool_allocator_chunk_init(
  rawptr data,
  isize  chunk_size,
  isize  block_size
) {
  Pool_Allocation_Header *prev = nil;
  for (uintptr i = 0; i < chunk_size; i += block_size) {
    Pool_Allocation_Header *p = (Pool_Allocation_Header *)((uintptr)data + i);
    *p = prev;
    prev = p;
  }
  return prev;
}

internal Allocator_Result pool_allocator_proc(
  rawptr               data,
  Allocator_Mode       mode,
  isize                size,
  isize                align,
  rawptr               old_memory,
  Source_Code_Location location
) {
  Allocator_Result result = {0};

  assert(data);
  Pool_Allocator *a = (Pool_Allocator *)data;
  rawptr new_chunk;

  assert(a->data);
  if (size > a->block_size) {
    result.err = AE_Invalid_Arguments;
    return result;
  }
  if (align) {
    if ((usize)a->free % align != 0) {
      result.err = AE_Invalid_Arguments;
      return result;
    }
  }

  switch (mode) {
  case AM_Alloc:
    if (!size) {
      result.err = AE_None;
      return result;
    }
    result.value = a->free;
    if (a->free) {
      a->free = *(Pool_Allocation_Header **)a->free;
    }
    result.err = (result.value || a->growing) ? AE_None : AE_Out_Of_Memory;
    if (!result.value && a->growing) {
      new_chunk = or_return_err(
          _mem_alloc(a->chunk_size + sizeof(rawptr), a->backing, location));
      assert(new_chunk);

      *(rawptr *)new_chunk = a->data;
      a->data = new_chunk;
      a->free = pool_allocator_chunk_init(
        (rawptr)((uintptr)a->data + sizeof(rawptr)),
        a->chunk_size,
        a->block_size
      );

      result.value = a->free;
      if (a->free) {
        a->free = *(Pool_Allocation_Header **)a->free;
      }
    }
    mem_zero(result.value, a->block_size);
    return result;
  case AM_Free:
    result.value = a->free;
    a->free      = (Pool_Allocation_Header *)old_memory;
    *a->free     = result.value;
    result.err   = AE_None;
    return result;
  case AM_Free_All:
    if (a->growing) {
      new_chunk = *(rawptr *)a->data;
      for (; new_chunk;) {
        rawptr n = *(rawptr *)(new_chunk);
        _mem_free(new_chunk, a->chunk_size, a->backing, location);
        new_chunk = n;
      }
      *(rawptr *)a->data = nil;
      mem_zero((rawptr)((uintptr)a->data + sizeof(rawptr)), a->chunk_size);
      a->free = pool_allocator_chunk_init(
        (rawptr)((uintptr)a->data + sizeof(rawptr)),
        a->chunk_size,
        a->block_size
      );
    } else {
      mem_zero(a->data, a->chunk_size);
      a->free = pool_allocator_chunk_init(a->data, a->chunk_size, a->block_size);
    }
    result.err = AE_None;
    return result;
  }
  unreachable();
}

[[nodiscard]]
internal Allocator pool_allocator(Pool_Allocator *pool, isize block_size,
                                  isize chunk_size, bsize growing,
                                  Allocator backing_allocator) {
  block_size = max(block_size, sizeof(Pool_Allocation_Header));
#define DEFAULT_POOL_CHUNK_SIZE 256
  chunk_size += (chunk_size == 0) * DEFAULT_POOL_CHUNK_SIZE * block_size;
  assert(chunk_size % block_size == 0);

  pool->backing    = backing_allocator;
  pool->block_size = block_size;
  pool->chunk_size = chunk_size;
  pool->growing    = growing;

  if (growing) {
    pool->data = unwrap_err(mem_alloc(pool->chunk_size + sizeof(rawptr), pool->backing));
    assert(pool->data);
    pool->free = pool_allocator_chunk_init(
      (rawptr)((uintptr)pool->data + sizeof(rawptr)),
      pool->chunk_size,
      pool->block_size
    );
  } else {
    pool->data = unwrap_err(mem_alloc(chunk_size, backing_allocator));
    pool->free = pool_allocator_chunk_init(pool->data, chunk_size, block_size);
  }

#undef DEFAULT_POOL_CHUNK_SIZE

  return (Allocator) {
      .proc = pool_allocator_proc,
      .data = pool,
  };
}

internal void pool_allocator_destroy(Pool_Allocator pool) {
  rawptr new_chunk;
  if (pool.growing) {
    new_chunk = pool.data;
    for (; new_chunk;) {
      rawptr n = *(rawptr *)(new_chunk);
      mem_free(new_chunk, pool.chunk_size + sizeof(rawptr), pool.backing);
      new_chunk = n;
    }
  } else {
    mem_free(pool.data, pool.chunk_size, pool.backing);
  }
}

typedef struct {
  Source_Code_Location location;
  isize                size;
  isize                id;
  rawptr               ptr;
  Allocator_Error      error;
  Allocator_Mode       mode;
} Tracked_Allocation;

typedef struct {
  Hash_Map(rawptr, Tracked_Allocation) allocations;
  Vector(Tracked_Allocation)           failed_allocations;
  isize                                current_allocation_index;
  Allocator                            backing;
} Tracking_Allocator;

internal Allocator_Result tracking_allocator_proc(
  rawptr data,
  Allocator_Mode mode,
  isize size,
  isize align,
  rawptr old_memory,
  Source_Code_Location location
) {
  Allocator_Result result = {0};

  assert(data);
  Tracking_Allocator *a = (Tracking_Allocator *)data;
  Tracked_Allocation t = {.location = location, .size = size, .mode = mode};
  Tracked_Allocation *old = nil;
  switch (mode) {
  case AM_Alloc:
    if (!size) {
      result.err = AE_None;
      return result;
    }
    result = mem_alloc_aligned(size, align, a->backing);
    t.id   = a->current_allocation_index;
    a->current_allocation_index += 1;
    if (!result.err) {
      hash_map_insert(&a->allocations, result.value, t);
    } else {
      t.error = result.err;
      t.size  = size;
      t.ptr   = result.value;
      vector_append(&a->failed_allocations, t);
      return result;
    }
    return result;
  case AM_Free:
    old = hash_map_get(a->allocations, old_memory);

    if (old) {
      if (old->size >= size) {
        result.err = mem_free(old_memory, size, a->backing);
        if (result.err) {
          t.size  = size;
          t.error = result.err;
          t.ptr   = old_memory;
          vector_append(&a->failed_allocations, t);
        } else {
          assert(hash_map_remove(&a->allocations, old_memory));
        }
      } else {
        t.size  = size;
        t.error = AE_Invalid_Arguments;
        t.ptr   = old_memory;
        vector_append(&a->failed_allocations, t);
      }
    } else {
      t.ptr   = old_memory;
      t.size  = 0;
      t.error = AE_Invalid_Arguments;
      vector_append(&a->failed_allocations, t);
    }
    return result;
  case AM_Free_All:
    result.err = mem_free_all(a->backing);
    if (!result.err) {
      hash_map_clear(&a->allocations);
    }
    return result;
  }
  unreachable();
}

internal void tracking_allocator_destroy(Tracking_Allocator track) {
  vector_delete(track.failed_allocations);
  hash_map_delete(track.allocations);
}

[[nodiscard]]
internal Allocator tracking_allocator_init(Tracking_Allocator *track,
                                           Allocator backing_allocator) {
  hash_map_init(&track->allocations, 1024, nil, nil, backing_allocator);
  vector_init(&track->failed_allocations, 0, 8, backing_allocator);
  track->backing = backing_allocator;
  return (Allocator) {
      .proc = tracking_allocator_proc,
      .data = track,
  };
}

typedef struct {
  byte *data;
  isize size;
  isize current;
} Ring_Buffer_Allocator;

internal Allocator_Result ring_buffer_allocator_proc(
  rawptr               data,
  Allocator_Mode       mode,
  isize                size,
  isize                align,
  rawptr               old_memory,
  Source_Code_Location location
) {
  (void)location;
  (void)old_memory;

  Allocator_Result result = {0};

  assert(data);
  Ring_Buffer_Allocator *r = (Ring_Buffer_Allocator *)data;

  uintptr align_offset;

  switch (mode) {
  case AM_Alloc:
    if (!size) {
      return result;
    }
    align_offset = required_align_offset(r->current, align);
    if (r->current + size + align_offset > r->size) {
      if (size > r->size) {
        result.err = AE_Out_Of_Memory;
        return result;
      }

      r->current = 0;
      mem_zero(r->data, r->size);
    }

    result.value = &r->data[r->current + align_offset];
    r->current  += size + align_offset;
    return result;
  case AM_Free:
  case AM_Free_All:
    result.err = AE_Unimplemented;
    return result;
  }
  unreachable();
}

[[nodiscard]]
internal Allocator ring_buffer_allocator(Ring_Buffer_Allocator *r) {
  return (Allocator) {
    .proc = ring_buffer_allocator_proc,
    .data = r,
  };
}

internal Allocator_Result nil_allocator_proc(
  rawptr               data,
  Allocator_Mode       mode,
  isize                size,
  isize                align,
  rawptr               old_memory,
  Source_Code_Location location
) {
  (void)data;
  (void)mode;
  (void)size;
  (void)align;
  (void)old_memory;
  (void)location;

  return (Allocator_Result) {.value = nil, .err = AE_None};
}


[[nodiscard]]
internal Allocator nil_allocator() {
  return (Allocator) {
    .proc = nil_allocator_proc,
    .data = nil,
  };
}

internal Allocator_Result panic_allocator_proc(
  rawptr               data,
  Allocator_Mode       mode,
  isize                size,
  isize                align,
  rawptr               old_memory,
  Source_Code_Location location
) {
  (void)data;
  (void)mode;
  (void)size;
  (void)align;
  (void)old_memory;
  (void)location;

  panic("panic allocator");
}

[[nodiscard]]
internal Allocator panic_allocator() {
  return (Allocator) {
    .proc = panic_allocator_proc,
    .data = nil,
  };
}

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

internal Allocator_Result default_allocator_proc(
  rawptr data,
  Allocator_Mode mode,
  isize size,
  isize align,
  rawptr old_memory,
  Source_Code_Location location
) {
  (void)location;

  Allocator_Result               result      = {0};
  Default_Allocator             *da          = (Default_Allocator *)data;
  Default_Allocator_Page_Header *page_header = nil;

  isize size_category = 0;
  if (size > 128) {
    size_category = 5;
  } else if (size > 64) {
    size_category = 4;
  } else if (size > 32) {
    size_category = 3;
  } else if (size > 16) {
    size_category = 2;
  } else if (size > 8) {
    size_category = 1;
  }

  switch (mode) {
  case AM_Alloc:
    switch (size_category) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      if (!da->fixed_allocator_free[size_category]) {
        da->fixed_allocator_free[size_category] =
          pool_allocator_chunk_init(os_allocate_pages(1), OS_PAGE_SIZE, 8 << size_category);
      }
      result.value = da->fixed_allocator_free[size_category];
      da->fixed_allocator_free[size_category] = *(rawptr *)result.value;
      mem_zero(result.value, size);
      return result;
    case 5:
      result.value = os_allocate_pages((size + sizeof(Default_Allocator_Page_Header) + OS_PAGE_SIZE - 1) / OS_PAGE_SIZE);
      assert(align <= sizeof(Default_Allocator_Page_Header));
      old_memory   = result.value;
      *(Default_Allocator_Page_Header *)result.value = (Default_Allocator_Page_Header) {
        .size = (size + MAX_ALIGN + OS_PAGE_SIZE - 1) / OS_PAGE_SIZE,
        .page = old_memory,
      };
      *(Default_Allocator_Page_Header **)&result.value += 1;
      return result;
    }

  case AM_Free:
    switch (size_category) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      if (da->fixed_allocator_free[size_category]) {
        *(rawptr *)old_memory = da->fixed_allocator_free[size_category];
      }
      da->fixed_allocator_free[size_category] = old_memory;
      return result;
    case 5:
      page_header = &((Default_Allocator_Page_Header *)old_memory)[-1];
      assert(page_header == page_header->page);
      assert(page_header->size < 100000);
      assert(os_deallocate_pages(page_header->page, page_header->size));
      return result;
    }

  case AM_Free_All:
    return (Allocator_Result) {.err = AE_Unimplemented};
  }
}

[[nodiscard]]
internal Allocator default_allocator_init(Default_Allocator *a) {
  *a = (Default_Allocator) {0};
  return (Allocator) {
    .proc = default_allocator_proc,
    .data = (rawptr)a,
  };
}

internal void default_allocator_destroy(Default_Allocator *a) {
  (void)a;
  unimplemented();
}
