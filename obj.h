#include "codin.h"

#include "linalg.h"

typedef struct {
  Vec3 position, normal;
  Vec2 tex_coords;
} Obj_Vertex;

typedef struct {
  Obj_Vertex a, b, c;
  i32        material;
} Obj_Triangle;

typedef struct {
  String path;
} Obj_Texture;

typedef struct {
  String name;
  union {
    struct {
      Color3      ambient, specular;
      f32         specular_exponent;
      Obj_Texture texture_ambient;
      Obj_Texture texture_specular_color;
      Obj_Texture texture_specular;
      Obj_Texture texture_alpha;
      Obj_Texture texture_bump;
    } simple;
    struct {
      f32         metallic, roughness, sheen, clearcoat, clearcoat_roughness, anisotropic;
      Obj_Texture texture_roughness;
      Obj_Texture texture_metallic;
      Obj_Texture texture_normal;
      Obj_Texture texture_sheen;
      Obj_Texture texture_base;
    } pbr;
  };
  Color3      diffuse;
  Obj_Texture texture_diffuse;
  Color3      emissive;
  Obj_Texture texture_emissive;
  f32         transparency, ior;
  u8          illumination_model;
  b8          is_pbr;
} Obj_Material;

typedef struct {
  String path, data;
} Obj_Material_File;

typedef struct {
  Slice(Obj_Triangle)      triangles;
  Slice(Obj_Material)      materials;
  Slice(Obj_Material_File) material_files;
} Obj_File;

extern b8   obj_load(String data, Obj_File *o, b8 load_materials, Allocator allocator);
extern void obj_destroy(Obj_File const *o, Allocator allocator);
