#include "iGraphics.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "iSound.h"
#define MAXPOWERUP 10
#define screenwidth 600
#define screenheight 600
#define R 10
#define C 12
#define PI 3.1416
int bricks[R][C];
int brick_width = 50;
int brick_height = 30;
int topgap = 80;
int leftgap = (600 - (C * brick_width)) / 2;
int bar_x = 200, bar_y = 0, bar_width = 100, bar_height = 10, bar_speed = 5;
int ball_x = 235, ball_y = bar_y + bar_height + 15, ball_radius = 10;
int speed = 10;
int theta = 60;
int dx = (int)speed * cos(theta * PI / 180);
int dy = (int)speed * sin(theta * PI / 180);
int score = 0;
int lives = 3;
int highscore = 0;
int paused = 0;
int currentview = 0;
int currentlevel = 1;
char playerinfo[500];
int previousview = 0;
int prevmusic = -1;
int mousemoved = 0;
int volume=80;
int easy[R][C] = {
    {2, 0, 1, 0, 1, 0, 2, 0, 1, 0, 1, 0},
    {0, 2, 0, 2, 0, 1, 0, 1, 0, 2, 0, 1},
    {1, 0, 1, 0, 2, 0, 1, 0, 1, 0, 1, 0},
    {0, 1, 0, 2, 0, 1, 0, 2, 0, 1, 0, 2},
    {1, 0, 2, 0, 1, 0, 1, 0, 1, 0, 2, 0},
    {0, 1, 0, 1, 0, 1, 0, 2, 0, 2, 0, 1},
    {1, 0, 2, 0, 1, 0, 1, 0, 1, 0, 2, 0},
    {0, 2, 0, 2, 0, 1, 0, 1, 0, 2, 0, 1},
    {1, 0, 1, 0, 2, 0, 1, 0, 1, 0, 1, 0},
    {0, 2, 0, 1, 0, 1, 0, 1, 0, 2, 0, 1}
};

int medium[R][C] = {
    {0, 2, 1, 0, 0, 2, 1, 0, 0, 2, 2, 0},
    {2, 0, 0, 2, 1, 0, 0, 2, 1, 0, 0, 2},
    {2, 0, 0, 2, 2, 0, 0, 2, 2, 0, 0, 2},
    {0, 1, 0, 2, 1, 2, 0, 0, 2, 2, 0, 1},
    {0, 2, 2, 0, 0, 2, 2, 0, 0, 1, 2, 0},
    {2, 0, 0, 1, 2, 0, 0, 2, 2, 0, 0, 2},
    {2, 0, 0, 2, 2, 0, 2, 2, 2, 0, 0, 2},
    {0, 2, 2, 0, 2, 2, 1, 0, 0, 2, 2, 0},
    {0, 2, 2, 0, 0, 2, 2, 0, 0, 2, 1, 0},
    {2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 2}
};

