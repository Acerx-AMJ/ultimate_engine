#include "engine.hpp"
#include <cmath>
#include <raylib.h>

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
   GeometryContainer.push_back(NewFace({-3, 0, 3.5f}, {1.0f, 1.0f, 0}, {0, PI / 2, 0}, {Red, Green, Blue}, {Green, Red, Blue}));

   GeometryContainer.push_back(NewFace({-50, -50, 0}, {100, 100, 0}, {PI / 2, 0, 0}, {Color{40, 40, 40}}));

   GeometryContainer.push_back(NewFace({-1, 0, 3}, {1, 1, 0}, {0, 0, 0}, {floor9}));
   GeometryContainer.push_back(NewFace({-1, 0, 3}, {1, 1, 0}, {PI / 2, 0, 0}, {floor5}));
   GeometryContainer.push_back(NewFace({1, 0, 3}, {1.5, 1.5, 0}, {0, 0, 0}, {Red}));
   GeometryContainer.push_back(NewFace({1, 0, 3}, {1.5, 1.5, 0}, {PI / 2, 0, 0}, {Blue}));
   GeometryContainer.push_back(NewFace({1, 0, 3}, {1.5, 1.5, 0}, {0, PI / 2, 0}, {Green}));
   GeometryContainer.push_back(NewFace({1, 0, 3}, {1.5, 1.5, 0}, {PI, 0, 0}, {Red}));
   GeometryContainer.push_back(NewFace({1, 0, 3}, {1.5, 1.5, 0}, {PI * 1.5, 0, 0}, {Blue}));
   GeometryContainer.push_back(NewFace({1, 0, 3}, {1.5, 1.5, 0}, {0, PI * 1.5, 0}, {Green}));
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
