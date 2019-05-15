#include "image.h"

Image::Image() {
  this->data = new int[this->channel * this->size * this->size];
  memset(this->data, 0, channel * size * size * sizeof(int));
}

Image::Image(int size, int channel = 1) {
  this->size    = size;
  this->channel = channel;
  this->data    = new int[channel * size * size];
  memset(this->data, 0, channel * size * size * sizeof(int));
}

Image::Image(const Image& img) {
  this->size    = img.get_size();
  this->channel = img.get_channel();
  this->data    = new int[this->channel * this->size * this->size];
  std::copy(img.data, img.data + img.channel * img.size * img.size, this->data);
}

Image& Image::operator=(const Image& img) {
  if (this != &img) {
    if ((this->size != img.get_size()) || (this->channel != img.get_channel())) {
      this->size    = img.get_size();
      this->channel = img.get_channel();
    }
    if (this->data != nullptr) {
      delete[] this->data;
      this->data = nullptr;
    }
    this->data = new int[this->channel * this->size * this->size];
    std::copy(img.data, img.data + (img.channel * img.size * img.size), this->data);
  }
  return *this;
}

Image& Image::operator+=(const Image& img) {
  if ((img.get_channel() >= this->channel) && (img.get_size() >= this->size)) {
    for (int c = 0; c < this->channel; c++)
      for (int i = 0; i < this->size; i++)
        for (int j = 0; j < this->size; j++) {
          this->set(i, j, c, this->at(i, j, c) + img.at(i, j, c));
        }
  }
  return *this;
}

Image& Image::operator-=(const Image& img) {
  if ((img.get_channel() >= this->channel) && (img.get_size() >= this->size)) {
    for (int c = 0; c < this->channel; c++)
      for (int i = 0; i < this->size; i++)
        for (int j = 0; j < this->size; j++) {
          this->set(i, j, c, this->at(i, j, c) - img.at(i, j, c));
        }
  }
  return *this;
}

Image::~Image() {
  delete[] (this->data);
}

int Image::at(int i, int j, int c = 0) const {
  if ((c < 0) || (c >= this->channel)) return 0;

  int m, n;

  if (i < 0) {
    m = 0;
  } else if (i < size) {
    m = i;
  } else {
    m = size - 1;
  }
  if (j < 0) {
    n = 0;
  } else if (j < size) {
    n = j;
  } else {
    n = size - 1;
  }
  return data[c * size * size + m * size + n];
}

int Image::at(double i, double j, int c = 0) const {
  int p    = (int)floor(i);
  int q    = (int)floor(j);
  double a = i - floor(i);
  double b = j - floor(j);

  return (1 - a) * (1 - b) * at(p, q, c) +
         (1 - a) * b * at(p, q + 1, c) +
         a * (1 - b) * at(p + 1, q, c) +
         a * b * at(p + 1, q + 1, c);
}

void Image::set(int i, int j, int c, int value) {
  if ((0 <= i) && (i < size) && (0 <= j) && (j < size) && (0 <= c) && (c < channel)) {
    this->data[c * size * size + i * size + j] = value;
  }
}

void Image::set(int i, int j, int value) {
  if ((0 <= i) && (i < size) && (0 <= j) && (j < size)) {
    for (int c = 0; c < this->channel; c++) {
      this->data[c * size * size + i * size + j] = value;
    }
  }
}

void Image::set(int i, int j, double value) {
  if ((0 <= i) && (i < size) && (0 <= j) && (j < size)) {
    for (int c = 0; c < this->channel; c++) {
      this->data[c * size * size + i * size + j] = (int)(value * 255.0);
    }
  }
}

void Image::set(int i, int j, int value[3]) {
  if ((0 <= i) && (i < size) && (0 <= j) && (j < size) && (this->channel == 3) && (value != nullptr)) {
    for (int c = 0; c < this->channel; c++) {
      this->data[c * size * size + i * size + j] = value[c];
    }
  }
}

