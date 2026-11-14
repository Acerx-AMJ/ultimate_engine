#include "geometry.hpp"
#include "math3d.hpp"

void SetupShapesVerticesBasedOnRotation(const std::vector<Vector3> &vertices, std::vector<Vector3> &points, Vector3 origin, Vector3 rotation) {
   points.resize(vertices.size());
   for (size_t i = 0; i < vertices.size(); ++i) {
      points[i] = Rotate3DAroundOrigin(vertices[i], origin, rotation);
   }
}

Geometry MakeGeometry(Vector3 pos, Vector3 size, Vector3 rotation, const std::vector<TriangleData> &triangles, const std::vector<Vector3> &vertices, const std::vector<Vector2> &uv, const std::vector<UETexture> &textures) {
   Geometry geometry;
   geometry.pos = pos;
   geometry.size = size;
   geometry.origin = pos + size * 0.5;
   geometry.rotation = rotation;
   geometry.triangles = triangles;
   geometry.vertices = vertices;
   geometry.uv = uv;
   geometry.textures = textures;
   SetupShapesVerticesBasedOnRotation(geometry.vertices, geometry.points, geometry.origin, geometry.rotation);
   return geometry;
}

// Geometry edit functions

void MoveGeometry(Geometry &geometry, Vector3 offset) {
   for (auto &vertex : geometry.vertices) {
      vertex = (vertex - geometry.pos) + offset;
   }
   geometry.pos = offset;
   geometry.origin = geometry.pos + geometry.size * 0.5;
   SetupShapesVerticesBasedOnRotation(geometry.vertices, geometry.points, geometry.origin, geometry.rotation);
}

void ScaleGeometry(Geometry &geometry, Vector3 scale) {
   Vector3 factor = scale / Vec3ZeroFix(geometry.size);
   for (auto &vertex : geometry.vertices) {
      vertex = (vertex - geometry.pos) * factor + geometry.pos;
   }

   geometry.size = scale;
   geometry.origin = geometry.pos + geometry.size * 0.5;
   SetupShapesVerticesBasedOnRotation(geometry.vertices, geometry.points, geometry.origin, geometry.rotation);
}

void RotateGeometry(Geometry &geometry, Vector3 rotation) {
   geometry.rotation = rotation;
   SetupShapesVerticesBasedOnRotation(geometry.vertices, geometry.points, geometry.origin, rotation);
}

// New geometry

// triangles - x, y, z, uv1, uv2, uv3, texture index
// uvs - u, v
// vertices - x, y, z

Geometry NewFace(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1, UETexture t2) {
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
      {pos.x, pos.y, pos.z},
      {pos.x + size.x, pos.y, pos.z},
      {pos.x + size.x, pos.y + size.y, pos.z},
      {pos.x, pos.y + size.y, pos.z},
   };

   std::vector<UETexture> textures = {t1, t2};
   return MakeGeometry(pos, size, rotation, triangles, vertices, uv, textures);
}

Geometry NewFace(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1) {
   return NewFace(pos, size, rotation, t1, t1);
}

Geometry NewCube(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6, UETexture t7, UETexture t8, UETexture t9, UETexture t10, UETexture t11, UETexture t12) {
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
      {pos.x, pos.y + size.y, pos.z + size.z},
      {pos.x + size.x, pos.y + size.y, pos.z + size.z},
      {pos.x + size.x, pos.y, pos.z + size.z},
      {pos.x, pos.y, pos.z + size.z},
      {pos.x, pos.y + size.y, pos.z},
      {pos.x + size.x, pos.y + size.y, pos.z},
      {pos.x + size.x, pos.y, pos.z},
      {pos.x, pos.y, pos.z},
   };

   std::vector<UETexture> textures = {t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12};
   return MakeGeometry(pos, size, rotation, triangles, vertices, uv, textures);
}

Geometry NewCube(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6) {
   return NewCube(pos, size, rotation, t1, t1, t2, t2, t3, t3, t4, t4, t5, t5, t6, t6);
}

Geometry NewCube(Vector3 pos, Vector3 size, Vector3 rotation, UETexture sides, UETexture top, UETexture bottom) {
   return NewCube(pos, size, rotation, sides, sides, sides, sides, sides, sides, sides, sides, top, top, bottom, bottom);
}

Geometry NewCube(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1) {
   return NewCube(pos, size, rotation, t1, t1, t1, t1, t1, t1, t1, t1, t1, t1, t1, t1);
}

Geometry NewWedge(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6, UETexture t7, UETexture t8) {
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
      {pos.x, pos.y + size.y, pos.z + size.z},
      {pos.x + size.x, pos.y + size.y, pos.z + size.z},
      {pos.x + size.x, pos.y, pos.z + size.z},
      {pos.x, pos.y, pos.z + size.z},
      {pos.x + size.x, pos.y, pos.z},
      {pos.x, pos.y, pos.z},
   };

   std::vector<UETexture> textures = {t1, t2, t3, t4, t5, t6, t7, t8};
   return MakeGeometry(pos, size, rotation, triangles, vertices, uv, textures);
}

Geometry NewWedge(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5) {
   return NewWedge(pos, size, rotation, t1, t1, t2, t2, t3, t4, t5, t5);
}

Geometry NewWedge(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1) {
   return NewWedge(pos, size, rotation, t1, t1, t1, t1, t1, t1, t1, t1);
}

Geometry NewPyramid(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1, UETexture t2, UETexture t3, UETexture t4, UETexture t5, UETexture t6) {
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
      {pos.x + size.x / 2.0f, pos.y + size.y, pos.z + size.z / 2.0f},
      {pos.x, pos.y, pos.z},
      {pos.x, pos.y, pos.z + size.z},
      {pos.x + size.x, pos.y, pos.z + size.z},
      {pos.x + size.x, pos.y, pos.z},
   };

   std::vector<UETexture> textures = {t1, t2, t3, t4, t5, t6};
   return MakeGeometry(pos, size, rotation, triangles, vertices, uv, textures);
}

Geometry NewPyramid(Vector3 pos, Vector3 size, Vector3 rotation, UETexture t1) {
   return NewPyramid(pos, size, rotation, t1, t1, t1, t1, t1, t1);
}
