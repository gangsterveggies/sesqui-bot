#include "common.h"
#include "board.h"
#include "timer.h"

// ./SESQUIBOT -i "p|3|4\$p|2|2\$p|4|2\$m|3|4|3|5\$p|4|5\$p|2|3\$m|2|3|4|3\$"

int help_flag; // Show help info
int method_flag; // Search method to use
int verbose; // Print intermediate solution boards
char* input_str; // Initial board string

void init()
{
  srand(time(NULL));

  help_flag = 0;
  verbose = 0;
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
    else if (strcmp(argv[i], "--mcst") == 0)
      method_flag = 1;
    else if (strcmp(argv[i], "--minimax") == 0)
      method_flag = 2;
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
    board initial_board = Board::string_to_board(input_str);
    Board::print_board(initial_board);
  }
}

char* calculate()
{
  char* moves = (char*) malloc(sizeof(char) * 20);
}

void output_result(char* output)
{
  
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
  char* output = calculate();
  Timer::stop();

  output_result(output);
  free(output);

  return 0;
}
