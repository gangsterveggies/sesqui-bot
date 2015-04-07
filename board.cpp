#include "board.h"

char Board::output[40];
int Board::dx_p[4] = {1, -1, 0, 0};
int Board::dy_p[4] = {0, 0, 1, -1};
int Board::dx_m[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
int Board::dy_m[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

int Board::heuristic(board b) {
  //TODO: a better heuristic

  int cur_player = Board::valid_board_player(b);

  if (win(b) == cur_player)
    return 10000;
  else if (win(b) != -1)
    return 0;

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
	if (nx < 0 || ny < 0 || nx >= 8 || ny >= 8)
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


int Board:: move_to_player(int move)
{
  if (move == 0)
    return 0;
  if (move <= 2)
    return 1;
  return ((move - 3) / 2) % 2;
}

board Board::string_to_board(char* input)
{
  board n_board = empty_board();
  int i, move = 0;

  for (i = 0; input[i]; i++)
  {
    n_board = make_move(n_board, parse_string(input + i), move_to_player(move++));
    
    while (input[i] != '$')
      i++;
  }

  return n_board;
}

int Board::string_to_moves(char* input)
{
  int i, move = 0;

  for (i = 0; input[i]; i++)
  {
    move++;
    
    while (input[i] != '$')
      i++;
  }

  return move;
}

Move Board::parse_string(char* input)
{
  char type = input[0];
  int pieces = 0;

  if (type == 'm')
  {
    int i = 2, current_x = 0, current_y = 0;
    while (input[i] != '|')
    {
      current_x *= 10;
      current_x += input[i] - '0';
      i++;
    }

    i++;
    while (input[i] != '|')
    {
      current_y *= 10;
      current_y += input[i] - '0';
      i++;
    }
    pieces += current_x + current_y * 8;

    i++, current_x = 0, current_y = 0;
    while (input[i] != '|')
    {
      current_x *= 10;
      current_x += input[i] - '0';
      i++;
    }

    i++;
    while (input[i] != '$')
    {
      current_y *= 10;
      current_y += input[i] - '0';
      i++;
    }
    pieces <<= 12;
    pieces += current_x + current_y * 8;
  }
  else
  {
    int i = 2, current_x = 0, current_y = 0;
    while (input[i] != '|')
    {
      current_x *= 10;
      current_x += input[i] - '0';
      i++;
    }

    i++;
    while (input[i] != '$')
    {
      current_y *= 10;
      current_y += input[i] - '0';
      i++;
    }
    pieces <<= 12;
    pieces += current_x + current_y * 8;
  }

  Move n_move = Move(type, pieces);
  return n_move;
}

board Board::empty_board()
{
  board n_board = board(0, 0);
  return n_board;
}

board Board::make_move(board input_board, Move move, int player)
{
  int bitmask = (1 << 6) - 1;
  if (move.first == 'm')
  {
    int second = (move.second & bitmask);
    int first = (move.second & (bitmask << 12)) >> 12;

    input_board.first ^= (1LL << first);
    input_board.first ^= (1LL << second);
    
    if ((input_board.second & (1LL << second)) > 0 != player > 0)
      input_board.second ^= (1LL << second);
  }
  else
  {
    int first = (move.second & bitmask);

    input_board.first ^= (1LL << first);
    
    if ((input_board.second & (1LL << first)) > 0 != player > 0)
      input_board.second ^= (1LL << first);
  }

  return input_board;
}

void Board::print_board(board input_board)
{
  int i, j;
  for (i = 0; i < 8; i++)
  {
    printf ("|");
    for (j = 0; j < 8; j++)
    {
      if (!(input_board.first & (1LL << (j + i * 8))))
        printf(".|");
      else if (!(input_board.second & (1LL << (j + i * 8))))
        printf("o|");
      else
        printf("x|");
    }
    printf("\n");
  }
}

char* Board::encode_move(Move move)
{
  int bitmask = (1 << 6) - 1;
  output[0] = move.first;

  if (move.first == 'm')
  {
    int second = (move.second & bitmask);
    int first = (move.second & (bitmask << 12)) >> 12;

    sprintf(output + 1, "|%d", first % 8);
    sprintf(output + 3, "|%d", first / 8);
    sprintf(output + 5, "|%d", second % 8);
    sprintf(output + 7, "|%d", second / 8);
    output[9] = '\0';
  }
  else
  {
    int first = (move.second & bitmask);

    sprintf(output + 1, "|%d", first % 8);
    sprintf(output + 3, "|%d", first / 8);
    output[6] = '\0';
  }

  return output;
}

vector<Move> Board::available_moves(board input_board, int player, int move, int can_place, int can_move)
{
  vector<Move> list_moves;
  int i, j, k;

  if (win(input_board) != -1)
    return list_moves;

  for (i = 0; i < 8; i++)
    for (j = 0; j < 8; j++)
    {
      if (move >= 3)
      {
        if (can_place && !(input_board.first & (1LL << (j + i * 8))) && (valid_position(input_board, j, i, player) && valid_place(input_board, j, i, player)))
          list_moves.push_back(Move('p', (j + i * 8)));

        if (can_move && (input_board.first & (1LL << (j + i * 8))) && (check_square(input_board, j, i) == player))
        {
          input_board.first ^= (1LL << (j + i * 8));

          for (k = 0; k < 8; k++)
          {
            int c_x = j + dx_m[k], c_y = i + dy_m[k];
            
            while (valid_square(input_board, c_x, c_y) && check_square(input_board, c_x, c_y) == -1 && valid_position(input_board, c_x, c_y, player))
            {
              list_moves.push_back(Move('m', (c_x + c_y * 8) + ((j + i * 8) << 12)));
              c_x += dx_m[k];
              c_y += dy_m[k];
            }
          }

          input_board.first ^= (1LL << (j + i * 8));
        }
      }
      else
      {
        if (check_square(input_board, j, i) == -1)
          list_moves.push_back(Move('p', (j + i * 8)));
      }
    }

  return list_moves;
}

int Board::check_square(board input_board, int position_x, int position_y)
{
  return (input_board.first & (1LL << (position_x + position_y * 8))) ? ((input_board.second & (1LL << (position_x + position_y * 8))) ? 1 : 0) : -1;
}

int Board::valid_square(board input_board, int position_x, int position_y)
{
  return position_x >= 0 && position_x <= 7 && position_y >= 0 && position_y <= 7;
}

int Board::valid_position(board input_board, int position_x, int position_y, int player)
{
  int mcol = player, ocol = 1 - player, i;

  // Check:
  // xo
  // .x
  int dxD[] = {0, 1};
  int dyD[] = {1, 0};

  int fl = 1;
  for (i = 0; i < 2; i++)
    fl &= ((valid_square(input_board, position_x + dxD[i], position_y + dyD[i])) && (check_square(input_board, position_x + dxD[i], position_y + dyD[i]) == ocol));
  fl &= ((valid_square(input_board, position_x + 1, position_y + 1)) && (check_square(input_board, position_x + 1, position_y + 1) == mcol));

  if (fl)
    return 0;

  // Check:
  // x.
  // ox
  dxD[1] = -1;
  dyD[0] = -1;

  fl = 1;
  for (i = 0; i < 2; i++)
    fl &= ((valid_square(input_board, position_x + dxD[i], position_y + dyD[i])) && (check_square(input_board, position_x + dxD[i], position_y + dyD[i]) == ocol));
  fl &= ((valid_square(input_board, position_x - 1, position_y - 1)) && (check_square(input_board, position_x - 1, position_y - 1) == mcol));

  return !fl;
}

int Board::valid_place(board input_board, int position_x, int position_y, int player)
{
  int fl = 0, i;
  for (i = 0; i < 4; i++)
    fl |= (valid_square(input_board, position_x + dx_p[i], position_y + dy_p[i]) && (check_square(input_board, position_x + dx_p[i], position_y + dy_p[i]) == player));
  return fl;
}

bool Board::terminal_test(board final) {
  //presumes a valid board
  if (win(final, 0) || win(final, 1))
    return true;
  if (final.first == (1 << 65)-1) //full board with no winner
    return true;
  //TODO handle no more movements case
  //if (available_moves(final, int player, int move, int can_place, int can_move).size() == 0)
  //  return true;
  return false;
}

int Board::win(board b) {
  if (win(b, 0))
    return 0;
  if (win(b, 1))
    return 1;
  return -1;
}

bool Board::win(board b, int player) {
  //presumes a valid board
  pair<int, int> cur, d[5];
  queue<pair<int, int> > q;
  int v[8][8]; memset(v, 0, sizeof v);

  if (player) {
    //player 1 is horizontal
    for (int i = 0; i < 8; i++)
      if (check_square(b, 0, i) == player) {
	q.push(make_pair(0,i));
	v[0][i] = 1;
      }
    d[0] = make_pair(1, 0); d[1] = make_pair(1, -1); d[2] = make_pair(1, 1); d[3] = make_pair(0, 1); d[4] = make_pair(0, -1);
  }
  else {
    //player 0 is vertical
    for (int i = 0; i < 8; i++)
      if (check_square(b, i, 0) == player) {
	q.push(make_pair(i,0));
	v[i][0] = 1;
      }
    d[0] = make_pair(0, 1); d[1] = make_pair(-1, 1); d[2] = make_pair(1, 1); d[3] = make_pair(1, 0); d[3] = make_pair(-1, 0);
  }

  //bfs with (8*8)/2 iterations on worst case 
  while (!q.empty()) {
    cur = q.front(); q.pop();
    
    for (int i = 0; i < 5; i++) {
      int nx = cur.first + d[i].first;
      int ny = cur.second + d[i].second;

      if (nx < 0 || ny < 0 || nx >= 8 || ny >= 8 || v[nx][ny])
	continue;

      if (check_square(b, nx, ny) != player)
        continue;

      if ((player && nx == 7) || (!player && ny == 7))
	return true;

      q.push(make_pair(nx, ny));
      v[nx][ny] = 1;
    }
  }

  return false;
}

int Board::valid_board_player(board b) {
  int h = 0, v = 0;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++) 
      if(check_square(b, i, j)) v++; else h++;
  return v != h+1 && (v || h); //1 if now is vertical's turn, 0 otherwise
}

