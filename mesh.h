#ifndef _MESH_H_
#define _MESH_H_

#include "material.h"

class Mesh {
public:
    struct Surface {
        int vertexId[3];
        int textureId[3];
        int normalId[3];
    };

    struct SurfaceGroup : public std::vector<Surface> {
        Material *pMtl = nullptr;
        bool smooth;
    };

private:
    using ParamMatcher =
        std::unordered_map<std::string_view, auto (*)(Mesh &m, std::istringstream &)->void>;
    const static ParamMatcher obj_param_matcher, mtl_param_matcher;
    std::unordered_map<std::string, Material> mtlPool_;
    static void fileParser_(std::filesystem::path const &path, Mesh &mesh,
                            ParamMatcher const &matcher);

public:
    std::vector<glm::vec3> vertices; // (x, y, z)
    std::vector<glm::vec3> texture;  // (u, v, w)
    std::vector<glm::vec3> normals;  // (x, y, z)
    std::vector<SurfaceGroup> surfaceGroups;

    static Mesh loadMesh(std::filesystem::path const &model); // path to model.{mtl, obj}
    static void renderMesh(const Mesh &mesh, float zoom);
};

#endif // _MESH_H_
