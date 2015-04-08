#include "mcts.h"

struct MCTS::node
{
  board state;
  double heuristic_value;
  vector<Move> untried_moves;
  vector<node*> children;
  Move move;
  double uct_value;
  int move_number;
  int has_move, has_place;
  int wins, visits;
  node* parent;
};

MCTS::MCTS()
{
  max_iterations = 1500;
  UCTK = 1;
}

MCTS::~MCTS()
{
  list_moves.clear();
  delete_nodes(root);
}

void MCTS::delete_nodes(node* current)
{
  int i;
  for (i = 0; i < (int)current->children.size(); i++)
    delete_nodes(current->children[i]);

  delete current;
}

Move MCTS::select_random_move(vector<Move> pos_moves, board current_board)
{
  int i;
  double best_outcome = 0;
  Move best_move;
  for (i = 0; i < 3; i++)
  {
    Move this_move = pos_moves[rand() % ((int)pos_moves.size())];
    board this_board = Board::make_move(current_board, this_move, Board::valid_board_player(current_board));
    double this_outcome = Board::heuristic_2(this_board);
    if (this_outcome > best_outcome)
    {
      best_outcome = this_outcome;
      best_move = this_move;
    }
  }

  return best_move;
}

MCTS::node* MCTS::UCT_select_child(node* current)
{
  node* next;
  double score_value = -100;
  int i;

  for (i = 0; i < (int)current->children.size(); i++)
  {
    double current_score = current->children[i]->uct_value;

    if (current_score > score_value)
    {
      next = current->children[i];
      score_value = current_score;
    }
  }

  return next;
}

Move MCTS::UCT(board current_board, int current_move, int current_has_place, int current_has_move)
{
  int i;

  root = new node();
  root->state = current_board;
  root->move_number = current_move;
  root->has_move = current_has_move;
  root->has_place = current_has_place;
  root->untried_moves = Board::available_moves(root->state, Board::move_to_player(current_move), current_move, !root->has_place, !root->has_move);
  root->parent = NULL;

  for (i = 0; i < max_iterations; i++)
  {
    node* current = root;

    while (current->untried_moves.empty() && !current->children.empty())
      current = UCT_select_child(current);

    board new_board = current->state;

    if (!current->untried_moves.empty())
    {
      int next_move = rand() % ((int)current->untried_moves.size());
      new_board = Board::make_move(new_board, current->untried_moves[next_move], Board::move_to_player(current->move_number));

      node* new_node = new node();
      new_node->state = new_board;
      new_node->heuristic_value = Board::heuristic_2(new_node->state);
      new_node->move_number = current->move_number + 1;
      new_node->move = current->untried_moves[next_move];
      new_node->wins = new_node->heuristic_value * 20;
      new_node->visits = 20;

      new_node->has_move = current->untried_moves[next_move].first == 'm';
      new_node->has_place = current->untried_moves[next_move].first == 'p';

      if (Board::move_to_player(new_node->move_number) != Board::move_to_player(current->move_number))
      {
        new_node->has_move = 0;
        new_node->has_place = 0;
      }

      new_node->untried_moves = Board::available_moves(new_node->state, Board::move_to_player(new_node->move_number), new_node->move_number, !new_node->has_place, !new_node->has_move);
      new_node->parent = current;

      current->untried_moves.erase(current->untried_moves.begin() + next_move);
      current->children.push_back(new_node);

      current = new_node;
    }

    int has_place = current->has_place;
    int has_move = current->has_move;
    int current_move = current->move_number;
    new_board = current->state;
    vector<Move> current_list = Board::available_moves(new_board, Board::move_to_player(current_move), current_move, !has_place, !has_move);

    while (!current_list.empty())
    {
      Move next_move = select_random_move(current_list, new_board);
      new_board = Board::make_move(new_board, next_move, Board::move_to_player(current_move));

      has_move = next_move.first == 'm';
      has_place = next_move.first == 'p';

      if (Board::move_to_player(current_move) != Board::move_to_player(current_move + 1))
      {
        has_move = 0;
        has_place = 0;
      }

      current_move++;      

      current_list = Board::available_moves(new_board, Board::move_to_player(current_move), current_move, !has_place, !has_move);
    }

    int winner = Board::win(new_board);

    while (current != NULL)
    {
      current->visits++;
      current->wins += winner == Board::move_to_player(current->move_number);

      if (current != root)
        current->uct_value = 6 * current->heuristic_value / (((double) current->visits + 1)) +  current->wins / ((double) current->visits) + UCTK * sqrt(2 * log(current->parent->visits) / ((double) current->visits));

      current = current->parent;
    }
  }

  Move best_move;
  int best_visits = 0;
  for (i = 0; i < (int)root->children.size(); i++)
  {
    if (root->children[i]->wins > best_visits)
    {
      best_move = root->children[i]->move;
      best_visits = root->children[i]->wins;
    }
  }
    
  return best_move;
}

void MCTS::solve(board initial_board, int initial_move)
{
/*  printf("%d\n", Board::win(initial_board));
  Board::print_board(initial_board);
  vector<Move> tmp = Board::available_moves(initial_board, Board::move_to_player(initial_move), initial_move, 0, 1);
  for (int i = 0; i < (int)tmp.size(); i++)
    printf("%s - ", Board::encode_move(tmp[i]));
  printf("\n\n");

  root = new node();
  return;*/

  list_moves.push_back(UCT(initial_board, initial_move, 0, 1));

  if (initial_move > 0)
  {
    delete_nodes(root);
    initial_board = Board::make_move(initial_board, list_moves[0], Board::move_to_player(initial_move));

    list_moves.push_back(UCT(initial_board, initial_move + 1, list_moves[0].first == 'p', list_moves[0].first == 'm'));
  }
}
