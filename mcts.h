#ifndef _MCTS_
#define _MCTS_

#include "common.h"
#include "search.h"
#include "board.h"

class MCTS : public Search
{
 public:

  MCTS();
  ~MCTS();

  void solve(board initial_board);

 private:

  struct node;
};

#endif