int hard[R][C] = {
    {0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0},
    {0, 0, 2, 0, 1, 2, 1, 1, 0, 2, 0, 0},
    {0, 1, 0, 2, 0, 1, 1, 0, 2, 0, 1, 0},
    {1, 0, 1, 0, 2, 0, 0, 1, 0, 2, 0, 1},
    {0, 2, 0, 1, 0, 1, 2, 0, 1, 0, 2, 0},
    {1, 0, 1, 0, 2, 0, 0, 2, 0, 1, 0, 1},
    {0, 2, 0, 2, 0, 1, 1, 0, 2, 0, 1, 0},
    {0, 0, 2, 0, 1, 1, 2, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
typedef struct
{
    char name[300];
    int score;
} playerrecord;

int namesize = 0;
int index = 0;

typedef struct powerups
{
    int functionality;
    int x, y;
    int enable;
} powerups;
powerups powerup[MAXPOWERUP];
void addscoreinboard()
{
    FILE *fp = fopen("saves/leaderboard.txt", "a");
    if (fp != NULL)
    {
        fprintf(fp, "%s %d\n", playerinfo, score);
        fclose(fp);
    }
}
bool scorekept = false;

void savehighscore()
{
    FILE *fp = fopen("saves/highscore.txt", "w");
    if (fp != NULL)
    {
        fprintf(fp, "%d\n", highscore);
        fclose(fp);
    }
}

void readhighscore()
{
    FILE *fp = fopen("saves/highscore.txt", "r");
    if (fp != NULL)
    {
        fscanf(fp, "%d", &highscore);
        fclose(fp);
    }
}

void enterlevel(int level)
{
    currentlevel = level;
    mousemoved = 0;
    if (level == 2)
    {
        for (int i = 0; i < R; i++)
        {
            for (int j = 0; j < C; j++)
            {
                bricks[i][j] = medium[i][j];
            }
        }
    }

    else if (level == 1)
    {

        for (int i = 0; i < R; i++)
        {
            for (int j = 0; j < C; j++)
            {
                bricks[i][j] = easy[i][j];
            }
        }
    }

    else if (level == 3)
    {
        for (int i = 0; i < R; i++)
        {
            for (int j = 0; j < C; j++)
            {
                bricks[i][j] = hard[i][j];
            }
        }
    }
    else
    {
        for (int i = 0; i < R; i++)
        {
            for (int j = 0; j < C; j++)
            {
                bricks[i][j] = easy[i][j];
            }
        }
    }
    ball_x = 235;
    ball_y = bar_y + bar_height + 15;
    bar_x = 200;
    score = 0;
    lives = 3;
}

playerrecord ranking[100];
int playernums = 0;
void showleaderboard()
{
    FILE *fp = fopen("saves/leaderboard.txt", "r");
    if (fp == NULL)
    {
        playernums = 0;
        return;
    }
    playernums = 0;
    while (fscanf(fp, "%s %d", ranking[playernums].name, &ranking[playernums].score) == 2)
    {
        playernums++;
        if (playernums >= 1000)
            break;
    }
    fclose(fp);

    // bubble sorting
    for (int i = 0; i < playernums - 1; i++)
    {
        for (int j = i + 1; j < playernums; j++)
        {
            if (ranking[j].score > ranking[i].score)
            {
                playerrecord temp = ranking[i];
                ranking[i] = ranking[j];
                ranking[j] = temp;
            }
        }
    }
}
// resume button er function
typedef struct previousstate
{
    int score;
    int currentlevel;
    int bar_x;
    int ball_x, ball_y;
    int dx, dy;
    int bricks[R][C];
    int lives;
} prevstate;

void savegameafterexit()
{
    prevstate state;
    state.score = score;
    state.currentlevel = currentlevel;
    state.bar_x = bar_x;
    state.ball_x = ball_x;
    state.ball_y = ball_y;
    state.dx = dx;
    state.dy = dy;
    state.lives = lives;

    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < C; j++)
        {
            state.bricks[i][j] = bricks[i][j];
        }
    }

    FILE *fp = fopen("saves/savegame.txt", "w");
    if (fp)
    {

        fprintf(fp, "Score: %d\n", state.score);
        fprintf(fp, "Current Level: %d\n", state.currentlevel);
        fprintf(fp, "Bar X: %d\n", state.bar_x);
        fprintf(fp, "Ball X: %d\n", state.ball_x);
        fprintf(fp, "Ball Y: %d\n", state.ball_y);
        fprintf(fp, "DX: %d\n", state.dx);
        fprintf(fp, "DY: %d\n", state.dy);
        fprintf(fp, "Lives: %d\n", state.lives);

        for (int i = 0; i < R; i++)
        {
            for (int j = 0; j < C; j++)
            {
                fprintf(fp, "%d ", state.bricks[i][j]);
            }
            fprintf(fp, "\n");
        }

        fclose(fp);
    }
}

bool load_game_state()
{
    FILE *fp = fopen("saves/savegame.txt", "r");
    if (fp == NULL)
    {
        return false;
    }

    prevstate state;

    int result = fscanf(fp, "Score: %d\n", &state.score);
    if (result != 1)
        return false;

    result = fscanf(fp, "Current Level: %d\n", &state.currentlevel);
    if (result != 1)
        return false;
    result = fscanf(fp, "Bar X: %d\n", &state.bar_x);
    if (result != 1)
        return false;
    result = fscanf(fp, "Ball X: %d\n", &state.ball_x);
    if (result != 1)
        return false;
    result = fscanf(fp, "Ball Y: %d\n", &state.ball_y);
    if (result != 1)
        return false;
    result = fscanf(fp, "DX: %d\n", &state.dx);
    if (result != 1)
        return false;
    result = fscanf(fp, "DY: %d\n", &state.dy);
    if (result != 1)
        return false;
    result = fscanf(fp, "Lives: %d\n", &state.lives);
    if (result != 1)
        return false;

    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < C; j++)
        {
            result = fscanf(fp, "%d", &state.bricks[i][j]);
            if (result != 1)
            {
                fclose(fp);
                return false;
            }
        }
    }

    fclose(fp);

    score = state.score;
    currentlevel = state.currentlevel;
    bar_x = state.bar_x;
    ball_x = state.ball_x;
    ball_y = state.ball_y;
    dx = state.dx;
    dy = state.dy;
    lives = state.lives;

    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < C; j++)
        {
            bricks[i][j] = state.bricks[i][j];
        }
    }

    return true;
}

