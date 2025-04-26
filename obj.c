#include "obj.h"

#include "strings.h"
#include "strconv.h"
#include "os.h"

typedef struct {
  i32 vertices  [3];
  i32 normals   [3];
  i32 tex_coords[3];
  i32 material_id;
} Obj_Face;

#define expect_prefix(PREFIX)                                                 \
  if (!string_has_prefix(line, LIT(PREFIX)) || line.len == LIT(PREFIX).len) { \
    continue;                                                                 \
  }                                                                           \
  line = slice_start(line, LIT(PREFIX).len);

internal void parse_texture(String line, Obj_Texture *texture) {
  line = string_trim_whitespace(line);

  *texture = (Obj_Texture) {0};
  for (isize i = line.len - 1; i >= 0; i -= 1) {
    if (rune_is_whitespace(IDX(line, i))) {
      texture->path = slice_start(line, i + 1);
      return;
    }
  }
}

internal void parse_color(String *line, Color3 *color) {
  for_range(i, 0, 3) {
    *line = string_trim_whitespace(*line);
    color->data[i] = parse_float(line);
  }
}

internal void parse_material_file(String data, Obj_File *o) {
  Obj_Material *mtl = nil;
  string_lines_iterator(data, line, _i, {
    line = string_trim_whitespace(line);
    if (string_has_prefix(line, LIT("newmtl "))) {
      line = slice_start(line, LIT("newmtl ").len);
      slice_iter(o->materials, material, i, {
        if (string_equal(material->name, line)) {
          mtl = material;
          break;
        }
      });
    }

    if (!mtl || line.len <= 0) {
      continue;
    }

    switch (IDX(line, 0)) {
    CASE 'K':
      expect_prefix("K");
      switch (IDX(line, 0)) {
      CASE 'a':
        line = slice_start(line, 1);
        parse_color(&line, &mtl->simple.ambient);
      CASE 'd':
        line = slice_start(line, 1);
        parse_color(&line, &mtl->diffuse);
      CASE 's':
        line = slice_start(line, 1);
        parse_color(&line, &mtl->simple.specular);
      CASE 'e':
        line = slice_start(line, 1);
        parse_color(&line, &mtl->emissive);
      }
    CASE 'P':
      mtl->is_pbr = true;
      expect_prefix("P");
      switch (IDX(line, 0)) {
      CASE 'r':
        line = slice_start(line, 1);
        line = string_trim_whitespace(line);
        mtl->pbr.roughness = parse_float(&line);
      CASE 'm':
        line = slice_start(line, 1);
        line = string_trim_whitespace(line);
        mtl->pbr.metallic = parse_float(&line);
      CASE 's':
        line = slice_start(line, 1);
        line = string_trim_whitespace(line);
        mtl->pbr.sheen = parse_float(&line);
      CASE 'c':
        line = slice_start(line, 1);
        if (IDX(line, 0) == 'r') {
          line = slice_start(line, 1);
          line = string_trim_whitespace(line);
          mtl->pbr.clearcoat_roughness = parse_float(&line);
        } else {
          line = string_trim_whitespace(line);
          mtl->pbr.clearcoat = parse_float(&line);
        }
      }
    CASE 'd':
      expect_prefix("d ");
      mtl->transparency = 1.0f - parse_float(&line);
    CASE 'i':
      expect_prefix("illum ");
      mtl->illumination_model = parse_int(&line);
    CASE 'a':
      expect_prefix("aniso ");
      mtl->is_pbr = true;
      mtl->pbr.anisotropic = parse_float(&line);
    CASE 'm':
      expect_prefix("map_");
      if (string_has_prefix(line, LIT("Ka"))) {
        parse_texture(line, &mtl->simple.texture_ambient);
      } else if (string_has_prefix(line, LIT("Kd"))) {
        parse_texture(line, &mtl->texture_diffuse);
      } else if (string_has_prefix(line, LIT("d"))) {
        parse_texture(line, &mtl->simple.texture_alpha);
      } else if (string_has_prefix(line, LIT("Ns"))) {
        parse_texture(line, &mtl->simple.texture_specular);
      } else if (string_has_prefix(line, LIT("Ks"))) {
        parse_texture(line, &mtl->simple.texture_specular_color);
      } else if (string_has_prefix(line, LIT("Ke"))) {
        parse_texture(line, &mtl->texture_emissive);
      } else if (string_has_prefix(line, LIT("Pr"))) {
        mtl->is_pbr = true;
        parse_texture(line, &mtl->pbr.texture_roughness);
      } else if (string_has_prefix(line, LIT("Pm"))) {
        mtl->is_pbr = true;
        parse_texture(line, &mtl->pbr.texture_metallic);
      } else if (string_has_prefix(line, LIT("Ps"))) {
        mtl->is_pbr = true;
        parse_texture(line, &mtl->pbr.texture_sheen);
      } else if (string_has_prefix(line, LIT("bump"))) {
        parse_texture(line, &mtl->simple.texture_bump);
      }
    CASE 'n':
      expect_prefix("norm ");
      mtl->is_pbr = true;
      parse_texture(line, &mtl->pbr.texture_normal);
    CASE 'b':
      expect_prefix("bump ");
      parse_texture(line, &mtl->simple.texture_bump);
    CASE 'N':
      if (string_has_prefix(line, LIT("Ni "))) {
        expect_prefix("Ni ");
        mtl->ior = parse_float(&line);
      } else {
        expect_prefix("Ns ");
        mtl->simple.specular_exponent = parse_float(&line);
      }
    }
  });
}

