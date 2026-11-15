#pragma once
#include "geometry.hpp"

void DrawTriangleColor(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float normal, Color color);
void DrawTriangleTexture(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float normal, Image texture, bool transparent);
void DrawTriangleCustom(Vector2 p1, Vector2 p2, Vector2 p3, Vector3 uv1, Vector3 uv2, Vector3 uv3, float normal, UETextureShader callback);

void RenderFrame();
