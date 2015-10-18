
#include <cstring>
#include <complex>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "state.h"
#include "utils.h"

state::state(const char* mask):
  field_mask(mask),
  zk_count(0), put_count(0),
  x0(0),x1(field_size_X-1),y0(0),y1(field_size_Y-1)
{
  memset(face,0,sizeof(face));
  memset(edge_x,0,sizeof(edge_x));
  memset(edge_y,0,sizeof(edge_y));
  if (field_mask) {
    for (size_t j = 0; j < field_size_Y; j++) {
      spaced[j] = field_size_X;
      for (size_t i = 0; i < field_size_X; i++) {
        spaced[j] -= face[j][i];
        spaced[j] -= char(1) - field_mask[(j*field_size_X)+i];
      }
    }
  }
}

state::state(const state& s):
  field_mask(s.field_mask),
  zk_count(s.zk_count), put_count(s.put_count),
  x0(s.x0),x1(s.x1),y0(s.y0),y1(s.y1)
{
  memcpy(face,s.face,sizeof(face));
  memcpy(edge_x,s.edge_x,sizeof(edge_x));
  memcpy(edge_y,s.edge_y,sizeof(edge_y));
  memcpy(spaced,s.spaced,sizeof(spaced));
}

std::string state::dump_face() const
{
  std::stringstream ss;
  for (size_t j = 0; j < field_size_Y; j++) {
    for (size_t i = 0; i < field_size_X; i++) {
      if (field_mask[j*32+i] == 0) {
        ss << '#';
      } else {
        ss << static_cast<int>(face[j][i]);
      }
    }
    ss << std::endl;
  }
  return ss.str();
}

std::string state::dump_edge_x() const
{
  std::stringstream ss;
  for (size_t j = 0; j < field_size_Y; j++) {
    for (size_t i = 0; i < field_size_X+1; i++) {
      ss << static_cast<int>(edge_x[j][i]);
    }
    ss << std::endl;
  }
  return ss.str();
}

std::string state::dump_edge_y() const
{
  std::stringstream ss;
  for (size_t j = 0; j < field_size_Y+1; j++) {
    for (size_t i = 0; i < field_size_X; i++) {
      ss << static_cast<int>(edge_y[j][i]);
    }
    ss << std::endl;
  }
  return ss.str();
}

int state::put_piece(const piece* p, const int x,  const int y)
{
  static const int fsX = static_cast<int>(field_size_X);
  static const int fsY = static_cast<int>(field_size_Y);
  static const int psX = static_cast<int>(piece::size_X);
  static const int psY = static_cast<int>(piece::size_Y);

  int i0 = (x < 0)? -x: 0;
  int j0 = (y < 0)? -y: 0;
  int i1 = (x+psX > fsX)? psX - ((x+psX) - fsX): psX;
  int j1 = (y+psY > fsY)? psY - ((y+psY) - fsY): psY;
  int r0 = 0,r1 = 0,r2 = 0;

  if (p->x0 < i0)    return 4;
  if (i1 +1 < p->x1) return 4;
  if (p->y0 < j0)    return 4;
  if (j1 +1 < p->y1) return 4;

  for (size_t j = j0; j < j1; j++) {
  for (size_t i = i0; i < i1; i++) {
    face[y+j][x+i] += p->face_at(i,j);
    r0 += face[y+j][x+i];
    r1 += face[y+j][x+i] * field_mask[(y+j)*(fsX)+(x+i)];
  }}

  if (r0 != r1) {
    return 1;
  }

  char a = max_element(*face, sizeof(face));

  if ( a != 1 ) {
    return 2;
  }

  for (size_t j = j0; j < j1; j++)   {
  for (size_t i = i0; i < i1+1; i++) {
    edge_x[y+j][x+i] += p->edge_x[j][i];
  }}
  for (size_t j = j0; j < j1+1; j++)   {
  for (size_t i = i0; i < i1; i++) {
    edge_y[y+j][x+i] += p->edge_y[j][i];
  }}

  char b = max_element(*edge_x,sizeof(edge_x));
  char c = max_element(*edge_y,sizeof(edge_y));

  if (zk_count != 0 && (b == 2 || c == 2)) {
  } else if (zk_count == 0) {
  } else {
    return 3;
  }

  // update edges
  for (size_t j = j0; j < j1; j++) {
  for (size_t i = i0; i < i1+1; i++) {
    edge_x[y+j][x+i] = std::min(char(1),edge_x[y+j][x+i]);
  }}
  for (size_t j = j0; j < j1+1; j++) {
  for (size_t i = i0; i < i1; i++) {
    edge_y[y+j][x+i] = std::min(char(1),edge_y[y+j][x+i]);
  }}

  for (int j = j0; j < j1; j++) {
    for (int i = i0; i < i1; i++) {
      spaced[y+j] -= p->face_at(i,j);
    }
  }

  if (zk_count != 0) {
    x0 = std::min(x0,p->x0 + x);
    x1 = std::max(x1,p->x1 + x);
    y0 = std::min(y0,p->y0 + y);
    y1 = std::max(y1,p->y1 + y);
  } else {
    x0 = p->x0 + x;
    x1 = p->x1 + x;
    y0 = p->y0 + y;
    y1 = p->y1 + y;
  }

  zk_count += p->zk_count;
  put_count += 1;

  return 0;
}

