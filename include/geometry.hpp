#pragma once
#include <raylib.h>
#include <vector>

enum UETextureType: char {
   UE_TEXTURE_NONE, UE_TEXTURE_COLOR, UE_TEXTURE_TEXTURE, UE_TEXTURE_SHADER
};

typedef Color (*UETextureShader)(int x, int y, struct DepthData depth, Vector2 uv, float lighting);

struct UETexture {
   UETextureType type;
   bool isTransparent = false;
   union {
      Color color;
      Image texture;
      UETextureShader callback;
   };

   UETexture()
      : type(UE_TEXTURE_NONE) {}

   UETexture(Color color)
      : type(UE_TEXTURE_COLOR), color(color) {
      if (color.a == 0) {
         type = UE_TEXTURE_NONE;
      }
      else if (color.a != 0 && color.a != 255) {
         isTransparent = true;
      }
   }

   UETexture(Image image)
      : type(UE_TEXTURE_TEXTURE), texture(image) {
      ImageFormat(&texture, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
      int size = texture.width * texture.height;
      Color *pixels = (Color*)texture.data;

      for (int i = 0; i < size; ++i) {
         if (pixels[i].a != 0 && pixels[i].a != 255) {
            isTransparent = true;
            break;
         }
      }
   }

   UETexture(UETextureShader callback)
      : type(UE_TEXTURE_SHADER), callback(callback) {}
};

struct SortData {
   float depth;
   bool transparent;
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

   Triangle(Vector3 p1, Vector3 p2, Vector3 p3, Vector2 uv1, Vector2 uv2, Vector2 uv3, float lighting, const UETexture *texture)
      : p1(p1), p2(p2), p3(p3), uv1(uv1), uv2(uv2), uv3(uv3), lighting(lighting), texture(texture) {}
};

void SetupShapesVerticesBasedOnRotation(const std::vector<Vector3> &vertices, std::vector<Vector3> &points, Vector3 origin, Vector3 rotation);
Geometry MakeGeometry(Vector3 pos, Vector3 size, Vector3 rotation, const std::vector<TriangleData> &triangles, const std::vector<Vector3> &vertices, const std::vector<Vector2> &uv, const std::vector<UETexture> &textures);

// Geometry edit functions

void MoveGeometry(Geometry &geometry, Vector3 offset);
void ScaleGeometry(Geometry &geometry, Vector3 scale);
void RotateGeometry(Geometry &geometry, Vector3 rotation);

// New geometry

Geometry NewFace(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1, UETexture t2);
Geometry NewFace(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1);

Geometry NewCube(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6, UETexture t7, UETexture t8, UETexture t9, UETexture t10, UETexture t11, UETexture t12);
Geometry NewCube(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6);
Geometry NewCube(Vector3 pos, Vector3 size, Vector3 rotation, UETexture sides, UETexture top, UETexture bottom);
Geometry NewCube(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1);

Geometry NewWedge(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6, UETexture t7, UETexture t8);
Geometry NewWedge(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5);
Geometry NewWedge(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1);

Geometry NewPyramid(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6);
Geometry NewPyramid(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1);
