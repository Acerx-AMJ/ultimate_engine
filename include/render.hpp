#pragma once
#include <raylib.h>

void DrawTriangleUV(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float normal);
void DrawTriangleColor(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float normal, Color color);
void DrawTriangleGradient(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float normal, Color color1, Color color2, Color color3);
void DrawTriangleTexture(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float normal, Image texture);

void RenderFrame();
