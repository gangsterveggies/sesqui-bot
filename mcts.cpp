#include "mcts.h"

struct MCTS::node
{
  board state;
  vector<Move> untried_moves;
  vector<node*> children;
  Move move;
  int move_number;
  int has_move, has_place;
  int wins, visits;
  node* parent;
};

MCTS::MCTS()
{
  max_iterations = 10;
  UCTK = 1;
}

MCTS::~MCTS()
{
  delete_nodes(root);
}

void MCTS::delete_nodes(node* current)
{
  int i;
  for (i = 0; i < (int)current->children.size(); i++)
    delete_nodes(current->children[i]);

  delete current;
}

MCTS::node* MCTS::UCT_select_child(node* current)
{
  node* next;
  double score_value = -1;
  int i;

  for (i = 0; i < (int)current->children.size(); i++)
  {
    double current_score = current->children[i]->wins / ((double) current->children[i]->visits) + UCTK * sqrt(2 * log(current->visits) / ((double) current->children[i]->visits));

    if (current_score > score_value)
    {
      next = current->children[i];
      score_value = current_score;
    }
  }

  return next;
}

Move MCTS::UCT(board current_board, int current_move)
{
  int i;

  root = new node();
  root->state = current_board;
  root->move_number = current_move;
  root->has_move = 0;
  root->has_place = 0;
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
      new_node->move_number = current->move_number + 1;
      new_node->move = current->untried_moves[next_move];
      new_node->wins = 0;
      new_node->visits = 0;

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
    vector<Move> current_list = Board::available_moves(new_board, Board::move_to_player(current_move), current_move, !has_place, !has_move);

    while (!current_list.empty())
    {
      Move next_move = current_list[rand() % ((int) current_list.size())];
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

    while (current != NULL)
    {
      current->visits++;
      current->wins += Board::win(new_board) == Board::move_to_player(current->move_number);
      current = current->parent;
    }
  }

  Move best_move;
  int best_visits = 0;
  for (i = 0; i < (int)root->children.size(); i++)
    if (root->children[i]->visits > best_visits)
    {
      best_move = root->children[i]->move;
      best_visits = root->children[i]->visits;
    }

  return best_move;
}

void MCTS::solve(board initial_board, int initial_move)
{
  list_moves.push_back(UCT(initial_board, initial_move));

  if (initial_move > 0)
  {
    initial_board = Board::make_move(initial_board, list_moves[0], Board::move_to_player(initial_move));
    list_moves.push_back(UCT(initial_board, initial_move + 1));
  }
}
