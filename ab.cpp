#include "ab.h"

AB::AB() {
  cur_depth = 0;
  max_depth = 10;
  pruning = true;
}

AB::AB(int mdepth){
  AB();
  max_depth = mdepth;
}

AB::~AB() {}

void AB::setPruning(bool value) {
  pruning = value;
}

void AB::solve(board initial_board, int initial_move, int mdepth) {
  swap(mdepth, max_depth);
  solve(initial_board, initial_move);
  swap(mdepth, max_depth);
}

void AB::solve(board initial_board, int initial_move) {
  pair<double, board> v;  
  cur_player = Board::valid_board_player(initial_board);
  if (cur_player) {
    DEBUG("Minimizing\n");
    v = MinValue(initial_board, -INF, INF);
  }
  else  
    v = MaxValue(initial_board, -INF, INF);
  DEBUG("Utility %lf\n", v.first);

  for (auto it : Successors(initial_board))
    if (applyMoves(initial_board, it, cur_player) == v.second) {
      list_moves = it;
      return;
    }
  DEBUG("Catastrophic error! Move not found\n");
}

board AB::applyMoves(board b, vector<Move> m, int player) {
  board nb = b;
  for (auto mov : m)
    nb = Board::make_move(nb, mov, player);
  return nb;
}

vector<vector<Move> > AB::Successors(board b) {
  static map<board, vector<vector<Move> > > memo;
  if (memo.find(b)!= memo.end())
    return memo[b];

  vector<Move> r1;
  vector<vector<Move> > r;
  int player = Board::valid_board_player(b);

  int npieces = 0;
  for (int i = 0; i < 64; i++)
    if ((1 << i) & b.first)
      npieces++;

  //first move
  r1 = Board::available_moves(b, player, npieces, 1, 1);
  
  if (npieces) {
    //second move
    for (auto m : r1)
      for (auto mov : Board::available_moves(Board::make_move(b, m, player), player, npieces, m.first != 'p' || npieces <= 1, m.first != 'm' && npieces > 2)) {
	vector<Move> v;
	v.push_back(m); v.push_back(mov);
	r.push_back(v);
      }
  }
  else
    for (auto m : r1) {
      vector<Move> v; v.push_back(m);
      r.push_back(v);
    }
  return memo[b] = r;
}

pair<double, board> AB::MaxValue(board b, double alpha, double beta) {
  //DEBUG("Entering MAX\n");
  cur_depth++;
  if (TerminalTest(b)) {
    cur_depth--;
    return make_pair(Utility(b), b);
  }

  int player = Board::valid_board_player(b);

  pair<double, board> v = make_pair(-INF, b);
  set<board> visited;

  for (auto it : Successors(b)) {
    if (visited.find(applyMoves(b, it, player)) != visited.end())
      continue;

    pair<double, board> other = MinValue(applyMoves(b, it, player), alpha, beta);
    if (other > v) {
      v = other;
      v.second = applyMoves(b, it, player);
    }

    if (pruning) {
      if (v.first >= beta) {
	cur_depth--;
	return v;
      }
      alpha = max(alpha, v.first);
    }
    visited.insert(applyMoves(b, it, player));
  }
  cur_depth--;
  return v;
}

pair<double, board> AB::MinValue(board b, double alpha, double beta) {
  //DEBUG("Entering MIN\n");
  cur_depth++;
  if (TerminalTest(b)) {
    cur_depth--;
    return make_pair(Utility(b), b);
  }

  int player = Board::valid_board_player(b);

  pair<double, board> v = make_pair(INF, b);
  set<board> visited;

  for (auto it : Successors(b)) {
    if (visited.find(applyMoves(b, it, player)) != visited.end())
      continue;

    pair<double, board> other = MaxValue(applyMoves(b, it, player), alpha, beta);
    if (other < v) {
      v = other;
      v.second = applyMoves(b, it, player);
    }

    if (pruning) {
      if (v.first <= alpha) {
	cur_depth--;
	return v;
      }
      beta = min(alpha, v.first);
    }
    visited.insert(applyMoves(b, it, player));
  }
  cur_depth--;
  return v;
}

double AB::Utility(board b) {
  return Board::heuristic_2(b);
}

bool AB::TerminalTest(board b) {
  if (Board::terminal_test(b) || cur_depth >= max_depth)
    return true;
  return false;
}
