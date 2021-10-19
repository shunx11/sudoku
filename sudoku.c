// This file implements functions for a game of sudoku

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "sudoku.h"
#include "array_tools.h"

// DIM is the dimension of the Sudoku board
#define DIM 9

// DIMBOX is the dimension of a box
static const int DIMBOX = 3;
// EMPTY is the code for an empty cell
static const int EMPTY = 0;
// PRINT holds the characters for printing out the Sudoku board
static const char PRINT[] = { '_', '1', '2', '3', '4',
                              '5', '6', '7', '8', '9' };
// SUCCESS indicates the successful execution of a function
static const int SUCCESS = 0;
// CONSTRAINT_VIOLATED indicates that a row-, column, or box-constraint is
//   violated
static const int CONSTRAINT_VIOLATED = -1;
// MAXNUM indicates the maximum number for a cell
static const int MAXNUM = 9;
// MINNUM indicates the minimum number for a cell
static const int MINNUM = 1;

struct sudoku {
  // puzzle contains the start state of the puzzle
  int puzzle[DIM * DIM];
  // solution contains is equal to puzzle at the beginning and then stores
  //   all numbers filled in by the player.
  int solution[DIM * DIM];
};

// see sudoku.h for documentation
struct sudoku *sudoku_read(void) {
  struct sudoku *su = malloc(sizeof(struct sudoku));
  assert(su);
  int *pPuz = su->puzzle, *pSol = su->solution;
  for (int cnt = 0; cnt < DIM * DIM; ++cnt, ++pPuz, ++pSol) {
    char c = '\0';
    assert(scanf(" %c", &c) == 1);
    *pPuz = *pSol = (c == PRINT[EMPTY]) ? 0 : c - '0';
  }
  return su;
}

// see sudoku.h for documentation
void sudoku_destroy(struct sudoku *su) {
  assert(su);
  free(su);
}

// solution_print_separator() prints out a separator between boxes.
// produces output
static void solution_print_separator(void) {
  for (int box = 0; box < DIM / DIMBOX; ++box) {
    printf("+");
    for (int i = 0; i < 3 * DIMBOX; ++i) {
      printf("-");
    }
  }
  printf("+\n");
}

// see sudoku.h for documentation
void solution_print(const struct sudoku *su) {
  assert(su);

  for (int r = 0; r < DIM; ++r) {
    if (r % DIMBOX == 0) {
      solution_print_separator();
    }
    for (int c = 0; c < DIM; ++c) {
      if (c % DIMBOX == 0) {
        printf("|");
      }
      printf(" %c ", PRINT[su->solution[r * DIM + c]]);
    }
    printf("|\n");
  }
  solution_print_separator();
  printf("\n");
}

// see sudoku.h for documentation
void solution_reset(struct sudoku *su) {
  assert(su);
  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
      su->solution[row * DIM + col] = su->puzzle[row * DIM + col];
    }
  }  
}

// see sudoku.h for documentation
bool cell_erase(struct sudoku *su, const int row, const int col) {
  assert(su);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);

  if (su->puzzle[row * DIM + col] != EMPTY) {
    return false;
  } else {
    su->solution[row * DIM + col] = EMPTY;
    return true;
  }
}

// === Helper Functions =======================================================

// checkrow(su, row, num) returns true if num is valid for the row and false,
//   otherwise
// requires: row must be valid
//           num must be valid
// time: O(n)
static bool checkrow(const struct sudoku *su, const int row, const int num) {
  assert(su);
  assert(0 <= row && row <= DIM - 1);
  assert(MINNUM <= num && num <= MAXNUM);
  for (int c = 0; c < DIM; ++c) {
    if (su->solution[row * DIM + c] == num) {
      return false;
    }
  }
  return true;
}

// checkcol(su, col, num) returns true if num is valid for the column and false
//   otherwise
// requires: col must be valid
//           num must be valid
// time: O(n)
static bool checkcol(const struct sudoku *su, const int col, const int num) {
  assert(su);
  assert(0 <= col && col <= DIM - 1);
  assert(MINNUM <= num && num <= MAXNUM);
  for (int r = 0; r < DIM; ++r) {
    if (su->solution[r * DIM + col] == num) {
      return false;
    }
  }
  return true;
}

