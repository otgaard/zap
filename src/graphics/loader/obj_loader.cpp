//
// Created by otgaard on 2018/02/25.
//

#include "obj_loader.hpp"
#include <string>
#include <fstream>
#include <streambuf>

namespace zap { namespace graphics {

std::string obj_loader::read_textfile(const std::string& path) {
    std::string str;
    std::ifstream file(path);
    file.seekg(0, std::ios::end);
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return str;
}

obj_loader::mesh_p3n3t2_u32_t obj_loader::load_mesh(const std::string& path) {
    mesh_p3n3t2_u32_t mesh;
    auto vbuf = std::make_unique<vbuf_p3n3t2_t>();
    auto ibuf = std::make_unique<ibuf_u32_t>();

    if (!vbuf->allocate() || !ibuf->allocate() || !mesh.allocate()) {
        LOG_ERR("Failed to create mesh");
        return mesh;
    }

    auto model = load_model(path);

    mesh.bind();

    vbuf->bind();
    if (!vbuf->initialise(model.first)) {
        LOG_ERR("Failed to initialise vertices");
        return mesh;
    }

    ibuf->bind();
    if (!ibuf->initialise(model.second)) {
        LOG_ERR("Failed to initialise faces");
        return mesh;
    }

    mesh.set_stream(vbuf.release(), true);
    mesh.set_index(ibuf.release(), true);

    return mesh;
}

obj_loader::model_t obj_loader::load_model(const std::string& path) {
    std::string file = read_textfile(path);
    if(file.empty()) {
        LOG_ERR("Failed to load file:", path);
        return std::make_pair(std::vector<vtx_p3n3t2_t>(), std::vector<uint32_t>());
    }

    std::vector<vtx_p3n3t2_t> positions;
    std::vector<uint32_t> faces;
    positions.reserve(100000); faces.reserve(100000);

    std::istringstream sstr(file);
    std::string line;
    while(std::getline(sstr, line)) {
        if (line[0] == '#') continue;
        else if (line[0] == 'v') {
            // process vertex
            std::istringstream parser(line);
            char op;
            float x, y, z;
            parser >> op >> x >> y >> z;
            vtx_p3n3t2_t vtx;
            vtx.position.set(x, y, z);
            vtx.normal.set(0.f, 0.f, 0.f);
            positions.push_back(vtx);
        } else if (line[0] == 'f') {
            // process face
            std::istringstream parser(line);
            char op;
            uint32_t i, j, k;
            parser >> op >> i >> j >> k;
            faces.push_back(i-1); faces.push_back(j-1); faces.push_back(k-1);
        }
    }

    LOG("Vertices:", positions.size(), "Faces:", faces.size());

    compute_normals(positions, faces, false);

    return std::make_pair(positions, faces);
}

}}