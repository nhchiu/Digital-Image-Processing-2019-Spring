#include "texture_analysis.h"
#include <vector>
#include <utility>

const double impulse_response_arrays[9][3][3] = {
  { { 1.0 / 36,  2.0 / 36,
    1.0 / 36                                                                                  },
    { 2.0 / 36,  4.0 / 26,  2.0 / 36 },
    { 1.0 / 36,  2.0 / 36,  1.0 / 36 } },
  { { 1.0 / 12,  0.0 / 12,
    -1.0 / 12                                                                                 },
    { 2.0 / 12,  0.0 / 12,  -2.0 / 12 },
    { 1.0 / 12,  0.0 / 12,  -1.0 / 12 } },
  { { -1.0 / 12, 2.0 / 12,
    -1.0 / 1                                                                                  },
    { -2.0 / 12, 4.0 / 12,  -2.0 / 12 },
    { -1.0 / 12, 2.0 / 12,  -1.0 / 12 } },
  { { -1.0 / 12, -2.0 / 12,
    -1.0 / 1                                                                                  },
    { 0.0 / 12,  0.0 / 12,  0.0 / 12 },
    { 1.0 / 12,  2.0 / 12,  1.0 / 12 } },
  { { 1.0 / 4,   0.0 / 4,
    -1.0 / 4                                                                                  },
    { 0.0 / 4,   0.0 / 4,   0.0 / 4  },
    { -1.0 / 4,  0.0 / 4,   1.0 / 4  } },
  { { -1.0 / 4,  2.0 / 4,
    -1.0 / 4                                                                                  },
    { 0.0 / 4,   0.0 / 4,   0.0 / 4  },
    { 1.0 / 4,   -2.0 / 4,  1.0 / 4  } },
  { { -1.0 / 12, -2.0 / 12,
    -1.0 / 12                                                                                 },
    { 2.0 / 12,  4.0 / 12,  2.0 / 12  },
    { -1.0 / 12, -2.0 / 12, -1.0 / 12 } },
  { { -1.0 / 4,  0.0 / 4,
    1.0 / 4                                                                                   },
    { 2.0 / 4,   0.0 / 4,   -2.0 / 4  },
    { -1.0 / 4,  0.0 / 4,   1.0 / 4 } },
  { { 1.0 / 4,   -2.0 / 4,
    1.0 / 4                                                                                   },
    { -2.0 / 4,  4.0 / 4,   -2.0 / 4  },
    { 1.0 / 4,   -2.0 / 4,  1.0 / 4 } }
};


Texture::Texture() {}

Texture::~Texture() {}

void Texture::Analyze(const Image& img) {
  this->image = img;

  // Step 1 - Convolution
  for (int n = 0; n < 9; n++) {
    (this->image).Conv<3>((impulse_response_arrays[n]), &(microstructure_arrays[n]));
    microstructure_arrays[n].Rescale();
  }

  // Step 2 - Energy Computation
  for (int n = 0; n < 9; n++) {
    Image tmp(image.get_size(), 1);
    int   offset = (energy_window_size / 2);
    for (int i = 0; i < image.get_size(); i++)
      for (int j = 0; j < image.get_size(); j++) {
        int energy = 0.0, count = 0;
        for (int x = -offset; x < energy_window_size - offset; x++)
          for (int y = -offset; y < energy_window_size - offset; y++) {
            energy += (microstructure_arrays[n]).at(i + x, j + y, 0) * (microstructure_arrays[n]).at(i + x, j + y, 0);
            count++;
          }
        tmp.set(i, j, 0, (int)sqrt(energy / count));
      }
    tmp.Rescale();
    features[n] = tmp;
  }
}

