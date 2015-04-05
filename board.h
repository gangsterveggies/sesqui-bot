#ifndef _BOARD_
#define _BOARD_

#include "common.h"

class Board
{
 public:

  static char output[40];

  static int move_to_player(int move);
  static board string_to_board(char* input);
  static int string_to_moves(char* input);
  static board empty_board();
  static board make_move(board input_board, Move move, int player);
  static void print_board(board input_board);
  static vector<Move> available_moves(board input_board, int player, int move, int can_place, int can_move);
  static char* encode_move(Move move);
  static bool terminal_test(board final);
  static bool win(board b, int player);
  static int valid_board_player(board b);

 private:

  static int dx_p[4];
  static int dy_p[4];
  static int dx_m[8];
  static int dy_m[8];

  static int valid_square(board input_board, int position_x, int position_y);
  static int check_square(board input_board, int position_x, int position_y);
  static int valid_position(board input_board, int position_x, int position_y, int player);
  static int valid_place(board input_board, int position_x, int position_y, int player);
  static Move parse_string(char* input);
};

#endif