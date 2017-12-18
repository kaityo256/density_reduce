#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>

const double L = 100.0;
const int G = 51;

struct Atom {
  double x, y, z;
};

void
make_dat(const std::string filename, std::vector<Atom> &atoms) {
  std::vector<int> num(G * G * G, 0);
  const double s = L / G;
  for (auto a : atoms) {
    int ix = static_cast<int>(a.x / s);
    int iy = static_cast<int>(a.y / s);
    int iz = static_cast<int>(a.z / s);
    int index = ix + iy * G + iz * G * G;
    assert(index < G * G * G);
    num[index]++;
  }
  const double v_inv = 1.0 / s / s / s;
  std::vector<double> density(G * G * G);
  for (int i = 0; i < G * G * G; i++) {
    density[i] = static_cast<double>(num[i]) * v_inv;
  }
  std::ofstream ofs(filename);
  ofs.write((char*)density.data(), sizeof(double)*density.size());
  std::cout << "Saved as " << filename << std::endl;
}

void
load_dump(const std::string filename, std::vector<Atom> &atoms) {
  std::ifstream ifs(filename);
  ifs.seekg(0, ifs.end);
  int size = ifs.tellg() / sizeof(double) / 3;
  ifs.seekg(0, ifs.beg);
  atoms.resize(size);
  for (int i = 0; i < size; i++) {
    ifs.read((char*)(&atoms[i].x), sizeof(double));
    ifs.read((char*)(&atoms[i].y), sizeof(double));
    ifs.read((char*)(&atoms[i].z), sizeof(double));
  }
}

int
main(void) {
  std::vector<Atom> atoms;
  load_dump("test.dump", atoms);
  make_dat("test.dat", atoms);
}
