#include "codin.h"

#include "linalg.h"

typedef struct {
  Vec3 position, normal;
  Vec2 tex_coords;
} Obj_Vertex;

typedef struct {
  Obj_Vertex a, b, c;
} Obj_Triangle;

typedef struct {
  Slice(Obj_Triangle) triangles;
} Obj_File;

extern b8 obj_load(String data, Obj_File *o, Allocator allocator);
