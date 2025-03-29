#include "codin.h"

#include "linalg.h"
#include "strings.h"
#include "fmt.h"

typedef struct {
  Vec3 position, normal;
  Vec2 tex_coords;
} Obj_Vertex;

typedef struct {
  i32 vertices  [3];
  i32 normals   [3];
  i32 tex_coords[3];
} _Obj_Face;

typedef struct {
  Obj_Vertex a, b, c;
} Obj_Triangle;

typedef struct {
  // Slice(Obj_Vertex) vertices;
  Slice(Obj_Triangle) triangles;
} Obj_File;

internal i32 parse_int(String *str) {
  i32 value = 0;
  b8 negative = false;
  while (str->len) {
    char c = str->data[0];
    switch (c) {
    CASE '0' ... '9':
      value *= 10;
      value += c - '0';
    CASE '-':
      negative = true;
    DEFAULT:
      return negative ? -value : value;
    }
    str->data += 1;
    str->len  -= 1;
  }
  return negative ? -value : value;
}

internal f32 parse_float(String *str) {
  f32 value = 0;
  b8 negative = false;
  while (str->len) {
    char c = str->data[0];
    switch (c) {
    CASE '0' ... '9':
      value *= 10;
      value += c - '0';
    CASE '.':
      str->data += 1;
      str->len  -= 1;

      f32 factor = 0.1f;
      while (str->len) {
        char c = str->data[0];
        switch (c) {
        CASE '0' ... '9':
          value  += (c - '0') * factor;
          factor *= 0.1f;
        DEFAULT:
          return negative ? -value : value;
        }
        str->data += 1;
        str->len  -= 1;
      }

      goto end;
    CASE '-':
      negative = true;
    DEFAULT:
  return negative ? -value : value;
    }
    str->data += 1;
    str->len  -= 1;
  }

end:
  return negative ? -value : value;
}

internal b8 obj_load(String data, Obj_File *o, Allocator allocator) {
  Vector(Vec3)      positions, normals;
  Vector(Vec2)      tex_coords;
  Vector(_Obj_Face) faces;

  vector_init(&positions,  0, 8, allocator);
  vector_init(&normals,    0, 8, allocator);
  vector_init(&tex_coords, 0, 8, allocator);
  vector_init(&faces,      0, 8, allocator);

  string_lines_iterator(data, line, _i, {
    if (line.len < 3) {
      continue;
    }
    char first = IDX(line, 0);
    switch (first) {
    CASE 'v':
      switch (IDX(line, 1)) {
      CASE ' ': {
        Vec3 v;
        line = slice_start(line, 2);
        v.x  = parse_float(&line);
        line = slice_start(line, 1);
        v.y  = parse_float(&line);
        line = slice_start(line, 1);
        v.z  = parse_float(&line);

        vector_append(&positions, v);
      }
      CASE 'n': {
        Vec3 v;
        line = slice_start(line, 3);
        v.x  = parse_float(&line);
        line = slice_start(line, 1);
        v.y  = parse_float(&line);
        line = slice_start(line, 1);
        v.z  = parse_float(&line);

        vector_append(&normals, v);
      }
      CASE 't': {
        Vec2 v;
        line = slice_start(line, 3);
        v.x  = parse_float(&line);
        line = slice_start(line, 1);
        v.y  = parse_float(&line);

        vector_append(&tex_coords, v);
      }
      DEFAULT:
        continue;
      }
    CASE 'f':
      if (IDX(line, 1) != ' ') {
        continue;
      }
      line  = slice_start(line, 2);

      _Obj_Face face;
      face.vertices[0]   = parse_int(&line);
      line               = slice_start(line, 1);
      face.tex_coords[0] = parse_int(&line);
      line               = slice_start(line, 1);
      face.normals[0]    = parse_int(&line);
      line               = slice_start(line, 1);

      face.vertices[1]   = parse_int(&line);
      line               = slice_start(line, 1);
      face.tex_coords[1] = parse_int(&line);
      line               = slice_start(line, 1);
      face.normals[1]    = parse_int(&line);
      line               = slice_start(line, 1);

      face.vertices[2]   = parse_int(&line);
      line               = slice_start(line, 1);
      face.tex_coords[2] = parse_int(&line);
      line               = slice_start(line, 1);
      face.normals[2]    = parse_int(&line);
      line               = slice_start(line, 1);

      vector_append(&faces, face);

      if (line.len > 0) {
        face.vertices[0]   = parse_int(&line);
        line               = slice_start(line, 1);
        face.tex_coords[0] = parse_int(&line);
        line               = slice_start(line, 1);
        face.normals[0]    = parse_int(&line);
        line               = slice_start(line, 1);

        vector_append(&faces, face);
      }
    DEFAULT:
      continue;
    }
  });

  slice_init(&o->triangles, faces.len, allocator);

  slice_iter_v(faces, face, i, {
    IDX(o->triangles, i) = ((Obj_Triangle) {
      .a = (Obj_Vertex) {
        .position   = IDX(positions,  face.vertices  [0] - 1),
        .normal     = IDX(normals,    face.normals   [0] - 1),
        .tex_coords = IDX(tex_coords, face.tex_coords[0] - 1),
      },
      .b = (Obj_Vertex) {
        .position   = IDX(positions,  face.vertices  [1] - 1),
        .normal     = IDX(normals,    face.normals   [1] - 1),
        .tex_coords = IDX(tex_coords, face.tex_coords[1] - 1),
      },
      .c = (Obj_Vertex) {
        .position   = IDX(positions,  face.vertices  [2] - 1),
        .normal     = IDX(normals,    face.normals   [2] - 1),
        .tex_coords = IDX(tex_coords, face.tex_coords[2] - 1),
      },
    });
  });

  vector_delete(positions );
  vector_delete(normals   );
  vector_delete(tex_coords);
  vector_delete(faces     );
  
  return true;
}
