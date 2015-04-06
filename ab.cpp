#include "ab.h"

AB::AB() {
  cur_depth = 0;
  max_depth = 10;
}

AB::AB(int mdepth){
  AB();
  max_depth = mdepth;
}

AB::~AB() {}

void AB::solve(board initial_board, int initial_move, int mdepth) {
  swap(mdepth, max_depth);
  solve(initial_board, initial_move);
  swap(mdepth, max_depth);
}

void AB::solve(board initial_board, int initial_move) {
  int v;
  cur_player = Board::valid_board_player(initial_board);
  /*
  if (cur_player)
    v = MinValue(initial_board, -INF, INF);
    else*/
  v = MaxValue(initial_board, -INF, INF);

  for (auto it : Successors(initial_board)) {
    if (value(applyMoves(initial_board, it, cur_player)) == v) {
      list_moves = it;
      break;
    }
  }
}

board AB::applyMoves(board b, vector<Move> m, int player) {
  board nb = b;
  for (auto mov : m)
    nb = Board::make_move(nb, mov, player);
  return nb;
}

vector<vector<Move> > AB::Successors(board b) {
  //if (sons.find(b) != sons.end())
  //return sons[b];

  vector<Move> r1;
  vector<vector<Move> > r;
  int player = Board::valid_board_player(b);
  player = 1;

  int npieces = 0;
  for (int i = 0; i < 64; i++)
    if ((1 << i) & b.first)
      npieces++;

  //first move
  r1 = Board::available_moves(b, player, npieces, 1, 1);
  
  if (npieces) {
    //second move
    for (auto m : r1)
      for (auto mov : Board::available_moves(Board::make_move(b, m, player), player, npieces+(m.first=='p'), m.first != 'p' || npieces <= 2, m.first != 'm' && npieces >= 3)) {
	vector<Move> v;
	v.push_back(m); v.push_back(mov);
	r.push_back(v);
      }
  }
  return r;
  //return sons[b] = r;
}

int AB::MaxValue(board b, int alpha, int beta) {
  if (val.find(b) != val.end())
    return value(b);
  cur_depth++;
  if (TerminalTest(b)) {
    cur_depth--;
    return Utility(b);
  }

  int v = -INF;
  for (auto it : Successors(b)) {
    v = max(v, MinValue(applyMoves(b, it, cur_player), alpha, beta));
    if (v >= beta) {
      cur_depth--;
      return v;
    }
    alpha = max(alpha, v);
  }
  cur_depth--;
  return v;
}

int AB::MinValue(board b, int alpha, int beta) {
  if (val.find(b) != val.end())
    return value(b);
  cur_depth++;
  if (TerminalTest(b)) {
    cur_depth--;
    return Utility(b);
  }

  int v = INF;
  for (auto it : Successors(b)) {
    v = min(v, MaxValue(applyMoves(b, it, !cur_player), alpha, beta));
    if (v <= alpha) {
      cur_depth--;
      return v;
    }
    beta = min(beta, v);
  }
  cur_depth--;
  return v;
}

int AB::Utility(board b) {
  if (Board::win(b) == 0)
    return val[b] = 8*8;
  else if (Board::win(b) == 1)
    return val[b] = -8*8;

  if (cur_depth >= max_depth)
    return val[b] = Board::heuristic(b);
  return 0;
}

int AB::value(board b) {
  if (val.find(b) != val.end())
    return val[b];
  return 0;
}

bool AB::TerminalTest(board b) {
  if (Board::terminal_test(b) || cur_depth >= max_depth)
    return true;
  return false;
}
