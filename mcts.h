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
  int glob_p;
  int maturity_threshold;
  int virtual_visits;
  int move_threshold;
  int place_threshold;
  int cache_size;
  int cache_threshold;
  int cache_frequency;

  void delete_nodes(node* current);
  double randomD();
  void fill_cache(node* current);
  Move select_random_move(board current_board, vector<pii> pieces, int player, int move, int can_place, int can_move);
//  Move select_random_move(vector<Move> pos_moves, board current_board);
  node* UCT_select_child(node* current);
  Move UCT(node* initial_node, board current_board, int current_move, int current_has_place, int current_has_move);
};

#endif
