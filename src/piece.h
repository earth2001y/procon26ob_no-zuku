#ifndef __PIECE_H__
#define __PIECE_H__

#include <string>

class piece {
public:
  static const size_t size_X = 8;
  static const size_t size_Y = 8;
  enum NORM { T=0, H };

  int  index;
  unsigned int zk_count;

  // boundary rect
  size_t x0,x1,y0,y1;
  int filled[size_Y];

  // operated info
  NORM norm;
  int  rotation;

  char face[piece::size_Y][piece::size_X];
  char edge_x[piece::size_Y][piece::size_X+1];
  char edge_y[piece::size_Y+1][piece::size_Y];

public:
  piece();
  piece(const piece& p);
  piece(const std::string& s, const int idx);

  inline const char& face_at(const size_t i, const size_t j) const {
    return face[j][i];
  }

  std::string dump_face() const;
  std::string dump_edge_x() const;
  std::string dump_edge_y() const;

  //
  void calc_edge();
  void calc_boundary_rect();
  int  diff(const piece& p) const;

public:
  piece turn() const;
  piece rotate() const;

};

#endif // __PIECE_H__

