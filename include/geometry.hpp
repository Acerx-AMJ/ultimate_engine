#pragma once
#include <raylib.h>
#include <vector>

enum UETextureType: char {
   UE_TEXTURE_NONE, UE_TEXTURE_COLOR, UE_TEXTURE_TEXTURE
};

struct UETexture {
   UETextureType type;
   bool isTransparent = false;
   union {
      Color color;
      Image texture;
   };

   UETexture();
   UETexture(Color color);
   UETexture(Image texture);
};

struct SortData {
   float depth;
   bool transparent;
};

struct DepthData {
   float w1, w2, w3, z;
   int di;
};

struct FragmentInput {
   int x, y;
   DepthData depth;
   Vector2 uv;
   Vector3 position;
   const UETexture *texture;
};

struct UEShader {
   Vector3 (*vertex)(Vector3 position, int index, void *data) = nullptr;
   Color (*fragment)(const FragmentInput &input, void *data) = nullptr;
   void *data = nullptr;
   bool transparent = false;
};

struct TriangleBounds {
   int xmin, xmax, ymin, ymax;
   float area, ia;
};

struct TriangleData {
   int x, y, z;
   int uv1, uv2, uv3;
   int texture;
};

struct Geometry {
   Vector3 pos, size, origin, rotation;
   std::vector<TriangleData> triangles;
   std::vector<Vector3> vertices;
   std::vector<Vector3> points;
   std::vector<Vector2> uv;
   std::vector<UETexture> textures;
};

struct Triangle {
   Vector3 p1, p2, p3;
   Vector2 uv1, uv2, uv3;
   float lighting = 1.0;
   const UETexture *texture = nullptr;
   int shader = -1;

   Triangle(Vector3 p1, Vector3 p2, Vector3 p3, Vector2 uv1, Vector2 uv2, Vector2 uv3, float lighting, const UETexture *texture, int shader);
};

// Geometry origin constants

static constexpr Vector3 ORIGIN_TOP_LEFT = {0.0f, 0.0f, 0.0f};
static constexpr Vector3 ORIGIN_CENTER = {0.5f, 0.5f, 0.5f};
static constexpr Vector3 ORIGIN_BOTTOM_LEFT = {1.0f, 1.0f, 1.0f};

// Geometry edit functions

void MoveGeometry(Geometry &geometry, Vector3 offset);
void ScaleGeometry(Geometry &geometry, Vector3 scale);
void RotateGeometry(Geometry &geometry, Vector3 rotation);
void ChangeOriginForGeometry(Geometry &geometry, Vector3 origin);

// New geometry

Geometry NewFace(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2);
Geometry NewFace(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1);
Geometry NewDoubleSidedFace(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4);
Geometry NewDoubleSidedFace(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2);
Geometry NewDoubleSidedFace(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1);

Geometry NewCube(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6, UETexture t7, UETexture t8, UETexture t9, UETexture t10, UETexture t11, UETexture t12);
Geometry NewCube(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6);
Geometry NewCube(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture sides, UETexture top, UETexture bottom);
Geometry NewCube(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1);

Geometry NewWedge(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6, UETexture t7, UETexture t8);
Geometry NewWedge(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5);
Geometry NewWedge(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1);

Geometry NewPyramid(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6);
Geometry NewPyramid(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1);
