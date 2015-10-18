
#include "game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <unistd.h>
#include <signal.h>

bool term;
void sig_term(int signo)
{
  term = true;
}

void game::init(const std::string& problem, const std::string ofile)
{
  term = false;
  signal(SIGTERM, sig_term);

  std::string s;
  std::stringstream ss;
  ss << problem;
  //std::cout << ss.str() << std::endl;

  // field
  std::memset(*field_mask,0,sizeof(field_mask));
  for (int j = 0; j < 32; j++) {
    ss >> s;
    // std::cout << s << std::endl << std::flush;
    const char* c = s.c_str();
    for (int i = 0; i < 32; i++) {
      field_mask[j][i] = (c[i] == '0')? 1: 0;
    }
  }
  std::cerr << "field read." << std::endl << std::flush;

  // piece num
  int num;
  ss >> num;
  std::cerr << "num read. " << num << std::endl << std::flush;

  // piece
  piece_list.assign(num, std::vector<const piece*>());
  for (int n = 0; n < num; n++) {
    std::string ps = "";
    for (int j = 0; j < 8; j++) {
      ss >> s;
      ps += s + " ";
    }
    std::cerr << "read piece " << n << std::endl << std::flush;

    const piece p0(ps,n);
    for (int t = 0; t < 2; t++) {
      piece* p00 =
        (t==0)? new piece(p0):
        (t==1)? new piece(p0.turn()): NULL;
      for (int r = 0; r < 4; r++) {
        const piece* p =
          (r==0)? new piece(*p00):
          (r==1)? new piece(p00->rotate()):
          (r==2)? new piece(p00->rotate().rotate()):
          (r==3)? new piece(p00->rotate().rotate().rotate()): NULL;
        bool similar = false;
        for (size_t m = 0; m < piece_list[n].size(); m++) {
          similar |= (piece_list[n][m]->diff(*p) == 0);
        }
        if (!similar) {
          piece_list[n].push_back(p);
        } else {
          delete p;
        }
      }
      delete p00;
    }

    piece_list[n].push_back(NULL);
  }

  best_zk_count = 0;
  best_put_count = 0;

  //
  outfile = ofile;

}

std::string game::dump(const record_t& working_record) const
{
  std::stringstream ss;
  for (int j = 0; j < 32; j++) {
    for (int i = 0; i < 32; i++) {
      ss << int(field_mask[j][i]);
    }
    ss << std::endl;
  }
  ss << std::endl;

  for (size_t n = 0; n < piece_list.size(); n++) {
    for (size_t m = 0; m < piece_list[n].size(); m++) {
      if (!piece_list[n][m]) continue;
      ss << piece_list[n][m]->dump_face() << std::endl;
    }
  }

  return ss.str();
}

void game::solve(const int np0, const int np1,
                 const int y0,  const int y1,
                 const int x0,  const int x1)
{
  const state s0 = state(*field_mask);

  for (int np = np0; np < np1; np++) {
    // record
    record_t working_record;
    working_record.assign(piece_list.size(), record_elem_t());

    int resol = solve_n(0,s0,np,y0,y1,x0,x1,working_record);
    if (resol == -2) break;
    if (resol < -3) break;
  }
}

int game::solve_n(const int n, const state& s0, const int np0,
                  const int y00, const int y01,
                  const int x00, const int x01,
                  record_t& working_record)
{
  const bool last_n = (n == piece_list.size() - 1);
  const size_t ns = piece_list[n].size();
  const int np1 = (n == 0)? std::min((np0+1),int(ns)): ns;
  record_elem_t& r = working_record[n];

  ///
  size_t pp_zk = 0;
  for (int m = n; m < piece_list.size(); m++) {
    pp_zk += piece_list[m][0]->zk_count;
  }
  if (best_zk_count > r.s.zk_count + pp_zk) return -3;

  for (int np = np0; np < np1; np++) {

    const piece* p = piece_list[n][np];
    r.p = p;

    if (p == NULL) {
      r.s = state(s0);
      if (last_n) {
        if (r.s.zk_count > best_zk_count ||
           (r.s.zk_count == best_zk_count && r.s.put_count < best_put_count)) {
          best_zk_count  = r.s.zk_count;
          best_put_count = r.s.put_count;
          on_solved(working_record);
        }
        return n-1;
      } else {
        int resol = solve_n(n+1,r.s,0,0,32,0,32,working_record);
        if (resol == -2) return n-1;
        if (resol !=  n) return resol;
      }
      if (term) { _exit(0); }
    } else {
      const int x0 = x00+std::max(-p->x0,s0.x0 - 1 - p->x1); //-p->x0;
      const int y0 = y00+std::max(-p->y0,s0.y0 - 1 - p->y1); //-p->y0;
      const int x1 = std::min(s0.x1 + 1 - p->x0,x01-p->x1); //32-p->x1;
      const int y1 = std::min(s0.y1 + 1 - p->y0,y01-p->y1); //32-p->x1;
      for (r.y = y0; r.y < y1; r.y++) {
        if (no_space_0(s0,r.y,p)) continue;
        for (r.x = x0; r.x < x1; r.x++) {
          r.s = state(s0);
          int res = r.s.put_piece(p,r.x,r.y);
          if (res == 0) {
            if (!last_n) {
              int resol = solve_n(n+1,r.s,0,0,32,0,32,working_record);
              if (resol == -3) {
                np = piece_list.size() - 2;
                r.y = y1;
                r.x = x1;
                break;
              } else if (resol < n) {
                return resol;
              }
            } else {
              if (r.s.zk_count > best_zk_count ||
                 (r.s.zk_count == best_zk_count && r.s.put_count < best_put_count)) {
                best_zk_count  = r.s.zk_count;
                best_put_count = r.s.put_count;
                on_solved(working_record);
                return 0;
              } else {
                return -2;
              }
            }
          }
        }
        if (term) { _exit(0); }
      }
    }
  }

  return n-1;
}

bool game::no_space_0(const state& s, const int y, const piece* p)
{
  bool f = true;
  for (int j = p->y0; j < p->y1+1; j++) {
    f &= s.spaced[y+j] < p->filled[j];
  }
  return f;
}

int game::on_solved(const record_t& working_record)
{
  std::ostream* osp;
  const size_t wn = working_record.size();
  std::cout << "solved " << best_zk_count << std::flush << std::endl;
  //std::cerr << working_record[wn-1].s.dump_face() << std::flush << std::endl;

  if (outfile == "-") {
    osp = &std::cout;
  } else {
    std::stringstream ss;
    ss << outfile << "/answer-" << std::setw(4) << std::setfill('0') << best_zk_count << '-' << std::setw(4) << (piece_list.size() - best_put_count) << ".txt";
    osp = new std::ofstream(ss.str().c_str(),std::ofstream::out);
  }

  std::ostream& os = *osp;
  for (size_t n = 0; n < wn; n++) {
    const record_elem_t& r = working_record[n];
    if (r.p) {
       os << r.x << ' ' << r.y << ' '
          << ((r.p->norm == piece::H)?'T':'H') << ' '
          << r.p->rotation;
    }
    os << "\r\n";
  }

  os << std::flush;
  if (outfile != "-") {
    delete osp;
  }

  return 0;
}

int game::view(const int n) const
{
  std::stringstream ss;
  ss << state(*field_mask).dump_face() << std::endl;
  for (int m = 0; m < std::min(n,int(piece_list.size())); m++) {
    const piece* p = piece_list[m][0];
    ss << p->dump_face() << " x" << piece_list[m].size() << std::endl;
  }
  std::cout << ss.str() << std::endl;

  return 0;
}
