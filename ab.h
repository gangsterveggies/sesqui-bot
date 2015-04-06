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

  unordered_map<board, vector<vector<Move> >, Board::hash> sons;
  unordered_map<board, int, Board::hash> val; //the ab value of a node

  vector<vector<Move> > Successors(board b);
  int MaxValue(board b, int alpha, int beta);
  int MinValue(board b, int alpha, int beta);
  int Utility(board b);
  int value(board b);
  bool TerminalTest(board b);
  board applyMoves(board b, vector<Move> m);
};

#endif
