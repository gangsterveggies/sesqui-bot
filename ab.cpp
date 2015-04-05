#include "ab.h"

struct pairhash {
//see http://stackoverflow.com/questions/20590656/error-for-hash-function-of-pair-of-ints
public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
    return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
  }
};

unordered_map<board, list<board>, pairhash> sons;

Move alphabeta(board b) {
}

int MaxValue(board b, int alpha, int beta) {
}

int MinValue(board b, int alpha, int beta) {
}

list<board>& Successors(board b) {
  //use an auxiliar key/value store?
}
