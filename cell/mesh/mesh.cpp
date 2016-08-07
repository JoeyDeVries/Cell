#include "mesh.h"

Mesh::Mesh()
{

}
Mesh::Mesh(std::vector<math::vec3> positions, std::vector<unsigned int> indices)
{
    Positions = positions;
    Indices = indices;
}
Mesh::Mesh(std::vector<math::vec3> positions, std::vector<math::vec2> uv, std::vector<unsigned int> indices)
{
    Positions = positions;
    UV = uv;
    Indices = indices;
}
Mesh::Mesh(std::vector<math::vec3> positions, std::vector<math::vec2> uv, std::vector<math::vec3> normals, std::vector<unsigned int> indices)
{
    Positions = positions;
    UV = uv;
    Normals = normals;
    Indices = indices;
}
Mesh::Mesh(std::vector<math::vec3> positions, std::vector<math::vec2> uv, std::vector<math::vec3> normals, std::vector<math::vec3> tangents, std::vector<math::vec3> bitangents, std::vector<unsigned int> indices)
{
    Positions = positions;
    UV = uv;
    Normals = normals;
    Tangents = tangents;
    Bitangents = bitangents;
    Indices = indices;
}

void Mesh::SetPositions(std::vector<math::vec3> positions)
{
    Positions = positions;
}

void Mesh::SetUVs(std::vector<math::vec2> uv)
{
    UV = uv;
}

void Mesh::SetNormals(std::vector<math::vec3> normals)
{
    Normals = normals;
}

void Mesh::SetTangents(std::vector<math::vec3> tangents, std::vector<math::vec3> bitangents)
{
    Tangents = tangents;
    Bitangents = bitangents;
}

void Mesh::Finalize(bool interleaved)
{
    // TODO(Joey): initialize VAO if not done before
    // TODO(Joey): preprocess buffer data as interleaved or seperate when specified
    // TODO(Joey): fill/update buffers (only on vertex data that is not empty; size() > 0)
    // TODO(Joey): configure vertex attributes (only on vertex data size() > 0)
}

void Mesh::calculateNormals(bool smooth)
{
    // TODO(Joey): manually calculate the normals of each vertex
}

void Mesh::calculateTangents()
{
    // TODO(Joey): walk overall the vertices and calculate the tangent space vectors manually
}