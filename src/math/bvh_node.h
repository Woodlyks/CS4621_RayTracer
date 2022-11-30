#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "../render/material.h"
#include "hittablelist.h"
#include "aabb.h"
#include "vec3.h"
#include <cmath>
#include <memory>
#include <algorithm>
#include "ray.h"
#include "hittable.h"

using std::make_shared;
using std::shared_ptr;

class bvh_node
{
public:
  bvh_node *left;
  bvh_node *right;
  aabb bbox;
  bool primitive;
  vector<hittable> surf;
  bvh_node();
  bvh_node(std::vector<shared_ptr<hittable>> &src_bojs, size_t start, size_t end, int idx);
  bool hit(ray r, double t_min, double t_max, hit_record &rec);
  bool bounding_box(double t_min, double t_max, aabb &output_box);
};

bool bvh_node::bounding_box(double t_min, double t_max, aabb &output_box)
{
  output_box = bbox;
  return true;
}

bool bvh_node::hit(ray r, double t_min, double t_max, hit_record &rec)
{
  if (!bbox.hit(r, t_min, t_max))
  {
    return false;
  }
  bool lhit = left->hit(r, t_min, t_max, rec);
  bool rhit = right->hit(r, t_min, t_max, rec);
  return lhit || rhit;
}

bvh_node ::bvh_node()
{
  vector<hittable> v;
  bbox = aabb();
  left = NULL;
  right = NULL;
  primitive = false;
  surf = v;
}
bool bbox_helper(aabb &bbox1, aabb &bbox2, aabb &bbox3)
{
  point3 v1 = vec3(std::min(bbox1.minimum.x(), bbox2.minimum.x()), std::min(bbox1.minimum.y(), bbox2.minimum.y()), std::min(bbox1.minimum.z(), bbox2.minimum.z()));
  point3 v2 = vec3(std::max(bbox1.maximum.x(), bbox2.maximum.x()), std::max(bbox1.maximum.y(), bbox2.maximum.y()), std::max(bbox1.maximum.z(), bbox2.maximum.z()));

  bbox3 = aabb(v1, v2);
  return true;
}

bool newcompare(shared_ptr<hittable> &a, shared_ptr<hittable> &b, int idx)
{
  aabb bbox1 = aabb();
  aabb bbox2 = aabb();
  a->bounding_box(bbox1);
  b->bounding_box(bbox2);
  return (bbox1.minimum[idx] < bbox2.minimum[idx]);
}

bool xcomp(shared_ptr<hittable> &a, shared_ptr<hittable> &b)
{
  newcompare(a, b, 0);
}
bool ycomp(shared_ptr<hittable> &a, shared_ptr<hittable> &b)
{
  newcompare(a, b, 1);
}
bool zcomp(shared_ptr<hittable> &a, shared_ptr<hittable> &b)
{
  newcompare(a, b, 2);
}

std::vector<hittable> join(std::vector<hittable> x, std::vector<hittable> y)
{
  std::vector<hittable> vec = x;
  for (int i = 0; i < y.size(); i++)
  {
    vec.push_back(y[i]);
  }
  // vec.reserve(x.size() + y.size());
  // vec.insert(vec.end(), x.begin(), x.end());
  // vec.insert(vec.end(), y.begin(), y.end());
  return vec;
}

bvh_node::bvh_node(std::vector<shared_ptr<hittable>> &src_objects, size_t start, size_t end, int idx)
{
  std::vector<shared_ptr<hittable>> &lst = src_objects;
  int lstsize = end - start;
  if (lstsize == 0)
  {
    bvh_node();
  }
  else if (lstsize == 1)
  {
    vector<hittable> v;
    bbox = aabb();
    lst[start]->bounding_box(bbox);
    v.push_back(*lst[start]);
    surf = v;
    primitive = true;
    left = NULL;
    right = NULL;
  }
  else if (lstsize == 2)
  {
    left = new bvh_node(lst, start, end - 1, 0);
    right = new bvh_node(lst, start + 1, end, 0);
    bbox = aabb();
    bbox_helper(left->bbox, right->bbox, bbox);
    primitive = false;
    surf = left->surf + right->surf;
  }
  else
  {
    size_t mid = (start + end) / 2;
    bbox = aabb();
    if (idx == 0)
    {
      sort(lst.begin() + start, lst.begin() + end, xcomp);
    }
    else if (idx == 1)
    {
      sort(lst.begin() + start, lst.begin() + end, ycomp);
    }
    else if (idx == 2)
    {
      sort(lst.begin() + start, lst.begin() + end, zcomp);
    }
    left = new bvh_node(lst, start, mid, ((idx + 1) % 3));
    right = new bvh_node(lst, mid, end, ((idx + 1) % 3));
    bbox = aabb();
    bbox_helper(left->bbox, right->bbox, bbox);
    primitive = false;
    surf = join(left->surf, right->surf);
  }
}
// // bool bvh_node ::hit(ray r, float t_min, float t_max)
// // {
// //   if (not bbox.hit(r, t_min, t_max))
// //   {
// //     return false;
// //   }

// //   bool rb = right->bbox.hit(r, t_min, t_max);
// //   bool lb = left->bbox.hit(r, t_min, t_max);
// //   return rb || lb;
// // }

#endif