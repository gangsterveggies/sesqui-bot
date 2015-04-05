#ifndef _BOARD_
#define _BOARD_

#include "common.h"

class Board
{
 public:

  static int move_to_player(int move);
  static board string_to_board(char* input);
  static board empty_board();
  static board make_move(board input_board, Move move, int player);
  static void print_board(board input_board);

 private:

  static Move parse_string(char* input);
};

#endif
