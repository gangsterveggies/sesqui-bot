#ifndef _UNIONFIND_
#define _UNIONFIND_

#include <stdlib.h>

class UnionFind {
public:
  UnionFind(int n);
  ~UnionFind();
  void setUnion(int a, int b);
  bool isSameSet(int a, int b);
  int setCount();
  int setSize(int a);
private:
  int setFind(int a);

  int * v;
  int * rank;
  int * sizes;
  int count;
  int size;
};

#endif
