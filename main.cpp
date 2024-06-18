#include <iostream>
#include <cmath>
#include <fstream>
#include <format>

struct Vec {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;

  Vec operator+(const Vec &other) const {
    return Vec{x + other.x, y + other.y, z + other.z};
  }
  Vec operator-(const Vec &other) const {
    return Vec{x - other.x, y - other.y, z - other.z};
  }
  Vec operator*(const float &scale) const {
    return Vec{x * scale, y * scale, z * scale};
  }
  float dot(const Vec &other) const {
    return x * other.x + y * other.y + z * other.z;
  }
  Vec cross(const Vec& other) const {
    return Vec{
      y * other.z - z * other.y,
      z * other.x - x * other.z,
      x * other.y - y * other.x};
  }
  float magnitude() const {
    return sqrtf(x * x + y * y + z * z);
  }
  Vec normalized() const {
    float mag = magnitude();
    return Vec{
      x / mag,
      y / mag,
      z / mag};
  }

};

struct Ray {
  Vec origin = Vec{};
  Vec direction = Vec{};

  Vec ray_at(float t) const
  {
    return (this->origin + (this->direction * t));
  }
};


void vec_write_color(std::ofstream& outfile, const Vec& color)
{
  int ir = static_cast<int>(255.999 * color.x); // Use 255.999 for better rounding
  int ig = static_cast<int>(255.999 * color.y);
  int ib = static_cast<int>(255.999 * color.z);


  outfile << ir << ' ' << ig << ' ' << ib << '\n';
}


/**
 * sphere equation
 *
 * (x - cx)^2 + (y - cy)^2 + (z - cz)^2 - r^2 = 0
 *
 *
 * parametric ray
 *
 * P(t) = o + td
 *
 *
 * solve t : ray intersects sphere
 *
 * (P - C) . (P - C) = (x - cx)^2 + (y - cy)^2 + (z - cz)^2 = r^2
 *
 * ((o + td) - C) . ((o + td) - C) - r^2 = 0
 *
 * (td + (o - c)) . (td + (o - c)) - r^2 = 0
 *
 *
 * (t^2 * d . d) + (2 * t * d . (o - c)) + ((o - c) . (o - c)) - r^2 = 0
 *
 * a = d . d
 * b = 2 * d . (o - c)
 * c = ((o - c) . (o - c)) - r^2
 */
float sphere(Vec center, float radius, Ray ray)
{

  Vec o = ray.origin;
  Vec d = ray.direction;

  Vec oc = o  - center;

  float a = (d.dot(d));
  float b = 2 * d.dot(oc);
  float c = oc.dot(oc) - radius * radius;

  float dc = b * b - 4 * a * c;

  if (dc < 0) {
    return -1;
  }

  return (-b + sqrtf(dc)) / 2 * a;
}

 Vec ray_color(Ray r)
{
  float t = sphere(Vec {0, 0, -3}, 1, r);

  if (t > 0) {
    Vec n = ((r.ray_at(t)) + Vec { 0, 0, 5 }).normalized();
    return Vec { n.x + 1, n.y + 1, n.z + 1 } * 0.5;
  }

  return Vec { 0.2, 0.2, 0.2 };
}



int main()
{
  float aspect = 4.0f / 3.0f;

  int ih = 480;
  int iw = aspect * ih;

  float vh = 2.0;
  float vw = vh * iw / ih;

  Vec eye = { 0, 0,  0 };
  Vec foc = { 0, 0, -1 };

  Vec vu = { vw,   0, 0 };
  Vec vv = {  0, -vh, 0 };

  Vec pu = vu*( 1.0f / iw);
  Vec pv = vv*( 1.0f / ih);

  Vec p0 = (
      ((eye + foc) + ((vu* -0.5) + (vv * -0.5)))+
      ((pu+ pv)* 0.5)
  );


  std::string filename = "image.ppm";  // Set your desired filename
  std::ofstream outfile(filename);
  outfile << std::format("P3\n640 480\n255\n");


  for (int j = 0; j < ih; ++j) {
    for (int i = 0; i < iw; ++i) {
      Vec p = (p0 + ((pu* i) + (pv* j)));
      Vec d = p - eye;
      Ray r = { p, d };

      vec_write_color(outfile,ray_color(r));
    }
  }

}
