#include "material.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

void Material::applyMaterial(const Material &mtl) {
    glMaterialfv(GL_FRONT, GL_AMBIENT, value_ptr(mtl.ambient));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, value_ptr(mtl.diffuse));
    glMaterialfv(GL_FRONT, GL_SPECULAR, value_ptr(mtl.specular));
    glMaterialfv(GL_FRONT, GL_EMISSION, value_ptr(mtl.emssion));
    glMaterialf(GL_FRONT, GL_SHININESS, mtl.Ns);
    glBindTexture(GL_TEXTURE_2D, mtl.mapKd);
    glEnable(GL_TEXTURE_2D);
}

void Material::clearMaterial() {
    Material default_mtl;
    applyMaterial(default_mtl);
}

void Material::loadTexture(GLuint &tex, std::filesystem::path const &texFile) {
    cv::Mat img = cv::imread(texFile.string());
    cv::flip(img, img, 0);
    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, img.cols, img.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
    // glTexImage2D(GL_TEXTURE_2D, 0, 3, img.cols, img.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE,
    // img.data);
    glBindTexture(GL_TEXTURE_2D, 0); // switch back to default texture
    img.release();
}

Material::~Material() {
    glDeleteTextures(4, &(this->mapKa)); // delete all textures
}
