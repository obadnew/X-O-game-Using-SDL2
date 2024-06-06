#include <SDL.h>
#include <iostream>
#include <SDL_ttf.h>
#include<string>
#include <SDL_image.h>
#include <SDL_mixer.h>



int WINDOW_WIDTH = 900;
int WINDOW_HEIGHT = 675;
int Counter = 0;
int X_Score = 0;
int O_Score = 0;
int number_of_draw = 0;
bool flag = false;


enum Player { NONE, PLAYER_X, PLAYER_O };
Player board[3][3] = { NONE };
Player currentPlayer = PLAYER_X; // Start with PLAYER_X





//ftt functions
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


//drawing functions
void drawThickLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness) {
    double angle = atan2(y2 - y1, x2 - x1);
    double half_thickness = thickness / 2.0;

    for (double i = -half_thickness; i <= half_thickness; i += 1.0) {
        double offset_x = i * sin(angle);
        double offset_y = i * -cos(angle);
        SDL_RenderDrawLine(renderer, x1 + offset_x, y1 + offset_y, x2 + offset_x, y2 + offset_y);
    }
}
void drawBoard(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < 3; ++i) {
        SDL_RenderDrawLine(renderer, i * (WINDOW_WIDTH / 4), 0, i * (WINDOW_WIDTH / 4), WINDOW_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, i * (WINDOW_HEIGHT / 3), WINDOW_WIDTH*0.75, i * (WINDOW_HEIGHT / 3));
    }
    drawThickLine(renderer, (WINDOW_WIDTH * 3 / 4), 0, 3 * (WINDOW_WIDTH / 4), WINDOW_HEIGHT,5);

}
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
void thickCircleOutline(SDL_Renderer* renderer, int x, int y, int radius, int thickness, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    for (int t = 0; t < thickness; t++) {
        circleOutline(renderer, x, y, radius + t - thickness / 2, r, g, b, a);
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


// button functions and classes
struct Button {
    SDL_Rect rect;
    SDL_Color color;
    SDL_Color hoverColor;
    SDL_Color clickColor;
    bool isHovered;
    bool isClicked;
    void (*onClick)(); // Function pointer for the button action
};
void renderButton(SDL_Renderer* renderer, Button* button, SDL_Texture* textTexture, SDL_Rect* textRect) {
    SDL_Color color = button->color;
    if (button->isClicked) {
        color = button->clickColor;
    }
    else if (button->isHovered) {
        color = button->hoverColor;
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &button->rect);
    SDL_RenderCopy(renderer, textTexture, nullptr, textRect);
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
bool isPointInRect(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
}
void Rest_Score() {
    std::cout << "Score reseted" << std::endl;
    X_Score = 0;
    O_Score = 0;
    number_of_draw = 0;
}
void quit_flag (){
    flag = true;
}





//games logic
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
bool checkWin(Player player, SDL_Renderer* renderer) {
    // Check rows and columns
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            drawThickLine(renderer, 40, (i + 1) * WINDOW_HEIGHT / 3 - WINDOW_HEIGHT / 6, WINDOW_WIDTH *3 / 4 -40, (i + 1) * WINDOW_HEIGHT / 3 - WINDOW_HEIGHT / 6, 5);
            SDL_RenderPresent(renderer);
            SDL_Delay(500);
            return true;
        }
        else if (board[0][i] == player && board[1][i] == player && board[2][i] == player) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            drawThickLine(renderer, (i + 1)  * WINDOW_WIDTH / 4 - WINDOW_WIDTH / 8, 40, (i + 1) * WINDOW_WIDTH / 4 - WINDOW_WIDTH / 8, WINDOW_HEIGHT-40, 5);
            SDL_RenderPresent(renderer);
            SDL_Delay(500);
            return true;
        }
    }
    // Check diagonals
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        drawThickLine(renderer, 40, 40, WINDOW_WIDTH * 3 / 4 - 40,  WINDOW_HEIGHT -40, 5);
        SDL_RenderPresent(renderer);
        SDL_Delay(500);
        return true;
    }
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        drawThickLine(renderer, WINDOW_WIDTH * 3 / 4 - 40, 40,  40, WINDOW_HEIGHT - 40, 5);
        SDL_RenderPresent(renderer);
        SDL_Delay(500);
        return true;
    }
    return false;
}



//main function

