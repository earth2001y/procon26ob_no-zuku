#ifndef __GAME_H__
#define __GAME_H__

#include <vector>
#include <string>
#include "piece.h"
#include "state.h"

class game {

  struct record_elem_t {
    record_elem_t():
      x(0),y(0),p(NULL),s(NULL)
    {
    }

    int x, y;
    const piece* p;
    state s;
  };

  char field_mask[32][32];

  typedef std::vector<record_elem_t> record_t;

  unsigned int best_zk_count;
  unsigned int best_put_count;

  std::vector<std::vector<const piece*> > piece_list;

private:
  std::string outfile;

public:
  void init(const std::string& problem, const std::string ofile);
  void solve(const int np0, const int np1,
             const int y0,  const int y1,
             const int x0,  const int x1);

  std::string dump(const record_t& working_record) const;
  int view(const int n) const;

private:
  int solve_n(const int n, const state& s0, const int np,
              const int y00, const int y01,
              const int x00, const int x01,
              record_t& working_record);
  int on_solved(const record_t& working_record);

  bool no_space_0(const state& s, const int y, const piece* p);
};

#endif // __GAME_H__

