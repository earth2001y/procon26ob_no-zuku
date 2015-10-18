
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include "piece.h"
#include "state.h"
#include "game.h"

int main(int argc, char** argv)
{
  std::stringstream problem;
  std::istream* is;
  bool viewmode = false;

  int np0 = 0;
  int np1 = 9;
  int y0 = 0;
  int y1 = 32;
  int x0 = 0;
  int x1 = 32;
  std::string infile("-");
  std::string outfile(".");

  int n = 1;
  while (n < argc) {
    if (std::string(argv[n]) == "-np") {
      np0 = std::atoi(argv[++n]);
      np1 = np0+1;
    } else if (std::string(argv[n]) == "-y") {
      y0 = std::atoi(argv[++n]);
      y1 = y0+1;
    } else if (std::string(argv[n]) == "-x") {
      x0 = std::atoi(argv[++n]);
      x1 = x0+1;
    } else if (std::string(argv[n]) == "-np0") {
      np0 = std::atoi(argv[++n]);
    } else if (std::string(argv[n]) == "-np1") {
      np1 = std::atoi(argv[++n]);
    } else if (std::string(argv[n]) == "-y0") {
      y0 = std::atoi(argv[++n]);
    } else if (std::string(argv[n]) == "-y1") {
      y1 = std::atoi(argv[++n]);
    } else if (std::string(argv[n]) == "-x0") {
      x0 = std::atoi(argv[++n]);
    } else if (std::string(argv[n]) == "-x1") {
      x1 = std::atoi(argv[++n]);
    } else if (std::string(argv[n]) == "-i") {
      infile = argv[++n];
    } else if (std::string(argv[n]) == "-o") {
      outfile = argv[++n];
    } else if (std::string(argv[n]) == "--view") {
      viewmode = true;
    }
    n++;
  }

  if (infile == "-") {
    is = &std::cin;
  } else {
    is = new std::ifstream(infile.c_str(),std::ifstream::in);
  }

  while (!is->eof()) {
    std::string s;
    std::getline(*is,s);
    problem << s << std::endl;
  }

  game g;
  g.init(problem.str(),outfile);
//std::cout << g.dump() << std::endl;

  if (viewmode) {
    g.view(2);
  } else {
    g.solve(np0,np1,y0,y1,x0,x1);
  }

  return 0;
}

