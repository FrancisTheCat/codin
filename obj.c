#include "obj.h"

#include "strings.h"
#include "strconv.h"

typedef struct {
  i32 vertices  [3];
  i32 normals   [3];
  i32 tex_coords[3];
} _Obj_Face;

extern b8 obj_load(String data, Obj_File *o, Allocator allocator) {
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
