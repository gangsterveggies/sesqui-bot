#include "mcts.h"

struct MCTS::node
{
  board state;
  double heuristic_value;
  vector<Move> untried_moves;
  vector<node*> children;
  vector<int> cache;
  Move move;
  double uct_value;
  int move_number;
  int has_move, has_place;
  int wins, visits;
  int explored;
  int maturity;
  node* parent;
};

MCTS::MCTS()
{
  max_iterations = 6000;
  UCTK = 0.4;
  maturity_threshold = 180;
  virtual_visits = 6;
  move_threshold = 45;
  place_threshold = 15;
  cache_size = 5;
  cache_threshold = 500000;
  cache_frequency = 30;
}

MCTS::~MCTS()
{
  list_moves.clear();
  delete_nodes(root);
}

void MCTS::delete_nodes(node* current)
{
  if (current == NULL)
    return;

  int i;
  for (i = 0; i < (int)current->children.size(); i++)
    delete_nodes(current->children[i]);

  delete current;
}

double MCTS::randomD()
{
  return rand() / (double) RAND_MAX;
}

Move MCTS::select_random_move(board current_board, vector<pii> pieces, int player, int move, int can_place, int can_move)
{
  Move best_move = Move((rand() % 2) ? 'm' : 'p', 0);

  if (!can_place)
    best_move.first = 'm';
  if (!can_move)
    best_move.first = 'p';
  if (!can_move && !can_place)
  {
    best_move.first = 'n';
    return best_move;
  }

  if (move <= 3)
  {
    best_move.first = 'p';

    int x = rand() % 8, y = rand() % 8;
    while (Board::check_square(current_board, x, y) != -1)
      x = rand() % 8, y = rand() % 8;

    best_move.second = x + y * 8;
    return best_move;
  }

  if (best_move.first == 'm')
  {
    int p = rand() % ((int) pieces.size());
    int x = (rand() % 3) - 1, y = (rand() % 3) - 1, av = rand() % 7 + 1;
    int iter = 0;

    while (!Board::can_move_piece(current_board, pieces[p].first, pieces[p].second, pieces[p].first + x * av, pieces[p].second + y * av))
    {
      p = rand() % ((int) pieces.size());
      x = (rand() % 3) - 1, y = (rand() % 3) - 1, av = rand() % 7 + 1;
      iter++;
      if (iter == move_threshold)
        break;
    }

    if (iter == move_threshold)
      return select_random_move(current_board, pieces, player, move, can_place, 0);

    glob_p = p;
    best_move.second = (pieces[p].first + x * av + (pieces[p].second + y * av) * 8) + ((pieces[p].first + pieces[p].second * 8) << 12);
  }
  else
  {
    best_move.first = 'p';
    int p = rand() % ((int) pieces.size());
    int x = (rand() % 3) - 1, y = (rand() % 3) - 1, pos = rand() % 2;
    int iter = 0;

    while (!Board::can_place_piece(current_board, pieces[p].first + x * pos, pieces[p].second + y * (1 - pos), player, move))
    {
//      if (glob_p == -1)
//        printf("%d %d\n", pieces[p].first + x * pos, pieces[p].second + y * (1 - pos));
      p = rand() % ((int) pieces.size());
      x = (rand() % 3) - 1, y = (rand() % 3) - 1, pos = rand() % 2;
      iter++;
      if (iter == place_threshold)
        break;
    }

    if (iter == place_threshold)
      return select_random_move(current_board, pieces, player, move, 0, can_move);

    best_move.second = pieces[p].first + x * pos + (pieces[p].second + y * (1 - pos)) * 8;
  }

/*  int i;
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
    }*/

  return best_move;
}

void MCTS::fill_cache(node* current)
{
  int visited[500];
  memset(visited, 0, sizeof visited);
  int i, j;

/*  if ((int)current->children.size() > 400)
  {
    printf("%lld %lld\n", current->state.first, current->state.second);
    Board::print_board(current->state);
    printf("%d\n", (int)current->children.size());
    for (i = 0; i < (int)current->children.size(); i++)
      printf("%s - ", Board::encode_move(current->children[i]->move));
    printf("\n\n");
    }*/

  for (j = 0; j < min(cache_size, (int)current->children.size()); j++)
  {
    int best;
    double score_value = -100;

    for (i = 0; i < (int)current->children.size(); i++)
      if (!visited[i])
      {
        double current_score = current->children[i]->uct_value;

        if (exp(current_score - score_value) > randomD())
        {
          best = i;
          score_value = current_score;
        }
      }

    current->cache.push_back(best);
    visited[best] = 1;
  }
}

