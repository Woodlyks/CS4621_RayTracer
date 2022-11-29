#include "all.h"
using namespace std;

color ray_color(ray &r, color background, hittable &world, int depth)
{
  hit_record rec;

  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0)
    return color(0, 0, 0);

  // If the ray hits nothing, return the background color.
  if (!world.hit(r, 0.001, infinity, rec))
    return background;

  ray scattered;
  color attenuation;
  color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

  if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    return emitted;

  return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

int main()
{
  // Image
  const double aspect_ratio = 16.0 / 9.0;
  const int image_width = 1080;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 50;
  const int max_depth = 50;
  srand(time(NULL));

  hittable_list world = GET_SCENE_1();
  camera cam(point3(13, 2, 4), point3(0, 0, 0), vec3(0, 1, 0), 50,
             aspect_ratio, 0, 20);

  freopen("random.ppm", "w", stdout);
  // Render
  cout << "P3\n"
       << image_width << ' ' << image_height << "\n255\n";

  for (int j = image_height - 1; j >= 0; --j)
  {
    cerr << "\rScanlines remaining: " << j << ' ' << flush;
    for (int i = 0; i < image_width; ++i)
    {
      color pixel_color(0, 0, 0);
      for (int k = 0; k < samples_per_pixel; k++)
      {
        double u = (i + rand_unit()) / (image_width - 1);
        double v = (j + rand_unit()) / (image_height - 1);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, color(0.4, 0.4, 0.4), world, max_depth);
      }
      // pixel_color =
      write_color(cout, pixel_color, samples_per_pixel);
    }
  }

  cerr << "\nDone.\n";
  return 0;
}