// checkbox(su, row, col, num) returns true if num is valid for the box it is
//   in and false otherwise
// requires: row and col are valid indices. 
//           num is an integer between 1 and 9.
// time: O(n^2) for n = DIMBOX
static bool checkbox(const struct sudoku *su, const int row,
                     const int col, const int num) {
  assert(su);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);
  assert(MINNUM <= num && num <= MAXNUM);
  int srbox = (row / DIMBOX) * DIMBOX;
  int scbox = (col / DIMBOX) * DIMBOX;
  for (int r = srbox; r < srbox + DIMBOX; ++r) {
    for (int c = scbox; c < scbox + DIMBOX; ++c) {
      if (su->solution[r * DIM + c] == num) {
        return false;
      }
    }
  }
  return true;
}
// ============================================================================

// see sudoku.h for documentation
bool cell_fill(struct sudoku *su, const int row, const int col, const int num) {
  assert(su);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);
  assert(MINNUM <= num && num <= MAXNUM);
  if (su->solution[row * DIM + col] != EMPTY) {
    return false;
  } else if (checkrow(su, row, num) == false) {
     return false;
  } else if (checkcol(su, col, num) == false) {
     return false;
  } else if (checkbox(su, row, col, num) == false) {
     return false;
  } else {
    su->solution[row * DIM + col] = num;
    return true;
  }
}

// see sudoku.h for documentation
bool puzzle_solved(const struct sudoku *su) {
  assert(su);
  int t = 0;
  for (int r = 0; r < DIM; ++r) {
    for (int c = 0; c < DIM; ++c) {
      if (su->solution[r * DIM + c] == EMPTY) {
        return false;
      } //else {
        if (checkrow(su, r, su->solution[r * DIM + c]) == false) {
          ++t;
        } else if (checkcol(su, c, su->solution[r * DIM + c]) == false) {
          ++t;
        } else if (checkbox(su, r, c, su->solution[r * DIM + c]) == false) {
          ++t;
        }
    }
  }
  return !t;
}

// see sudoku.h for documentation
int cell_choices(const struct sudoku *su, const int row,
                 const int col, int choices[]) {
  assert(su);
  assert(choices);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);
  int num_choices = 0;
  if (su->solution[row * DIM + col] != EMPTY) {
    return num_choices;
  }
  for (int n = MINNUM; n < MAXNUM + 1; ++n) {
    if (checkrow(su, row, n) == true) {
      if (checkcol(su, col, n) == true) {
        if (checkbox(su, row, col, n) == true) {
          ++num_choices;
          choices[num_choices - 1] = n;
        }
      }
    }
  }
  return num_choices;
}

// see sudoku.h for documentation
bool cell_hint(const struct sudoku *su, int *row, int *col) {
  assert(su);
  assert(row);
  assert(col);
  for (int r = 0; r < DIM; ++r) {
    for (int c = 0; c < DIM; ++c) {
      if (su->solution[r * DIM + c] == EMPTY) {
        int choices[9];
        int choices_len = cell_choices(su, r, c, choices);
        if (choices_len == 1) {
          *row = r;
          *col = c;
          return true;
        }
      }
    }
  }
  return false;
}

// solveworker(su, row, col) takes a sudoku board and tries to fill in empty
//   cells and solve the board.
// requires: row and col are valid indices. 
//           num is an integer between 1 and 9.
// effects: might mutate *su
// time: O(n) n = MAXNUM
int solveworker(struct sudoku *su, int row, int col) {
  assert(su);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM);
  if (row == DIM - 1 && col == DIM) {
    return !SUCCESS;
  }
  if (col == DIM) {
    ++row;
    col = 0;
  }
  if (su->solution[row * DIM + col] != EMPTY) {
    return solveworker(su, row, col + 1);
  }
  for (int num = MINNUM; num <= MAXNUM; ++num) {
    if (cell_fill(su, row, col, num) == true) {
      if (solveworker(su, row, col + 1) == !SUCCESS) {
        return !SUCCESS;
      }
    }
    cell_erase(su, row, col);
  }
  return SUCCESS;
}

// see sudoku.h for documentation
bool sudoku_solve(struct sudoku *su) {
  assert(su);
  return solveworker(su, 0, 0);
}