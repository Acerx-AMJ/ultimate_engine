#include "render.hpp"
#include "engine.hpp"
#include "math3d.hpp"
#include <raymath.h>
#include <algorithm>

// near plane clipping

void BuildNearPlaneClipping(std::vector<Triangle> &triangles, Vector3 p1, Vector3 p2, Vector3 p3, Vector2 uv1, Vector2 uv2, Vector2 uv3, float lighting, const UETexture *texture) {
   bool p1front = (p1.z >= NearClip);
   bool p2front = (p2.z >= NearClip);
   bool p3front = (p3.z >= NearClip);

   if (p1front && p2front && p3front) {
      triangles.emplace_back(p1, p2, p3, uv1, uv2, uv3, lighting, texture);
      return;
   } else if (!p1front && !p2front && !p3front) {
      return;
   }

   struct Vertex {
      Vector3 p;
      Vector2 uv;
   };

   Vertex v[4];
   int vertexCount = 0;

   auto HandleEdge = [&](Vector3 p1, Vector3 p2, Vector2 uv1, Vector2 uv2, bool f1, bool f2) {
      if (f1) {
         v[vertexCount++] = {p1, uv1};
      }

      if (f1 != f2) {
         float s = (NearClip - p1.z) / (p2.z - p1.z);
         Vector3 p3 = p1 + (p2 - p1) * s;
         Vector2 uv3 = uv1 + (uv2 - uv1) * s;
         v[vertexCount++] = {p3, uv3};
      }
   };

   HandleEdge(p1, p2, uv1, uv2, p1front, p2front);
   HandleEdge(p2, p3, uv2, uv3, p2front, p3front);
   HandleEdge(p3, p1, uv3, uv1, p3front, p1front);

   if (vertexCount == 3) {
      triangles.emplace_back(v[0].p, v[1].p, v[2].p, v[0].uv, v[1].uv, v[2].uv, lighting, texture);
   } else {
      triangles.emplace_back(v[0].p, v[1].p, v[2].p, v[0].uv, v[1].uv, v[2].uv, lighting, texture);
      triangles.emplace_back(v[0].p, v[2].p, v[3].p, v[0].uv, v[2].uv, v[3].uv, lighting, texture);
   }
}

// drawing

void DrawTriangleUV(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float lighting) {
   TriangleBounds box = TriangleBoundingBox(p1, p2, p3);
   if (box.area == 0.0 || box.xmin > box.xmax || box.ymin > box.ymax) return;

   for (int dy = box.ymin; dy <= box.ymax; ++dy) {
      for (int dx = box.xmin; dx <= box.xmax; ++dx) {
         Vector3 d = TriangleEdgeValues(dx, dy, p1, p2, p3);
         if (!InsideTriangle(d.x, d.y, d.z)) continue;
         DepthData depth = TriangleDepth(box.area, d.x, d.y, d.z, uv1.x, uv2.x, uv3.x, dx, dy);
         if (DepthBuffer[depth.di] >= depth.z) continue;
         DepthBuffer[depth.di] = depth.z;
         Vector2 uv = TriangleUV(depth.w1, depth.w2, depth.w3, uv1, uv2, uv3);
         ImageDrawPixel(&Canvas, dx, dy, DrawColor(255.0f * uv.x, 255.0f * uv.y, 0.0f, lighting, depth.z));
      }
   }
}

void DrawTriangleColor(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float lighting, Color color) {
   TriangleBounds box = TriangleBoundingBox(p1, p2, p3);
   if (box.area == 0.0 || box.xmin > box.xmax || box.ymin > box.ymax) return;

   for (int dy = box.ymin; dy <= box.ymax; ++dy) {
      for (int dx = box.xmin; dx <= box.xmax; ++dx) {
         Vector3 d = TriangleEdgeValues(dx, dy, p1, p2, p3);
         if (!InsideTriangle(d.x, d.y, d.z)) continue;
         DepthData depth = TriangleDepth(box.area, d.x, d.y, d.z, uv1.x, uv2.x, uv3.x, dx, dy);
         if (DepthBuffer[depth.di] >= depth.z) continue;
         DepthBuffer[depth.di] = depth.z;
         ImageDrawPixel(&Canvas, dx, dy, DrawColor(color.r, color.g, color.b, lighting, depth.z));
      }
   }
}

void DrawTriangleGradient(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float lighting, Color color1, Color color2, Color color3) {
   TriangleBounds box = TriangleBoundingBox(p1, p2, p3);
   if (box.area == 0.0 || box.xmin > box.xmax || box.ymin > box.ymax) return;

   for (int dy = box.ymin; dy <= box.ymax; ++dy) {
      for (int dx = box.xmin; dx <= box.xmax; ++dx) {
         Vector3 d = TriangleEdgeValues(dx, dy, p1, p2, p3);
         if (!InsideTriangle(d.x, d.y, d.z)) continue;
         DepthData depth = TriangleDepth(box.area, d.x, d.y, d.z, uv1.x, uv2.x, uv3.x, dx, dy);
         if (DepthBuffer[depth.di] >= depth.z) continue;
         DepthBuffer[depth.di] = depth.z;
         Vector2 uv = TriangleUV(depth.w1, depth.w2, depth.w3, uv1, uv2, uv3);
         float w3 = 1.0f - uv.x - uv.y;
         float r = uv.x * color1.r + uv.y * color2.r + w3 * color3.r;
         float g = uv.x * color1.g + uv.y * color2.g + w3 * color3.g;
         float b = uv.x * color1.b + uv.y * color2.b + w3 * color3.b;
         ImageDrawPixel(&Canvas, dx, dy, DrawColor(r, g, b, lighting, depth.z));
      }
   }
}

