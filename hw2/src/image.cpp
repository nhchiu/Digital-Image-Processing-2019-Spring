#include "image.h"

Image::Image() { this->data = new unsigned char[this->size * this->size]; }

Image::Image(int size) {
  this->size = size;
  this->data = new unsigned char[size * size];
}

Image::Image(const Image &img) {
  this->size = img.get_size();
  this->data = new unsigned char[size * size];
  for (int i = 0; i < this->size; i++)
    for (int j = 0; j < this->size; j++) {
      this->set(i, j, img.at(i, j));
    }
}

Image &Image::operator=(const Image &img) {
  this->size = img.get_size();
  this->data = new unsigned char[size * size];
  for (int i = 0; i < this->size; i++)
    for (int j = 0; j < this->size; j++) {
      this->set(i, j, img.at(i, j));
    }
  return *this;
}

Image::~Image() { delete (this->data); }

unsigned char Image::at(int i, int j) const {
  int m, n;

  if (i < 0) {
    m = -i;
  } else if (i < size) {
    m = i;
  } else {
    m = 2 * size - i - 2;
  }
  if (j < 0) {
    n = -j;
  } else if (j < size) {
    n = j;
  } else {
    n = 2 * size - j - 2;
  }
  return data[m * size + n];
}

unsigned char Image::at(double i, double j) const {
  int p = (int)floor(i);
  int q = (int)floor(j);
  double a = i - floor(i);
  double b = j - floor(j);

  return (unsigned char)((1 - a) * (1 - b) * at(p, q) +
                         (1 - a) * b * at(p, q + 1) +
                         a * (1 - b) * at(p + 1, q) + a * b * at(p + 1, q + 1));
}

void Image::set(int i, int j, unsigned char value) {
  if ((0 <= i) && (i < size) && (0 <= j) && (j < size))
    this->data[i * size + j] = value;
}

int open_file(FILE *&p_file, const char *filename, const char *mode = "wb") {
  if (!(p_file = fopen(filename, mode))) {
    fprintf(stderr, "Cannot open file %s!\n", filename);
    return 0;
  } else {
    return 1;
  }
}

int Image::ReadRaw(const char *path) {
  FILE *file;

  if (!open_file(file, path, "rb")) {
    return 0;
  }
  fread(data, sizeof(unsigned char), size * size, file);
  fclose(file);
  return 1;
}

int Image::WriteRaw(const char *path) {
  FILE *file;

  if (!open_file(file, path, "wb")) {
    return 0;
  }
  fwrite(data, sizeof(unsigned char), size * size, file);
  fclose(file);
  return 1;
}
