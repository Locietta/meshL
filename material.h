#ifndef _MATERIAL_H_
#define _MATERIAL_H_

class Material {
public:
    GLuint mapKa = 0, mapKd = 0, mapKs = 0, mapNs = 0;
    glm::vec3 ambient = {0.2, 0.2, 0.2}, diffuse = {0.8, 0.8, 0.8}, specular = {0, 0, 0},
              emssion = {0, 0, 0.1};
    float Ns = 50.0, Ni = 1.0; // NOLINT: `Ns` for reflection factor, `Ni` for optical density
    int illumination;
    static void applyMaterial(const Material &mtl);
    static void clearMaterial();
    static void loadTexture(GLuint &tex, std::filesystem::path const &texFile);
    ~Material();
};

#endif // _MATERIAL_H_
