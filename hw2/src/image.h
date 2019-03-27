#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cmath>
#include <cstdio>

using namespace std;

#define SIZE 256

class Image {
 private:
  int size = SIZE;
  unsigned char *data;

 public:
  Image();
  Image(int size);
  Image(const Image &img);
  ~Image();
  Image &operator=(const Image &img);

  int get_size() const { return this->size; }
  unsigned char at(int i, int j) const;
  unsigned char at(double i, double j) const;
  void set(int i, int j, unsigned char value);

  int ReadRaw(const char *path);
  int WriteRaw(const char *path);

  template <int s>
  void Conv(double (&filter)[s][s], double offset = 0, Image *dst = nullptr) {
    if (dst == nullptr) dst = this;
    // Calculate the range after filtering
    Image tmp = Image(get_size());
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        double sum = 0;
        for (int m = -(s / 2); m <= (s / 2); m++) {
          for (int n = -(s / 2); n <= (s / 2); n++) {
            sum += this->at(i + m, j + n) * filter[m + (s / 2)][n + (s / 2)];
          }
        }
        tmp.set(i, j, (unsigned char)fmin(255.0, fmax((sum + offset), 0.0)));
      }
    }
    *dst = tmp;
  }
};

#endif
