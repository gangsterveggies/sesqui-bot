#include "common.h"
#include "board.h"
#include "timer.h"
#include "search.h"
#include "mcts.h"
#include "ab.h"

// ./SESQUIBOT -i "p|3|4\$p|2|2\$p|4|2\$m|3|4|3|5\$p|4|5\$p|2|3\$m|2|3|4|3\$"

int help_flag; // Show help info
int method_flag; // Search method to use
int verbose; // Print intermediate solution boards
char* input_str; // Initial board string
board initial_board; // Initial board object
int initial_move; // Move of initial board

void init()
{
  srand(7331);//time(NULL));

  help_flag = 0;
  verbose = 0;

  method_flag = 0;
  initial_move = 0;

  initial_board = Board::empty_board();
}

int process_arguments(int argc, char *argv[])
{
  int i;
  for (i = 1; i < argc; i++)
    if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
      help_flag = 1;
    else if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--verbose") == 0))
      verbose = 1;
    else if ((strcmp(argv[i], "-i") == 0))
    {
      input_str = argv[i + 1];
      i++;
    }
    else if (strcmp(argv[i], "--ab") == 0)
      method_flag = 0;
    else if (strcmp(argv[i], "--mcts") == 0)
      method_flag = 1;
    else if (strcmp(argv[i], "--minimax") == 0)
      method_flag = 2;
    else if (strcmp(argv[i], "--random") == 0)
      method_flag = 3;
    else
      return 1;
  return 0;
}

void output_help()
{
  printf("\t\t---- Sesqui Bot ----\n");
  printf("usage:\n");
  printf("\t./SESQUIBOT [arguments]\n\n");
  printf("Available arguments:\n");
  printf("\t-h, --help\t\tdisplay this help file\n"
         "\t-v, --verbose\n"
    );
}

void read_input()
{
  if (input_str != NULL)
  {
    initial_board = Board::string_to_board(input_str);
    initial_move = Board::string_to_moves(input_str);
  }
}

vector<Move> calculate()
{
  vector<Move> list_moves;
  Search* engine;

  if (method_flag == 0)
  {
    engine = new AB(4);
  }
  else if (method_flag == 1)
  {
    engine = new MCTS();
  }
  else if (method_flag == 2)
  {
  }
  else
  {
    vector<Move> moves = Board::available_moves(initial_board, Board::move_to_player(initial_move), initial_move, 0, 1);
    list_moves.push_back(moves[rand() % ((int)moves.size())]);
    moves = Board::available_moves(Board::make_move(initial_board, list_moves[0], Board::move_to_player(initial_move)), Board::move_to_player(initial_move + 1), initial_move + 1, 1, 0);
    list_moves.push_back(moves[rand() % ((int)moves.size())]);
  }

  engine->solve(initial_board, initial_move);
  list_moves = engine->get_solution();

  delete engine;

  return list_moves;
}

void output_result(vector<Move> list_moves)
{
  if (verbose)
  {
    printf("Initial board:\n");
    Board::print_board(initial_board);
    printf("\nMove in PM notation:\n");
  }

  int i;

  for (i = 0; i < (int)list_moves.size(); i++)
  {
    printf("%s%c", Board::encode_move(list_moves[i]), i == (int)list_moves.size() - 1 ? '\n' : '$');
    initial_board = Board::make_move(initial_board, list_moves[i], Board::move_to_player(i + initial_move));
  }

  if (verbose)
  {
    printf("\nBoard after set of moves:\n");
    Board::print_board(initial_board);
  }
}

int main(int argc, char *argv[])
{
  init();

  if (process_arguments(argc, argv) || help_flag)
  {
    output_help();
    return !help_flag;
  }

  read_input();

  Timer::start();
  vector<Move> list_moves = calculate();
  Timer::stop();

  output_result(list_moves);
  /*
  if (!Board::win(initial_board, 0))
    printf("no!!!\n");
  if (Board::win(make_pair((1 << 65)-1, 0LL), 0))
  printf("yay white wins!!!\n");*/
  return 0;
}
