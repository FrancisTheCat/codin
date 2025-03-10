#include "codin.h"

#include "image.h"
#include "linalg.h"
#include "os.h"

#define WIDTH   256
#define HEIGHT  256
#define SAMPLES 16

typedef struct {
  Color3 color;
  Vec3   position;
  Vec3   normal;
  f32    distance;
} Hit;

typedef struct {
  Vec3 position;
  f32  radius;
} Sphere;

typedef struct {
  Vec3 position;
  Vec3 direction;
} Ray;

typedef struct {
  Vec3 position;
  f32  aspect;
} Camera;

Camera camera;

b8 ray_sphere_hit(Ray ray, Sphere sphere, Hit *hit) {
  Vec3 o = vec3_sub(ray.position, sphere.position);

  float a = vec3_dot(ray.direction, ray.direction);
  float b = 2 * vec3_dot(ray.direction, o);
  float c = vec3_dot(o, o) - sphere.radius * sphere.radius;

  float d = b * b - 4 * a * c;

  if (d < 0) {
    return false;
  }

  float d_sqrt   = sqrt_f32(d);
  float distance = (-b - d_sqrt) / (2 * a);

  if (distance > 100 || distance < 0.01) {
    return false;
  }

  hit->distance = distance;

  Vec3 hit_pos  = vec3_scale(ray.direction, distance);
  hit->position = vec3_add(ray.position, hit_pos);

  Vec3 norm     = vec3_sub(hit->position, sphere.position);
  hit->normal   = vec3_scale(norm, 1.0 / sphere.radius);

  return true;
}

Color3 per_pixel(Vec2 uv) {
  Ray ray = {
    .position  = vec3(0, 0, 5),
    .direction = vec3_normalize(vec3(uv.x, uv.y, -1)),
  };
  Sphere sphere = {
    .position = vec3(0, 0, 0),
    .radius   = 1,
  };
  Color3 accumulated_tint = vec3(1, 1, 1);
  Hit hit;
  hit.color = vec3(0.9, 0.9, 0.9);
  for_range(i, 0, 8) {
    if (ray_sphere_hit(ray, sphere, &hit)) {
      accumulated_tint = vec3_mul(accumulated_tint, hit.color);
      ray.position  = hit.position;
      ray.direction = vec3_reflect(ray.direction, hit.normal);
    } else {
      Vec3 sky_color = vec3_lerp(vec3(0.5, 0.7, 1), vec3(1, 1, 1), 0.5 * (ray.direction.y + 1));
      return vec3_mul(sky_color, accumulated_tint);
    }
  }
  return (Color3) {0};
}

f32 hash12(Vec2 p) {
  Vec3 p3 = vec3_fract(vec3_scale(vec3(p.x, p.y, p.x), .1031));
  f32 dot = vec3_dot(p3, vec3_add(vec3(p3.y, p3.z, p3.x), vec3(33.33, 33.33, 33.33)));
  return fract_f32((p3.x + p3.y + dot * 2) * (p3.z + dot));
}

i32 main() {
  Image image = {
    .components = 3,
    .pixel_type = PT_u8,
    .width      = WIDTH,
    .height     = HEIGHT,
  };

  image.pixels = slice_make(Byte_Slice, WIDTH * HEIGHT * 3, context.allocator);

  for_range(y, 0, HEIGHT) {
    for_range(x, 0, WIDTH) {
      Color3 color = vec3(0, 0, 0);

      for_range(sample, 0, SAMPLES) {
        color = vec3_add(color, per_pixel(
          vec2(
            ((f32)x + hash12(vec2((f32)(x + sample * 50), (f32)y)) - 0.5) / WIDTH  - 0.5,
            ((f32)y + hash12(vec2((f32)x, (f32)(y + sample * 50))) - 0.5) / HEIGHT - 0.5
          )
        ));
      }

      IDX(image.pixels, 3 * (x + y * HEIGHT) + 0) = (u8)(color.data[0] / SAMPLES * 255);
      IDX(image.pixels, 3 * (x + y * HEIGHT) + 1) = (u8)(color.data[1] / SAMPLES * 255);
      IDX(image.pixels, 3 * (x + y * HEIGHT) + 2) = (u8)(color.data[2] / SAMPLES * 255);
    }
  }

  file_remove(LIT("output.png"));
  Fd output_file = unwrap_err(file_open(LIT("output.png"), FP_Read_Write | FP_Create | FP_Truncate));
  Writer output_writer = writer_from_handle(output_file);
  assert(png_save_writer(&output_writer, &image));
  file_close(output_file);
}
