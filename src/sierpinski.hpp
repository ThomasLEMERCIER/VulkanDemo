#include "model.hpp"

void populate_vertices(std::vector<vdem::VdemModel::Vertex>& vertices, int depth, float x, float y, float size) {
  if (depth == 0) {
    vertices.push_back({{x, y}});
    vertices.push_back({{x + size, y}});
    vertices.push_back({{x + size / 2, y - size}});
  } else {
    populate_vertices(vertices, depth - 1, x, y, size / 2);
    populate_vertices(vertices, depth - 1, x + size / 2, y, size / 2);
    populate_vertices(vertices, depth - 1, x + size / 4, y - size / 2, size / 2);
  }
}
