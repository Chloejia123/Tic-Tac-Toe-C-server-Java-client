/**
 * This is the server file written in c, can be ran using "make run_native"
 * or run in docker using "make run"
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ulfius.h>

#define PORT 1234
// A struct stores a move the server to take.
struct chosen_move {
  int row, col;
};
typedef struct chosen_move Move;

int player_id = 1;
char *player = "[O]";   // player's mark
char *computer = "[X]"; // computer's mark

/* 
 Input: const struct _u_request * request, struct _u_response * response, void
  user_data
  Return: a new game with a human player and a server player, also decide who
  goes first.
  Side-effect: none.
*/
int create_game(const struct _u_request *request, struct _u_response *response,
                void *user_data) {
  char greeting[50];
  sprintf(greeting, "Welcome To The Game! You are player %d", player_id++);
  json_auto_t *json_ini = NULL;
  json_ini = json_object();
  json_auto_t *json_greeting_message = json_string(greeting);
  json_object_set(json_ini, "greeting", json_greeting_message);

  // 0 means sever goes first, 1 means player goes first.
  int upper = 2;
  srand(time(NULL));
  int target_int = -1;
  while (target_int == -1) {
    srand(time(NULL));
    target_int = rand() % upper;
  }

  char who_go_first[12];
  if (target_int == 0) {
    sprintf(who_go_first, "%s", "Server");
  } else {
    sprintf(who_go_first, "%s", "Client");
  }

  json_auto_t *json_who_go_first = json_string(who_go_first);

  json_object_set(json_ini, "who_go_first", json_who_go_first);

  ulfius_set_json_body_response(response, 200, json_ini);

  return U_CALLBACK_CONTINUE;
}

/*
  Input: The 2d array board
  Output: The result if the game can go on
  Return: int to indicate if the game can still go on
*/
int has_empty_cell(const char *board[3][3]) {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (strcmp(board[i][j], "[ ]") == 0) {
        return 1;
      }
  return 0;
}

/*
  Input: The 2d array board
  Output: The result of the value of each move
  Return: The result of the value of each move
*/
int eval_move(const char *board[3][3]) {
  for (int row = 0; row < 3; row++) {
    if (strcmp(board[row][0], board[row][1]) == 0 &&
        strcmp(board[row][1], board[row][2]) == 0) {
      if (strcmp(board[row][0], computer) == 0)
        return +10;
      else if (strcmp(board[row][0], player) == 0)
        return -10;
    }
  }

  for (int col = 0; col < 3; col++) {
    if (strcmp(board[0][col], board[1][col]) == 0 &&
        strcmp(board[1][col], board[2][col]) == 0) {
      if (strcmp(board[0][col], computer) == 0)
        return +10;

      else if (strcmp(board[0][col], player) == 0)
        return -10;
    }
  }

  if (strcmp(board[0][0], board[1][1]) == 0 &&
      strcmp(board[1][1], board[2][2]) == 0) {
    if (strcmp(board[0][0], computer) == 0)
      return +10;
    else if (strcmp(board[0][0], player) == 0)
      return -10;
  }

  if (strcmp(board[0][2], board[1][1]) == 0 &&
      strcmp(board[1][1], board[2][0]) == 0) {
    if (strcmp(board[0][2], computer) == 0)
      return +10;
    else if (strcmp(board[0][2], player) == 0)
      return -10;
  }

  return 0;
}

/* Input: The board, current depth and
          int isMax, indicating who to go currently.
   Return: best_move of the server.
   Side-effect: none.
*/
int minimax(const char *board[3][3], int depth, int isMax) {
  int score = eval_move(board);

  if (score == 10)
    return score;

  if (score == -10)
    return score;

  if (has_empty_cell(board) == 0)
    return 0;

  // If this maximizer's move
  if (isMax) {
    int best = -1000;

    // Traverse all cells
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        // Check if cell is empty
        if (strcmp(board[i][j], "[ ]") == 0) {
          // Make the move
          board[i][j] = computer;

          // Call minimax recursively and choose
          // the maximum value

          best = (best > minimax(board, depth + 1, !isMax)
                      ? best
                      : minimax(board, depth + 1, !isMax));

          // Undo the move
          board[i][j] = "[ ]";
        }
      }
    }
    return best;
  }

  // If this minimizer's move
  else {
    int best = 1000;

    // Traverse all cells
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        // Check if cell is empty
        if (strcmp(board[i][j], "[ ]") == 0) {
          // Make the move
          board[i][j] = player;

          // Call minimax recursively and choose
          // the minimum value
          best = (best < minimax(board, depth + 1, !isMax)
                      ? best
                      : minimax(board, depth + 1, !isMax));

          // Undo the move
          board[i][j] = "[ ]";
        }
      }
    }
    return best;
  }
}