void DuplicateTextureBlock(const Image& texture_block, Image& dst) {
  int block_size         = texture_block.get_size();
  int channel_number     = texture_block.get_channel();
  unsigned int min_error = 2000000000;
  int overlap;

  for (int ov = block_size / 10; ov < block_size / 2; ov++) {
    // Calculate error
    unsigned int error = 0;
    int count          = 0;
    for (int c = 0; c < channel_number; c++) {
      // Horizontal
      for (int x = 0; x < ov; x++)
        for (int y = 0; y < block_size; y++) {
          error +=
            ((texture_block.at(x, y, c) -
              texture_block.at((block_size - ov) + x, y, c)) *
             (texture_block.at(x, y, c) -
              texture_block.at((block_size - ov) + x, y, c)));
          count++;
        }

      // Vertical
      for (int x = 0; x < block_size; x++)
        for (int y = 0; y < ov; y++) {
          error +=
            ((texture_block.at(x, y, c) -
              texture_block.at(x, (block_size - ov) + y, c)) *
             (texture_block.at(x, y, c) -
              texture_block.at(x, (block_size - ov) + y, c)));
          count++;
        }
    }
    error /= count;
    if (error < min_error) {
      min_error = error;
      overlap   = ov;
    }
  }
  Image tmp1(2 * block_size - overlap, channel_number);
  Image tmp2(2 * block_size - overlap, channel_number);

  for (int c = 0; c < channel_number; c++)
    for (int i = 0; i < block_size; i++)
      for (int j = 0; j < block_size; j++) {
        tmp1.set(i, j, c, texture_block.at(i, j, c));
        tmp2.set(i, j + block_size - overlap, c, texture_block.at(i, j, c));
      }

  // Horizontal duplication
  vector<vector<int> > error1(block_size, vector<int>(overlap + 2, 0));
  vector<vector<int> > back_trace1(block_size, vector<int>(overlap + 2, 0));
  for (int i = 0; i < block_size; i++) {
    error1[i][0]           = channel_number * 255 * 255* (i + 1);
    error1[i][overlap + 1] = channel_number * 255 * 255* (i + 1);
  }
  for (int j = block_size - overlap; j < block_size; j++) {
    for (int c = 0; c < channel_number; c++) {
      error1[0][j - (block_size - overlap) + 1] +=
        ((tmp1.at(0, j, c) - tmp2.at(0, j, c)) *
         (tmp1.at(0, j, c) - tmp2.at(0, j, c)));
    }
  }
  for (int i = 1; i < block_size; i++)
    for (int j = block_size - overlap; j < block_size; j++) {
      for (int c = 0; c < channel_number; c++) {
        error1[i][j - (block_size - overlap) + 1] +=
          ((tmp1.at(i, j, c) - tmp2.at(i, j, c)) *
           (tmp1.at(i, j, c) - tmp2.at(i, j, c)));
      }
      if (error1[i - 1][j - (block_size - overlap)]  <
          error1[i - 1][j - (block_size - overlap) + 1]) {
        if (error1[i - 1][j - (block_size - overlap)]  <
            error1[i - 1][j - (block_size - overlap) + 2]) {
          error1[i][j - (block_size - overlap) + 1]     += error1[i - 1][j - (block_size - overlap)];
          back_trace1[i][j - (block_size - overlap) + 1] = j - (block_size - overlap);
        }
        else {
          error1[i][j - (block_size - overlap) + 1]     += error1[i - 1][j - (block_size - overlap) + 2];
          back_trace1[i][j - (block_size - overlap) + 1] = j - (block_size - overlap) + 2;
        }
      } else {
        if (error1[i - 1][j - (block_size - overlap) + 1]  <
            error1[i - 1][j - (block_size - overlap) + 2]) {
          error1[i][j - (block_size - overlap) + 1]     += error1[i - 1][j - (block_size - overlap) + 1];
          back_trace1[i][j - (block_size - overlap) + 1] = j - (block_size - overlap) + 1;
        }
        else {
          error1[i][j - (block_size - overlap) + 1]     += error1[i - 1][j - (block_size - overlap) + 2];
          back_trace1[i][j - (block_size - overlap) + 1] = j - (block_size - overlap) + 2;
        }
      }
    }
  int min_error1 = 2100000000;
  int min_j      = 0;
  for (int j = 1; j < overlap + 1; j++) {
    if (error1[block_size - 1][j] < min_error1) {
      min_error1 = error1[block_size - 1][j];
      min_j      = j;
    }
  }
  for (int i = block_size - 1; i >= 0; i--) {
    for (int j = block_size - overlap + min_j - 1; j < 2 * block_size - overlap; j++) {
      for (int c = 0; c < channel_number; c++)
        tmp1.set(i, j, c, tmp2.at(i, j, c));
    }
    min_j = back_trace1[i][min_j];
  }

  // Vertical duplication
  tmp2.clear();
  for (int c = 0; c < channel_number; c++)
    for (int i = 0; i < block_size; i++)
      for (int j = 0; j < 2 * block_size - overlap; j++) {
        tmp2.set(i + block_size - overlap, j, c, tmp1.at(i, j, c));
      }
  vector<vector<int> > error2(overlap + 2, vector<int>(2 * block_size - overlap, 0));
  vector<vector<int> > back_trace2(overlap + 2, vector<int>(2 * block_size - overlap, 0));
  for (int i = 0; i < 2 * block_size - overlap; i++) {
    error2[0][i]           = channel_number * 255 * 255 * (i + 1);
    error2[overlap + 1][i] = channel_number * 255 * 255 * (i + 1);
  }
  for (int i = block_size - overlap; i < block_size; i++) {
    for (int c = 0; c < channel_number; c++) {
      error2[i - (block_size - overlap) + 1][0] +=
        ((tmp1.at(i, 0, c) - tmp2.at(i, 0, c)) *
         (tmp1.at(i, 0, c) - tmp2.at(i, 0, c)));
    }
  }
  for (int j = 1; j < 2 * block_size - overlap; j++)
    for (int i = block_size - overlap; i < block_size; i++) {
      for (int c = 0; c < channel_number; c++) {
        error2[i - (block_size - overlap) + 1][j] +=
          ((tmp1.at(i, j, c) - tmp2.at(i, j, c)) *
           (tmp1.at(i, j, c) - tmp2.at(i, j, c)));
      }
      if (error2[i - (block_size - overlap)][j - 1]  <
          error2[i - (block_size - overlap) + 1][j - 1]) {
        if (error2[i - (block_size - overlap)][j - 1]  <
            error2[i - (block_size - overlap) + 2][j - 1]) {
          error2[i - (block_size - overlap) + 1][j]     += error2[i - (block_size - overlap)][j - 1];
          back_trace2[i - (block_size - overlap) + 1][j] = i - (block_size - overlap);
        }
        else {
          error2[i - (block_size - overlap) + 1][j]     += error2[i - (block_size - overlap) + 2][j - 1];
          back_trace2[i - (block_size - overlap) + 1][j] = i - (block_size - overlap) + 2;
        }
      } else {
        if (error2[i - (block_size - overlap) + 1][j - 1]  <
            error2[i - (block_size - overlap) + 2][j - 1]) {
          error2[i - (block_size - overlap) + 1][j]     += error2[i - (block_size - overlap) + 1][j - 1];
          back_trace2[i - (block_size - overlap) + 1][j] = i - (block_size - overlap) + 1;
        }
        else {
          error2[i - (block_size - overlap) + 1][j]     += error2[i - (block_size - overlap) + 2][j - 1];
          back_trace2[i - (block_size - overlap) + 1][j] = i - (block_size - overlap) + 2;
        }
      }
    }
  int min_error2 = 2100000000;
  int min_i      = 0;
  for (int i = 1; i < overlap + 1; i++) {
    if (error2[i][2 * block_size - overlap - 1] < min_error2) {
      min_error2 = error2[i][2 * block_size - overlap - 1];
      min_i      = i;
    }
  }
  for (int j = 2 * block_size - overlap - 1; j >= 0; j--) {
    for (int i = block_size - overlap + min_i - 1; i < 2 * block_size - overlap; i++) {
      for (int c = 0; c < channel_number; c++)
        tmp1.set(i, j, c, tmp2.at(i, j, c));
    }
    min_i = back_trace2[min_i][j];
  }

  // Assign to dst
  dst = tmp1;
}

