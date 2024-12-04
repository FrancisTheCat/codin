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
  return (u64)h;
}

#define Hash_Map_Node(K, V)                                                    \
  struct {                                                                     \
    V value;                                                                   \
    K key;                                                                     \
    u64 hash;                                                                  \
    rawptr next;                                                               \
  }

#define Hash_Map(K, V)                                                         \
  struct {                                                                     \
    Hash_Map_Node(K, V) * data;                                                \
    b8 *used_nodes;                                                            \
    isize len;                                                                 \
    isize cap;                                                                 \
    b8 (*compare)(K, K);                                                    \
    u64 (*hash)(K);                                                            \
    Allocator allocator;                                                       \
    Allocator node_allocator;                                                  \
  }

#define Hash_Set(K) Hash_Map(K, struct {})

#define hash_map_init(map, capacity, compare_func, hash_func, ally)            \
  {                                                                            \
    (map)->data = (type_of((map)->data))unwrap_err(                            \
        mem_alloc(sizeof(*((map)->data)) * capacity, ally));                   \
    (map)->used_nodes = (b8 *)unwrap_err(mem_alloc(capacity, ally));           \
    (map)->len = 0;                                                            \
    (map)->cap = capacity;                                                     \
    (map)->allocator = ally;                                                   \
    (map)->hash = hash_func;                                                   \
    (map)->compare = compare_func;                                             \
    Pool_Allocator *pool =                                                     \
        (Pool_Allocator *)unwrap_err(mem_alloc(sizeof(Pool_Allocator), ally)); \
    (map)->node_allocator =                                                    \
        pool_allocator(pool, sizeof(type_of((map)->data[0])), 0, true, ally);  \
  }

#define hash_map_delete(map)                                                   \
  {                                                                            \
    mem_free((map).data, (map).cap * sizeof((map.data)[0]), (map).allocator);  \
    mem_free((map).used_nodes, (map).cap, (map).allocator);                    \
    pool_allocator_destroy(*(Pool_Allocator *)(map).node_allocator.data);      \
    mem_free((map).node_allocator.data, sizeof(Pool_Allocator),                \
             (map).allocator);                                                 \
  }

#define hash_map_clear(map)                                                    \
  (map)->len = 0;                                                              \
  mem_free_all((map)->node_allocator);                                         \
  mem_zero((map)->used_nodes, (map)->cap);                                     \
  mem_zero((map)->data, (map)->cap * sizeof((map)->data[0]));

