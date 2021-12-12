#ifndef _MESH_H_
#define _MESH_H_
namespace loia {

class Material {
private:
    using MtlCallback = void (*)(const Material &);
    using TexCallback = void (*)(GLuint &tex, std::filesystem::path const &texFile);

public:
    glm::vec3 ambient = {0.2, 0.2, 0.2}, diffuse = {0.8, 0.8, 0.8}, specular = {0, 0, 0},
              emssion = {0, 0, 0.1};
    GLuint mapKa, mapKd, mapKs, mapNs;
    float Ns = 50.0, Ni = 1.0; // NOLINT: `Ns` for reflection factor, `Ni` for optical density
    int illumination;

    // user should register callbacks
    static MtlCallback apply_cb_default;   // apply the material
    static TexCallback tex_binder_default; // read a pic and bind to texture-id
    MtlCallback applyCallback = nullptr;
    TexCallback texBinder = nullptr;

    static void applyMaterial(const Material &mtl);
    static void clearMaterial();

    void loadTexture(GLuint &tex, std::filesystem::path const &texFile);
    ~Material();
};

using MaterialPool = std::unordered_map<std::string, Material>;
MaterialPool loadMaterialPool(std::filesystem::path const &mtlPath);

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
    MaterialPool mtlPool_;
    static void fileParser_(std::filesystem::path const &path, Mesh &mesh,
                            ParamMatcher const &matcher);
    friend MaterialPool loadMaterialPool(std::filesystem::path const &mtlPath);

public:
    std::vector<glm::vec3> vertices; // (x, y, z)
    std::vector<glm::vec3> texture;  // (u, v, w)
    std::vector<glm::vec3> normals;  // (x, y, z)
    std::vector<SurfaceGroup> surfaceGroups;

    static Mesh loadMesh(std::filesystem::path const &model); // path to model.{mtl, obj}
    static void renderMesh(const Mesh &mesh, float zoom);
};

} // namespace loia
#endif // _MESH_H_
