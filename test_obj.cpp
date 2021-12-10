#include "mesh.h"
#include <chrono>
#include <iostream>

using namespace std;

int main() {
    auto start = chrono::steady_clock::now();
    // const auto mesh = Mesh::loadMesh("../simple_car.obj");
    // const auto mesh = Mesh::loadMesh("../awesome_car.obj"); // 350+MB, take ~1min to load it
    const auto mesh = Mesh::loadMesh("../assets/simple_car.obj"); // cubic, used for test
    auto end = chrono::steady_clock::now();

    size_t size = 0;
    for (const auto &f_group : mesh.surfaceGroups) {
        size += f_group.size();
    }
    cout << "load " << mesh.surfaceGroups.size() << " surface groups(" << size << " surfaces) in "
         << (chrono::duration_cast<chrono::milliseconds>(end - start)).count() << " milliseconds"
         << endl;
}