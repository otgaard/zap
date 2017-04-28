/* Created by Darren Otgaar on 2017/04/08. http://www.github.com/otgaard/zap */
#include "loader.hpp"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <maths/io.hpp>
#include <tools/log.hpp>

struct loader::state_t {
    std::unique_ptr<Assimp::Importer> importer;
};

loader::loader() : state_(new state_t()), s(*state_.get()) {
}

loader::~loader() = default;

std::vector<loader::mesh_p3n3t2_trii_t> loader::load(const std::string& path) {
    std::vector<loader::mesh_p3n3t2_trii_t> result;
    if(!s.importer) s.importer = std::unique_ptr<Assimp::Importer>(new Assimp::Importer());

    auto scene_ptr = s.importer->ReadFile(path, aiProcessPreset_TargetRealtime_Quality);
    if(!scene_ptr) {
        LOG_ERR("Failed to load model file:", path);
        return result;
    }

    LOG("Mesh Count:", LOG_BLUE, scene_ptr->mNumMeshes);

    for(uint32_t i = 0; i != scene_ptr->mNumMeshes; ++i) {
        loader::mesh_p3n3t2_trii_t mesh;
        mesh.allocate();
        mesh.bind();

        auto* ptr = scene_ptr->mMeshes[i];
        LOG(ptr->mNumVertices, ptr->mMaterialIndex);
        LOG(ptr->HasPositions(), ptr->HasNormals(), ptr->HasTextureCoords(0), ptr->HasTextureCoords(1), ptr->HasTextureCoords(2), ptr->mNumUVComponents);
        LOG("Indexed:", ptr->HasFaces());

        auto vbuf = new zap::graphics::vbuf_p3n3t2_t();
        if(!vbuf->allocate()) {
            LOG_ERR("Failed to allocate vertex_buffer");
            return result;
        }

        vbuf->bind();
        if(!vbuf->initialise(ptr->mNumVertices)) {
            LOG_ERR("Failed to initialise vertex_buffer with", ptr->mNumVertices, "vertices");
            return result;
        }

        //int mesh_id = 0;
        if(vbuf->map(zap::engine::buffer_access::BA_WRITE_ONLY)) {
            //LOG("Mesh:", mesh_id++);
            for(uint32_t v = 0; v != ptr->mNumVertices; ++v) {
                auto& vtx = (*vbuf)[v];
                vtx.position = *reinterpret_cast<zap::maths::vec3f*>(&ptr->mVertices[v]);
                if(ptr->HasNormals()) vtx.normal = *reinterpret_cast<zap::maths::vec3f*>(&ptr->mNormals[v]);
                if(ptr->HasTextureCoords(0)) {
                    vtx.texcoord1.x = ptr->mTextureCoords[0]->x;
                    vtx.texcoord1.y = ptr->mTextureCoords[0]->y;
                }
            }
            vbuf->unmap();
        }

        auto ibuf = new zap::graphics::ibuf_tri4_t();
        if(!ibuf->allocate()) {
            LOG_ERR("Failed to allocate index_buffer");
            return result;
        }

        ibuf->bind();
        if(!ibuf->initialise(ptr->mNumFaces*3)) {
            LOG_ERR("Failed to initialise index_buffer with", ptr->mNumFaces, "triangles");
            return result;
        }

        if(ibuf->map(zap::engine::buffer_access::BA_WRITE_ONLY)) {
            unsigned int idx = 0;
            for(uint32_t t = 0; t != ptr->mNumFaces; ++t) {
                for(uint32_t j = 0; j != ptr->mFaces[t].mNumIndices; ++j) {
                    (*ibuf)[idx++] = ptr->mFaces[t].mIndices[j];
                }
            }
            ibuf->unmap();
        }

        mesh.set_stream(vbuf);
        mesh.set_index(ibuf);
        result.emplace_back(std::move(mesh));
    }

    return result;
}
