#include "engine.hpp"
#include "math3d.hpp"
#include "render.hpp"

void InitEngine() {
   Canvas = GenImageColor(Width, Height, BLACK);
}

void CloseEngine() {
   UnloadImage(Canvas);
}

void UpdateEngine() {
   if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      Rotation.x = Rotation.x - GetMouseDelta().y * 0.01;
      Rotation.x = std::max(-PI / 2.0f, std::min(PI / 2.0f, Rotation.x));
      Rotation.y = Rotation.y + GetMouseDelta().x * 0.01;
   }

   Vector3 toffset;
   toffset.x = (IsKeyDown(KEY_A) - IsKeyDown(KEY_D)) * GetFrameTime();
   toffset.y = (IsKeyDown(KEY_Q) - IsKeyDown(KEY_E)) * GetFrameTime();
   toffset.z = (IsKeyDown(KEY_S) - IsKeyDown(KEY_W)) * GetFrameTime();

   if (toffset != Vector3{0.0f, 0.0f, 0.0f}) {
      Vector3 rotatedoffset = Rotate3D(toffset, {0, -Rotation.y, -Rotation.z});
      Offset += rotatedoffset;
   }
   Timer += GetFrameTime();
}

void RenderEngine() {
   RenderFrame();
   Texture texture = LoadTextureFromImage(Canvas);

   BeginDrawing();
   ClearBackground(BLACK);
   DrawTexturePro(texture, {0, 0, (float)texture.width, (float)texture.height}, {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, {0, 0}, 0, WHITE);
   DrawFPS(0, 0);
   EndDrawing();

   UnloadTexture(texture);
}
