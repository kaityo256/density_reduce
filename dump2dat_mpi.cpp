#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#include <mpi.h>

const double L = 100.0;
const int G = 51;

template<class T>
void
myallreduce(void *s, void *r, int count) {
  printf("Unsupported Type\n");
  MPI_Abort(MPI_COMM_WORLD, 1);
}

template<> void myallreduce<int>(void *s, void *r, int count) {
  MPI_Allreduce(s, r, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
}

template <class T>
void
AllreduceVector(std::vector<T> &sendbuf, std::vector<T> &recvbuf) {
  const int count = sendbuf.size();
  recvbuf.resize(count);
  std::fill(recvbuf.begin(), recvbuf.end(), 0);
  myallreduce<T>(sendbuf.data(), recvbuf.data(), count);
}

struct Atom {
  double x, y, z;
  Atom(double _x, double _y, double _z) {
    x = _x;
    y = _y;
    z = _z;
  }
};

struct Myrect {
  double sx, sy, sz;
  double ex, ey, ez;
  Myrect(int rank, int d[3], double L) {
    int ix = rank % d[0];
    int iy = (rank / d[0]) % d[1];
    int iz = (rank / d[0] / d[1]);
    double gx = L / d[0];
    double gy = L / d[1];
    double gz = L / d[2];
    sx = gx * ix;
    ex = sx + gx;
    sy = gx * iy;
    ey = sy + gy;
    sz = gz * iz;
    ez = sz + gz;
  }
  bool is_inside(double x, double y, double z) {
    if (x < sx)return false;
    if (x >= ex)return false;
    if (y < sy)return false;
    if (y >= ey)return false;
    if (z < sz)return false;
    if (z >= ez)return false;
    return true;
  }
};

void
make_dat(const std::string filename, std::vector<Atom> &atoms, int rank) {
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
  std::vector<int> num_recv;
  AllreduceVector(num, num_recv);
  const double v_inv = 1.0 / s / s / s;
  std::vector<double> density(G * G * G);
  for (int i = 0; i < G * G * G; i++) {
    density[i] = static_cast<double>(num_recv[i]) * v_inv;
  }
  if (rank == 0) {
    std::ofstream ofs(filename);
    ofs.write((char*)density.data(), sizeof(double)*density.size());
    std::cout << "Saved as " << filename << std::endl;
  }
}

void
load_dump(const std::string filename, std::vector<Atom> &atoms, Myrect &rect) {
  std::ifstream ifs(filename);
  ifs.seekg(0, ifs.end);
  int size = ifs.tellg() / sizeof(double) / 3;
  ifs.seekg(0, ifs.beg);
  for (int i = 0; i < size; i++) {
    double x, y, z;
    ifs.read((char*)(&x), sizeof(double));
    ifs.read((char*)(&y), sizeof(double));
    ifs.read((char*)(&z), sizeof(double));
    if (rect.is_inside(x, y, z)) {
      atoms.push_back(Atom(x, y, z));
    }
  }
}

void
save_txt(int rank, std::vector<Atom> &atoms) {
  char filename[256];
  sprintf(filename, "rank%02d.txt", rank);
  std::ofstream ofs(filename);
  for (auto &a : atoms) {
    ofs << a.x << " " << a.y << " " << a.z << std::endl;
  }
}

int
main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  int d[3] = {};
  MPI_Dims_create(size, 3, d);
  Myrect rect(rank, d, L);
  std::vector<Atom> atoms;
  load_dump("test.dump", atoms, rect);
  save_txt(rank, atoms);
  make_dat("test_mpi.dat", atoms, rank);
  MPI_Finalize();
}
