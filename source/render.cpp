#include "render.hpp"
#include "engine.hpp"
#include "math3d.hpp"
#include <raymath.h>
#include <algorithm>

constexpr float InverseColorConversion = 1.0f / 255.0f;

// near plane clipping

struct Vertex {
   Vector3 p;
   Vector2 uv;
};

void BuildNearPlaneClipping(Vector3 p1, Vector3 p2, Vector3 p3, Vector2 uv1, Vector2 uv2, Vector2 uv3, float lighting, const UETexture *texture) {
   Vertex in[3] = {{p1, uv1}, {p2, uv2}, {p3, uv3}};
   Vertex poly[4];
   int count = 0;
 
   for (int i = 0; i < 3; ++i) {
      Vertex a = in[i];
      Vertex b = in[(i + 1) % 3];
      bool aFront = (a.p.z >= NearClip);
      bool bFront = (b.p.z >= NearClip);

      if (aFront) {
         poly[count++] = a;
      }
      if (aFront != bFront) {
         float s = (NearClip - a.p.z) / (b.p.z - a.p.z);
         poly[count++] = {a.p + (b.p - a.p) * s, a.uv + (b.uv - a.uv) * s};
      }
   }
 
   for (int i = 1; i + 1 < count; ++i) {
      RenderTriangles.emplace_back(poly[0].p, poly[i].p, poly[i + 1].p, poly[0].uv, poly[i].uv, poly[i + 1].uv, lighting, texture);
   }
}

// drawing

void DrawTriangleColor(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float lighting, Color color) {
   TriangleBounds box = TriangleBoundingBox(p1, p2, p3);
   if (box.area == 0.0f || color.a == 0) return;
   Vector3 invZ = {uv1.x, uv2.x, uv3.x};
   Vector3 stepX = TriangleEdgeStepX(p1, p2, p3);
   Vector3 stepY = TriangleEdgeStepY(p1, p2, p3);
   Vector3 rowEdges = TriangleEdgeValues(box.xmin, box.ymin, p1, p2, p3);
   Color *pixels = (Color*)Canvas.data;
   float a = color.a * InverseColorConversion;
   float ma = 1.0f - a;

   for (int dy = box.ymin; dy <= box.ymax; ++dy, rowEdges += stepY) {
      Vector3 edges = rowEdges;
      bool entered = false;

      for (int dx = box.xmin; dx <= box.xmax; ++dx, edges += stepX) {
         if (!InsideTriangle(edges)) {
            if (entered) break;
            continue;
         }
         entered = true;

         DepthData depth = TriangleDepth(box.ia, edges, invZ, dx, dy);
         if (DepthBuffer[depth.di] >= depth.z) continue;
         if (color.a != 255) {
            Color dst = pixels[depth.di];
            pixels[depth.di] = DrawColor(color.r * a + dst.r * ma, color.g * a + dst.g * ma, color.b * a + dst.b * ma, 255, lighting, depth.z);
         }
         else {
            DepthBuffer[depth.di] = depth.z;
            pixels[depth.di] = DrawColor(color.r, color.g, color.b, color.a, lighting, depth.z);
         }
      }
   }
}

void DrawTriangleTexture(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float lighting, Image texture) {
   TriangleBounds box = TriangleBoundingBox(p1, p2, p3);
   if (box.area == 0.0f) return;
   Vector3 invZ = {uv1.x, uv2.x, uv3.x};
   Vector3 stepX = TriangleEdgeStepX(p1, p2, p3);
   Vector3 stepY = TriangleEdgeStepY(p1, p2, p3);
   Vector3 rowEdges = TriangleEdgeValues(box.xmin, box.ymin, p1, p2, p3);
   Color *pixels = (Color*)Canvas.data;

   for (int dy = box.ymin; dy <= box.ymax; ++dy, rowEdges += stepY) {
      Vector3 edges = rowEdges;
      bool entered = false;

      for (int dx = box.xmin; dx <= box.xmax; ++dx, edges += stepX) {
         if (!InsideTriangle(edges)) {
            if (entered) break;
            continue;
         }
         entered = true;

         DepthData depth = TriangleDepth(box.ia, edges, invZ, dx, dy);
         if (DepthBuffer[depth.di] >= depth.z) continue;

         Vector2 uv = TriangleUV(depth.z, depth.w1, depth.w2, depth.w3, uv1, uv2, uv3);
         int t = TriangleUVTextureCoords(uv.x, uv.y, texture.width, texture.height);
         Color color = ((const Color*)texture.data)[t];
         if (color.a == 0) continue;

         if (color.a != 255) {
            Color dst = pixels[depth.di];
            float a = color.a * InverseColorConversion;
            float ma = 1.0f - a;
            pixels[depth.di] = DrawColor(color.r * a + dst.r * ma, color.g * a + dst.g * ma, color.b * a + dst.b * ma, 255, lighting, depth.z);
         }
         else {
            DepthBuffer[depth.di] = depth.z;
            pixels[depth.di] = DrawColor(color.r, color.g, color.b, color.a, lighting, depth.z);
         }
      }
   }
}

