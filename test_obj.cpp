#include "mesh.h"
#include <chrono>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace std;

void texBind(GLuint &tex, std::filesystem::path const &texFile) {
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

void applyMtl(const Material &mtl) {
    glMaterialfv(GL_FRONT, GL_AMBIENT, value_ptr(mtl.ambient));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, value_ptr(mtl.diffuse));
    glMaterialfv(GL_FRONT, GL_SPECULAR, value_ptr(mtl.specular));
    glMaterialfv(GL_FRONT, GL_EMISSION, value_ptr(mtl.emssion));
    glMaterialf(GL_FRONT, GL_SHININESS, mtl.Ns);
    glBindTexture(GL_TEXTURE_2D, mtl.mapKd);
    glEnable(GL_TEXTURE_2D);
}

int main() {
    Material::tex_binder_default = texBind;
    auto start = chrono::steady_clock::now();
    // const auto mesh = Mesh::loadMesh("../simple_car.obj");
    // const auto mesh = Mesh::loadMesh("../awesome_car.obj"); // 350+MB, take ~1min to load it
    const auto mesh = Mesh::loadMesh("../examples/simple_car.obj");
    auto end = chrono::steady_clock::now();

    size_t size = 0;
    for (const auto &f_group : mesh.surfaceGroups) {
        size += f_group.size();
    }
    cout << "load " << mesh.surfaceGroups.size() << " surface groups(" << size << " surfaces) in "
         << (chrono::duration_cast<chrono::milliseconds>(end - start)).count() << " milliseconds"
         << endl;
}