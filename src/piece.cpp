
#include <iostream>
#include <sstream>
#include <complex>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include "piece.h"
#include "utils.h"

piece::piece():
  index(0),zk_count(0),rotation(0),norm(piece::T),
  x0(0), x1(size_X-1), y0(0), y1(size_Y-1)
{
  std::memset(face,0,sizeof(face));
  std::memset(edge_x,0,sizeof(edge_x));
  std::memset(edge_y,0,sizeof(edge_y));
  std::memset(filled,0,sizeof(filled));
}

piece::piece(const piece& s):
  index(s.index), zk_count(s.zk_count),
  rotation(s.rotation),norm(s.norm),
  x0(s.x0), x1(s.x1), y0(s.y0), y1(s.y1)
{
  memcpy(face,s.face,sizeof(face));
  memcpy(edge_x,s.edge_x,sizeof(edge_x));
  memcpy(edge_y,s.edge_y,sizeof(edge_y));
  memcpy(filled,s.filled,sizeof(filled));
}

piece::piece(const std::string& s, const int idx):
  index(idx),rotation(0),norm(piece::T)
{
  std::stringstream ss;
  std::memset(face,0,sizeof(face));
  ss << s;

  zk_count = 0;
  for (size_t j = 0; j < piece::size_Y; j++) {
    for (size_t i = 0; i < piece::size_X; i++) {
      char c;
      do {
        ss >> c;
        face[j][i] = (c != EOF)? (c-'0'):0;
        zk_count += face[j][i];
      } while(c != '1' && c != '0' && c != EOF);
    }
  }
  calc_edge();
  calc_boundary_rect();
}

std::string piece::dump_face() const
{
  std::stringstream ss;
  for (size_t j = 0; j < piece::size_Y; j++) {
    for (size_t i = 0; i < piece::size_X; i++) {
      ss << static_cast<int>(face[j][i]);
    }
    ss << std::endl;
  }
  return ss.str();
}

std::string piece::dump_edge_x() const
{
  std::stringstream ss;
  for (size_t j = 0; j < piece::size_Y; j++) {
    for (size_t i = 0; i < piece::size_X+1; i++) {
      ss << static_cast<int>(edge_x[j][i]);
    }
    ss << std::endl;
  }
  return ss.str();
}

std::string piece::dump_edge_y() const
{
  std::stringstream ss;
  for (size_t j = 0; j < piece::size_Y+1; j++) {
    for (size_t i = 0; i < piece::size_X; i++) {
      ss << static_cast<int>(edge_y[j][i]);
    }
    ss << std::endl;
  }
  return ss.str();
}

void piece::calc_edge()
{
  for (size_t j = 0; j < piece::size_Y; j++) {
  for (size_t i = 1; i < piece::size_X; i++) {
    edge_x[j][i] = std::abs(face_at(i-1,j) - face_at(i,j));
  }}
  for (size_t j = 0; j < piece::size_Y; j++) {
    edge_x[j][0] = face_at(1,j);
    edge_x[j][piece::size_X] = face_at(piece::size_X-1,j);
  }
  for (size_t j = 1; j < piece::size_Y; j++) {
  for (size_t i = 0; i < piece::size_X; i++) {
    edge_y[j][i] = std::abs(face_at(i,j-1) - face_at(i,j));
  }}
  for (size_t i = 0; i < piece::size_X; i++) {
    edge_y[0][i] = face_at(i,1);
    edge_y[piece::size_Y][i] = face_at(i,piece::size_Y-1);
  }
  return;
}

void piece::calc_boundary_rect()
{
  for (x0 = 0; x0 < piece::size_X; x0++) {
    char r = 0;
    for (size_t j = 0; j < piece::size_Y; j++) { r += face_at(x0,j); }
    if (r != 0) break;
  }
  for (x1 = piece::size_X-1; x1 != 0; x1--) {
    char r = 0;
    for (size_t j = 0; j < piece::size_Y; j++) { r += face_at(x1,j); }
    if (r != 0) break;
  }
  for (y0 = 0; y0 < piece::size_Y; y0++) {
    char r = 0;
    for (size_t i = 0; i < piece::size_X; i++) { r += face_at(i,y0); }
    if (r != 0) break;
  }
  for (y1 = piece::size_Y-1; y1 != 0; y1--) {
    char r = 0;
    for (size_t i = 0; i < piece::size_X; i++) { r += face_at(i,y1); }
    if (r != 0) break;
  }
  for (int j = 0; j < piece::size_Y; j++) {
    filled[j] = 0;
    for (size_t i = 0; i < piece::size_X; i++) {
      filled[j] += face_at(i,j);
    }
  }
  return;
}

int piece::diff(const piece& p) const
{
  if ((x1-x0) != (p.x1-p.x0)) return -1;
  if ((y1-y0) != (p.y1-p.y0)) return -1;

  int r = 0;
  for (size_t j = 0; j < y1 - y0 + 1; j++) {
  for (size_t i = 0; i < x1 - x0 + 1; i++) {
    r += std::abs(face_at(x0+i,y0+j) - p.face_at(p.x0+i,p.y0+j));
  }}
  return r;
}

piece piece::turn() const
{
  piece p = *this;
  p.norm = (norm == piece::T)? piece::H: piece::T;
  for (size_t j = 0; j < piece::size_Y; j++) {
  for (size_t i = 0; i < piece::size_X; i++) {
    p.face[j][i] = this->face[j][piece::size_X-i-1];
  }}
  p.calc_edge();
  p.calc_boundary_rect();
  return p;
}

piece piece::rotate() const
{
  piece p = *this;
  p.rotation = this->rotation + 90;
  for (size_t j = 0; j < piece::size_Y; j++) {
  for (size_t i = 0; i < piece::size_X; i++) {
    p.face[j][i] = this->face[piece::size_X-i-1][j];
  }}
  p.calc_edge();
  p.calc_boundary_rect();
  return p;
}

