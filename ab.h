#ifndef _AB_
#define _AB_

#include "common.h"
#include "board.h"
#include "unionfind.h"
#include "search.h"

class AB : public Search {
public:
  AB();
  ~AB();
  void solve(board initial_board);
protected:
  vector<Move> list_moves;
};

#endif