extern bool obj_load(String data, Obj_File *o, bool load_materials, Allocator allocator) {
  *o = (Obj_File) {0};

  Vector(Vec3)         positions, normals;
  Vector(Vec2)         tex_coords;
  Vector(Obj_Face)     faces;
  Vector(String)       material_names, material_files;

  vector_init(&positions,      0, 8, allocator);
  vector_init(&normals,        0, 8, allocator);
  vector_init(&tex_coords,     0, 8, allocator);
  vector_init(&faces,          0, 8, allocator);
  vector_init(&material_names, 0, 8, allocator);
  vector_init(&material_files, 0, 8, allocator);

  i32 current_material_id = -1;

  string_lines_iterator(data, line, _i, {
    line = string_trim_whitespace(line);
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

      Obj_Face face;
      face.material_id = current_material_id;
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
    CASE 'u':
      if (load_materials && string_has_prefix(line, LIT("usemtl"))) {
        line = slice_start(line, LIT("usemtl").len);
        line = string_trim_whitespace(line);

        slice_iter_v(material_names, name, i, {
          if (string_equal(name, line)) {
            current_material_id = i;
            goto material_found;
          }
        });

        vector_append(&material_names, line);
        current_material_id += 1;
        material_found:
      } else {
        continue;
      }
    CASE 'm':
      if (load_materials && string_has_prefix(line, LIT("mtllib"))) {
        line = slice_start(line, LIT("mtllib").len);
        line = string_trim_whitespace(line);

        vector_append(&material_files, line);
      } else {
        continue;
      }
    DEFAULT:
      continue;
    }
  });

  slice_init(&o->triangles, faces.len, allocator);

  slice_iter_v(faces, face, i, {
    Obj_Triangle t = { .material = face.material_id, };
    for_range(j, 0, 3) {
      t.vertices[j] = (Obj_Vertex) {
        .position   = IDX(positions,  face.vertices  [j] - 1),
        .normal     = IDX(normals,    face.normals   [j] - 1),
        .tex_coords = IDX(tex_coords, face.tex_coords[j] - 1),
      };
    }
    IDX(o->triangles, i) = t;
  });

  if (load_materials) {
    slice_init(&o->material_files, material_files.len, allocator);
    slice_init(&o->materials,      material_names.len, allocator);

    slice_iter(o->materials, material, i, {
      material->name = IDX(material_names, i);
    })

    slice_iter(o->material_files, file, i, {
      file->path = IDX(material_files, i);
      file->data = bytes_to_string(or_else_err(read_entire_file_path(IDX(material_files, i), allocator), (Byte_Slice) {0}));
      parse_material_file(file->data, o);
    });
  }

  vector_delete(positions     );
  vector_delete(normals       );
  vector_delete(tex_coords    );
  vector_delete(faces         );
  vector_delete(material_names);
  
  return true;
}

extern void obj_destroy(Obj_File const *o, Allocator allocator) {
  slice_delete(o->triangles,      allocator);
  slice_delete(o->materials,      allocator);
  slice_iter_v(o->material_files, file, _i, {
    slice_delete(file.data, allocator);
  });
  slice_delete(o->material_files, allocator);
}
