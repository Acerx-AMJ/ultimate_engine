#include "engine.hpp"
#include "math3d.hpp"
#include "render.hpp"

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
   Timer += dt;
}

void RenderEngine() {
   RenderFrame();
   UpdateTexture(CanvasTexture, Canvas.data);

   BeginDrawing();
   ClearBackground(BLACK);
   DrawTexturePro(CanvasTexture, {0, 0, (float)Width, (float)Height}, {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, {0, 0}, 0, WHITE);
   if (ShowFPS) DrawFPS(0, 0);
   EndDrawing();
}