void DrawTriangleTexture(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float lighting, Image texture) {
   TriangleBounds box = TriangleBoundingBox(p1, p2, p3);
   if (box.area == 0.0 || box.xmin > box.xmax || box.ymin > box.ymax) return;

   for (int dy = box.ymin; dy <= box.ymax; ++dy) {
      for (int dx = box.xmin; dx <= box.xmax; ++dx) {
         Vector3 d = TriangleEdgeValues(dx, dy, p1, p2, p3);
         if (!InsideTriangle(d.x, d.y, d.z)) continue;
         DepthData depth = TriangleDepth(box.area, d.x, d.y, d.z, uv1.x, uv2.x, uv3.x, dx, dy);
         if (DepthBuffer[depth.di] >= depth.z) continue;
         DepthBuffer[depth.di] = depth.z;
         Vector2 uv = TriangleUV(depth.w1, depth.w2, depth.w3, uv1, uv2, uv3);
         TextureCoords t = TriangleUVTextureCoords(uv.x, uv.y, texture.width, texture.height);
         Color pixel = GetImageColor(texture, t.x, t.y);
         ImageDrawPixel(&Canvas, dx, dy, DrawColor(pixel.r, pixel.g, pixel.b, lighting, depth.z));
      }
   }
}

void DrawTriangleCustom(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float lighting, UETextureCustomCallback callback) {
   TriangleBounds box = TriangleBoundingBox(p1, p2, p3);
   if (box.area == 0.0 || box.xmin > box.xmax || box.ymin > box.ymax) return;

   for (int dy = box.ymin; dy <= box.ymax; ++dy) {
      for (int dx = box.xmin; dx <= box.xmax; ++dx) {
         Vector3 d = TriangleEdgeValues(dx, dy, p1, p2, p3);
         if (!InsideTriangle(d.x, d.y, d.z)) continue;
         DepthData depth = TriangleDepth(box.area, d.x, d.y, d.z, uv1.x, uv2.x, uv3.x, dx, dy);
         if (DepthBuffer[depth.di] >= depth.z) continue;
         DepthBuffer[depth.di] = depth.z;
         Vector2 uv = TriangleUV(depth.w1, depth.w2, depth.w3, uv1, uv2, uv3);
         ImageDrawPixel(&Canvas, dx, dy, callback(dx, dy, d, depth, uv, lighting));
      }
   }
}

// frame rendering

void RenderFrame() {
   Vector3 light = Rotate3D(LightDirection, Rotation);
   std::vector<Triangle> triangles;
   std::vector<Vector3> points;

   // build the triangles
   for (const Geometry &shape: GeometryContainer) {
      points.resize(shape.points.size());
      for (size_t i = 0; i < shape.points.size(); ++i) {
         points[i] = Rotate3D(shape.points[i] + Offset, Rotation);
      }

      for (const Tr6 &t: shape.triangles) {
         Vector3 p1 = points[t.x];
         Vector3 p2 = points[t.y];
         Vector3 p3 = points[t.z];

         Vector2 uv1 = shape.uv[t.uv1];
         Vector2 uv2 = shape.uv[t.uv2];
         Vector2 uv3 = shape.uv[t.uv3];

         Vector3 cross = Vector3CrossProduct(p2 - p1, p3 - p1);
         Vector3 normal = Vector3Normalize(cross);
         float dot = Vector3DotProduct(normal, light);
         float lighting = Ambient + (1 - Ambient) * std::max(dot, 0.0f);

         const UETexture *texture = &shape.textures[t.texture];
         if (texture->type == UE_TEXTURE_GRADIENT) {
            uv1 = {1.0f, 0.0f};
            uv2 = {0.0f, 1.0f};
            uv3 = {0.0f, 0.0f};
         }

         BuildNearPlaneClipping(triangles, p1, p2, p3, uv1, uv2, uv3, lighting, texture);
      }
   }

   // clear the buffers
   ImageClearBackground(&Canvas, BLACK);
   std::fill(DepthBuffer.begin(), DepthBuffer.end(), 0.0f);

   // draw triangles
   for (const Triangle &t: triangles) {
      Vector2 s1 = TranslateToScreen(t.p1);
      Vector2 s2 = TranslateToScreen(t.p2);
      Vector2 s3 = TranslateToScreen(t.p3);

      if (TriangleNormalZ(s1, s2, s3) > 0) continue;
      Vector3 uv1 = TriangleUVData(t.p1.z, t.uv1);
      Vector3 uv2 = TriangleUVData(t.p2.z, t.uv2);
      Vector3 uv3 = TriangleUVData(t.p3.z, t.uv3);

      switch (t.texture->type) {
      case UE_TEXTURE_UV: DrawTriangleUV(s1, s2, s3, uv1, uv2, uv3, t.lighting); break;
      case UE_TEXTURE_COLOR: DrawTriangleColor(s1, s2, s3, uv1, uv2, uv3, t.lighting, t.texture->color); break;
      case UE_TEXTURE_GRADIENT: DrawTriangleGradient(s1, s2, s3, uv1, uv2, uv3, t.lighting, t.texture->g1, t.texture->g2, t.texture->g3); break;
      case UE_TEXTURE_TEXTURE: DrawTriangleTexture(s1, s2, s3, uv1, uv2, uv3, t.lighting, t.texture->texture); break;
      case UE_TEXTURE_CUSTOM: DrawTriangleCustom(s1, s2, s3, uv1, uv2, uv3, t.lighting, t.texture->callback); break;
      default: break;
      }
   }
}
