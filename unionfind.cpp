#include "unionfind.h"

UnionFind::UnionFind(int n) {
  size = n;
  v = (int *) malloc(n * sizeof(int));
  rank = (int *) malloc(n * sizeof(int));
  sizes = (int *) malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) {
    v[i] = 0;
    rank[i] = 0;
    sizes[i] = 1;
  }
  count = n;
}

UnionFind::~UnionFind() {
  free(v); free(rank); free(sizes);
}

int UnionFind::setSize(int a) {
  return sizes[a];
}

int UnionFind::setCount() {
  return count;
}

int UnionFind::setFind(int i) {
  return ((v[i] == i) ? i : (v[i] = setFind(v[i])));
}

bool UnionFind::isSameSet(int a, int b) {
  return (setFind(a) == setFind(b)); 
}

void UnionFind::setUnion(int a, int b)
{
  if (!isSameSet(a, b))
  {
    count--;
    sizes[a] = sizes[b] = sizes[a] + sizes[b];
    int x = setFind(a), y = setFind(b);
    if (rank[x] > rank[y]) v[y] = x;
    else
    {
      v[x] = y;
      if (rank[x] == rank[y]) rank[y]++;
    }
  }
}