/* 
  Input: The board.
  Return: Get best move of the server.
  Side-effect: none.
*/
Move get_optimal_move(const char *board[3][3]) {
  Move best_move;
  best_move.row = -10;
  best_move.col = -10;
  int bestVal = -500;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      // Check if celll is empty
      if (strcmp(board[i][j], "[ ]") == 0) {

        // Make the move
        board[i][j] = computer;

        // compute evaluation function for this
        // move.
        int move_val = minimax(board, 0, 0);

        // Undo the move
        board[i][j] = "[ ]";

        // If the value of the current move is
        // more than the best value, then update
        // best/
        if (move_val > bestVal) {
          best_move.row = i;
          best_move.col = j;
          bestVal = move_val;
        }
      }
    }
  }
  return best_move;
}

/* 
  Input:const struct _u_request * request, struct _u_response * response, void
   * user_data
  Return: Server to make move.
  Side-effect: none.
*/
int take_move(const struct _u_request *request, struct _u_response *response,
              void *user_data) {
  json_auto_t *board = ulfius_get_json_body_request(request, NULL);

  if (!board) {
    printf("board is null, exiting\n");
  }

  json_auto_t *jsonBoardArray;
  json_auto_t *jsonNewBoard = NULL;
  jsonBoardArray = json_object_get(board, "boardArray");
  int board_size = json_array_size(jsonBoardArray);
  const char *board_from_client[board_size];
  json_auto_t *board_to_client = NULL;

  size_t index;
  json_t *value;
  json_array_foreach(jsonBoardArray, index, value) {
    board_from_client[index] = json_string_value(value);
  }

  const char *new_board[3][3];
  int index_2D = 0;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      new_board[i][j] =
          json_string_value(json_array_get(jsonBoardArray, index_2D));
      index_2D++;
    }
  }

  int valid = -1;
  while (valid == -1) {
    Move best_move;
    best_move = get_optimal_move(new_board);
    int row = best_move.row;
    int col = best_move.col;
    int move = row * 3 + col;
    if (strcmp(board_from_client[move], "[ ]") == 0) {
      board_from_client[move] = "[X]";
      valid = 1;
    }
  }

  printf("The board status for player %d is: \n", player_id-1);
  for (int i = 0; i < 9; i++) {
    if (i % 3 == 0 && i != 0) {
      printf("%s", "\n");
    }
    printf("%s", board_from_client[i]);
  }
  printf("%s", "\n\n");

  json_t *array = json_array();
  for (int i = 0; i < board_size; i++) {
    board_to_client = json_string(board_from_client[i]);
    json_array_insert(array, i, board_to_client);
  }

  jsonNewBoard = json_object();
  json_object_set(jsonNewBoard, "boardArray", array);
  ulfius_set_json_body_response(response, 200, jsonNewBoard);
  return U_CALLBACK_CONTINUE;
}

/*
  main function
 */
int main(void) {
  struct _u_instance instance;

  // Initialize instance with the port number
  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error ulfius_init_instance, abort\n");
    return (1);
  }

  // Endpoint list declaration - initialize game
  ulfius_add_endpoint_by_val(&instance, "GET", "/game", NULL, 0, &create_game,
                             NULL);

  // Endpoint list declaration - initialize game
  ulfius_add_endpoint_by_val(&instance, "POST", "/move", NULL, 0, &take_move,
                             NULL);

  // Start the framework
  if (ulfius_start_framework(&instance) == U_OK) {
    printf("Start Game Server at Port: %d\n", instance.port);

    // Wait for the user to press <enter> in console to quit the application
    getchar();
  } else {
    fprintf(stderr, "Error starting framework\n");
  }
  printf("Game Server now closed.\n");

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);

  return 0;
}
