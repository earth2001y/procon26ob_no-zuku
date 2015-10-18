#ifndef __STATE_H__
#define __STATE_H__

#include <vector>
#include <string>
#include "piece.h"

class state
{
  static const size_t field_size_X = 32;
  static const size_t field_size_Y = 32;

  const char* field_mask; // char[field_size_Y+2][field_size_Y+2]

public:
  // stage cache
  char face[field_size_Y][field_size_X];
  char edge_x[field_size_Y][field_size_X+1];
  char edge_y[field_size_Y+1][field_size_Y];
  unsigned int zk_count;
  unsigned int put_count;

  // boundary rect
  size_t x0,x1,y0,y1;

  char spaced[field_size_Y];

public:
  state(const char* mask);
  state(const state& s);

  inline char& face_at(const size_t i, const size_t j) {
    return face[j][i];
  }

  std::string dump_face() const;
  std::string dump_edge_x() const;
  std::string dump_edge_y() const;

public:

  int put_piece(const piece* p, const int x,  const int y);

protected:

};

#endif // __STATE_H__