bool save_exists()
{
    FILE *fp = fopen("saves/savegame.bin", "rb");
    if (fp)
    {
        fclose(fp);
        return true;
    }
    return false;
}

void afterexiting()
{
    if (currentview == 1) // inside game
    {
        savegameafterexit();

       
        if (scorekept!=1 && strlen(playerinfo) > 0 && score > 0) {
            addscoreinboard();
            scorekept = true;
        }
    }
}
void welcomepage()
{

    iShowImage(0, 0, "assets/sprites/welcomepage.jpg");

    // iPlaySound("assets/sounds/gameOpening.wav",true,80);
    // // iStopSound(channel);
}

void exitpage()
{
    iShowImage(0, 0, "assets/sprites/exitgame.jpeg");
}
void levelpage()
{

    iShowImage(0, 0, "assets/sprites/level1.jpg");
}
void instructions()
{
    // iStopAllSounds();
    iShowImage(0, 0, "assets/sprites/instructions.jpg");
}
void settings()
{
    // iStopAllSounds();
    iShowImage(0, 0, "assets/sprites/settings.jpg");
}
void gameover()
{
    readhighscore();
    if (score > highscore)
    {
        highscore = score;
        savehighscore();
        currentview = 9;
    }
    else
    {
        iShowImage(0, 0, "assets/sprites/gameover1.jpg");

        currentview = 6;
    }
}

void leaderboard()
{
    // iStopAllSounds();
    iShowImage(0, 0, "assets/sprites/leaderboard.jpg");
    for (int i = 0; i < playernums && i < 15; i++)
    {
        char line[50];
        sprintf(line, "%d. %s : %d", i + 1, ranking[i].name, ranking[i].score);
        iSetColor(0, 0, 0);
        iText(200, 420 - i * 30, line, GLUT_BITMAP_HELVETICA_18);
    }
}
void entername()
{
    // iStopAllSounds();
    iShowImage(0, 0, "assets/sprites/name.jpg");
}
void creditspage()
{
    // iStopAllSounds();
    iShowImage(0, 0, "assets/sprites/creditspage.jpg");
}
void wongame()
{
    readhighscore();
    iShowImage(0, 0, "assets/sprites/won.png");
    // char highscoreText[100];
    char scoreshow[100];
    char highscoreshow[100];
    sprintf(scoreshow, "%d", score);
    sprintf(highscoreshow, "%d", highscore);
    iSetColor(255, 255, 255);
    iText(408, 200, scoreshow, GLUT_BITMAP_TIMES_ROMAN_24);
    iText(408, 97, highscoreshow, GLUT_BITMAP_TIMES_ROMAN_24);
    iText(150, 50, "Press Esc to go to Menu", GLUT_BITMAP_HELVETICA_18);
}

void maingame()
{
    
    iShowImage(0, 0, "assets/sprites/background1.jpg");
    iSetColor(255, 255, 0);
    iFilledCircle(ball_x, ball_y, ball_radius);
    iShowImage(bar_x, bar_y, "assets/sprites/bar.png");

    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < C; j++)
        {
            if (bricks[i][j] > 0)
            {
                int x = leftgap + j * brick_width;
                int y = screenheight - topgap - (i + 1) * brick_height;

                if (bricks[i][j] == 2)
                {
                    iSetColor(255, 0, 0);
                }
                else if (bricks[i][j] == 1)
                {
                    iSetColor(255, 255, 0);
                }
                else
                {
                    iSetColor(0, 0, 255);
                }

                iRectangle(x, y, brick_width, brick_height);
                iFilledRectangle(x, y, brick_width, brick_height);
                iSetColor(0, 0, 0);
                iRectangle(x, y, brick_width, brick_height); // boundary
            }
        }
    }

    for (int i = 0; i < MAXPOWERUP; i++)
    {  if(powerup[i].enable==1){
        if (powerup[i].functionality == 1)
        {
            iShowImage(powerup[i].x, powerup[i].y, "assets//sprites//powscore.png");
        }
        else if (powerup[i].functionality == 0)
        {
            iShowImage(powerup[i].x, powerup[i].y, "assets//sprites//powliv.png");
        }
    }
    }
}

