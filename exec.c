#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Structure to represent the game state
typedef struct {
    char board[3][3];
    char current_player;
    int game_over;
    int game_mode; // 1 for Play with X, 2 for Play with O
} Game;

// Function prototypes
void init_game(Game *game);
void draw_board(const Game *game);
void play_turn(Game *game);
void put_symbol(Game *game, int position);
void switch_player(Game *game);
int check_win(const Game *game);
int check_draw(const Game *game);
void save_winner(char winner);
void show_menu();
int get_menu_choice();
void computer_turn(Game *game);
int find_best_move(const Game *game);

int main() {
    Game game;
    int menu_choice;
    
    while (1) {
        show_menu();
        menu_choice = get_menu_choice();
        
        if (menu_choice == 3) {
            printf("\nThank you for playing! Goodbye!\n");
            break;
        }
        
        init_game(&game);
        game.game_mode = menu_choice;
        
        // Main game loop
        while (!game.game_over) {
            draw_board(&game);
            
            // Check if it's computer's turn
            if ((game.game_mode == 1 && game.current_player == 'O') ||
                (game.game_mode == 2 && game.current_player == 'X')) {
                printf("Computer's turn...\n");
                computer_turn(&game);
            } else {
                play_turn(&game);
            }
            
            // Check for win
            if (check_win(&game)) {
                draw_board(&game);
                printf("\nPlayer %c wins!\n", game.current_player);
                save_winner(game.current_player);
                game.game_over = 1;
            }
            // Check for draw
            else if (check_draw(&game)) {
                draw_board(&game);
                printf("\nGame Draw\n");
                save_winner('D');
                game.game_over = 1;
            }
            else {
                // Switch to next player
                switch_player(&game);
            }
        }
        
        printf("\nPress Enter to return to menu...");
        getchar();
        getchar();
    }
    
    return 0;
}

// Display main menu
void show_menu() {
    printf("\n");
    printf("========MENU========\n");
    printf("1 : Play with X\n");
    printf("2 : Play with O\n");
    printf("3 : Exit\n");
    printf("Enter your choice:> ");
}

// Get valid menu choice
int get_menu_choice() {
    int choice;
    
    while (1) {
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // Clear input buffer
            printf("Invalid input! Enter your choice:> ");
            continue;
        }
        
        if (choice >= 1 && choice <= 3) {
            return choice;
        }
        
        printf("Invalid choice! Enter your choice:> ");
    }
}

// Initialize the game structure
void init_game(Game *game) {
    // Initialize empty board
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            game->board[i][j] = ' ';
        }
    }
    
    // Set starting player to X
    game->current_player = 'X';
    
    // Game is not over
    game->game_over = 0;
    
    // Seed random for computer moves
    srand(time(NULL));
}

// Display the game board
void draw_board(const Game *game) {
    printf("\n");
    printf("        %c   :   %c   :   %c\n", game->board[0][0], game->board[0][1], game->board[0][2]);
    printf("    -----------------------\n");
    printf("        %c   :   %c   :   %c\n", game->board[1][0], game->board[1][1], game->board[1][2]);
    printf("    -----------------------\n");
    printf("        %c   :   %c   :   %c\n", game->board[2][0], game->board[2][1], game->board[2][2]);
    printf("\n");
}

// Handle a player's turn
void play_turn(Game *game) {
    int position;
    int valid_move = 0;
    
    while (!valid_move) {
        printf("Your Turn :> ");
        
        if (scanf("%d", &position) != 1) {
            while (getchar() != '\n'); // Clear input buffer
            printf("Invalid input! Please enter a number between 1 and 9.\n");
            continue;
        }
        
        // Validate position range
        if (position < 1 || position > 9) {
            printf("Invalid position! Please enter a number between 1 and 9.\n");
            continue;
        }
        
        // Convert position to row and column
        int row = (position - 1) / 3;
        int col = (position - 1) % 3;
        
        // Check if position is empty
        if (game->board[row][col] == ' ') {
            put_symbol(game, position);
            valid_move = 1;
        } else {
            printf("Position already occupied! Choose another position.\n");
        }
    }
}

