#include "ab.h"

unordered_map<board, list<board>, Board::hash> sons;
unordered_map<board, int, Board::hash> val; //the ab value of a node

list<Move>& Successors(board b);
int MaxValue(board b, int alpha, int beta);
int MinValue(board b, int alpha, int beta);
int Utility(board b);
bool TerminalTest(board b);
int value(board b);
int heuristic(board b);
vector<Move> alphabeta(board b, int depth);
vector<Move> alphabeta(board b);

int max_depth = 10;
int cur_depth = 0;
int cur_player;

void AB::solve(board initial_board) {
  list_moves = alphabeta(initial_board);
}

vector<Move> alphabeta(board b, int depth) {
  swap(depth, max_depth);
  vector<Move> m = alphabeta(b);
  swap(depth, max_depth);
  return m;
}

vector<Move> alphabeta(board b) {
  int v;
  cur_player = Board::valid_board_player(b);
  if (cur_player)
    v = MinValue(b, -INF, INF);
  else
    v = MaxValue(b, -INF, INF);

  /*
  for (auto it : Successors(b))
    if (value(it) == v)
    return it;*/
  return vector<Move>();
}

list<Move>& Successors(board b) {
}

int MaxValue(board b, int alpha, int beta) {
  if (TerminalTest(b))
    return Utility(b);
  int v = -INF;
  for (auto it : Successors(b)) {
    v = max(v, MinValue(it, alpha, beta));
    if (v >= beta)
      return v;
    alpha = max(alpha, v);
  }
  return v;
}

int MinValue(board b, int alpha, int beta) {
  if (TerminalTest(b))
    return Utility(b);
  int v = INF;
  for (auto it : Successors(b)) {
    v = min(v, MinValue(it, alpha, beta));
    if (v <= alpha)
      return v;
    beta = min(beta, v);
  }
  return v;
}

int Utility(board b) {
  if (Board::win(b, cur_player))
    return INF-1;
  else if (Board::win(b, !cur_player))
    return -INF+1;

  if (cur_depth >= max_depth)
    return heuristic(b);
  return 0;
}

int heuristic(board b) {
  //TODO: a better heuristic
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

int value(board b) {
  if (val.find(b) != val.end())
    return val[b];
  return 0;
}

bool TerminalTest(board b) {
  if (Board::terminal_test(b) || cur_depth >= max_depth)
    return true;
  return false;
}
