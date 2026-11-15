#pragma once
#include "engine.hpp"
#include <raymath.h>
#include <algorithm>
#include <cmath>

// 3D math

inline Vector2 TranslateToScreen(Vector3 p) {
   float x2d = ((p.x / p.z + 1.0f) / 2.0f) * Width;
   float y2d = (1.0f - ((p.y / p.z + 1.0f) / 2.0f)) * Height;
   return {x2d, y2d};
}

inline Vector3 Rotate3D(Vector3 pos, Vector3 rotation) {
   Vector3 result = pos;

   float sx = sin(rotation.x), cx = cos(rotation.x);
   float sy = sin(rotation.y), cy = cos(rotation.y);
   float sz = sin(rotation.z), cz = cos(rotation.z);

   float x1 = result.x * cy - result.z * sy;
   float z2 = result.x * sy + result.z * cy;
   result.x = x1;
   result.z = z2;

   float y1 = result.y * cx - result.z * sx;
   float z1 = result.y * sx + result.z * cx;
   result.y = y1;
   result.z = z1;

   float x2 = result.x * cz - result.y * sz;
   float y2 = result.x * sz + result.y * cz;
   result.x = x2;
   result.y = y2;

   return result;
}

inline Vector3 Rotate3DAroundOrigin(Vector3 pos, Vector3 origin, Vector3 rotation) {
   if (rotation == Vector3{0.0f, 0.0f, 0.0f}) return pos;
   return Rotate3D(pos - origin, rotation) + origin;
}

inline Vector3 Vec3ZeroFix(Vector3 p1) {
   return {p1.x == 0.0f ? 1.0f : p1.x, p1.y == 0.0f ? 1.0f : p1.y, p1.z == 0.0f ? 1.0f : p1.z};
}

// Triangle math

struct DepthData {
   float w1, w2, w3, z;
   int di;
};

struct TriangleBounds {
   int xmin, xmax, ymin, ymax;
   float area, ia;
};

inline DepthData TriangleDepth(float ia, Vector3 d, Vector3 invZ, int dx, int dy) {
   float w3 = d.x * ia, w1 = d.y * ia, w2 = d.z * ia;
   float z = w1 * invZ.x + w2 * invZ.y + w3 * invZ.z;
   int di = dx + dy * Width;
   return {w1, w2, w3, z, di};
}

inline bool InsideTriangle(Vector3 d) {
   return (d.x <= 0.0f && d.y <= 0.0f && d.z <= 0.0f) || (d.x >= 0.0f && d.y >= 0.0f && d.z >= 0.0f);
}

inline float TriangleNormalZ(Vector2 p1, Vector2 p2, Vector2 p3) {
   float ax = p2.x - p1.x, ay = p2.y - p1.y;
   float bx = p3.x - p1.x, by = p3.y - p1.y;
   return ax * by - ay * bx;
}

inline float TriangleSign(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y) {
   return (p1x - p3x) * (p2y - p3y) - (p2x - p3x) * (p1y - p3y);
}

inline Vector3 TriangleEdgeValues(int dx, int dy, Vector2 p1, Vector2 p2, Vector2 p3) {
   float x = dx + 0.5f;
   float y = dy + 0.5f;
   float d1 = TriangleSign(x, y, p1.x, p1.y, p2.x, p2.y);
   float d2 = TriangleSign(x, y, p2.x, p2.y, p3.x, p3.y);
   float d3 = TriangleSign(x, y, p3.x, p3.y, p1.x, p1.y);
   return {d1, d2, d3};
}

inline Vector3 TriangleEdgeStepX(Vector2 p1, Vector2 p2, Vector2 p3) {
   return {p1.y - p2.y, p2.y - p3.y, p3.y - p1.y};
}

inline Vector3 TriangleEdgeStepY(Vector2 p1, Vector2 p2, Vector2 p3) {
   return {p2.x - p1.x, p3.x - p2.x, p1.x - p3.x};
}
 
inline bool TriangleOffScreen(Vector2 p1, Vector2 p2, Vector2 p3) {
   return std::max({p1.x, p2.x, p3.x}) < 0.0f || std::min({p1.x, p2.x, p3.x}) >= Width
       || std::max({p1.y, p2.y, p3.y}) < 0.0f || std::min({p1.y, p2.y, p3.y}) >= Height;
}

inline float TriangleArea(Vector2 p1, Vector2 p2, Vector2 p3) {
   return p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y);
}

inline TriangleBounds TriangleBoundingBox(Vector2 p1, Vector2 p2, Vector2 p3) {
   int xmin = std::clamp(std::floor(std::min({p1.x, p2.x, p3.x})), 0.0f, Width - 1.0f);
   int xmax = std::clamp(std::ceil(std::max({p1.x, p2.x, p3.x})), 0.0f, Width - 1.0f);
   int ymin = std::clamp(std::floor(std::min({p1.y, p2.y, p3.y})), 0.0f, Height - 1.0f);
   int ymax = std::clamp(std::ceil(std::max({p1.y, p2.y, p3.y})), 0.0f, Height - 1.0f);
   float area = TriangleArea(p1, p2, p3);
   return {xmin, xmax, ymin, ymax, area, 1.0f / area};
}

inline Vector3 TriangleUVData(float z, Vector2 uv) {
   return {1.0f / z, uv.x / z, uv.y / z};
}

inline Vector2 TriangleUV(float z, float w1, float w2, float w3, Vector3 uv1, Vector3 uv2, Vector3 uv3) {
   float r = w1 * uv1.y + w2 * uv2.y + w3 * uv3.y;
   float g = w1 * uv1.z + w2 * uv2.z + w3 * uv3.z;
   return {r / z, g / z};
}

inline int TriangleUVTextureCoords(float u, float v, int twidth, int theight) {
   int tx = std::clamp<int>(u * twidth, 0, twidth - 1);
   int ty = std::clamp<int>(theight - (v * theight), 0, theight - 1);
   return ty * twidth + tx;
}

// Misc. math

inline float Fog(float z) {
   return 1.0f - std::clamp((1.0f / z - FogStart) / (FogEnd - FogStart), 0.0f, 1.0f);
}

inline unsigned char ToByte(float v) {
   return (v < 0.0f ? 0 : v >= 255.0f ? 255 : (unsigned char)v);
}

inline Color DrawColor(float r, float g, float b, float a, float lighting, float z) {
   float fog = Fog(z);
   return {ToByte(r * fog * lighting), ToByte(g * fog * lighting), ToByte(b * fog * lighting), ToByte(a)};
}
