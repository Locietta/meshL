#include "mesh.h"
#include "obj_loader.h"
#include <chrono>
#include <glm/vec4.hpp>
#include <iostream>

using namespace std;

ObjIniter obj_initer;

loia::Mesh car;

struct Light {
    glm::vec4 pos, ambient, diffuse, specular;

    static void applyLight(GLenum lightId, const Light &light) {
        glLightfv(lightId, GL_POSITION, glm::value_ptr(light.pos));
        glLightfv(lightId, GL_AMBIENT, glm::value_ptr(light.ambient));
        glLightfv(lightId, GL_DIFFUSE, glm::value_ptr(light.diffuse));
        glLightfv(lightId, GL_SPECULAR, glm::value_ptr(light.specular));
        glEnable(lightId);
    }
};

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    auto start = chrono::steady_clock::now();
    car = loia::Mesh::loadMesh("../assets/simple_car.obj");
    auto end = chrono::steady_clock::now();

    size_t size = 0;
    for (const auto &f_group : car.surfaceGroups) {
        size += f_group.size();
    }
    cout << "load " << car.surfaceGroups.size() << " surface groups(" << size << " surfaces) in "
         << (chrono::duration_cast<chrono::milliseconds>(end - start)).count() << " milliseconds" << endl;
}

void display(void) {
    // clear window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Light sun_light = {{0.0, 0, 0, 1.0},          // pos
                       {0.9F, 0.9F, 0.9F, 1.0F},  // ambient
                       {0.3F, 0.0F, 0.0F, 0.8F},  // diffuse
                       {1.0F, 0.7F, 0.2F, 1.0F}}; // specular

    Light::applyLight(GL_LIGHT0, sun_light);

    // draw
    glRotatef(45, 0, 1, 0);
    loia::Mesh::renderMesh(car, 0.1);
    glTranslatef(0, 0, -10);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // refresh window
    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(800, 600);
    glutCreateWindow("test mesh loader");
    init();

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}