int main(int argc, char* argv[]) {
    // Initialize SDL, tiff and mixer
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create preface window
    
    SDL_Window* window_preface = SDL_CreateWindow("Tic-Tac-Toe", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window_preface == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1; 
    }

    // Create preface renderer
    
    SDL_Renderer* renderer_preface = SDL_CreateRenderer(window_preface, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer_preface == nullptr) {
        SDL_DestroyWindow(window_preface);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    


  

    //Audio code
      // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    Mix_Chunk* chunk = Mix_LoadWAV("C:\\Users\\obadz\\OneDrive\\Desktop\\SDL2\\SDL2\\mixkit-video-game-mystery-alert-234.wav");
    Mix_Chunk* chunk_win = Mix_LoadWAV("C:\\Users\\obadz\\OneDrive\\Desktop\\SDL2\\SDL2\\mixkit-video-game-win-2016.wav");



    //create font
    TTF_Font* font = TTF_OpenFont("C:\\Users\\obadz\\OneDrive\\Desktop\\SDL2\\SDL2\\AovelSansRounded-rdDL.ttf", 16); // Path to your .ttf file and font size
    if (!font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    TTF_Font* font_Score = TTF_OpenFont("C:\\Users\\obadz\\OneDrive\\Desktop\\SDL2\\SDL2\\AovelSansRounded-rdDL.ttf", 20); // Path to your .ttf file and font size
    if (!font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    TTF_Font* font_win = TTF_OpenFont("C:\\Users\\obadz\\OneDrive\\Desktop\\SDL2\\SDL2\\AovelSansRounded-rdDL.ttf", 75); // Path to your .ttf file and font size
    if (!font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    Button button_preface;
    button_preface.color = { 0, 0, 255, 0 };      // Blue
    button_preface.hoverColor = { 0, 100, 255, 255 }; // Light blue
    button_preface.clickColor = { 0, 0, 150, 255 }; // Dark blue
    button_preface.isHovered = false;
    button_preface.isClicked = false;
    button_preface.onClick = quit_flag;
    button_preface.rect = { WINDOW_WIDTH * 7 / 9, WINDOW_HEIGHT / 4, 100, 40 };





    SDL_Rect textRect_preface;
    SDL_Color textColor = { 0, 0, 0, 255 }; // Black text
    int textWpreface = 0, textHpreface = 0;
    SDL_Texture* textTexture_preface = renderText("play", textColor, font_win, renderer_preface);
    SDL_QueryTexture(textTexture_preface, nullptr, nullptr, &textWpreface, &textHpreface);

    SDL_SetRenderDrawColor(renderer_preface, 255, 255, 255, 255);
    SDL_RenderClear(renderer_preface);
    
   
    bool running = true;
    SDL_Event event1;
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event1)){
            handleButtonEvent(&event1, &button_preface);
            button_preface.rect = { WINDOW_WIDTH /2, WINDOW_HEIGHT /2, 170, 80 };
            textRect_preface = { WINDOW_WIDTH /2, WINDOW_HEIGHT / 2, textWpreface, textHpreface };
            renderButton(renderer_preface, &button_preface, textTexture_preface, &textRect_preface);
            SDL_RenderCopy(renderer_preface, textTexture_preface, nullptr, &textRect_preface);
            SDL_RenderPresent(renderer_preface);
            if (flag == true)
                break;

            
        }
        if (flag == true)
            break;
    }
    SDL_DestroyRenderer(renderer_preface);
    SDL_DestroyWindow(window_preface);









    SDL_Window* window_main = SDL_CreateWindow("Tic-Tac-Toe", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window_main == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer_main = SDL_CreateRenderer(window_main, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer_main == nullptr) {
        SDL_DestroyWindow(window_main);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    //icon code
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Surface* iconSurface = IMG_Load("C:\\Users\\obadz\\OneDrive\\Desktop\\SDL2\\SDL2\\tic-tac-toe.png");
    if (!iconSurface) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer_main);
        SDL_DestroyWindow(window_main);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Surface* imageSurface = IMG_Load("C:\\Users\\obadz\\OneDrive\\Desktop\\SDL2\\SDL2\\tic-tac-toe_1.png");
    if (!iconSurface) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer_main);
        SDL_DestroyWindow(window_main);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_SetWindowIcon(window_main, iconSurface);
    SDL_FreeSurface(iconSurface);

    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer_main, imageSurface);
    SDL_FreeSurface(imageSurface);  // We can free the surface after creating the texture
    if (!imageTexture) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer_main);
        SDL_DestroyWindow(window_main);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture* textTexture = renderText("Rest Game", textColor, font, renderer_main);
    SDL_Texture* textTexture1 = renderText("Rest Score", textColor, font, renderer_main);
    SDL_Texture* textTexture_X_Score = renderText("X Score", textColor, font_Score, renderer_main);
    SDL_Texture* textTexture_O_Score = renderText("O Score", textColor, font_Score, renderer_main);
    SDL_Texture* textTexture_draw_Score = renderText("Number of draws", textColor, font_Score, renderer_main);
    SDL_Texture* textTexture_X_Score_1 = renderText(std::to_string(X_Score), textColor, font_Score, renderer_main);
    SDL_Texture* textTexture_O_Score_1 = renderText(std::to_string(O_Score), textColor, font_Score, renderer_main);
    SDL_Texture* textTexture_draw_Score_1 = renderText(std::to_string(number_of_draw), textColor, font_Score, renderer_main);
    SDL_Texture* textTexture_X_win = renderText("Player X Won", textColor, font_win, renderer_main);
    SDL_Texture* textTexture_O_win = renderText("Player O Won", textColor, font_win, renderer_main);
    SDL_Texture* textTexture_No_win = renderText("No Winner", textColor, font_win, renderer_main);


    int textW = 0, textH = 0;
    int textW1 = 0, textH1 = 0;
    int textW2 = 0, textH2 = 0;
    int textW3 = 0, textH3 = 0;
    int textW4 = 0, textH4 = 0;

    SDL_QueryTexture(textTexture, nullptr, nullptr, &textW, &textH);
    SDL_QueryTexture(textTexture1, nullptr, nullptr, &textW, &textH);

    SDL_QueryTexture(textTexture_X_Score, nullptr, nullptr, &textW1, &textH1);
    SDL_QueryTexture(textTexture_O_Score, nullptr, nullptr, &textW1, &textH1);
    SDL_QueryTexture(textTexture_draw_Score, nullptr, nullptr, &textW3, &textH3);

    SDL_QueryTexture(textTexture_X_Score_1, nullptr, nullptr, &textW2, &textH2);
    SDL_QueryTexture(textTexture_O_Score_1, nullptr, nullptr, &textW2, &textH2);
    SDL_QueryTexture(textTexture_draw_Score_1, nullptr, nullptr, &textW2, &textH2);

    SDL_QueryTexture(textTexture_X_win, nullptr, nullptr, &textW4, &textH4);
    SDL_QueryTexture(textTexture_O_win, nullptr, nullptr, &textW4, &textH4);
    SDL_QueryTexture(textTexture_No_win, nullptr, nullptr, &textW4, &textH4);







    SDL_Rect textRect;
    SDL_Rect textRect1;
    SDL_Rect textRectX;
    SDL_Rect textRectO;
    SDL_Rect textRectdraw;
    SDL_Rect textRectX_1;
    SDL_Rect textRectO_1;
    SDL_Rect textRectdraw_1;
    SDL_Rect textRectX_win;
    SDL_Rect textRectO_win;
    SDL_Rect textRectNo_win;
    SDL_Rect dstRect1;

    //clear screen button


    Button button;
    button.color = { 0,192,192,192 };      // Blue
    button.hoverColor = { 0, 100, 255, 255 }; // Light blue
    button.clickColor = { 0, 0, 150, 255 }; // Dark blue
    button.isHovered = false;
    button.isClicked = false;
    button.onClick = resetGame;


    //rest score button
    Button button1;
    button1.color = { 0, 0, 255, 0 };      // Blue
    button1.hoverColor = { 0, 100, 255, 255 }; // Light blue
    button1.clickColor = { 0, 0, 150, 255 }; // Dark blue
    button1.isHovered = false;
    button1.isClicked = false;
    button1.onClick = Rest_Score;
    button1.rect = { WINDOW_WIDTH * 7 / 9, WINDOW_HEIGHT / 4, 100, 40 };

    
    // Main loop
    running = true;
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
                    Mix_PlayChannel(-1, chunk, 0);
                   
                    drawMarks(renderer_main);

                    if (checkWin(currentPlayer, renderer_main)) {
                        if (currentPlayer == PLAYER_X) {
                            Mix_PlayChannel(-1, chunk_win, 0);
                            textTexture_X_win = renderText("Player X Won", textColor, font_win, renderer_main);
                            SDL_RenderCopy(renderer_main, textTexture_X_win, nullptr, &textRectX_win);
                            SDL_RenderPresent(renderer_main);
                            SDL_Delay(1500);
                        }
                        else {
                            Mix_PlayChannel(-1, chunk_win, 0);
                            textTexture_O_win = renderText("Player O Won", textColor, font_win, renderer_main);
                            SDL_RenderCopy(renderer_main, textTexture_O_win, nullptr, &textRectO_win);
                            SDL_RenderPresent(renderer_main);
                            SDL_Delay(1500);
                        }
                        currentPlayer == PLAYER_X ? X_Score++ : O_Score++;
                        textTexture_X_Score_1 = renderText(std::to_string(X_Score), textColor, font_Score, renderer_main);
                        textTexture_O_Score_1 = renderText(std::to_string(O_Score), textColor, font_Score, renderer_main);
                        textTexture_draw_Score_1 = renderText(std::to_string(number_of_draw), textColor, font_Score, renderer_main);
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

        button.rect = { WINDOW_WIDTH * 7 / 9-10, WINDOW_HEIGHT / 4, 90, 40 };
        button1.rect = { WINDOW_WIDTH * 8 / 9, WINDOW_HEIGHT / 4, 90, 40 };
        textRect = { WINDOW_WIDTH * 7 / 9-2 , WINDOW_HEIGHT / 4 +10 , textW, textH }; 
        textRect1 = { WINDOW_WIDTH * 8 / 9 +8 ,WINDOW_HEIGHT / 4 +10, textW, textH };
        textRectX = { WINDOW_WIDTH * 7 / 9  , WINDOW_HEIGHT /10* 4 ,textW1, textH1 };
        textRectO = { WINDOW_WIDTH * 7 / 9 , WINDOW_HEIGHT /20*11,textW1, textH1 };
        textRectdraw = { WINDOW_WIDTH * 7 / 9  , WINDOW_HEIGHT / 10*7,textW3, textH3 };
        textRectX_1 = { WINDOW_WIDTH * 9 / 10 + 50, WINDOW_HEIGHT / 10 * 4 ,textW2, textH2 };
        textRectO_1 = { WINDOW_WIDTH * 9 / 10 + 50 , WINDOW_HEIGHT / 20 * 11, textW2, textH2 };
        textRectdraw_1 = { WINDOW_WIDTH * 9 / 10 +50, WINDOW_HEIGHT / 10 * 7,textW2, textH2 };
        textRectX_win = { WINDOW_WIDTH /2*3/4 -200 , WINDOW_HEIGHT /2 - 75,textW4, textH4 };
        textRectO_win = { WINDOW_WIDTH / 2 * 3 / 4 - 200 , WINDOW_HEIGHT / 2 - 75,textW4, textH4 };
        textRectNo_win = { WINDOW_WIDTH / 2 * 3 / 4 - 200 , WINDOW_HEIGHT / 2 - 75,textW4, textH4 };
        dstRect1 = { WINDOW_WIDTH /10*8+20, WINDOW_HEIGHT / 20, WINDOW_WIDTH/9, WINDOW_HEIGHT/7 };


        // Clear the screen
        SDL_SetRenderDrawColor(renderer_main, 255, 255, 255, 255);
        SDL_RenderClear(renderer_main);


        // Render the button
        renderButton(renderer_main, &button, textTexture, &textRect);
        renderButton(renderer_main, &button1, textTexture1, &textRect1);

        SDL_RenderCopy(renderer_main, textTexture_X_Score, nullptr, &textRectX);
        SDL_RenderCopy(renderer_main, textTexture_O_Score, nullptr, &textRectO);
        SDL_RenderCopy(renderer_main, textTexture_draw_Score, nullptr, &textRectdraw);
        SDL_RenderCopy(renderer_main, textTexture_X_Score_1, nullptr, &textRectX_1);
        SDL_RenderCopy(renderer_main, textTexture_O_Score_1, nullptr, &textRectO_1);
        SDL_RenderCopy(renderer_main, textTexture_draw_Score_1, nullptr, &textRectdraw_1);
        SDL_RenderCopy(renderer_main, imageTexture, nullptr, &dstRect1);


        // Draw the game board and marks
        drawBoard(renderer_main);
        drawMarks(renderer_main);

        // Present the renderer
        SDL_RenderPresent(renderer_main);



        if (Counter > 8) {
           Counter = 0;
           number_of_draw++;
           SDL_RenderCopy(renderer_main, textTexture_No_win, nullptr, &textRectNo_win);
           SDL_RenderPresent(renderer_main);
           SDL_Delay(1500);          
           textTexture_X_Score_1 = renderText(std::to_string(X_Score), textColor, font_Score, renderer_main);
           textTexture_O_Score_1 = renderText(std::to_string(O_Score), textColor, font_Score, renderer_main);
           textTexture_draw_Score_1 = renderText(std::to_string(number_of_draw), textColor, font_Score, renderer_main);
           resetGame();
        }

        int x, y;
        SDL_GetMouseState(&x, &y);
        if (isPointInRect(x, y, button1.rect)) {
            textTexture_X_Score_1 = renderText(std::to_string(X_Score), textColor, font_Score, renderer_main);
            textTexture_O_Score_1 = renderText(std::to_string(O_Score), textColor, font_Score, renderer_main);
            textTexture_draw_Score_1 = renderText(std::to_string(number_of_draw), textColor, font_Score, renderer_main);
        }


    }

    // Clean up
    SDL_DestroyTexture(imageTexture);
    Mix_FreeChunk(chunk);
    Mix_FreeChunk(chunk_win);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer_main);
    SDL_DestroyWindow(window_main);
    SDL_Quit();
    TTF_Quit();
    return 0;
}
