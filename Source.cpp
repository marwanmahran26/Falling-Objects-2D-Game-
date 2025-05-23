#include <windows.h>
#include <conio.h>
#include "CC212SGL.h"
#include <stdlib.h> 

#pragma comment(lib, "CC212SGL.lib")

CC212SGL gr;

struct GameObject {
    int x, y;
    int width, height;
    int active;
};

#define MaxApples 100
#define basket_WIDTH 300
#define basket_HEIGHT 150
#define basket_SPEED 73
#define APPLE_WIDTH 150
#define APPLE_HEIGHT 160
#define APPLE_SPEED 70
#define Heart_WIDTH 70
#define Heart_HEIGHT 70

void displayMainMenu();
void resetGame();
void controlbucket();
void drawGameObjects();
void dropAPPLES();
void moveAPPLEs();
void checkAPPLECollisions();
void handleCollision(int appleIndex);
bool checkCollision(GameObject apple, GameObject basket);
void displayGameOver();

int startImage;
int basketImage;
int APPLEImage;
int backgroundImage;
int HeartImage;
int score = 0;
int lives = 3;
int gameOver = 0;
int APPLEDropTime = 0;

GameObject basket;
GameObject APPLEs[MaxApples];

int main() {
    gr.setup();
    gr.setFullScreenMode();
    gr.fillScreen(BLACK);

    basket.x = (gr.getWindowWidth() / 2) - (basket_WIDTH / 2);
    basket.y = gr.getWindowHeight() - basket_HEIGHT - 15;
    basket.width = basket_WIDTH;
    basket.height = basket_HEIGHT;

    basketImage = gr.loadImage("bucket1.png");
    APPLEImage = gr.loadImage("apple.png");
    backgroundImage = gr.loadImage("background2.jpg");
    HeartImage = gr.loadImage("heart.png");

    gr.resizeImage(basketImage, basket_WIDTH, basket_HEIGHT);
    gr.resizeImage(APPLEImage, APPLE_WIDTH, APPLE_HEIGHT);
    gr.resizeImage(backgroundImage, gr.getWindowWidth(), gr.getWindowHeight());
    gr.resizeImage(HeartImage, Heart_WIDTH, Heart_HEIGHT);

    displayMainMenu();

    while (1) {
        resetGame();

        while (!gameOver) {
            controlbucket();
            dropAPPLES();
            moveAPPLEs();
            checkAPPLECollisions();

            gr.beginDraw();
            gr.fillScreen(BLACK);
            gr.drawImage(backgroundImage, 0, 0, 0);
            drawGameObjects();
            gr.endDraw();
            Sleep(50);
        }

        displayGameOver();

        while (1) {
            if (GetAsyncKeyState('R') & 0x8000) {
                break; 
            }
            Sleep(100); 
        }
    }

    return 0;
}

void displayMainMenu() {
    while (1) {
        gr.beginDraw();
        gr.fillScreen(BLACK);
        startImage = gr.loadImage("start.png");
        gr.resizeImage(startImage, gr.getWindowWidth(), gr.getWindowHeight());
        gr.drawImage(startImage, 0, 0, 0);

        gr.setFontSizeAndBoldness(50, 150); 
        gr.setDrawingColor(BROWN);
        gr.drawText(gr.getWindowWidth() / 2 - 150, gr.getWindowHeight() / 2 + 300, "Press ENTER to Start");
        gr.setDrawingColor(RED);
        gr.drawText(gr.getWindowWidth() / 2 - 150, gr.getWindowHeight() / 2 + 400, "Press ESC to Exit");

        gr.endDraw();

        if (GetAsyncKeyState(VK_RETURN) & 0x8000) { 
            return;
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            exit(0); 
        }

        Sleep(100);  
    }
}

void resetGame() {
    score = 0;
    lives = 3;
    gameOver = 0;
    APPLEDropTime = 0;

    for (int i = 0; i < MaxApples; i++) {
        APPLEs[i].active = 0; 
    }
}

void controlbucket() {
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        if (basket.x > 0)
            basket.x -= basket_SPEED;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        if (basket.x < gr.getWindowWidth() - basket.width)
            basket.x += basket_SPEED;
    }
}

void drawGameObjects() {
    if (gameOver) {
        return;
    }

    gr.drawImage(basketImage, basket.x, basket.y, NULL);

    for (int i = 0; i < MaxApples; i++) {
        if (APPLEs[i].active) {
            gr.drawImage(APPLEImage, APPLEs[i].x, APPLEs[i].y, NULL);
        }
    }

    gr.setDrawingColor(WHITE);
    gr.setFontSizeAndBoldness(40, 40);
    char statusText[100];
    snprintf(statusText, sizeof(statusText), "Score: %d", score);
    gr.drawText(20, 20, statusText);

    int heartX = (gr.getWindowWidth() / 2) - ((Heart_WIDTH + 10) * lives) / 2;
    int heartY = 20;
    for (int i = 0; i < lives; i++) {
        gr.drawImage(HeartImage, heartX + (Heart_WIDTH + 10) * i, heartY, NULL);
    }
}

void dropAPPLES() {
    APPLEDropTime++;
    if (APPLEDropTime > 10) {
        for (int i = 0; i < MaxApples; i++) {
            if (!APPLEs[i].active) {
                APPLEs[i].x = rand() % (gr.getWindowWidth() - APPLE_WIDTH);
                APPLEs[i].y = 0;
                APPLEs[i].width = APPLE_WIDTH;
                APPLEs[i].height = APPLE_HEIGHT;
                APPLEs[i].active = 1;
                break;
            }
        }
        APPLEDropTime = 0;
    }
}

void moveAPPLEs() {
    for (int i = 0; i < MaxApples; i++) {
        if (APPLEs[i].active) {
            APPLEs[i].y += APPLE_SPEED;
            if (APPLEs[i].y > gr.getWindowHeight()) {
                APPLEs[i].active = 0;
                lives--;
                if (lives == 0) {
                    gameOver = 1;
                }
            }
        }
    }
}

void checkAPPLECollisions() {
    for (int i = 0; i < MaxApples; i++) {
        if (APPLEs[i].active && checkCollision(APPLEs[i], basket)) {
            handleCollision(i);
        }
    }
}
//starting positon fo2 w kol ama nzawed bnenzel taht
// Check if an apple is colliding with the basket
bool checkCollision(GameObject apple, GameObject basket) {
    return apple.x < basket.x + basket.width &&
        apple.x + apple.width > basket.x &&
        apple.y < basket.y + basket.height &&
        apple.y + apple.height > basket.y;
}

void handleCollision(int appleIndex) {
    APPLEs[appleIndex].active = 0;
    score++;
}

void displayGameOver() {
    gr.beginDraw();
    gr.fillScreen(BLACK);

    gr.setFontSizeAndBoldness(100, 100);
    gr.setDrawingColor(RED);
    gr.drawText(gr.getWindowWidth() / 2 - 200, gr.getWindowHeight() / 2 - 50, "GAME OVER");

    char finalScore[50];
    snprintf(finalScore, sizeof(finalScore), "Final Score: %d", score);//printf doesn’t return a string—it prints the text directly, which is useless for gr.drawText().
    gr.setDrawingColor(WHITE);
    gr.drawText(gr.getWindowWidth() / 2 - 150, gr.getWindowHeight() / 2 + 50, finalScore);

    gr.setFontSizeAndBoldness(40, 40);
    gr.setDrawingColor(GREEN);
    gr.drawText(gr.getWindowWidth() / 2 - 250, gr.getWindowHeight() - 100, "Press 'R' to Play Again");

    gr.endDraw();
    Sleep(1000);
}