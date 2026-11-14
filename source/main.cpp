#include "engine.hpp"
#include "math3d.hpp"

int main() {
   InitWindow(800, 800, "Ultimate Engine");
   SetTraceLogLevel(LOG_ERROR);
   InitEngine();

   // insert geometry here
   UETexture floor5 = LoadImage("assets/industrial_floor5.png");
   UETexture floor9 = LoadImage("assets/industrial_floor9.png");
   UETexture wall8 = LoadImage("assets/industrial_wall8.png");
   UETexture tile = LoadImage("assets/tile.png");
   UETexture rainbow = LoadImage("assets/rainbow.png");
   UETexture bricks = LoadImage("assets/bricks.png");

   GeometryContainer.push_back(NewCube({3.25, 0.25, 0}, {1, 1, 1}, {0, 0, 0}, rainbow));
   GeometryContainer.push_back(NewCube({3, 0.5, 2}, {1.5, 1.5, 1.5}, {0, 0, 0}, bricks));
   GeometryContainer.push_back(NewWedge({3, 0, -2}, {1, 1, 1}, {0, -PI / 2, 0}, tile));
   GeometryContainer.push_back(NewPyramid({-3, 0, -1}, {1.5, 1.5, 1.5}, {0, 0, 0}, floor5));
   GeometryContainer.push_back(NewFace({-3, 0, 1.5f}, {1.0f, 1.0f, 0}, {0, PI / 2, 0}, {UETexture(UE_TEXTURE_UV)}));
   GeometryContainer.push_back(NewFace({-3, 0.0f, 3.0f}, {1.0f, 1.0f, 0}, {0, PI / 2, 0}, {[](int x, int y, Vector3 edges, DepthData depth, Vector2 uv, float lighting) -> Color {
        float wave1 = 0.50f + 0.18f * std::sin(uv.x * 18.0f - Timer * 2.0f);
        float wave2 = 0.50f + 0.14f * std::sin(uv.x * 32.0f + Timer * 1.4f);
        float wave3 = wave1 + wave2 - 0.5f;
        return DrawColor(
            255.0f * std::exp(-std::abs(uv.y - wave1) * 45.0f) + 255.0f * (std::abs(uv.y - 0.5f) < 0.004f),
            255.0f * std::exp(-std::abs(uv.y - wave2) * 45.0f) + 255.0f * (std::abs(uv.y - 0.5f) < 0.004f),
            255.0f * std::exp(-std::abs(uv.y - wave3) * 45.0f) + 255.0f * (std::abs(uv.y - 0.5f) < 0.004f),
            lighting,
            depth.z
        );
   }}));
   GeometryContainer.push_back(NewFace({-3, 0, 4.5f}, {1.0f, 1.0f, 0}, {0, PI / 2, 0}, {[](int x, int y, Vector3 edges, DepthData depth, Vector2 uv, float lighting) -> Color {
      float t = uv.x - 0.5f;
      float f1 = 0.5f - sin(t * 20.0f + Timer * 3.0f) * 0.25f;
      float f2 = 10.0f * t * t;
      float f3 = 0.5f + (t == 0.0f ? 0.0f : 0.015f / t);
      return DrawColor(
         255.0f / (std::abs(uv.y - f1) * 30.0f),
         255.0f / (std::abs(uv.y - f2) * 30.0f),
         255.0f / (std::abs(uv.y - f3) * 30.0f),
         lighting,
         depth.z
      );
   }}));
   GeometryContainer.push_back(NewFace({-3, 0.0f, 6.0f}, {1.0f, 1.0f, 0}, {0, PI / 2, 0}, {[](int x, int y, Vector3 edges, DepthData depth, Vector2 uv, float lighting) -> Color {
        float px = (uv.x - 0.5f) * 8.0f;
        float py = (uv.y - 0.5f) * 8.0f;
        float warpX = px + 1.2f * std::sin(py * 1.7f + Timer);
        float warpY = py + 1.2f * std::cos(px * 1.5f - Timer * 0.8f);
        float pattern = std::sin(warpX * 2.0f + std::sin(warpY * 2.5f + Timer));
        pattern += std::cos(warpY * 2.0f + std::sin(warpX * 2.0f - Timer));
        pattern *= 0.5f;
        float red = 127.5f + 127.5f * std::sin(pattern * 3.0f + Timer);
        float green = 127.5f + 127.5f * std::sin(pattern * 3.0f + Timer + 2.1f);
        float blue = 127.5f + 127.5f * std::sin(pattern * 3.0f + Timer + 4.2f);
        return DrawColor(red, green, blue, lighting, depth.z);
   }}));

   GeometryContainer.push_back(NewFace({-50, -50, 0}, {100, 100, 0}, {PI / 2, 0, 0}, {Color{40, 40, 40}}));

   GeometryContainer.push_back(NewFace({-1, 0, 3}, {1, 1, 0}, {0, 0, 0}, floor9));
   GeometryContainer.push_back(NewFace({-1, 0, 3}, {1, 1, 0}, {PI / 2, 0, 0}, floor5));
   GeometryContainer.push_back(NewFace({1, 0, 3}, {1.5, 1.5, 0}, {0, 0, 0}, Red));
   GeometryContainer.push_back(NewFace({1, 0, 3}, {1.5, 1.5, 0}, {PI / 2, 0, 0}, Blue));
   GeometryContainer.push_back(NewFace({1, 0, 3}, {1.5, 1.5, 0}, {0, PI / 2, 0}, Green));
   GeometryContainer.push_back(NewFace({1, 0, 3}, {1.5, 1.5, 0}, {PI, 0, 0}, Red));
   GeometryContainer.push_back(NewFace({1, 0, 3}, {1.5, 1.5, 0}, {PI * 1.5, 0, 0}, Blue));
   GeometryContainer.push_back(NewFace({1, 0, 3}, {1.5, 1.5, 0}, {0, PI * 1.5, 0}, Green));
   GeometryContainer.push_back(NewCube({1, 0, -2}, {1, 1, 1}, {0, PI, 0}, wall8, floor9, floor9));
   GeometryContainer.push_back(NewWedge({0, 0, -2}, {1, 1, 1}, {0, PI, 0}, wall8, floor9, wall8, wall8, floor9));

   while (!WindowShouldClose()) {
      UpdateEngine();

      // update geometry here
      RotateGeometry(GeometryContainer[0], {Timer, Timer, Timer});
      MoveGeometry(GeometryContainer[1], {GeometryContainer[1].pos.x, 0.5f + sinf(Timer) * 0.5f, GeometryContainer[1].pos.z});
      ScaleGeometry(GeometryContainer[1], {1.25f + sinf(Timer * 2.0f) * 0.25f, GeometryContainer[1].size.y, 1.25f + cosf(Timer * 2.0f) * 0.25f});

      RenderEngine();
   }

   CloseEngine();
   CloseWindow();
}
