#ifndef _SEARCH_
#define _SEARCH_

#include "common.h"

class Search
{
 public:

  virtual ~Search() {};

  virtual void solve(board initial_board, int initial_move) {};
  vector<Move> get_solution();

 protected:

  vector<Move> list_moves;
};

#endif
