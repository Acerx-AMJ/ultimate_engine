#include "engine.hpp"
#include "math3d.hpp"

void InitEngine() {
   Canvas = GenImageColor(Width, Height, BLACK);
   CanvasTexture = LoadTextureFromImage(Canvas);
   SetTextureFilter(CanvasTexture, TEXTURE_FILTER_POINT);
   DepthBuffer.resize(Width * Height, 0.0f);
}

void CloseEngine() {
   UnloadTexture(CanvasTexture);
   UnloadImage(Canvas);
}

void UpdateEngine() {
   float dt = GetFrameTime();
   Vector2 mouse = GetMouseDelta();
   Timer += dt;
   if (!FreeCamera) return;

   if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Rotation.x = std::clamp(Rotation.x - mouse.y * 0.01f, -PI / 2.0f, PI / 2.0f);
      Rotation.y = Rotation.y + mouse.x * 0.01f;
   }

   float speed = ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) + 1) * dt;
   Vector3 move = {
      (IsKeyDown(KEY_A) - IsKeyDown(KEY_D)) * speed,
      (IsKeyDown(KEY_Q) - IsKeyDown(KEY_E)) * speed,
      (IsKeyDown(KEY_S) - IsKeyDown(KEY_W)) * speed,
   };
   Offset += Rotate3D(move, {0.0f, -Rotation.y, -Rotation.z});
}

// near plane clipping

struct Vertex {
   Vector3 p;
   Vector2 uv;
};

void BuildNearPlaneClipping(Vector3 p1, Vector3 p2, Vector3 p3, Vector2 uv1, Vector2 uv2, Vector2 uv3, float lighting, const UETexture *texture, int shader) {
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
      RenderTriangles.emplace_back(poly[0].p, poly[i].p, poly[i + 1].p, poly[0].uv, poly[i].uv, poly[i + 1].uv, lighting, texture, shader);
   }
}

// rendering

constexpr float InverseColorConversion = 1.0f / 255.0f;

template <typename SurfaceAt>
void RasterizeTriangle(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float lighting, SurfaceAt surfaceAt) {
   TriangleBounds box = TriangleBoundingBox(p1, p2, p3);
   if (box.area == 0.0f) return;
   Vector3 invZ = {uv1.x, uv2.x, uv3.x};
   Vector3 stepX = TriangleEdgeStepX(p1, p2, p3);
   Vector3 stepY = TriangleEdgeStepY(p1, p2, p3);
   Vector3 rowEdges = TriangleEdgeValues(box.xmin, box.ymin, p1, p2, p3);
   Color *pixels = (Color*)Canvas.data;
   float fogScale = 1.0f / (FogEnd - FogStart);

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

         Color color = surfaceAt(dx, dy, depth);
         if (color.a == 0) continue;
         float fog = 1.0f - std::clamp((1.0f / depth.z - FogStart) * fogScale, 0.0f, 1.0f);
         Color src = {ToByte(color.r * fog * lighting), ToByte(color.g * fog * lighting), ToByte(color.b * fog * lighting), color.a};

         if (color.a == 255) {
            DepthBuffer[depth.di] = depth.z;
            pixels[depth.di] = src;
         }
         else {
            Color dst = pixels[depth.di];
            float a = color.a * InverseColorConversion;
            float ma = 1.0f - a;
            pixels[depth.di] = {ToByte(src.r * a + dst.r * ma), ToByte(src.g * a + dst.g * ma), ToByte(src.b * a + dst.b * ma), 255};
         }
      }
   }
}

Color SampleTexture(const UETexture &texture, Vector2 uv) {
   switch (texture.type) {
   case UE_TEXTURE_COLOR: return texture.color;
   case UE_TEXTURE_TEXTURE: return SampleImage(texture.texture, uv);
   default: return WHITE;
   }
}

void BeginRender() {
   RenderTriangles.clear();
   Light = Rotate3D(LightDirection, Rotation);
   RenderShaders.clear();
   CurrentShader = -1;
}

