/* Created by Darren Otgaar on 2017/04/08. http://www.github.com/otgaard/zap */
#include "loader.hpp"
#include <assimp/Scene.h>
#include <assimp/Importer.hpp>
#include <assimp/PostProcess.h>
#include <tools/log.hpp>

struct loader::state_t {
    std::unique_ptr<Assimp::Importer> importer;
};

loader::loader() : state_(new state_t()), s(*state_.get()) {
}

loader::~loader() = default;

void loader::load(const std::string& path) {
    if(!s.importer) s.importer = std::make_unique<Assimp::Importer>();

    auto scene_ptr = s.importer->ReadFile(path, aiProcessPreset_TargetRealtime_Quality);
    if(!scene_ptr) {
        LOG_ERR("Failed to load model file:", path);
        return;
    }
}