// Computer's turn with simple AI
void computer_turn(Game *game) {
    int position = find_best_move(game);
    put_symbol(game, position);
    printf("Computer chose position %d\n", position);
}

// Find best move for computer (simple AI)
int find_best_move(const Game *game) {
    // Try to win
    for (int pos = 1; pos <= 9; pos++) {
        int row = (pos - 1) / 3;
        int col = (pos - 1) % 3;
        
        if (game->board[row][col] == ' ') {
            // Temporarily place symbol
            Game temp = *game;
            temp.board[row][col] = game->current_player;
            
            if (check_win(&temp)) {
                return pos;
            }
        }
    }
    
    // Block opponent from winning
    char opponent = (game->current_player == 'X') ? 'O' : 'X';
    for (int pos = 1; pos <= 9; pos++) {
        int row = (pos - 1) / 3;
        int col = (pos - 1) % 3;
        
        if (game->board[row][col] == ' ') {
            // Temporarily place opponent symbol
            Game temp = *game;
            temp.board[row][col] = opponent;
            temp.current_player = opponent;
            
            if (check_win(&temp)) {
                return pos;
            }
        }
    }
    
    // Take center if available
    if (game->board[1][1] == ' ') {
        return 5;
    }
    
    // Take a corner
    int corners[] = {1, 3, 7, 9};
    for (int i = 0; i < 4; i++) {
        int pos = corners[i];
        int row = (pos - 1) / 3;
        int col = (pos - 1) % 3;
        if (game->board[row][col] == ' ') {
            return pos;
        }
    }
    
    // Take any available position
    for (int pos = 1; pos <= 9; pos++) {
        int row = (pos - 1) / 3;
        int col = (pos - 1) % 3;
        if (game->board[row][col] == ' ') {
            return pos;
        }
    }
    
    return 1; // Fallback (shouldn't reach here)
}

// Place symbol on the board
void put_symbol(Game *game, int position) {
    // Convert position (1-9) to row and column (0-2)
    int row = (position - 1) / 3;
    int col = (position - 1) % 3;
    
    // Place current player's symbol
    game->board[row][col] = game->current_player;
}

// Switch to the other player
void switch_player(Game *game) {
    if (game->current_player == 'X') {
        game->current_player = 'O';
    } else {
        game->current_player = 'X';
    }
}

// Check if current player has won
int check_win(const Game *game) {
    char player = game->current_player;
    
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (game->board[i][0] == player && 
            game->board[i][1] == player && 
            game->board[i][2] == player) {
            return 1;
        }
    }
    
    // Check columns
    for (int j = 0; j < 3; j++) {
        if (game->board[0][j] == player && 
            game->board[1][j] == player && 
            game->board[2][j] == player) {
            return 1;
        }
    }
    
    // Check diagonal (top-left to bottom-right)
    if (game->board[0][0] == player && 
        game->board[1][1] == player && 
        game->board[2][2] == player) {
        return 1;
    }
    
    // Check diagonal (top-right to bottom-left)
    if (game->board[0][2] == player && 
        game->board[1][1] == player && 
        game->board[2][0] == player) {
        return 1;
    }
    
    return 0;
}

// Check if the game is a draw
int check_draw(const Game *game) {
    // Check if all cells are occupied
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (game->board[i][j] == ' ') {
                return 0; // Found empty cell, not a draw
            }
        }
    }
    return 1; // All cells occupied
}

// Save the game result to a file
void save_winner(char winner) {
    FILE *file = fopen("results.txt", "a");
    
    if (file == NULL) {
        printf("Error: Could not open results.txt\n");
        return;
    }
    
    if (winner == 'D') {
        fprintf(file, "Draw\n");
    } else {
        fprintf(file, "Winner : %c\n", winner);
    }
    
    fclose(file);
    printf("Result saved to results.txt\n");
}