void EndRender() {
   // clear the buffers
   ImageClearBackground(&Canvas, BLACK);
   std::fill(DepthBuffer.begin(), DepthBuffer.end(), 0.0f);

   // sort triangles
   RenderTriangleKeys.resize(RenderTriangles.size());
   RenderTriangleOrder.resize(RenderTriangles.size());

   for (size_t i = 0; i < RenderTriangles.size(); ++i) {
      const Triangle &t = RenderTriangles[i];
      RenderTriangleKeys[i].depth = t.p1.z + t.p2.z + t.p3.z;
      RenderTriangleKeys[i].transparent = t.texture->isTransparent || (t.shader >= 0 && RenderShaders[t.shader].transparent);
      RenderTriangleOrder[i] = i;
   }
   std::sort(RenderTriangleOrder.begin(), RenderTriangleOrder.end(), [&](int a, int b) {
      SortData ad = RenderTriangleKeys[a];
      SortData bd = RenderTriangleKeys[b];
      if (ad.transparent != bd.transparent) {
         return !ad.transparent && bd.transparent;
      }
      if (ad.transparent) {
         return bd.depth < ad.depth;
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

      const UEShader *shader = (t.shader >= 0 ? &RenderShaders[t.shader] : nullptr);
      if (shader && shader->fragment) {
         RasterizeTriangle(s1, s2, s3, uv1, uv2, uv3, t.lighting, [&](int x, int y, const DepthData &d){
            FragmentInput input = {x, y, d, TriangleUV(d.z, d.w1, d.w2, d.w3, uv1, uv2, uv3), PixelPosition(x, y, d.z), t.texture};
            return shader->fragment(input, shader->data);
         });
         continue;
      }

      switch (t.texture->type) {
      case UE_TEXTURE_COLOR:
         RasterizeTriangle(s1, s2, s3, uv1, uv2, uv3, t.lighting, [color = t.texture->color](int, int, const DepthData&) { return color; });
         break;
      case UE_TEXTURE_TEXTURE:
         RasterizeTriangle(s1, s2, s3, uv1, uv2, uv3, t.lighting, [&](int, int, const DepthData &d) { return SampleImage(t.texture->texture, TriangleUV(d.z, d.w1, d.w2, d.w3, uv1, uv2, uv3)); });
         break;
      default: break;
      }
   }

   // show it
   UpdateTexture(CanvasTexture, Canvas.data);

   BeginDrawing();
   ClearBackground(BLACK);
   DrawTexturePro(CanvasTexture, {0, 0, (float)Width, (float)Height}, {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, {0, 0}, 0, WHITE);
   if (ShowFPS) DrawFPS(0, 0);
   EndDrawing();
}

void BeginShader(const UEShader &shader) {
   CurrentShader = RenderShaders.size();
   RenderShaders.push_back(shader);
}

void EndShader() {
   CurrentShader = -1;
}

void RenderGeometry(const Geometry &geometry) {
   const UEShader *shader = (CurrentShader >= 0 ? &RenderShaders[CurrentShader] : nullptr);

   // apply vertex shader
   RenderPoints.resize(geometry.points.size());
   for (size_t i = 0; i < geometry.points.size(); ++i) {
      Vector3 point = geometry.points[i];
      if (shader && shader->vertex) point = shader->vertex(point, i, shader->data);
      RenderPoints[i] = Rotate3D(point + Offset, Rotation);
   }

   // build the triangles
   for (const TriangleData &t: geometry.triangles) {
      Vector3 p1 = RenderPoints[t.x];
      Vector3 p2 = RenderPoints[t.y];
      Vector3 p3 = RenderPoints[t.z];

      Vector2 uv1 = geometry.uv[t.uv1];
      Vector2 uv2 = geometry.uv[t.uv2];
      Vector2 uv3 = geometry.uv[t.uv3];

      Vector3 cross = Vector3CrossProduct(p2 - p1, p3 - p1);
      Vector3 normal = Vector3Normalize(cross);
      float dot = Vector3DotProduct(normal, Light);
      float lighting = Ambient + (1 - Ambient) * std::max(dot, 0.0f);

      const UETexture *texture = &geometry.textures[t.texture];
      BuildNearPlaneClipping(p1, p2, p3, uv1, uv2, uv3, lighting, texture, CurrentShader);
   }
}
