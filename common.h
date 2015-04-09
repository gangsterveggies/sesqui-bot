#ifndef _COMMON_
#define _COMMON_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <queue>
#include <list>
#include <unordered_map>
#include <map>
#include <time.h>

#define INF 1e30

using namespace std;

typedef pair<int, int> pii;
typedef pair<char, int> Move;

/*
 * Move representation:
 *
 * char -> 'm' or 'p': move or place
 *
 * int -> first 12 bits: x + y * 8 of piece
 *     -> last 12 bits: x + y * 8 of new piece (in case of move)
 *
 */

typedef pair<long long int, long long int> board;

/*
 * Board representation:
 *
 *  1  2  3  4  5  6  7  8
 *  9 10 11 12 13 14 15 16
 * 17 18 19 20 21 22 23 24
 * 25 26 27 28 29 30 31 32
 * 33 34 35 36 37 38 39 40
 * 41 42 43 44 45 46 47 48
 * 49 50 51 52 53 54 55 56
 * 57 58 59 60 61 62 63 64
 *
 * b.first -> Is there a piece
 * b.second -> 0 = White, 1 = Black
 *
 */

#endif
