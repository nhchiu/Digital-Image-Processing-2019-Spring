#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cmath>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <tuple>

using namespace std;

#define SIZE 256

class Image {
private:

  int size    = SIZE;
  int channel = 1;
  int *data;

public:

  Image();
  Image(int size, int channel);
  Image(const Image& img);
  ~Image();
  Image& operator      =(const Image& img);
  Image& operator      +=(const Image& img);
  Image& operator      -=(const Image& img);
  friend Image operator+(Image lhs, const Image& rhs) {
    lhs += rhs;
    return lhs;
  }

  friend Image operator-(Image lhs, const Image& rhs) {
    lhs += rhs;
    return lhs;
  }

  friend bool operator==(const Image& l, const Image& r) {
    if ((l.size != r.size) || (l.channel != r.channel)) {
      return false;
    } else {
      return memcmp(l.data, r.data, l.channel * l.size * l.size * sizeof(int)) == 0;
    }
  }

   friend bool operator!=(const Image& lhs, const Image& rhs) {
    return !(lhs == rhs);
  }

  int get_size() const {
    return this->size;
  }

  int get_channel() const {
    return this->channel;
  }

  int  at(int i, int j, int c) const;
  int  at(double i, double j, int c) const;
  void set(int i, int j, int c, int value);
  void set(int i, int j, int value);
  void set(int i, int j, double value);
  void set(int i, int j, int value[3]);
  void set(int i, int j, double value[3]);
  void clear();

  void Clamp();
  void Rescale();
  void Binarize(int threshold);
  void Binarize(double threshold);
  void Intersection(const Image& img);
  void Union(const Image& img);
  void Complement();

  int  ReadRaw(const char *path);
  int  WriteRaw(const char *path);

  template<int s>
  void Conv(const double (& filter)[s][s], Image *dst = nullptr) {
    if (dst == nullptr) dst = this;

    // Calculate the range after filtering
    Image tmp    = Image(size, channel);
    int   offset = (s / 2);
    for (int c = 0; c < channel; c++)
      for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
          double sum = 0;
          for (int m = -offset; m < s - offset; m++) {
            for (int n = -offset; n < s - offset; n++) {
              sum += this->at(i + m, j + n, c) * filter[m + offset][n + offset];
            }
          }
          tmp.set(i, j, c, (int)sum);
        }
      }
    *dst = tmp;
  }


  int  Bond(int x, int y);
  int  NeighborPixelStack(int x, int y);
};

#endif // ifndef _IMAGE_H_