MCTS::node* MCTS::UCT_select_child(node* current)
{
  node* next;
  double score_value = -100;
  int i;

  if ((int)current->cache.size() > 0)
  {
    for (i = 0; i < (int)current->cache.size(); i++)
    {
      double current_score = current->children[current->cache[i]]->uct_value;

      if (current_score > score_value)
      {
        next = current->children[current->cache[i]];
        score_value = current_score;
      }
    }

    if ((current->visits - cache_threshold) % cache_frequency)
    {
      current->cache.clear();
      fill_cache(current);
    }

    return next;
  }
  else if (current->visits >= cache_threshold)
    fill_cache(current);

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

Move MCTS::UCT(node* initial_node, board current_board, int current_move, int current_has_place, int current_has_move)
{
  int i, expanded = 0;

  if (initial_node == NULL)
  {
    root = new node();
    root->state = current_board;
    root->move_number = current_move;
    root->maturity = maturity_threshold;
    root->explored = 1;
    root->has_move = current_has_move;
    root->has_place = current_has_place;
    root->untried_moves = Board::available_moves(root->state, Board::move_to_player(current_move), current_move, !root->has_place, !root->has_move);
  }
  root->parent = NULL;

  if (!root->explored)
    root->untried_moves = Board::available_moves(root->state, Board::move_to_player(current_move), current_move, !root->has_place, !root->has_move);

  for (i = 0; i < max_iterations; i++)
  {
//    printf("%d\n", i);
    node* current = root;

    while (current->untried_moves.empty() && !current->children.empty() && current->maturity >= maturity_threshold)
    {
      current = UCT_select_child(current);
      if (!current->explored)
      {
        current->untried_moves = Board::available_moves(current->state, Board::move_to_player(current->move_number), current->move_number, !current->has_place, !current->has_move);
        current->explored = 1;
      }
    }

    current->maturity++;

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
      new_node->wins = new_node->heuristic_value * virtual_visits;
      new_node->visits = virtual_visits;
      new_node->explored = 0;

      new_node->has_move = current->untried_moves[next_move].first == 'm';
      new_node->has_place = current->untried_moves[next_move].first == 'p';

      if (Board::move_to_player(new_node->move_number) != Board::move_to_player(current->move_number))
      {
        new_node->has_move = 0;
        new_node->has_place = 0;

        node* grandpa = current;
        int player = Board::move_to_player(new_node->move_number);
        while (grandpa != NULL && Board::move_to_player(grandpa->move_number) != player)
          grandpa = grandpa->parent;

        if (grandpa != NULL)
        {
          new_node->wins += grandpa->wins;
          new_node->visits += grandpa->visits;
        }
      }
      else
      {
        new_node->wins += current->wins;
        new_node->visits += current->visits;
      }

      new_node->parent = current;

      current->untried_moves.erase(current->untried_moves.begin() + next_move);
      current->children.push_back(new_node);

      current = new_node;
    }

    int has_place = current->has_place;
    int has_move = current->has_move;
    int current_move = current->move_number;
    new_board = current->state;
    vector<pii> pieces[2];

    int x, y;
    for (y = 0; y < 8; y++)
      for (x = 0; x < 8; x++)
        if (Board::check_square(new_board, x, y) != -1)
          pieces[Board::check_square(new_board, x, y)].push_back(pii(x, y));

    int max_l = max(30 - ((int) pieces[0].size()) + ((int) pieces[1].size()) * 3, 5);

    int l = 0, j;
    while (l++ < max_l && ((int) pieces[0].size()) + ((int) pieces[1].size()) < 64) //Board::win(new_board) == -1)
    {
      int player = Board::move_to_player(current_move);
      Move win_move = Move('n', 0), next_move;
      board old_board = new_board, another_board;

      Move another_move;
      double another_board_heuristic;

      next_move = select_random_move(new_board, pieces[player], player, current_move, !has_place, !has_move);
      if (next_move.first == 'n')
        break;

      new_board = Board::make_move(old_board, next_move, player);
      double new_board_heuristic = Board::heuristic_2(new_board);
      int tresh = 3;
/*      if (((int) pieces[0].size()) + ((int) pieces[1].size()) > 80)
        tresh = 20;
      else if (((int) pieces[0].size()) + ((int) pieces[1].size()) > 60)
        tresh = 9;
      else if (((int) pieces[0].size()) + ((int) pieces[1].size()) > 40)
      tresh = 5;*/

      for (j = 0; j < tresh; j++)
      {
        Move another_move = select_random_move(new_board, pieces[player], player, current_move, !has_place, !has_move);
        if (another_move.first == 'n')
          break;

        another_board = Board::make_move(old_board, another_move, player);
        another_board_heuristic = Board::heuristic_2(another_board);
        if (another_board_heuristic > new_board_heuristic)
        {
          new_board_heuristic = another_board_heuristic;
          next_move = another_move;
        }
      }

      new_board = Board::make_move(old_board, next_move, player);

      has_move = next_move.first == 'm';
      has_place = next_move.first == 'p';

      if (Board::move_to_player(current_move) != Board::move_to_player(current_move + 1))
      {
        has_move = 0;
        has_place = 0;
      }

      int bitmask = (1 << 6) - 1;
      int new_place = (next_move.second & bitmask);

      /*if (i == 552)
      {
        if (next_move.first == 'm')
          printf("%d %d -> ", pieces[player][glob_p].first, pieces[player][glob_p].second);
        printf("%d - %c - %d %d\n%d - ", player, next_move.first, new_place % 8, new_place / 8, (int)pieces[player].size());
        for (int k = 0; k < (int)pieces[player].size(); k++)
          printf("(%d, %d) ", pieces[player][k].first, pieces[player][k].second);
        printf("\n");
        Board::print_board(new_board);
        printf("%d\n", Board::win(new_board));
      }*/

      if (next_move.first == 'm')
        pieces[player][glob_p] = pii(new_place % 8, new_place / 8);
      else
        pieces[player].push_back(pii(next_move.second % 8, next_move.second / 8));

      current_move++;
    }

    int winner = Board::win(new_board);
    if (winner == -1)
      winner = Board::heuristic_2(new_board) > 0.5 ? Board::valid_board_player(new_board) : 1 - Board::valid_board_player(new_board);

    while (current != NULL)
    {
      current->visits++;
      current->wins += winner == Board::move_to_player(current->move_number);

      if (current != root)
        current->uct_value = 8 * (current->heuristic_value - 0.5) / (((double) current->visits + 1)) +  current->wins / ((double) current->visits) + UCTK * sqrt(log(current->parent->visits) / ((double) current->visits));

      current = current->parent;
    }
  }

  Move best_move;
  int best_visits = -1, index;
  for (i = 0; i < (int)root->children.size(); i++)
  {
    if (root->children[i]->visits > best_visits)
    {
      best_move = root->children[i]->move;
      best_visits = root->children[i]->visits;
      index = i;
    }

//    printf("%s\n", Board::encode_move(root->children[i]->move));
//    printf("%d %d\n", root->children[i]->wins, root->children[i]->visits);

    if (root->children[i]->heuristic_value == 1.0)
      return root->children[i]->move;
  }

  double move_heuristic = root->children[index]->heuristic_value;

  for (i = 0; i < (int)root->children.size(); i++)
    if (root->children[i]->visits - best_visits < 10 && root->children[i]->heuristic_value > move_heuristic)
    {
      best_move = root->children[i]->move;
      move_heuristic = root->children[i]->heuristic_value;
    }

//  printf("%c - %s - %d\n", best_move.first, Board::encode_move(best_move), (int)root->children.size());

  return best_move;
}

void MCTS::solve(board initial_board, int initial_move)
{
/*
  printf("%d\n", Board::valid_board_player(initial_board));
  Board::print_board(initial_board);
  printf("%lf\n", Board::heuristic_2(initial_board));

  root = new node();
  return;*/

  if (initial_move == 0)
  {
    list_moves.push_back(Move('p', 3 + rand() % 2 + 8 * (3 + rand() % 2)));
    return;
  }

  list_moves.push_back(UCT(NULL, initial_board, initial_move, 0, 0));

  for (int i = 0; i < (int)root->children.size(); i++)
  {
    if (root->children[i]->move == list_moves[0])
    {
      root = root->children[i];
      delete root->parent;
      break;
    }
  }

  initial_board = Board::make_move(initial_board, list_moves[0], Board::move_to_player(initial_move));

  list_moves.push_back(UCT(root, initial_board, initial_move + 1, list_moves[0].first == 'p', list_moves[0].first == 'm'));
}
