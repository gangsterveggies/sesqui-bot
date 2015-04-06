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

  void solve(board initial_board, int initial_move);

 private:

  struct node;
  int max_iterations;
  double UCTK;
  node* root;

  void delete_nodes(node* current);
  node* UCT_select_child(node* current);
  Move UCT(board current_board, int current_move);
};

#endif