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

// camera
inline Vector3 Rotation = {0.0f, 0.0f, 0.0f};
inline Vector3 Offset = {0.0f, -1.0f, 0.0f};
inline float Timer = 0.0f;

// settings
inline int Width = 200;
inline int Height = 200;
inline Vector3 LightDirection = {0.436f, -0.872f, 0.218f};
inline float Ambient = 0.6f;
inline float FogStart = 2.0f;
inline float FogEnd = 10.0f;
inline float NearClip = 0.01f;
inline bool ShowFPS = true;

// engine
inline Image Canvas;
inline Texture CanvasTexture;
inline std::vector<float> DepthBuffer;
inline std::vector<Geometry> GeometryContainer;
inline std::vector<Triangle> RenderTriangles;
inline std::vector<Vector3> RenderPoints;
inline std::vector<SortData> RenderTriangleKeys;
inline std::vector<int> RenderTriangleOrder;

void InitEngine();
void CloseEngine();
void UpdateEngine();
void RenderEngine();
