#include "ab.h"

int heuristic(board b) {
  //TODO: a better heuristic
  int cur_player = Board::valid_board_player(b);

  UnionFind sets(8*8);
  int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
  int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++) {
      if (Board::check_square(b, i, j) == -1)
	continue;
      for (int k = 0; k < 8; k++) {
	int nx = i + dx[k];
	int ny = j + dy[k];
	if (nx < 0 || ny < 0 || nx >= 8 || ny >= 9)
	  continue;
	if (Board::check_square(b, i, j) == Board::check_square(b, nx, ny))
	  sets.setUnion(i*8+j, nx*8+ny);
      }
    }
  int mine = 0;
  int other = 0;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      if (Board::check_square(b, i, j) == cur_player)
	mine += sets.setSize(8*i+j);
      else if (Board::check_square(b, i, j) == !cur_player)
	other += sets.setSize(8*i+j);
  //effectively returns the difference of the sums of the squares of the length of each "chain"
  //could be done with dfs, but way more fun with UF
  return mine-other;
}

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
  if (cur_player)
    v = MinValue(initial_board, -INF, INF);
  else
    v = MaxValue(initial_board, -INF, INF);

  for (auto it : Successors(initial_board)) {
    if (value(applyMoves(initial_board, it)) == v) {
      list_moves = it;
      break;
    }
  }
}

board AB::applyMoves(board b, vector<Move> m) {
  board nb = b;
  for (auto mov : m)
    nb = Board::make_move(nb, mov, cur_player);
  return nb;
}

vector<vector<Move> > AB::Successors(board b) {
  if (sons.find(b) != sons.end())
    return sons[b];

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
      for (auto mov : Board::available_moves(Board::make_move(b, m, player), player, npieces, m.first != 'p' || npieces <= 2, m.first != 'm' && npieces >= 3)) {
	vector<Move> v;
	v.push_back(m); v.push_back(mov);
	r.push_back(v);
      }
  }
  return sons[b] = r;
}

int AB::MaxValue(board b, int alpha, int beta) {
  cur_depth++;
  if (TerminalTest(b)) {
    cur_depth--;
    return Utility(b);
  }

  int v = -INF;
  for (auto it : Successors(b)) {
    v = max(v, MinValue(applyMoves(b, it), alpha, beta));
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
  cur_depth++;
  if (TerminalTest(b)) {
    cur_depth--;
    return Utility(b);
  }

  int v = INF;
  for (auto it : Successors(b)) {
    v = min(v, MaxValue(applyMoves(b, it), alpha, beta));
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
  if (Board::win(b, cur_player))
    return val[b] = INF-1;
  else if (Board::win(b, !cur_player))
    return val[b] = -INF+1;

  if (cur_depth >= max_depth)
    return val[b] = heuristic(b);
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
