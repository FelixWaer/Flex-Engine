#include "FXE_Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "../../FlexLibrary/Flextimer.h"
#include "../EngineApplication.h"
#include "../FXE_RendererManager.h"

FlexMesh::FlexMesh(std::string modelPath)
{
    FXE::MeshManager.emplace_back(this);
    ModelPath = modelPath;
    load_Mesh();
}

void FlexMesh::init_Mesh(std::string modelPath)
{
    FXE::MeshManager.emplace_back(this);
    ModelPath = modelPath;
    load_Mesh();

    BufferCreateInfo vertexBufferInfo;
    vertexBufferInfo.BufferSize = sizeof(Vertices[0]) * Vertices.size();
    vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    vertexBufferInfo.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    vertexBufferInfo.usage_2 = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBufferInfo.properties_2 = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    vertexBufferInfo.Data = Vertices.data();

    VertexBuffer.init_Buffer(vertexBufferInfo);

    BufferCreateInfo IndexBufferInfo;
    IndexBufferInfo.BufferSize = sizeof(Indices[0]) * Indices.size();
    IndexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    IndexBufferInfo.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    IndexBufferInfo.usage_2 = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    IndexBufferInfo.properties_2 = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    IndexBufferInfo.Data = Indices.data();
    
    IndexBuffer.init_Buffer(IndexBufferInfo);
}

void FlexMesh::cleanup_Mesh()
{
    VertexBuffer.cleanup_Buffer();
    IndexBuffer.cleanup_Buffer();
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