void Image::set(int i, int j, double value[3]) {
  if ((0 <= i) && (i < size) && (0 <= j) && (j < size) && (this->channel == 3) && (value != nullptr)) {
    for (int c = 0; c < this->channel; c++) {
      this->data[c * size * size + i * size + j] = (int)(value[c] * 255.0);
    }
  }
}

void Image::clear() {
  memset(this->data, 0, channel * size * size * sizeof(int));
}

void Image::Clamp() {
  for (int i = 0; i < channel * size * size; i++) {
    data[i] = min(255, max(data[i], 0));
  }
}

void Image::Rescale() {
  int max_value = 0, min_value = 255;

  for (int i = 0; i < channel * size * size; i++) {
    if (data[i] > max_value) max_value = data[i];
    if (data[i] < min_value) min_value = data[i];
  }
  double range = max_value - min_value;
  for (int i = 0; i < channel * size * size; i++) {
    data[i] = (int)((data[i] - min_value) * 255 / range);
  }
}

void Image::Binarize(int threshold) {
  for (int i = 0; i < channel * size * size; i++) {
    data[i] = (data[i] > threshold) ?  255 : 0;
  }
}

void Image::Binarize(double threshold) {
  int int_threshold = (int)(threshold * 255.0);

  Binarize(int_threshold);
}

void Image::Intersection(const Image& img) {
  if ((size != img.get_size()) || (channel != img.get_channel())) return;

  for (int c = 0; c < channel; c++)
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        set(i, j, c, min(this->at(i, j, c), img.at(i, j, c)));
      }
    }
}

void Image::Union(const Image& img) {
  if ((size != img.get_size()) || (channel != img.get_channel())) return;

  for (int c = 0; c < channel; c++)
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        set(i, j, c, max(this->at(i, j, c), img.at(i, j, c)));
      }
    }
}

void Image::Complement() {
  for (int i = 0; i < channel * size * size; i++) {
    data[i] = 255 - data[i];
  }
}

int open_file(FILE *& p_file, const char *filename, const char *mode = "wb") {
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
  unsigned char tmp_data[channel * size * size];
  fread(tmp_data, sizeof(unsigned char), channel * size * size, file);
  for (int i = 0; i < channel * size * size; i++) {
    data[i] = (int)tmp_data[i];
  }
  fclose(file);
  return 1;
}

int Image::WriteRaw(const char *path) {
  FILE *file;

  if (!open_file(file, path, "wb")) {
    return 0;
  }
  unsigned char tmp_data[channel * size * size];
  for (int i = 0; i < channel * size * size; i++) {
    tmp_data[i] = (unsigned char)min(255, max(data[i], 0));
  }
  fwrite(tmp_data, sizeof(unsigned char), channel * size * size, file);
  fclose(file);
  return 1;
}

int Image::Bond(int x, int y) {
  int four_connected[4] = { at(x,     y + 1,     0),
                            at(x - 1, y,     0),
                            at(x,     y - 1, 0),
                            at(x + 1, y,     0) };
  int eight_connected[4] = { at(x - 1, y + 1, 0),
                             at(x - 1, y - 1, 0),
                             at(x + 1, y - 1, 0),
                             at(x + 1, y + 1, 0) };
  int bond = 0;

  for (int i = 0; i < 4; i++) {
    if (four_connected[i] > 0) bond += 2;
    if (eight_connected[i] > 0) bond += 1;
  }
  return bond;
}

int Image::NeighborPixelStack(int x, int y) {
  int neighbor_stack[8] = { at(x,     y + 1,     0),
                            at(x - 1, y + 1, 0),
                            at(x - 1, y,     0),
                            at(x - 1, y - 1, 0),
                            at(x,     y - 1, 0),
                            at(x + 1, y - 1, 0),
                            at(x + 1, y,     0),
                            at(x + 1, y + 1, 0) };
  int stack = 0;

  for (int i = 0; i < 8; i++) {
    stack = stack << 1;
    if (neighbor_stack[i] > 0)
      stack |= 0b00000001;
  }
  return stack;
}
