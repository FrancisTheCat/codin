#include "codin.h"

internal String path_to_absolute(String relative, Allocator allocator) {
  unimplemented();
}

internal b8 path_is_absolute(String path) {
  return path.len && (path.data[0] == '/');
}

internal b8 path_is_dir(String path) {
  return path.len && (path.data[path.len - 1] == '/');
}

internal String path_get_dir(String path) {
  for (isize i = path.len - 1; i > 0; i -= 1) {
    if (path.data[i] == '/') {
      return (String){.data = path.data, .len = i + 1};
    }
  }

  return (String){0};
}

#define path_get_base(path) path_get_name(path)

internal String path_get_name(String path) {
  for (isize i = path.len - 1; i > 0; i -= 1) {
    if (path.data[i] == '/') {
      return (String){.data = path.data + i + 1, .len = path.len - i - 1};
    }
  }

  return (String){0};
}

internal String path_get_stem(String path) {
  path = path_get_name(path);

  for (isize i = path.len - 1; i > 0; i -= 1) {
    if (path.data[i] == '.') {
      return (String){.data = path.data, .len = i};
    }
  }

  return (String){0};
}

internal String path_get_extension(String path) {
  for (isize i = path.len - 1; i > 0; i -= 1) {
    if (path.data[i] == '.') {
      return (String){.data = path.data + i, .len = path.len - i + 1};
    } else if (path.data[i] == '/') {
      return (String){0};
    }
  }

  return (String){0};
}
