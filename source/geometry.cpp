#include "geometry.hpp"
#include "math3d.hpp"
#include <cstdio>

// UETexture constructors

UETexture::UETexture()
   : type(UE_TEXTURE_NONE) {}

UETexture::UETexture(Color color)
   : type(UE_TEXTURE_COLOR), color(color) {
   // just don't render the triangle if it's fully transparent
   if (color.a == 0) {
      type = UE_TEXTURE_NONE;
   }

   // let the rasterizer know that this triangle is transparent
   if (color.a != 0 && color.a != 255) {
      isTransparent = true;
   }
}

UETexture::UETexture(Image image)
   : type(UE_TEXTURE_TEXTURE), texture(image) {
   // we need the image to be RGBA so that we can reliably index colors
   // in the rasterizer. without this, images will look corrupted
   ImageFormat(&texture, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
   int size = texture.width * texture.height;
   Color *pixels = (Color*)texture.data;

   for (int i = 0; i < size; ++i) {
      // we handle for a == 0 in the rasterizer and simply discard the
      // pixel. transparency would be overkill for simple cutouts
      if (pixels[i].a != 0 && pixels[i].a != 255) {
         isTransparent = true;
         break;
      }
   }
}

// Triangle constructor

Triangle::Triangle(Vector3 p1, Vector3 p2, Vector3 p3, Vector2 uv1, Vector2 uv2, Vector2 uv3, float lighting, const UETexture *texture, int shader)
   : p1(p1), p2(p2), p3(p3), uv1(uv1), uv2(uv2), uv3(uv3), lighting(lighting), texture(texture), shader(shader) {}

// Geometry edit functions

void SetupShapesVerticesBasedOnRotation(Geometry &geometry) {
   Vector3 anchor = geometry.pos + geometry.size * geometry.origin;
   size_t verticeCount = geometry.vertices.size();
   geometry.points.resize(verticeCount);

   for (size_t i = 0; i < verticeCount; ++i) {
      Vector3 world = geometry.pos + geometry.vertices[i] * geometry.size;
      geometry.points[i] = Rotate3DAroundOrigin(world, anchor, geometry.rotation);
   }
}

Geometry MakeGeometry(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, const std::vector<TriangleData> &triangles, const std::vector<Vector3> &vertices, const std::vector<Vector2> &uv, const std::vector<UETexture> &textures) {
   Geometry geometry;
   geometry.pos = pos;
   geometry.size = size;
   geometry.origin = origin;
   geometry.rotation = rotation;
   geometry.triangles = triangles;
   geometry.vertices = vertices;
   geometry.uv = uv;
   geometry.textures = textures;
   SetupShapesVerticesBasedOnRotation(geometry);
   return geometry;
}

// Geometry edit functions

void MoveGeometry(Geometry &geometry, Vector3 offset) {
   geometry.pos = offset;
   SetupShapesVerticesBasedOnRotation(geometry);
}

void ScaleGeometry(Geometry &geometry, Vector3 scale) {
   geometry.size = scale;
   SetupShapesVerticesBasedOnRotation(geometry);
}

void RotateGeometry(Geometry &geometry, Vector3 rotation) {
   geometry.rotation = rotation;
   SetupShapesVerticesBasedOnRotation(geometry);
}

void ChangeOriginForGeometry(Geometry &geometry, Vector3 origin) {
   geometry.origin = origin;
   SetupShapesVerticesBasedOnRotation(geometry);
}

// New geometry

// triangles - x, y, z, uv1, uv2, uv3, texture index
// uvs - u, v
// vertices - x, y, z

Geometry NewFace(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2) {
   std::vector<TriangleData> triangles = {
      {0, 1, 3, 0, 1, 3, 0},
      {1, 2, 3, 1, 2, 3, 1},
   };

   std::vector<Vector2> uv = {
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
   };

   std::vector<Vector3> vertices = {
      {0.0f, 0.0f, 0.0f},
      {1.0f, 0.0f, 0.0f},
      {1.0f, 1.0f, 0.0f},
      {0.0f, 1.0f, 0.0f},
   };

   std::vector<UETexture> textures = {t1, t2};
   return MakeGeometry(pos, size, origin, rotation, triangles, vertices, uv, textures);
}

Geometry NewFace(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1) {
   return NewFace(pos, size, origin, rotation, t1, t1);
}

Geometry NewDoubleSidedFace(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4) {
   std::vector<TriangleData> triangles = {
      {0, 1, 3, 0, 1, 3, 0},
      {1, 2, 3, 1, 2, 3, 1},
      {3, 1, 0, 3, 1, 0, 2},
      {3, 2, 1, 3, 2, 1, 3},
   };

   std::vector<Vector2> uv = {
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
   };

   std::vector<Vector3> vertices = {
      {0.0f, 0.0f, 0.0f},
      {1.0f, 0.0f, 0.0f},
      {1.0f, 1.0f, 0.0f},
      {0.0f, 1.0f, 0.0f},
   };

   std::vector<UETexture> textures = {t1, t2, t3, t4};
   return MakeGeometry(pos, size, origin, rotation, triangles, vertices, uv, textures);
}

Geometry NewDoubleSidedFace(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2) {
   return NewDoubleSidedFace(pos, size, origin, rotation, t1, t1, t2, t2);
}

Geometry NewDoubleSidedFace(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1) {
   return NewDoubleSidedFace(pos, size, origin, rotation, t1, t1, t1, t1);
}

Geometry NewCube(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6, UETexture t7, UETexture t8, UETexture t9, UETexture t10, UETexture t11, UETexture t12) {
   std::vector<TriangleData> triangles = {
      {7, 6, 5, 0, 1, 2, 0},
      {5, 4, 7, 2, 3, 0, 1},
      {3, 7, 4, 0, 1, 2, 2},
      {4, 0, 3, 2, 3, 0, 3},
      {2, 3, 0, 0, 1, 2, 4},
      {0, 1, 2, 2, 3, 0, 5},
      {6, 2, 1, 0, 1, 2, 6},
      {1, 5, 6, 2, 3, 0, 7},
      {4, 5, 1, 0, 1, 2, 8},
      {1, 0, 4, 2, 3, 0, 9},
      {3, 2, 6, 0, 1, 2, 10},
      {6, 7, 3, 2, 3, 0, 11},
   };

   std::vector<Vector2> uv = {
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
   };

   std::vector<Vector3> vertices = {
      {0.0f, 1.0f, 1.0f},
      {1.0f, 1.0f, 1.0f},
      {1.0f, 0.0f, 1.0f},
      {0.0f, 0.0f, 1.0f},
      {0.0f, 1.0f, 0.0f},
      {1.0f, 1.0f, 0.0f},
      {1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 0.0f},
   };

   std::vector<UETexture> textures = {t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12};
   return MakeGeometry(pos, size, rotation, origin, triangles, vertices, uv, textures);
}

Geometry NewCube(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6) {
   return NewCube(pos, size, rotation, origin, t1, t1, t2, t2, t3, t3, t4, t4, t5, t5, t6, t6);
}

Geometry NewCube(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture sides, UETexture top, UETexture bottom) {
   return NewCube(pos, size, rotation, origin, sides, sides, sides, sides, sides, sides, sides, sides, top, top, bottom, bottom);
}

Geometry NewCube(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1) {
   return NewCube(pos, size, rotation, origin, t1, t1, t1, t1, t1, t1, t1, t1, t1, t1, t1, t1);
}

Geometry NewWedge(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6, UETexture t7, UETexture t8) {
   std::vector<TriangleData> triangles = {
      {0, 1, 2, 2, 3, 0, 0},
      {2, 3, 0, 0, 1, 2, 1},
      {3, 2, 4, 0, 1, 2, 2},
      {4, 5, 3, 2, 3, 0, 3},
      {0, 3, 5, 3, 0, 1, 4},
      {4, 2, 1, 0, 1, 2, 5},
      {4, 1, 0, 1, 2, 3, 6},
      {0, 5, 4, 3, 0, 1, 7},
   };

   std::vector<Vector2> uv = {
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
   };

   std::vector<Vector3> vertices = {
      {0.0f, 1.0f, 1.0f},
      {1.0f, 1.0f, 1.0f},
      {1.0f, 0.0f, 1.0f},
      {0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 0.0f},
   };

   std::vector<UETexture> textures = {t1, t2, t3, t4, t5, t6, t7, t8};
   return MakeGeometry(pos, size, origin, rotation, triangles, vertices, uv, textures);
}

Geometry NewWedge(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5) {
   return NewWedge(pos, size, origin, rotation, t1, t1, t2, t2, t3, t4, t5, t5);
}

Geometry NewWedge(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1) {
   return NewWedge(pos, size, origin, rotation, t1, t1, t1, t1, t1, t1, t1, t1);
}

Geometry NewPyramid(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6) {
   std::vector<TriangleData> triangles = {
      {0, 2, 1, 4, 0, 3, 0},
      {0, 3, 2, 4, 0, 3, 1},
      {0, 4, 3, 4, 0, 3, 2},
      {0, 1, 4, 4, 0, 3, 3},
      {1, 2, 3, 0, 1, 2, 4},
      {3, 4, 1, 2, 3, 0, 5},
   };

   std::vector<Vector2> uv = {
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
      {1.0f, 0.5f},
   };

   std::vector<Vector3> vertices = {
      {0.5f, 1.0f, 0.5f},
      {0.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, 0.0f},
   };

   std::vector<UETexture> textures = {t1, t2, t3, t4, t5, t6};
   return MakeGeometry(pos, size, origin, rotation, triangles, vertices, uv, textures);
}

Geometry NewPyramid(Vector3 pos, Vector3 size, Vector3 origin, Vector3 rotation, UETexture t1) {
   return NewPyramid(pos, size, origin, rotation, t1, t1, t1, t1, t1, t1);
}
