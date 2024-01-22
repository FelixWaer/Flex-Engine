#include "FXE_Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "../../FlexLibrary/Flextimer.h"

FlexMesh::FlexMesh(std::string modelPath)
{
    ModelPath = modelPath;
    load_Mesh();
}

void FlexMesh::init_Mesh(std::string modelPath)
{
    ModelPath = modelPath;
    load_Mesh();
}


void FlexMesh::load_Mesh()
{
    FlexTimer timer("Model loading");

    tinyobj::attrib_t attribute;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning, error;

    if (!tinyobj::LoadObj(&attribute, &shapes, &materials, &warning, &error, ModelPath.c_str()))
    {
        throw std::runtime_error(warning + error);
    }

    std::unordered_map<FXE::Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            FXE::Vertex vertex{};

            vertex.pos = {
                attribute.vertices[3 * index.vertex_index + 0],
                attribute.vertices[3 * index.vertex_index + 1],
                attribute.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoordinates = {
                attribute.texcoords[2 * index.texcoord_index + 0],
                1.0f - attribute.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (uniqueVertices.contains(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(Vertices.size());
                Vertices.push_back(vertex);
            }

            Indices.push_back(uniqueVertices[vertex]);
        }
    }
}