void TextureSynthesis(const Image& texture,
                      const Image& mask,
                      int size, Image& dst) {
  int channel_number = texture.get_channel();

  int block_size           = 84;
  int overlap              = block_size / 6;
  unsigned int   min_error = 2000000000;
  pair<int, int> min_coordinate(0, 0);

  for (int i = 0; i < texture.get_size() - block_size; i++)
    for (int j = 0; j < texture.get_size() - block_size; j++) {
      // Check if the block fits in the mask
      bool fits = true;
      for (int x = 0; fits && x < block_size; x++)
        for (int y = 0; y < block_size; y++) {
          if (mask.at(i + x, j + y, 0) == 0) {
            fits = false;
            break;
          }
        }
      if (fits) {
        // Calculate error
        unsigned int error = 0;
        for (int c = 0; c < channel_number; c++) {
          // Horizontal
          for (int x = 0; x < overlap; x++)
            for (int y = 0; y < block_size; y++) {
              error +=
                ((texture.at(i + x, j + y, c) -
                  texture.at(i + (block_size - overlap) + x, j + y, c)) *
                 (texture.at(i + x, j + y, c) -
                  texture.at(i + (block_size - overlap) + x, j + y, c)));
            }

          // Vertical
          for (int x = 0; x < block_size; x++)
            for (int y = 0; y < overlap; y++) {
              error +=
                ((texture.at(i + x, j + y, c) -
                  texture.at(i + x, j + (block_size - overlap) + y, c)) *
                 (texture.at(i + x, j + y, c) -
                  texture.at(i + x, j + (block_size - overlap) + y, c)));
            }
        }
        if (error < min_error) {
          min_error      = error;
          min_coordinate = pair<int, int>(i, j);
        }
      }
    }

  Image texture_block(block_size, channel_number);
  for (int c = 0; c < channel_number; c++)
    for (int i = 0; i < block_size; i++)
      for (int j = 0; j < block_size; j++) {
        texture_block.set(i, j, c,
                          texture.at(min_coordinate.first + i,
                                     min_coordinate.second + j, c));
      }

  while (texture_block.get_size() < size) {
    DuplicateTextureBlock(texture_block, texture_block);
  }
  Image result(size, channel_number);
  for (int c = 0; c < channel_number; c++)
    for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++) {
        result.set(i, j, c,
                   texture_block.at(i, j, c));
      }
  dst = result;
}
