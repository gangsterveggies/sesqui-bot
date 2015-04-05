#include "board.h"

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

Move Board::parse_string(char* input)
{
  char type = input[0];
  int pieces = 0;

  if (type == 'm')
  {
    int i = 2, current = 0;
    while (input[i] != '|')
    {
      current *= 10;
      current += input[i] - '0';
      i++;
    }
    pieces = current;

    i++, current = 0;
    while (input[i] != '|')
    {
      current *= 10;
      current += input[i] - '0';
      i++;
    }
    pieces <<= 6;
    pieces += current;

    i++, current = 0;
    while (input[i] != '|')
    {
      current *= 10;
      current += input[i] - '0';
      i++;
    }
    pieces <<= 6;
    pieces += current;

    i++, current = 0;
    while (input[i] != '$')
    {
      current *= 10;
      current += input[i] - '0';
      i++;
    }
    pieces <<= 6;
    pieces += current;
  }
  else
  {
    int i = 2, current = 0;
    while (input[i] != '|')
    {
      current *= 10;
      current += input[i] - '0';
      i++;
    }
    pieces = current;

    i++, current = 0;
    while (input[i] != '$')
    {
      current *= 10;
      current += input[i] - '0';
      i++;
    }
    pieces <<= 6;
    pieces += current;
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
  long long int bitmask = (1 << 6) - 1;
  if (move.first == 'm')
  {
    int second_y = (move.second & bitmask);
    int second_x = (move.second & (bitmask << 6)) >> 6;
    int first_y = (move.second & (bitmask << 12)) >> 12;
    int first_x = (move.second & (bitmask << 18)) >> 18;

    input_board.first ^= (1LL << (first_x + first_y * 8));
    input_board.first ^= (1LL << (second_x + second_y * 8));
    
    if ((input_board.second & (1LL << (second_x + second_y * 8))) > 0 != player > 0)
      input_board.second ^= (1LL << (second_x + second_y * 8));
  }
  else
  {
    int first_y = (move.second & bitmask);
    int first_x = (move.second & (bitmask << 6)) >> 6;

    input_board.first ^= (1LL << (first_x + first_y * 8));
    
    if ((input_board.second & (1LL << (first_x + first_y * 8))) > 0 != player > 0)
      input_board.second ^= (1LL << (first_x + first_y * 8));
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
