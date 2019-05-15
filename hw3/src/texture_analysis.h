#ifndef _TEXTURE_ANALYSIS_H_
#define _TEXTURE_ANALYSIS_H_

#include "image.h"

using namespace std;

class Texture {
private:

  Image image;
  Image microstructure_arrays[9];
  Image features[9];
  int energy_window_size = 19;

public:

  Texture();
  ~Texture();

  void   Analyze(const Image& img);

  Image& get_microstructure_arrays(int n) {
    return microstructure_arrays[n];
  }

  Image& get_features(int n) {
    return features[n];
  }
};

void TextureSynthesis(const Image& texture, const Image& mask, int size, Image& dst);

#endif // _TEXTURE_ANALYSIS_H_
