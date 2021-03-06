#include "mesh.h"
#include "string_ext.hpp"
#include <charconv>

using namespace std;

template <typename T>
inline T svto(string_view sv) { // NOLINT(readability-identifier-naming)
    if constexpr (is_integral<T>::value) {
        T res = 0;
        auto [p, ec] = from_chars(sv.data(), sv.data() + sv.size(), res);
        assert(ec == errc());
        return res;
    } else if constexpr (is_floating_point<T>::value) {
        auto sz = sv.size();
        char *temp_str = new char[sz + 1];
        memcpy(temp_str, sv.data(), sz * sizeof(char));
        temp_str[sz] = '\0';
        T res = atof(temp_str);
        delete[] temp_str;
        return res;
    } else { // unsuported conversion
        static_assert(is_integral<T>::value || is_floating_point<T>::value, "Invalid conversion from std::string_view");
    }
}

namespace fs = std::filesystem;

namespace loia {

namespace { //< static local global states
    Mesh::SurfaceGroup curr_surface_group;

    string curr_mtl_name;

    fs::path curr_obj_path, curr_mtllib_path;
} // namespace

Material::MtlCallback Material::apply_cb_default = nullptr;
Material::TexCallback Material::tex_binder_default = nullptr;

void Mesh::fileParser_(fs::path const &path, Mesh &mesh, ParamMatcher const &matcher) {
    ifstream fin(path);
    if (!fin) return;
    while (true) {
        string line;
        if (!getline(fin, line)) break; // eof
        trim(line);
        if (line.empty()) continue;

        while (line.back() == '\\') { // use `\` to concatenate lines
            string next_line;
            if (!getline(fin, next_line)) { // unexpected EOF
                cerr << "'\\' concatenation expect a new line ..." << endl;
                exit(1);
            }
            line.back() = ' ';
            trim(next_line);
            line += next_line;
        }
        /// ignore empty line and comments
        if (line.empty() || line.front() == '#') continue;

        istringstream iss(line);
        string indicator; // indicate what this line stands for
        iss >> indicator;
        auto it = matcher.find(indicator);
        if (it != matcher.end()) {
            it->second(mesh, iss); // ignore unsupported indicator
        }
    }
}

Mesh Mesh::loadMesh(std::filesystem::path const &model) {
    curr_obj_path = model;
    Mesh mesh;
    Mesh::fileParser_(model, mesh, obj_param_matcher);
    if (!curr_surface_group.empty()) {
        mesh.surfaceGroups.push_back(move(curr_surface_group));
        // curr_surface_group.clear();
    }
    return mesh;
}

void Mesh::renderMesh(const Mesh &mesh, float zoom) {
    for (const auto &f_group : mesh.surfaceGroups) {
        if (f_group.pMtl != nullptr) {
            f_group.pMtl->apply();
        }
        glShadeModel((f_group.smooth) ? GL_SMOOTH : GL_FLAT);

        for (const auto &f : f_group) {
            glBegin(GL_TRIANGLES);
            for (int i = 0; i < 3; ++i) {
                if (f.normalId[i] != -1) {
                    glNormal3fv(value_ptr(mesh.normals[f.normalId[i]]));
                }
                if (f.textureId[i] != -1) {
                    glTexCoord3fv(value_ptr(mesh.texture[f.textureId[i]]));
                }
                glVertex3fv(value_ptr(mesh.vertices[f.vertexId[i]] * zoom));
            }
            glEnd();
        }
        glBindTexture(GL_TEXTURE_2D, 0); // switch to default texture
    }
}

inline void readPoint(const string &buf, Mesh::Surface &s, int i) {
    list<string_view> tokens;
    split(buf, tokens, '/');
    s.vertexId[i] = svto<int>(tokens.front()) - 1; // vertex-id is mandatory
    if (tokens.size() == 2) {
        s.textureId[i] = svto<int>(tokens.back()) - 1;
    } else {
        const auto &vt_str_view = *next(tokens.begin());
        if (!vt_str_view.empty()) {
            s.textureId[i] = svto<int>(vt_str_view) - 1;
        } else {
            s.textureId[i] = -1;
        }
        s.normalId[i] = svto<int>(tokens.back()) - 1;
    }
}

// clang-format off
const Mesh::ParamMatcher Mesh::obj_param_matcher{
    {"v", [](Mesh &m, istringstream &iss) {
        float x, y, z, w = 1.0F;
        iss >> x >> y >> z >> w;
        m.vertices.emplace_back(x/w, y/w, z/w);
    }},
    {"vt", [](Mesh &m, istringstream &iss) {
        float u, v = 0, w = 0;
        iss >> u >> v >> w;
        m.texture.emplace_back(u, v, w);
    }},
    {"vn", [](Mesh &m, istringstream &iss) {
        float x, y, z;
        iss >> x >> y >> z;
        m.normals.emplace_back(x, y, z);
    }},
    {"f", [](Mesh & /*m*/, istringstream &iss) {
        Surface s = {};
        string buf;
        for (int i = 0; i < 3; ++i) {
            iss >> buf;
            readPoint(buf, s, i);
        }
        curr_surface_group.emplace_back(s);
        if (iss >> buf) {
            Surface s1 = {};
            readPoint(buf, s1, 0);
            s1.vertexId[1] = s.vertexId[0];
            s1.vertexId[2] = s.vertexId[2];
            s1.normalId[1] = s.normalId[0];
            s1.normalId[2] = s.normalId[2];
            s1.textureId[1] = s.textureId[0];
            s1.textureId[2] = s.textureId[2];
            curr_surface_group.emplace_back(s1);
        }
    }},
    {"mtllib", [](Mesh& m, istringstream& iss) {
        string temp;
        getline(iss, temp);
        trim(temp);
        curr_mtllib_path = temp; // so if there's space in path, it'll be safe

        if (curr_mtllib_path.is_relative()) {
            curr_mtllib_path = curr_obj_path.parent_path() / curr_mtllib_path;
        }

        Mesh::fileParser_(curr_mtllib_path, m, mtl_param_matcher);
    }},
    {"usemtl", [](Mesh& m, istringstream& iss) {
        if (!curr_surface_group.empty()) {
            auto it = m.mtlPool_.find(curr_mtl_name);
            if (it != m.mtlPool_.end()) {
                curr_surface_group.pMtl = &it->second;
            }
            m.surfaceGroups.push_back(curr_surface_group);
            curr_surface_group.clear();
        }
        iss >> curr_mtl_name;
    }},
    {"s", [](Mesh&  /*m*/, istringstream& iss) {
        string str;
        iss >> str;
        curr_surface_group.smooth = (str != "off");
    }},
};

const Mesh::ParamMatcher Mesh::mtl_param_matcher {
    {"newmtl", [](Mesh& m, istringstream& iss) {
        iss >> curr_mtl_name;
        m.mtlPool_.emplace(curr_mtl_name, Material{});
    }},
    {"Ns", [](Mesh& m, istringstream& iss) {
        iss >> m.mtlPool_[curr_mtl_name].Ns;
    }},
    {"Ka", [](Mesh& m, istringstream& iss) {
        glm::vec3 ka;
        iss >> ka.r >> ka.g >> ka.b;
        m.mtlPool_[curr_mtl_name].ambient = ka;
    }},
    {"Kd", [](Mesh& m, istringstream& iss) {
        glm::vec3 kd;
        iss >> kd.r >> kd.g >> kd.b;
        m.mtlPool_[curr_mtl_name].diffuse = kd;
    }},
    {"Ks", [](Mesh& m, istringstream& iss) {
        glm::vec3 ks;
        iss >> ks.r >> ks.g >> ks.b;
        m.mtlPool_[curr_mtl_name].specular = ks;
    }},
    {"Ke", [](Mesh& m, istringstream& iss) {
        glm::vec3 ke;
        iss >> ke.r >> ke.g >> ke.b;
        m.mtlPool_[curr_mtl_name].emssion = ke;
    }},
    {"Ni", [](Mesh& m, istringstream& iss) {
        iss >> m.mtlPool_[curr_mtl_name].Ni;
    }},
    {"illum", [](Mesh& m, istringstream& iss) {
        iss >> m.mtlPool_[curr_mtl_name].illumination;
    }},
    {"map_Kd", [](Mesh& m, istringstream& iss) {
        string temp;
        getline(iss, temp);
        trim(temp); // so if there's space in path, it'll be safe
        fs::path texture_path = temp; 

        if (texture_path.is_relative()) {
            texture_path = curr_mtllib_path.parent_path() / texture_path;
        }
        auto &curr_mtl = m.mtlPool_[curr_mtl_name];
        curr_mtl.loadTexture(curr_mtl.mapKd, texture_path);
    }},
};
// clang-format on

void Material::apply() const {
    if (this->applyCallback == nullptr) {
        if (Material::apply_cb_default == nullptr) {
            throw std::logic_error("need to register apply callback for class Material!");
        }
        apply_cb_default(*this);
    } else {
        this->applyCallback(*this);
    }
}

void Material::clearMaterial() {
    Material default_mtl;
    default_mtl.apply();
}

// NOLINTNEXTLINE(readability-make-member-function-const)
void Material::loadTexture(GLuint &tex, fs::path const &texFile) {
    if (this->texBinder == nullptr) {
        if (Material::tex_binder_default == nullptr) {
            throw std::logic_error("need to register texture binder callback for class Material!");
        }
        Material::tex_binder_default(tex, texFile);
    } else {
        this->texBinder(tex, texFile);
    }
}

Material::~Material() {
    glDeleteTextures(4, &(this->mapKa)); // delete all textures
}

MaterialPool loadMaterialPool(std::filesystem::path const &mtlPath) {
    Mesh m;
    Mesh::fileParser_(mtlPath, m, Mesh::mtl_param_matcher);
    return move(m.mtlPool_);
}

} // namespace loia