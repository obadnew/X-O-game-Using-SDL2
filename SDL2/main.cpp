#include <SDL.h>
#include <iostream>
#include <SDL_ttf.h>

//#include <SDL2/SDL2_gfxPrimitives.h>

int WINDOW_WIDTH = 900;
int WINDOW_HEIGHT = 675;
int Counter = 0;
int X_Score = 0;
int O_Score = 0;
int number_of_draw = 0;



enum Player { NONE, PLAYER_X, PLAYER_O };
Player board[3][3] = { NONE };
Player currentPlayer = PLAYER_X; // Start with PLAYER_X





SDL_Texture* renderText(const std::string& message, SDL_Color color, TTF_Font* font, SDL_Renderer* renderer) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, message.c_str(), color);
    if (!surface) {
        std::cerr << "TTF_RenderText_Blended Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
void drawThickLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness) {
    double angle = atan2(y2 - y1, x2 - x1);
    double half_thickness = thickness / 2.0;

    for (double i = -half_thickness; i <= half_thickness; i += 1.0) {
        double offset_x = i * sin(angle);
        double offset_y = i * -cos(angle);
        SDL_RenderDrawLine(renderer, x1 + offset_x, y1 + offset_y, x2 + offset_x, y2 + offset_y);
    }
}
void onButtonClick() {
    std::cout << "Score reseted" << std::endl;
    X_Score = 0;
    O_Score = 0;
    number_of_draw = 0;
}

void drawBoard(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < 3; ++i) {
        SDL_RenderDrawLine(renderer, i * (WINDOW_WIDTH / 4), 0, i * (WINDOW_WIDTH / 4), WINDOW_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, i * (WINDOW_HEIGHT / 3), WINDOW_WIDTH*0.75, i * (WINDOW_HEIGHT / 3));
    }
    drawThickLine(renderer, (WINDOW_WIDTH * 3 / 4), 0, 3 * (WINDOW_WIDTH / 4), WINDOW_HEIGHT,5);

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


struct Button {
    SDL_Rect rect;
    SDL_Color color;
    SDL_Color hoverColor;
    SDL_Color clickColor;
    bool isHovered;
    bool isClicked;
    void (*onClick)(); // Function pointer for the button action
};

void renderButton(SDL_Renderer* renderer, Button* button) {
    SDL_Color color;
    if (button->isClicked) {
        color = button->clickColor;
    }
    else if (button->isHovered) {
        color = button->hoverColor;
    }
    else {
        color = button->color;
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &button->rect);
}
void handleButtonEvent(SDL_Event* event, Button* button) {
    int x, y;
    SDL_GetMouseState(&x, &y);

    bool inside = (x >= button->rect.x && x <= button->rect.x + button->rect.w &&
        y >= button->rect.y && y <= button->rect.y + button->rect.h);

    if (inside) {
        button->isHovered = true;
        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
            button->isClicked = true;
        }
        else if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
            button->isClicked = false;
            if (button->onClick != nullptr) {
                button->onClick(); // Trigger button action
            }
        }
    }
    else {
        button->isHovered = false;
        button->isClicked = false;
    }
}

// Function to draw a thick line



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
                    drawThickLine(renderer, col * (WINDOW_WIDTH / 4) +0.05* WINDOW_WIDTH*0.75, row * (WINDOW_HEIGHT / 3) + 0.05 * WINDOW_HEIGHT,
                        (col + 1) * (WINDOW_WIDTH / 4) - 0.05 * WINDOW_WIDTH*0.75, (row + 1) * (WINDOW_HEIGHT / 3) - 0.05 * WINDOW_HEIGHT, 7);
                    drawThickLine(renderer, (col + 1) * (WINDOW_WIDTH / 4) - 0.05 * WINDOW_WIDTH*0.75, row * (WINDOW_HEIGHT / 3) + 0.05 * WINDOW_HEIGHT,
                        col * (WINDOW_WIDTH / 4) + 0.05 * WINDOW_WIDTH*0.75, (row + 1) * (WINDOW_HEIGHT / 3) - 0.05 * WINDOW_HEIGHT,7);
                }
                else if (board[row][col] == PLAYER_O) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue color for 'O'
                    int centerX = col * (WINDOW_WIDTH / 4) + (WINDOW_WIDTH / 8);
                    int centerY = row * (WINDOW_HEIGHT / 3) + (WINDOW_HEIGHT / 6);
                    int radius;
                    if (WINDOW_WIDTH< WINDOW_HEIGHT){
                         radius = (WINDOW_WIDTH / 8) - 10;
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
    Counter = 0;


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
    //clear screen button
    Button button;
    button.rect = { 700, 50, 60, 25 };
    button.color = { 0,192,192,192 };      // Blue
    button.hoverColor = { 0, 100, 255, 255 }; // Light blue
    button.clickColor = { 0, 0, 150, 255 }; // Dark blue
    button.isHovered = false;
    button.isClicked = false;
    button.onClick = resetGame;


    //rest score button
    Button button1;
    button1.rect = { 700, 150, 60, 25 };
    button1.color = { 0, 0, 255, 0 };      // Blue
    button1.hoverColor = { 0, 100, 255, 255 }; // Light blue
    button1.clickColor = { 0, 0, 150, 255 }; // Dark blue
    button1.isHovered = false;
    button1.isClicked = false;
    button1.onClick = onButtonClick;

    // Main event loop
    bool running = true;
    SDL_Event event;
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            handleButtonEvent(&event, &button);
            handleButtonEvent(&event, &button1);

            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
               
                int col = event.button.x / (WINDOW_WIDTH / 4);
                int row = event.button.y / (WINDOW_HEIGHT / 3);
                if (col == 3) {
                    break;

                }
                if (board[row][col] == NONE) {
                    board[row][col] = currentPlayer;
                    if (checkWin(currentPlayer)) {
                        std::cout << (currentPlayer == PLAYER_X ? "Player X" : "Player O") << " wins!" << std::endl;
                        currentPlayer == PLAYER_X ? X_Score++ : O_Score++;
                        std::cout << "X Score = "<< X_Score<< ",   O Score = "<<O_Score<<",   number of draws = "<< number_of_draw<<std::endl;
                        resetGame();
                    }
                    else {
                        currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
                        Counter++;
                    }
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
        

        // Render the button
        renderButton(renderer, &button);
        renderButton(renderer, &button1);

        // Draw the game board and marks
        drawBoard(renderer);
        drawMarks(renderer);

        // Present the renderer
        SDL_RenderPresent(renderer);
        if (Counter > 8) {
           Counter = 0;
           number_of_draw++;
           std::cout << "it is a draw "<< std::endl<<"X Score = " << X_Score << ", O Score = " << O_Score << ", number of draws = " << number_of_draw << std::endl;
           resetGame();
        }

       


    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
