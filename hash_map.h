// A simple hashmap where if there is a hash collision, we just look for the next unused entry  
// An unused entry is represented by a value of 0 in the hash and a previously used, but now    
// removed entry is represented by a 1. This means that the hash function should not emit those 
// values. The tombstone 1 value is neccessary that colliding entries that were behind a now    
// removed entry dont get lost. (hashes returned by user hash functions that violate this      
// requirement will be fixed automatically by adding 2 to avoid bugs)
#include "codin.h"

#define INITIAL_HASH_SEED 0xcbf29ce484222325

#define HASH_MASK (u64)(((u64)1 << (u64)(8 * sizeof(uintptr) - 1)) - 1)

internal u64 default_hasher(rawptr data, uintptr seed, isize N) {
  u64 h = seed + INITIAL_HASH_SEED;
  byte *p = (byte *)data;
  for (int i = 0; i < N; i += 1) {
    h = (h ^ (u64)p[0]) * 0x100000001b3;
    p += 1;
  }
  h &= HASH_MASK;
  if (h < 2) {
    h += 2;
  }
  return (u64)h;
}

#define Hash_Map_Entry(K, V)                                                   \
  struct {                                                                     \
    u64 hash;                                                                  \
    V   value;                                                                 \
    K   key;                                                                   \
  }

#define Hash_Map(K, V)                                                         \
  struct {                                                                     \
    Hash_Map_Entry(K, V) *data;                                                \
    isize len;                                                                 \
    isize cap;                                                                 \
    b8 (*compare)(K, K);                                                       \
    u64 (*hash)(K);                                                            \
    Allocator allocator;                                                       \
  }

#define Hash_Set(K) Hash_Map(K, struct {})

#define hash_map_init(map, capacity, compare_func, hash_func, ally)            \
  {                                                                            \
    (map)->data       = (type_of((map)->data))unwrap_err(                      \
        mem_alloc(sizeof(*((map)->data)) * capacity, ally));                   \
    (map)->len        = 0;                                                     \
    (map)->cap        = capacity;                                              \
    (map)->allocator  = ally;                                                  \
    (map)->hash       = hash_func;                                             \
    (map)->compare    = compare_func;                                          \
  }

#define hash_map_delete(map)                                                   \
  mem_free((map).data, (map).cap * sizeof((map.data)[0]), (map).allocator); 

#define hash_map_clear(map)                                                    \
  mem_zero((map)->data, (map)->len * sizeof((map)->data[0]));                  \
  (map)->len = 0;

#define _hash_map_hash(map_, key_) ({                                          \
    u64 _hash;                                                                 \
    if (map_.hash) {                                                           \
      _hash = map_.hash(key_);                                                 \
      if (_hash < 2) { _hash += 2; } /* save users that can't read the docs */ \
    } else {                                                                   \
      _hash = default_hasher(&key_, INITIAL_HASH_SEED, sizeof(key_));          \
    }                                                                          \
    _hash;                                                                     \
  })

#define _hash_compare_entry(map_, entry_, key_, hash_)                         \
  (entry_->hash == hash_ &&                                                    \
   (((map_).compare && (map_).compare(entry_->key, key_)) ||                   \
    (!(map_).compare && mem_compare(&entry_->key, &key_, sizeof(key_)))))

