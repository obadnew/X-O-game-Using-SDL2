#include <SDL.h>
#include <iostream>
//#include <SDL2/SDL2_gfxPrimitives.h>

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 600;
int Counter = 0;
enum Player { NONE, PLAYER_X, PLAYER_O };
Player board[3][3] = { NONE };
Player currentPlayer = PLAYER_X; // Start with PLAYER_X

void drawBoard(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < 3; ++i) {
        SDL_RenderDrawLine(renderer, i * (WINDOW_WIDTH / 3), 0, i * (WINDOW_WIDTH / 3), WINDOW_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, i * (WINDOW_HEIGHT / 3), WINDOW_WIDTH, i * (WINDOW_HEIGHT / 3));
    }
}

// Function to draw a single circle outline
void circleOutline(SDL_Renderer* renderer, int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    int offset_x = 0;
    int offset_y = radius;
    int d = 1 - radius;

    while (offset_y >= offset_x) {
        SDL_RenderDrawPoint(renderer, x + offset_x, y + offset_y);
        SDL_RenderDrawPoint(renderer, x + offset_y, y + offset_x);
        SDL_RenderDrawPoint(renderer, x - offset_x, y + offset_y);
        SDL_RenderDrawPoint(renderer, x - offset_y, y + offset_x);
        SDL_RenderDrawPoint(renderer, x + offset_x, y - offset_y);
        SDL_RenderDrawPoint(renderer, x + offset_y, y - offset_x);
        SDL_RenderDrawPoint(renderer, x - offset_x, y - offset_y);
        SDL_RenderDrawPoint(renderer, x - offset_y, y - offset_x);

        if (d < 0) {
            d += 2 * offset_x + 3;
        }
        else {
            d += 2 * (offset_x - offset_y) + 5;
            offset_y--;
        }
        offset_x++;
    }
}

// Function to draw a thick circle outline
void thickCircleOutline(SDL_Renderer* renderer, int x, int y, int radius, int thickness, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    for (int t = 0; t < thickness; t++) {
        circleOutline(renderer, x, y, radius + t - thickness / 2, r, g, b, a);
    }
}



// Function to draw a thick line
void drawThickLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness) {
    double angle = atan2(y2 - y1, x2 - x1);
    double half_thickness = thickness / 2.0;

    for (double i = -half_thickness; i <= half_thickness; i += 1.0) {
        double offset_x = i * sin(angle);
        double offset_y = i * -cos(angle);
        SDL_RenderDrawLine(renderer, x1 + offset_x, y1 + offset_y, x2 + offset_x, y2 + offset_y);
    }
}



/*void thickCircleRGBA(SDL_Renderer* renderer, int x, int y, int radius, int thickness, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    for (int t = 0; t < thickness; t++) {
        int rr = radius + t - thickness / 2;
        for (int w = 0; w < rr * 2; w++) {
            int h = (int)sqrt(rr * rr - (rr - w) * (rr - w));
            SDL_RenderDrawPoint(renderer, x + (rr - w), y + h);
            SDL_RenderDrawPoint(renderer, x + (rr - w), y - h);
        }
    }
}*/

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

// Function to draw a filled circle
void filledCircleRGBA(SDL_Renderer* renderer, int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    for (int w = -radius; w <= radius; w++) {
        for (int h = -radius; h <= radius; h++) {
            if ((w * w + h * h) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + w, y + h);
            }
        }
    }
}

void thickCircleRGBA(SDL_Renderer* renderer, int x, int y, int radius, int thickness, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    for (int t = 0; t < thickness; t++) {
        filledCircleRGBA(renderer, x, y, radius + t - thickness / 2, r, g, b, a);
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
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for 'X'
                    drawThickLine(renderer, col * (WINDOW_WIDTH / 3) +0.05* WINDOW_WIDTH, row * (WINDOW_HEIGHT / 3) + 0.05 * WINDOW_HEIGHT,
                        (col + 1) * (WINDOW_WIDTH / 3) - 0.05 * WINDOW_WIDTH, (row + 1) * (WINDOW_HEIGHT / 3) - 0.05 * WINDOW_HEIGHT, 7);
                    drawThickLine(renderer, (col + 1) * (WINDOW_WIDTH / 3) - 0.05 * WINDOW_WIDTH, row * (WINDOW_HEIGHT / 3) + 0.05 * WINDOW_HEIGHT,
                        col * (WINDOW_WIDTH / 3) + 0.05 * WINDOW_WIDTH, (row + 1) * (WINDOW_HEIGHT / 3) - 0.05 * WINDOW_HEIGHT,7);
                }
                else if (board[row][col] == PLAYER_O) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue color for 'O'
                    int centerX = col * (WINDOW_WIDTH / 3) + (WINDOW_WIDTH / 6);
                    int centerY = row * (WINDOW_HEIGHT / 3) + (WINDOW_HEIGHT / 6);
                    int radius;
                    if (WINDOW_WIDTH< WINDOW_HEIGHT){
                         radius = (WINDOW_WIDTH / 6) - 10;
                    }
                    else {
                         radius = (WINDOW_HEIGHT / 6) - 10;

                    }
                    thickCircleOutline(renderer, centerX, centerY, radius,7, 0, 0, 255, 255);

                }
            }
        }
    }

}

void resetGame() {
    // Reset the board
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            board[row][col] = NONE;
        }
    }
    // Reset the current player
    currentPlayer = PLAYER_X;
}


bool checkWin(Player player) {
    // Check rows and columns
    for (int i = 0; i < 3; ++i) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
            return true;
        }
    }
    // Check diagonals
    if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
        (board[0][2] == player && board[1][1] == player && board[2][0] == player)) {
        return true;
    }
    return false;
}


void testDrawCircle(SDL_Renderer* renderer) {
    // Test by drawing a circle in the middle of the screen
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    int radius = 50;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue color
    SDL_RenderDrawCircle(renderer, centerX, centerY, radius);
}


int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Tic-Tac-Toe", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Main event loop
    bool running = true;
    SDL_Event event;
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int col = event.button.x / (WINDOW_WIDTH / 3);
                int row = event.button.y / (WINDOW_HEIGHT / 3);
                if (board[row][col] == NONE) {
                    board[row][col] = currentPlayer;
                    if (checkWin(currentPlayer)) {
                        std::cout << (currentPlayer == PLAYER_X ? "Player X" : "Player O") << " wins!" << std::endl;
                        Counter = 0;
                        resetGame();
                    }
                    else {
                        currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;

                    }
                }
                Counter++;
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

        // Present the renderer
        SDL_RenderPresent(renderer);
        if (Counter > 8) {
           Counter = 0;
           resetGame();
        }
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
