/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2016 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int min(int val1, int val2);

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Test");

    int ballRadius = 50;
    Vector2 ballPos = {(float)screenWidth / 2.0, (float)screenHeight / 2.0};
    bool shortcut = true;

    SetTargetFPS(240);               // Set our game to run at 240 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose() && shortcut)    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL)) {
            ShowCursor();
        }
        else {
            HideCursor();
        }
        if ((IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL)) && IsKeyDown(KEY_X)) {
            shortcut = false;
        }

        if (IsKeyDown(KEY_RIGHT) && ballPos.x < screenWidth - ballRadius) ballPos.x += 3.7;
        if (IsKeyDown(KEY_LEFT) && ballPos.x > 0 + ballRadius) ballPos.x -= 3.7;
        if (IsKeyDown(KEY_UP) && ballPos.y > 0 + ballRadius) ballPos.y -= 2.0;
        if (IsKeyDown(KEY_DOWN) && ballPos.y < screenHeight - ballRadius) ballPos.y += 2.0;
        if (IsKeyDown(KEY_EQUAL) && ballRadius < min(screenHeight / 2.0, screenWidth / 2.0) &&
            (ballPos.y > 0 + ballRadius && ballPos.y < screenHeight - ballRadius &&
                ballPos.x > 0 + ballRadius && ballPos.x < screenWidth - ballRadius)) ballRadius += 1;
        if (IsKeyDown(KEY_MINUS) && ballRadius > 10) ballRadius -= 1;

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(DARKGRAY);

            DrawCircleV(ballPos, ballRadius, MAROON);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

int min(int val1, int val2) {
    return val1 < val2 ? val1 : val2;
}