#define hash_map_get(MAP, KEY)                                                 \
  ({                                                                           \
    type_of(KEY) key_ = (KEY);                                                 \
    type_of(MAP) map_ = (MAP);                                                 \
    u64 hash = _hash_map_hash(map_, key_);                                     \
    isize index = map_.cap ? hash % map_.cap : 0;                              \
    type_of(map_.data->value) *value = nil;                                    \
                                                                               \
    for (;map_.cap; index = (index + 1) % map_.cap) {                          \
      type_of(map_.data) entry = &map_.data[index];                            \
      if (!entry->hash) { break; }                                             \
      if (_hash_compare_entry(map_, entry, key_, hash)) {                      \
        value = &entry->value;                                                 \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    value;                                                                     \
  })

#define hash_map_contains(MAP, KEY)                                            \
  ({                                                                           \
    type_of(KEY) key_ = (KEY);                                                 \
    type_of(MAP) map_ = (MAP);                                                 \
    u64 hash = _hash_map_hash(map_, key_);                                     \
    isize index = map_.cap ? hash % map_.cap : 0;                              \
    b8 contains = false;                                                       \
                                                                               \
    for (;map_.cap; index = (index + 1) % map_.cap) {                          \
      type_of(map_.data) entry = &map_.data[index];                            \
      if (!entry->hash) { break; }                                             \
      if (_hash_compare_entry(map_, entry, key_, hash)) {                      \
        contains = true;                                                       \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    contains;                                                                  \
  })

#define hash_map_insert(MAP, KEY, VALUE)                                       \
  {                                                                            \
    type_of(KEY  ) key_   = (KEY);                                             \
    type_of(MAP  ) map_   = (MAP);                                             \
    type_of(VALUE) value_ = (VALUE);                                           \
    if (!map_->allocator.proc) { map_->allocator = context.allocator; }        \
    if (map_->len * 3 >= map_->cap * 2) {                                      \
      isize cap_ = max(64, 2 * map_->cap);                                     \
      type_of(map_->data) new_data = (type_of(map_->data))unwrap_err(          \
          mem_alloc(sizeof(*(map_->data)) * cap_, map_->allocator));           \
      for_range(i, 0, map_->cap) {                                             \
        type_of(*new_data) e = map_->data[i];                                  \
        isize index = e.hash % cap_;                                           \
                                                                               \
        if (e.hash < 2) { continue; }                                          \
        for (;; index = (index + 1) % cap_) {                                  \
          type_of(map_->data) entry = &new_data[index];                        \
          if (!entry->hash) {                                                  \
            entry->value = e.value;                                            \
            entry->key   = e.key;                                              \
            entry->hash  = e.hash;                                             \
            break;                                                             \
          }                                                                    \
          if (_hash_compare_entry((*map_), entry, (e.key), (e.hash))) {        \
            unreachable();                                                     \
          }                                                                    \
        }                                                                      \
      }                                                                        \
      mem_free(map_->data, map_->cap * sizeof(*map_->data), map_->allocator);  \
      map_->data = new_data;                                                   \
      map_->cap  = cap_;                                                       \
    }                                                                          \
    u64 hash = _hash_map_hash((*map_), key_);                                  \
    isize index = hash % map_->cap;                                            \
                                                                               \
    for (;; index = (index + 1) % map_->cap) {                                 \
      type_of(map_->data) entry = &map_->data[index];                          \
      if (entry->hash < 2) {                                                   \
        entry->value = value_;                                                 \
        entry->key   = key_;                                                   \
        entry->hash  = hash;                                                   \
        map_->len   += 1;                                                      \
        break;                                                                 \
      }                                                                        \
      if (_hash_compare_entry((*map_), entry, key_, hash)) {                   \
        entry->value = value_;                                                 \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  }

#define hash_map_remove(MAP, KEY)                                              \
  ({                                                                           \
    b8 ok = false;                                                             \
    type_of(KEY  ) key_   = (KEY);                                             \
    type_of(MAP  ) map_   = (MAP);                                             \
    u64 hash = _hash_map_hash((*map_), key_);                                  \
    isize index = map_->cap ? hash % map_->cap : 0;                            \
                                                                               \
    for (;map_->cap; index = (index + 1) % map_->cap) {                        \
      type_of(map_->data) entry = &map_->data[index];                          \
      if (!entry->hash) {                                                      \
        ok = false;                                                            \
        break;                                                                 \
      }                                                                        \
      if (_hash_compare_entry((*map_), entry, key_, hash)) {                   \
        entry->hash = 1;                                                       \
        ok = true;                                                             \
        map_->len -= 1;                                                        \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    ok;                                                                        \
  })

#define hash_map_reserve(MAP, NEW_CAP)                                         \
  {                                                                            \
    type_of(MAP)     map_ = (MAP);                                             \
    type_of(NEW_CAP) cap_ = (NEW_CAP);                                         \
    if (cap_ > map_->cap) {                                                    \
      type_of(map_->data) new_data = (type_of(map_->data))unwrap_err(          \
          mem_alloc(sizeof(*(map_->data)) * cap_, map_->allocator));           \
      for_range(i, 0, map_->cap) {                                             \
        type_of(*new_data) e = new_data[i];                                    \
        isize index = e.hash % cap_;                                           \
                                                                               \
        for (;; index = (index + 1) % cap_) {                                  \
          type_of(map_->data) entry = &map_->data[index];                      \
          if (entry->hash < 2) {                                               \
            entry->value = e.value;                                            \
            entry->key   = e.key;                                              \
            entry->hash  = e.hash;                                             \
            break;                                                             \
          }                                                                    \
          if (_hash_compare_entry((*map_), entry, (e.key), (e.hash))) {        \
            unreachable();                                                     \
          }                                                                    \
        }                                                                      \
      }                                                                        \
      mem_free(map_->data, map_->cap * sizeof(*map_->data), map_->allocator);  \
      map_->data = new_data;                                                   \
      map_->cap  = cap_;                                                       \
    }                                                                          \
  }

#define hash_map_iter(map, key_, value_, BLOCK)                                \
  for_range(_hash_map_iter_index, 0, (map).cap) {                              \
    type_of(map.data) _hash_map_iter_entry =                                   \
      &map.data[_hash_map_iter_index];                                         \
    type_of(_hash_map_iter_entry->key)    key_   =                             \
      _hash_map_iter_entry->key;                                               \
    type_of(&_hash_map_iter_entry->value) value_ =                             \
      &_hash_map_iter_entry->value;                                            \
    if (_hash_map_iter_entry->hash > 1) { BLOCK; }                             \
  }                                                                            \

internal u64 cstring_hash(cstring value) {
  isize len = cstring_len(value);
  return default_hasher((rawptr)value, INITIAL_HASH_SEED, len);
}

internal u64 string_hash(String value) {
  return default_hasher((rawptr)value.data, INITIAL_HASH_SEED, value.len);
}
