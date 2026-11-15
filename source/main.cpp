#include "engine.hpp"
#include "math3d.hpp"

Color UVShader(const FragmentInput &in, void*) {
   return {ToByte(255.0f * in.uv.x), ToByte(255.0f * in.uv.y), 0, 255};
}

Color WavesShader(const FragmentInput &in, void*) {
   Vector2 uv = in.uv;
   float wave1 = 0.50f + 0.18f * std::sin(uv.x * 18.0f - Timer * 2.0f);
   float wave2 = 0.50f + 0.14f * std::sin(uv.x * 32.0f + Timer * 1.4f);
   float wave3 = wave1 + wave2 - 0.5f;
   float line = 255.0f * (std::abs(uv.y - 0.5f) < 0.004f);
   return {
      ToByte(255.0f * std::exp(-std::abs(uv.y - wave1) * 45.0f) + line),
      ToByte(255.0f * std::exp(-std::abs(uv.y - wave2) * 45.0f) + line),
      ToByte(255.0f * std::exp(-std::abs(uv.y - wave3) * 45.0f) + line),
      255
   };
}

Color AcidShader(const FragmentInput &in, void*) {
   float px = (in.uv.x - 0.5f) * 8.0f;
   float py = (in.uv.y - 0.5f) * 8.0f;
   float warpX = px + 1.2f * std::sin(py * 1.7f + Timer);
   float warpY = py + 1.2f * std::cos(px * 1.5f - Timer * 0.8f);
   float pattern = std::sin(warpX * 2.0f + std::sin(warpY * 2.5f + Timer));
   pattern += std::cos(warpY * 2.0f + std::sin(warpX * 2.0f - Timer));
   pattern *= 0.5f;
   float red = 127.5f + 127.5f * std::sin(pattern * 3.0f + Timer);
   float green = 127.5f + 127.5f * std::sin(pattern * 3.0f + Timer + 1.9f);
   float blue = 127.5f + 127.5f * std::sin(pattern * 3.0f + Timer + 4.1f);
   return {ToByte(red), ToByte(green), ToByte(blue), 255};
}

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
   UETexture lotus = LoadImage("assets/lotus.png");
   UETexture slime = LoadImage("assets/slime_block.png");

   Geometry ground = NewFace({-50, -50, 0}, {100, 100, 0}, {PI / 2, 0, 0}, {Color{40, 40, 40, 255}});

   Geometry rainbowCube = NewCube({3.25, 0.25, 0}, {1, 1, 1}, {0, 0, 0}, rainbow);
   Geometry brickCube = NewCube({3, 0.5, 2}, {1.5, 1.5, 1.5}, {0, 0, 0}, bricks);
   Geometry tileWedge = NewWedge({3, 0, -2}, {1, 1, 1}, {0, -PI / 2, 0}, tile);
   Geometry industrialCube = NewCube({1, 0, -2}, {1, 1, 1}, {0, PI, 0}, wall8, floor9, floor9);
   Geometry industrialWedge = NewWedge({0, 0, -2}, {1, 1, 1}, {0, PI, 0}, wall8, floor9, wall8, wall8, floor9);

   Geometry cutout = NewDoubleSidedFace({-1, 0, 5}, {1, 1, 0}, {0, 0, 0}, lotus);
   Geometry transparentSlime = NewCube({0.5f, 0, 5}, {1, 1, 1}, {0, 0, 0}, slime);
   Geometry transparentColor = NewCube({2.0f, 0, 5}, {1, 1, 1}, {0, 0, 0}, {Color{255, 125, 0, 125}});

   Geometry floor1 = NewDoubleSidedFace({-1, 0, 3}, {1, 1, 0}, {0, 0, 0}, floor9);
   Geometry floor2 = NewDoubleSidedFace({-1, 0, 3}, {1, 1, 0}, {PI / 2, 0, 0}, floor5);
   Geometry red = NewDoubleSidedFace({1, 0, 3}, {1.5, 1.5, 0}, {0, 0, 0}, Red);
   Geometry green = NewDoubleSidedFace({1, 0, 3}, {1.5, 1.5, 0}, {PI / 2, 0, 0}, Blue);
   Geometry blue = NewDoubleSidedFace({1, 0, 3}, {1.5, 1.5, 0}, {0, PI / 2, 0}, Green);

   Geometry pyramid = NewPyramid({-3, 0, -1}, {1.5, 1.5, 1.5}, {0, 0, 0}, floor5);
   Geometry uv = NewFace({-3, 0, 1.5f}, {1, 1, 0}, {0, PI / 2, 0}, Black);
   Geometry waves = NewFace({-3, 0, 3.0f}, {1, 1, 0}, {0, PI / 2, 0}, Black);
   Geometry acid = NewFace({-3, 0, 4.5f}, {1, 1, 0}, {0, PI / 2, 0}, Black);

   UEShader uvShader = {nullptr, UVShader};
   UEShader waveShader = {nullptr, WavesShader};
   UEShader acidShader = {nullptr, AcidShader};

   while (!WindowShouldClose()) {
      UpdateEngine();

      // update geometry here
      RotateGeometry(rainbowCube, {Timer, Timer, Timer});
      MoveGeometry(brickCube, {brickCube.pos.x, 0.5f + sinf(Timer) * 0.5f, brickCube.pos.z});
      ScaleGeometry(brickCube, {1.25f + sinf(Timer * 2.0f) * 0.25f, brickCube.size.y, 1.25f + cosf(Timer * 2.0f) * 0.25f});

      // render geometry here
      BeginRender();
         RenderGeometry(ground);

         RenderGeometry(rainbowCube);
         RenderGeometry(brickCube);
         RenderGeometry(tileWedge);
         RenderGeometry(industrialCube);
         RenderGeometry(industrialWedge);

         RenderGeometry(floor1);
         RenderGeometry(floor2);
         RenderGeometry(red);
         RenderGeometry(green);
         RenderGeometry(blue);

         RenderGeometry(cutout);
         RenderGeometry(transparentSlime);
         RenderGeometry(transparentColor);

         RenderGeometry(pyramid);
         BeginShader(uvShader);
            RenderGeometry(uv);
         EndShader();
         BeginShader(waveShader);
            RenderGeometry(waves);
         EndShader();
         BeginShader(acidShader);
            RenderGeometry(acid);
         EndShader();
      EndRender();
   }
   CloseEngine();
   CloseWindow();
}