void iDraw()
{
    iClear();

    // musics
    if (currentview != prevmusic)
    {
        iStopAllSounds();
        if (currentview == 0 )
        {
            iPlaySound("assets/sounds/gameopen.wav", true, volume);
        }
        else if (currentview == 6 || currentview == 9)
        {
            iPlaySound("assets/sounds/gameover.wav", true, volume);
        }
        else if(currentview==2 || currentview==3 || currentview==4|| currentview==7|| currentview==8|| currentview==10 ){
            iPlaySound("assets/sounds/otherpages.wav",true,volume);
        }

        prevmusic = currentview;
    }

    if (currentview == 1)
    {
        maingame();

        char scoreText[20];
        char livesText[20];
        char levelText[20];

        sprintf(scoreText, "Score: %d", score);

        sprintf(livesText, "Lives: %d ", lives);

        sprintf(levelText, "Level %d", currentlevel);

        iSetColor(255, 0, 0);

        iText(20, 570, scoreText, GLUT_BITMAP_HELVETICA_18);

        iText(270, 570, levelText, GLUT_BITMAP_HELVETICA_18);

        iText(480, 570, livesText, GLUT_BITMAP_HELVETICA_18);

        if (paused == 1)
        {
            iSetColor(0, 0, 0);
            iText(250, 300, "PAUSED!", GLUT_BITMAP_TIMES_ROMAN_24);
        }
    }

    else if (currentview == 0)
    {

        welcomepage();

        //  iPlaySound("assets/sounds/gameOpening.wav",true,80);
    }

    else if (currentview == 2)

        levelpage();

    else if (currentview == 5)

        exitpage();

    else if (currentview == 3)

        instructions();

    else if (currentview == 6)
    {
        gameover();

        char scoreshow[30];
        char highscoreshow[30];
        sprintf(scoreshow, "Your Score : %d", score);

        sprintf(highscoreshow, "High Score : %d", highscore);

        iSetColor(255, 255, 255);

        iText(200, 250, scoreshow, GLUT_BITMAP_TIMES_ROMAN_24);
        iText(200, 220, highscoreshow, GLUT_BITMAP_TIMES_ROMAN_24);
        iText(150, 170, " Press Esc to return to Main Menu", GLUT_BITMAP_HELVETICA_18);
    }
    else if (currentview == 4)
    {

        leaderboard();
    }
    else if (currentview == 7)
    {
        entername();

        iSetColor(0, 0, 0);
        iText(130, 325, playerinfo, GLUT_BITMAP_HELVETICA_18);

        iSetColor(255, 0, 0);
    }
    else if (currentview == 8)
    {
        creditspage();
        iSetColor(255, 255, 255);
        iText(180, 550, "Press  Enter to start game", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    else if (currentview == 9)
    {
        wongame();
    }
    else if (currentview == 10)
    {
        settings();
    }
}

void iMouseMove(int mx, int my)
{
    if (paused != 1)
    {
        bar_x = mx - bar_width / 2;

        if (bar_x < 0)

            bar_x = 0;
        if (bar_x + bar_width > screenwidth)
        {
            bar_x = screenwidth - bar_width + 20;
        }

        mousemoved = 1;
    }
}

void iMouseDrag(int mx, int my) {}
void iMouseWheel(int dir, int mx, int my) {}
/*
for your convenience:
currentview=0 ---menupage
currentview=1 ---maingame
currentview=2 ---levelpage
currentview=3 ---instructions
currentview=4 ---leaderboard
currentview=5 ---exitpage
currentview=6 ---gameover
currentview=7 ---entername
currentview=8 --- creditspage
currentview=10---settingspage
*/
void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        previousview = currentview;
        if (currentview == 0)
        {
            // iStopAllSounds();
            //   iPlaySound("assets/sounds/gameOpening.wav",true,80);
            if (mx >= 200 && mx <= 364 && my >= 405 && my <= 440)
            {

                currentview = 7;

                playerinfo[0] = '\0';
                index = 0;
            }
            else if (mx >= 150 && mx <= 364 && my >= 65 && my <= 130)
            {
                currentview = 5;
                exit(0);
            }
            else if (mx >= 150 && mx <= 364 && my >= 360 && my <= 395)
            {
                currentview = 7;
                playerinfo[0] = '\0';
                index = 0;
            }
            else if (mx >= 150 && mx <= 364 && my >= 200 && my <= 235)
            {
                if (save_exists())
                {
                    if (load_game_state())
                    {
                        currentview = 1; 
                        paused = 0;
                        scorekept = false;
                    }
                }

                return;
            }
            else if (mx >= 150 && mx <= 364 && my >= 150 && my <= 180)
            {
                currentview = 8; // credits page
            }
            else if (mx >= 150 && mx <= 364 && my >= 255 && my <= 290)
            {
                currentview = 3; // instructions page
            }
            else if (mx >= 150 && mx <= 364 && my >= 305 && my <= 340)
            {
                currentview = 4;
                showleaderboard();
            }
            else if (mx >= 525 && mx <= 569 && my >= 520 && my <= 560)
            {
                currentview = 10; // settings page
            }
        }
        else if (currentview == 2)
        {
            if (mx >= 200 && mx <= 364 && my >= 300 && my <= 370)
            {
                currentview = 1;
                enterlevel(3);
            }
            else if (mx >= 200 && mx <= 364 && my >= 440 && my <= 520)
            {
                currentview = 1;
                enterlevel(1);
            }
            else if (mx >= 160 && mx <= 390 && my >= 380 && my <= 430)
            {
                currentview = 1;
                enterlevel(2);
            }
        }
    }
}
void iKeyboard(unsigned char key)
{

    if (currentview == 1 && paused == 0) {

       if (key == 'a' || key == 'A') {
         bar_x = bar_x-bar_speed *10;
        if (bar_x < 0) {
            bar_x = 0;
        }
        mousemoved = 1;
    }
    if (key == 'd' || key == 'D') {
        bar_x = bar_x+bar_speed *10;
        if (bar_x + bar_width > screenwidth) 
        {
            bar_x = screenwidth - bar_width;
        }
        mousemoved = 1;
    }
}


        
   if (key == 27)
{ // press Esc to go to home page anytime
    if (currentview == 1)
    {
       
        if (scorekept!=1 && strlen(playerinfo) > 0 && score > 0) {
            addscoreinboard();
            scorekept = true;
        }
        currentview = 6;
    }
    else
    {
        currentview = 0;
        score = 0;
        lives = 3;
        scorekept = false;
    }
}




    if (currentview == 7)
    {
        if (index < 29 && key >= 32 && key <= 126)
        {
            playerinfo[index++] = key;

            playerinfo[index] = '\0';
        }
    }
    // spacebar pause
    if (currentview == 1)
    {

        if (key == ' ')
        {
            paused = 1 - paused;
        }
    }
    if (currentview == 7 || currentview == 8)
    {
        if (key == '\r' || key == '\n')
        {
            currentview = 2;

            scorekept = false;
            // Start game
        }
        else if (key == 8)
        {
            if (index > 0)
            {
                index--;
                playerinfo[index] = '\0';
            }
        }
    }
    if (key == 27)
    {
        if (currentview == 9)
        {
            currentview = 0;
            score = 0;
            lives = 3;
            scorekept = false;
        }
    }

    if (key == 'X' || key == 'x')
    {
        currentview = 5; // exit the game
    }
}
void iSpecialKeyboard(unsigned char key)
{

    if (key == GLUT_KEY_END)
    {
        if (index > 0)
        {
            index--;
            playerinfo[index] = '\0';
        }
    }

    if (key == GLUT_KEY_LEFT)
    {
        currentview = previousview;
    }
    if (currentview == 10)
    {
        if (key == GLUT_KEY_UP)
        {
            volume=volume+10;
            iStopAllSounds();
             iPlaySound("assets/sounds/otherpages.wav", true, volume);
        }
        else if (key == GLUT_KEY_DOWN)
        {
            volume=volume-10;
             iStopAllSounds();
             iPlaySound("assets/sounds/otherpages.wav", true, volume);
        }
        
       ;
        
    }
}
void ballmovement()
{
    if (currentview != 1)
    {
        return;
    }

    if (paused == 1)
    {
        return;
    }
    if (mousemoved == 0)
    {
        return;
    }

    ball_x += dx;
    ball_y += dy;

    if (ball_x - ball_radius <= 0 || ball_x + ball_radius >= screenwidth)
    {
        dx = -dx;
        if (ball_x - ball_radius <= 0)
        {
            ball_x = ball_radius;
        }
        if (ball_x + ball_radius >= screenwidth)
        {
            ball_x = screenwidth - ball_radius;
        }
    }

    if (ball_y + ball_radius >= screenheight)
    {
        dy = -dy;
        ball_y = screenheight - ball_radius;
    }

    int remainingbricks = 0;
    for (int i = 0; i < R; i++)
    {
        for (int j = 0; j < C; j++)
        {
            if (bricks[i][j] > 0)
            {
                remainingbricks = 1;

                int brickx = leftgap + j * brick_width;
                int bricky = screenheight - topgap - (i + 1) * brick_height;

                if (ball_x + ball_radius >= brickx && ball_x - ball_radius <= brickx + brick_width && ball_y + ball_radius >= bricky && ball_y - ball_radius <= bricky + brick_height)
                {
                    if (bricks[i][j] == 2)
                    {
                        bricks[i][j] = 1;
                        score += 5;
                        for (int q = 0; q < MAXPOWERUP; q++)
                        {
                            if (powerup[q].enable == 0)
                            {
                                powerup[q].enable = 1;
                                powerup[q].x = brickx + brick_width / 2;
                                powerup[q].y = bricky;
                                powerup[q].functionality = rand() % 2;
                                break;
                            }
                        }
                    }
                    else if (bricks[i][j] == 1)
                    {
                        bricks[i][j] = 0;
                        score += 5;
                    }
                    // if (rand() % 5 == 0)
                    // {

                    //     for (int q = 0; q < MAXPOWERUP; q++)
                    //     {
                    //         if (powerup[q].enable == 0)
                    //         {
                    //             powerup[q].enable = 1;
                    //             powerup[q].x = brickx + brick_width / 2;
                    //             powerup[q].y = bricky;
                    //             powerup[q].functionality = rand() % 2;
                    //             break;
                    //         }
                    //     }
                    // }
                    if (ball_x + ball_radius >= brickx && ball_x - ball_radius <= brickx + brick_width)
                    {
                        dy = -dy;
                    }
                    else if (ball_y + ball_radius >= bricky && ball_y - ball_radius <= bricky + brick_height)
                    {
                        dx = -dx;
                    }

                    iPlaySound("assets/sounds/Ballbrick.wav", false, volume);

                    goto clip;
                }
            }
        }
    }
clip:

    if (ball_y <= bar_y + bar_height + 20 && ball_x + ball_radius >= bar_x && ball_x <= bar_x + bar_width)
    {

        theta = (bar_x + bar_width - ball_x) + 50;
        dx = (int)speed * cos(theta * PI / 180);
        dy = (int)speed * sin(theta * PI / 180);

        if (dy < 0)
        {
            dy = -dy;
        }
        iPlaySound("assets/sounds/BallandPad.wav", false, volume);
    }

    if (ball_y < 0)
    {
        lives--;
    }

    if (lives <= 0)
    {
        if (score > highscore)
        {
            highscore = score;
            savehighscore();
            currentview = 9;
        }
        else
            currentview = 6;

        if (!scorekept)
        {
            addscoreinboard();
            scorekept = true;
        }
        return;
    }
if (remainingbricks == 0)
{
    if (score > highscore)
    {
        highscore = score;
        savehighscore();
    }
    if (!scorekept)
    {
        addscoreinboard();
        scorekept = true;
    }
    currentview = 9;
    return;
}

    if (ball_y < 0)
    {
        ball_x = bar_x + bar_width / 2;
        ball_y = bar_y + bar_height + 20;
    }

    for (int i = 0; i < MAXPOWERUP; i++)
    {
        if (powerup[i].enable == 1)
        {
            powerup[i].y = powerup[i].y - 5;
            if (powerup[i].y <= bar_y + bar_height && powerup[i].x >= bar_x && powerup[i].x <= bar_x + bar_width)
            {
                if (powerup[i].functionality == 1)
                {
                    score += 10;
                }
                else if (powerup[i].functionality == 0)
                {
                    lives += 1;
                }
                powerup[i].enable = 0;
            }
            else if (powerup[i].y <= 0)
            {
                powerup[i].enable = 0;
            }
        }
    }
}


int main(int argc, char *argv[])
{

    srand(time(NULL));
    for (int i = 0; i < MAXPOWERUP; i++)
    {

        powerup[i].enable = 0;
    }
    atexit(afterexiting); // ber hoar por game save
    glutInit(&argc, argv);
    readhighscore();
    enterlevel(1);
    iSetTimer(20, ballmovement);
    iInitializeSound();
    iInitialize(screenwidth, screenheight, "DX Ball");
    return 0;
}
