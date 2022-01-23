
#include "obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glu.h>

static void texBind(GLuint &tex, std::filesystem::path const &texFile) {
    int width, height, nr_channels;
    unsigned char *data = stbi_load(texFile.string().c_str(), &width, &height, &nr_channels, STBI_rgb);
    glEnable(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0); // switch back to default texture
    stbi_image_free(data);
}

static void applyMtl(const loia::Material &mtl) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, value_ptr(mtl.ambient));
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, value_ptr(mtl.diffuse));
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, value_ptr(mtl.specular));
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, value_ptr(mtl.emssion));
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mtl.Ns);
    glBindTexture(GL_TEXTURE_2D, mtl.mapKd);
    glEnable(GL_TEXTURE_2D);
}

ObjIniter::ObjIniter() {
    loia::Material::tex_binder_default = texBind;
    loia::Material::apply_cb_default = applyMtl;
}