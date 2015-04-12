#ifndef _AB_
#define _AB_

#include "common.h"
#include "board.h"
#include "search.h"

int heuristic(board b);

class AB : public Search {
public:
  AB();
  ~AB();
  AB(int mdepth);
  void solve(board initial_board, int initial_move);
  void solve(board initial_board, int initial_move, int mdepth);

private:
  int cur_depth;
  int cur_player;
  int max_depth;

  //unordered_map<board, double, Board::hash> val;

  vector<vector<Move> > Successors(board b);
  pair<double, board> MaxValue(board b, double alpha, double beta);
  pair<double, board> MinValue(board b, double alpha, double beta);
  double Utility(board b);
  bool TerminalTest(board b);
  board applyMoves(board b, vector<Move> m, int player);
};

#endif