void DrawTriangleCustom(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float lighting, UETextureShader callback) {
   TriangleBounds box = TriangleBoundingBox(p1, p2, p3);
   if (box.area == 0.0f) return;
   Vector3 invZ = {uv1.x, uv2.x, uv3.x};
   Vector3 stepX = TriangleEdgeStepX(p1, p2, p3);
   Vector3 stepY = TriangleEdgeStepY(p1, p2, p3);
   Vector3 rowEdges = TriangleEdgeValues(box.xmin, box.ymin, p1, p2, p3);
   Color *pixels = (Color*)Canvas.data;

   for (int dy = box.ymin; dy <= box.ymax; ++dy, rowEdges += stepY) {
      Vector3 edges = rowEdges;
      bool entered = false;

      for (int dx = box.xmin; dx <= box.xmax; ++dx, edges += stepX) {
         if (!InsideTriangle(edges)) {
            if (entered) break;
            continue;
         }
         entered = true;

         DepthData depth = TriangleDepth(box.ia, edges, invZ, dx, dy);
         if (DepthBuffer[depth.di] >= depth.z) continue;
         DepthBuffer[depth.di] = depth.z;

         Vector2 uv = TriangleUV(depth.z, depth.w1, depth.w2, depth.w3, uv1, uv2, uv3);
         pixels[depth.di] = callback(dx, dy, depth, uv, lighting);
      }
   }
}

// frame rendering

void RenderFrame() {
   Vector3 light = Rotate3D(LightDirection, Rotation);
   RenderTriangles.clear();

   // build the triangles
   for (const Geometry &shape: GeometryContainer) {
      RenderPoints.resize(shape.points.size());
      for (size_t i = 0; i < shape.points.size(); ++i) {
         RenderPoints[i] = Rotate3D(shape.points[i] + Offset, Rotation);
      }

      for (const TriangleData &t: shape.triangles) {
         Vector3 p1 = RenderPoints[t.x];
         Vector3 p2 = RenderPoints[t.y];
         Vector3 p3 = RenderPoints[t.z];

         Vector2 uv1 = shape.uv[t.uv1];
         Vector2 uv2 = shape.uv[t.uv2];
         Vector2 uv3 = shape.uv[t.uv3];

         Vector3 cross = Vector3CrossProduct(p2 - p1, p3 - p1);
         Vector3 normal = Vector3Normalize(cross);
         float dot = Vector3DotProduct(normal, light);
         float lighting = Ambient + (1 - Ambient) * std::max(dot, 0.0f);

         const UETexture *texture = &shape.textures[t.texture];
         BuildNearPlaneClipping(p1, p2, p3, uv1, uv2, uv3, lighting, texture);
      }
   }

   // clear the buffers
   ImageClearBackground(&Canvas, BLACK);
   std::fill(DepthBuffer.begin(), DepthBuffer.end(), 0.0f);

   // sort nearest first
   RenderTriangleKeys.resize(RenderTriangles.size());
   RenderTriangleOrder.resize(RenderTriangles.size());

   for (size_t i = 0; i < RenderTriangles.size(); ++i) {
      RenderTriangleKeys[i].depth = RenderTriangles[i].p1.z + RenderTriangles[i].p2.z + RenderTriangles[i].p3.z;
      RenderTriangleKeys[i].transparent = RenderTriangles[i].texture->isTransparent;
      RenderTriangleOrder[i] = i;
   }
   std::sort(RenderTriangleOrder.begin(), RenderTriangleOrder.end(), [&](int a, int b) {
      SortData ad = RenderTriangleKeys[a];
      SortData bd = RenderTriangleKeys[b];
      if (ad.transparent != bd.transparent) {
         return !ad.transparent && bd.transparent;
      }
      return ad.depth < bd.depth;
   });
 
   for (int i: RenderTriangleOrder) {
      const Triangle &t = RenderTriangles[i];
      Vector2 s1 = TranslateToScreen(t.p1);
      Vector2 s2 = TranslateToScreen(t.p2);
      Vector2 s3 = TranslateToScreen(t.p3);

      if (TriangleNormalZ(s1, s2, s3) > 0 || TriangleOffScreen(s1, s2, s3)) continue;
      Vector3 uv1 = TriangleUVData(t.p1.z, t.uv1);
      Vector3 uv2 = TriangleUVData(t.p2.z, t.uv2);
      Vector3 uv3 = TriangleUVData(t.p3.z, t.uv3);

      switch (t.texture->type) {
      case UE_TEXTURE_COLOR: DrawTriangleColor(s1, s2, s3, uv1, uv2, uv3, t.lighting, t.texture->color); break;
      case UE_TEXTURE_TEXTURE: DrawTriangleTexture(s1, s2, s3, uv1, uv2, uv3, t.lighting, t.texture->texture); break;
      case UE_TEXTURE_SHADER: DrawTriangleCustom(s1, s2, s3, uv1, uv2, uv3, t.lighting, t.texture->callback); break;
      default: break;
      }
   }
}
