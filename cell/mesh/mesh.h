#ifndef CELL_MESH_H
#define CELL_MESH_H

#include <vector>

#include <math/linear_algebra/vector.h>

/* NOTE(Joey):

  Manually define a list of topology types as we don't want to
  directly link a mesh to an OpenGL toplogy type as this would
  reduce the renderer's cross compatbility.

*/
enum TOPOLOGY
{
    POINT,
    LINE,
    LINE_STRIP,
    TRIANGLES,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
};

/* NOTE(Joey):

  Mesh

*/
class Mesh
{
private:
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_EBO;
public:
    std::vector<math::vec3> Positions;
    std::vector<math::vec2> UV;
    std::vector<math::vec3> Normals;
    std::vector<math::vec3> Tangents;
    std::vector<math::vec3> Bitangents;

    std::vector<unsigned int> Indices;
    TOPOLOGY Topology;

    // NOTE(Joey): we support multiple ways of initializing a mesh
    Mesh();
    Mesh(std::vector<math::vec3> positions, std::vector<unsigned int> indices);
    Mesh(std::vector<math::vec3> positions, std::vector<math::vec2> uv, std::vector<unsigned int> indices);
    Mesh(std::vector<math::vec3> positions, std::vector<math::vec2> uv, std::vector<math::vec3> normals, std::vector<unsigned int> indices);
    Mesh(std::vector<math::vec3> positions, std::vector<math::vec2> uv, std::vector<math::vec3> normals, std::vector<math::vec3> tangents, std::vector<math::vec3> bitangents, std::vector<unsigned int> indices);

    // NOTE(Joey): set vertex data manually
    // TODO(Joey): not sure if these are required if we can directly set vertex data from public fields; construct several use-cases to test.
    void SetPositions(std::vector<math::vec3> positions);
    void SetUVs(std::vector<math::vec2> uv);
    void SetNormals(std::vector<math::vec3> normals);
    void SetTangents(std::vector<math::vec3> tangents, std::vector<math::vec3> bitangents); // NOTE(Joey): you can only set both tangents and bitangents at the same time to prevent mismatches

    // NOTE(Joey): commits all buffers and attributes to the renderer
    void Finalize(bool interleaved = true);

private:
    void calculateNormals(bool smooth = true);
    void calculateTangents();
};

#endif