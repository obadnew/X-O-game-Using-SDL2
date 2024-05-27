#include <SDL.h>
#include <iostream>

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 600;

enum Player { NONE, PLAYER_X, PLAYER_O };
Player board[3][3] = { NONE };

void drawBoard(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < 3; ++i) {
        SDL_RenderDrawLine(renderer, i * (WINDOW_WIDTH / 3), 0, i * (WINDOW_WIDTH / 3), WINDOW_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, i * (WINDOW_HEIGHT / 3), WINDOW_WIDTH, i * (WINDOW_HEIGHT / 3));
    }
}

void SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius) {
    int offsetX, offsetY, d;
    offsetX = 0;
    offsetY = radius;
    d = radius - 1;
    while (offsetY >= offsetX) {
        SDL_RenderDrawPoint(renderer, x + offsetX, y + offsetY);
        SDL_RenderDrawPoint(renderer, x + offsetY, y + offsetX);
        SDL_RenderDrawPoint(renderer, x - offsetX, y + offsetY);
        SDL_RenderDrawPoint(renderer, x - offsetY, y + offsetX);
        SDL_RenderDrawPoint(renderer, x + offsetX, y - offsetY);
        SDL_RenderDrawPoint(renderer, x + offsetY, y - offsetX);
        SDL_RenderDrawPoint(renderer, x - offsetX, y - offsetY);
        SDL_RenderDrawPoint(renderer, x - offsetY, y - offsetX);
        if (d >= 2 * offsetX) {
            d -= 2 * offsetX + 1;
            offsetX++;
        }
        else if (d < 2 * (radius - offsetY)) {
            d += 2 * offsetY - 1;
            offsetY--;
        }
        else {
            d += 2 * (offsetY - offsetX - 1);
            offsetY--;
            offsetX++;
        }
    }
}

bool placeMark(int x, int y, Player currentPlayer) {
    int row = y / (WINDOW_HEIGHT / 3);
    int col = x / (WINDOW_WIDTH / 3);
    if (board[row][col] == NONE) {
        board[row][col] = currentPlayer;
        return true;
    }
    return false;
}

void drawMarks(SDL_Renderer* renderer) {
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (board[row][col] != NONE) {
                if (board[row][col] == PLAYER_X) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderDrawLine(renderer, col * (WINDOW_WIDTH / 3), row * (WINDOW_HEIGHT / 3),
                        (col + 1) * (WINDOW_WIDTH / 3), (row + 1) * (WINDOW_HEIGHT / 3));
                    SDL_RenderDrawLine(renderer, (col + 1) * (WINDOW_HEIGHT / 3), row * (WINDOW_HEIGHT / 3),
                        col * (WINDOW_HEIGHT / 3), (row + 1) * (WINDOW_HEIGHT / 3));
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    int centerX = col * (WINDOW_WIDTH / 3) + (WINDOW_WIDTH / 6);
                    int centerY = row * (WINDOW_HEIGHT / 3) + (WINDOW_HEIGHT / 6);
                    int radius = (WINDOW_WIDTH / 6) - 10;
                    SDL_RenderDrawCircle(renderer, centerX, centerY, radius);
                }
            }
        }
    }
}
Player checkWin() {
    // Check rows and columns
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != NONE) {
            return board[i][0];
        }
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != NONE) {
            return board[0][i];
        }
    }
    // Check diagonals
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != NONE) {
        return board[0][0];
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != NONE) {
        return board[0][2];
    }
    return NONE;
}



int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Tic-Tac-Toe", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    Player currentPlayer = PLAYER_X;
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (placeMark(event.button.x, event.button.y, currentPlayer)) {
                    if (checkWin() != NONE) {
                        std::cout << "Player " << (currentPlayer == PLAYER_X ? "X" : "O") << " wins!\n";
                        memset(board, NONE, sizeof(board));
                    }
                    currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
                }
            }
            else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    WINDOW_WIDTH = event.window.data1;
                    WINDOW_HEIGHT = event.window.data2;
                }
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Draw the game board and marks
        drawBoard(renderer);
        drawMarks(renderer);

        SDL_RenderPresent(renderer);
    }
   

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Draw the game board and marks
        drawBoard(renderer);
        drawMarks(renderer);

        SDL_RenderPresent(renderer);
    

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
