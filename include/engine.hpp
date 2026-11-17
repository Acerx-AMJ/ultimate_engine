#pragma once
#include "geometry.hpp"

// colors
inline constexpr Color Red = {255, 0, 0, 255};
inline constexpr Color Green = {0, 255, 0, 255};
inline constexpr Color Blue = {0, 0, 255, 255};
inline constexpr Color Yellow = {255, 255, 0, 255};
inline constexpr Color Magenta = {255, 0, 255, 255};
inline constexpr Color Cyan = {0, 255, 255, 255};
inline constexpr Color Pink = {255, 0, 128, 255};
inline constexpr Color Orange = {255, 128, 0, 255};
inline constexpr Color Lime = {128, 255, 0, 255};
inline constexpr Color Teal = {0, 255, 128, 255};
inline constexpr Color SkyBlue = {0, 128, 255, 255};
inline constexpr Color Purple = {128, 0, 255, 255};
inline constexpr Color Black = {0, 0, 0, 255};

// camera
inline Vector3 Rotation = {0.0f, 0.0f, 0.0f};
inline Vector3 Offset = {0.0f, -1.0f, 0.0f};
inline float Timer = 0.0f;

// settings
enum RenderMode: char {
   RENDER_MODE_DEFAULT, RENDER_MODE_PIXEL, RENDER_MODE_CIRCULAR_PIXEL, RENDER_MODE_SOBEL, RENDER_MODE_BLOOM
};

inline int Width = 200;
inline int Height = 200;
inline RenderMode Mode = RENDER_MODE_DEFAULT;
inline Vector3 LightDirection = {0.436f, -0.872f, 0.218f};
inline float Ambient = 0.6f;
inline float FogStart = 2.0f;
inline float FogEnd = 10.0f;
inline float NearClip = 0.01f;
inline bool ShowFPS = true;
inline bool FreeCamera = true;

// engine
void InitEngine();
void CloseEngine();
void UpdateEngine();

void BeginRender();
void EndRender();
void BeginShader(const UEShader &shader);
void EndShader();

Color SampleTexture(const UETexture &texture, Vector2 uv);
void RenderGeometry(const Geometry &geometry);