// NOLINTBEGIN
#define hash_map_get(map, key_)                                                \
  ({                                                                           \
    u64 hash;                                                                  \
    type_of((map).data[0].key) key_copy = key_;                                \
    if ((map).hash) {                                                          \
      hash = (map).hash(key_);                                                 \
    } else {                                                                   \
      hash = default_hasher(&key_copy, INITIAL_HASH_SEED, sizeof(key_copy));   \
    }                                                                          \
    isize index = hash % (map).cap;                                            \
    type_of((map).data) node = &(map).data[index];                             \
    type_of(node->value) *value = nil;                                         \
    if ((map).used_nodes[index]) {                                             \
      for (;; node = ((type_of(node))node->next)) {                            \
        if (node->hash == hash &&                                              \
            ((((map)).compare && ((map)).compare(node->key, key_)) ||          \
             (!((map)).compare &&                                              \
              mem_compare(&node->key, &key_copy, sizeof(key_copy))))) {        \
          value = &node->value;                                                \
          break;                                                               \
        }                                                                      \
        if (!node->next) {                                                     \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    value;                                                                     \
  })
// NOLINTEND

#define hash_map_contains(map, key_)                                           \
  ({                                                                           \
    u64 hash;                                                                  \
    type_of((map).data[0].key) key_copy = key_;                                \
    if ((map).hash) {                                                          \
      hash = (map).hash(key_);                                                 \
    } else {                                                                   \
      hash = default_hasher(&key_copy, INITIAL_HASH_SEED, sizeof(key_copy));   \
    }                                                                          \
    isize index = hash % (map).cap;                                            \
    type_of((map).data[0]) node = (map).data[index];                           \
    b64 ret = false;                                                           \
    if ((map).used_nodes[index]) {                                             \
      for (;; node = *((type_of(node) *)node.next)) {                          \
        if (node.hash == hash &&                                               \
            (((map).compare && (map).compare(node.key, key_)) ||               \
             (!(map).compare &&                                                \
              mem_compare(&node.key, &key_copy, sizeof(key_copy))))) {         \
          ret = true;                                                          \
          break;                                                               \
        }                                                                      \
        if (!node.next) {                                                      \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    ret;                                                                       \
  })

// NOLINTBEGIN
#define hash_map_insert(map, key_, value_)                                     \
  {                                                                            \
    u64 hash;                                                                  \
    type_of((map)->data[0].key) key_copy = key_;                               \
    if ((map)->hash) {                                                         \
      hash = (map)->hash(key_);                                                \
    } else {                                                                   \
      hash = default_hasher(&key_copy, INITIAL_HASH_SEED, sizeof(key_copy));   \
    }                                                                          \
    isize index = hash % (map)->cap;                                           \
    type_of((map)->data) node = &(map)->data[index];                           \
    b64 done = false;                                                          \
    if ((map)->used_nodes[index]) {                                            \
      for (;; node = ((type_of(node))node->next)) {                            \
        if (node->hash == hash &&                                              \
            (((map)->compare && (map)->compare(node->key, key_)) ||            \
             (!(map)->compare &&                                               \
              mem_compare(&node->key, &key_copy, sizeof(key_copy))))) {        \
          done = true;                                                         \
          node->value = value_;                                                \
          break;                                                               \
        }                                                                      \
        if (!node->next) {                                                     \
          break;                                                               \
        }                                                                      \
      }                                                                        \
    } else {                                                                   \
      assert(node->next == nil);                                               \
                                                                               \
      node->value = value_;                                                    \
      node->key = key_;                                                        \
      node->hash = hash;                                                       \
      node->next = nil;                                                        \
      done = true;                                                             \
      (map)->used_nodes[index] = true;                                         \
      (map)->len += 1;                                                         \
    }                                                                          \
    if (!done) {                                                               \
      node->next =                                                             \
          unwrap_err(mem_alloc(sizeof(*node), (map)->node_allocator));         \
      assert(node->next);                                                      \
      type_of(node) new_node = (type_of(node))node->next;                      \
      new_node->hash = hash;                                                   \
      new_node->value = value_;                                                \
      new_node->key = key_;                                                    \
      new_node->next = nil;                                                    \
                                                                               \
      (map)->len += 1;                                                         \
    }                                                                          \
  }
// NOLINTEND

#define hash_map_remove(map, key_)                                             \
  ({                                                                           \
    b8 ok;                                                                     \
    u64 hash;                                                                  \
    type_of((map)->data[0].key) key_copy = key_;                               \
    if ((map)->hash) {                                                         \
      hash = (map)->hash(key_);                                                \
    } else {                                                                   \
      hash = default_hasher(&key_copy, INITIAL_HASH_SEED, sizeof(key_copy));   \
    }                                                                          \
    isize index = hash % (map)->cap;                                           \
    type_of((map)->data) node = &(map)->data[index];                           \
    type_of((map)->data) prev = nil;                                           \
    if ((map)->used_nodes[index]) {                                            \
      for (;; node = (type_of(node))node->next) {                              \
        if (node->hash == hash &&                                              \
            (((map)->compare && (map)->compare(node->key, key_)) ||            \
             (!(map)->compare &&                                               \
              mem_compare(&node->key, &key_copy, sizeof(key_copy))))) {        \
          if (prev) {                                                          \
            mem_free(prev->next, sizeof(*node), (map)->node_allocator);        \
            prev->next = node->next;                                           \
          } else {                                                             \
            if (node->next) {                                                  \
              rawptr ptr_copy = node->next;                                    \
              (map)->data[index] = *(type_of(node))node->next;                 \
              mem_free(ptr_copy, sizeof(*node), (map)->node_allocator);        \
            } else {                                                           \
              (map)->used_nodes[index] = false;                                \
            }                                                                  \
          }                                                                    \
          (map)->len -= 1;                                                     \
          ok = true;                                                           \
          break;                                                               \
        }                                                                      \
        if (!node->next) {                                                     \
          break;                                                               \
        }                                                                      \
        prev = node;                                                           \
      }                                                                        \
    }                                                                          \
    ok;                                                                        \
  })

#define hash_map_iter(map, key_, value_, BLOCK)                                \
  {                                                                            \
    for (isize _hash_map_iteration_index = 0;                                  \
         _hash_map_iteration_index < (map).cap;                                \
         _hash_map_iteration_index += 1) {                                     \
      type_of((map).data[0]) *_hash_map_iteration_node =                       \
          &(map).data[_hash_map_iteration_index];                              \
      if ((map).used_nodes[_hash_map_iteration_index]) {                       \
        for (;;) {                                                             \
          type_of(_hash_map_iteration_node->key) key_ =                        \
              _hash_map_iteration_node->key;                                   \
          type_of(_hash_map_iteration_node->value) *value_ =                   \
              &_hash_map_iteration_node->value;                                \
                                                                               \
          { BLOCK }                                                            \
                                                                               \
          if (_hash_map_iteration_node->next) {                                \
            _hash_map_iteration_node = (type_of(                               \
                _hash_map_iteration_node))_hash_map_iteration_node->next;      \
          } else {                                                             \
            break;                                                             \
          }                                                                    \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }

internal u64 cstring_hash(cstring value) {
  isize len = cstring_len(value);
  return default_hasher((rawptr)value, INITIAL_HASH_SEED, len);
}

internal u64 string_hash(String value) {
  return default_hasher((rawptr)value.data, INITIAL_HASH_SEED, value.len);
}
