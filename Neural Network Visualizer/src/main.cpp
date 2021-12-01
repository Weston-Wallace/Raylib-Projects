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
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

int min(int val1, int val2);
int sum(const vector<int>& vec);
int sum(const vector<int>&& vec);

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    const int networkWidth = screenWidth / 2;
    const int networkHeight = screenHeight;

    InitWindow(screenWidth, screenHeight, "Neural Network Visualizer");

    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));               // Set our game to run at 60 frames-per-second

    vector<int> shape({2, 8, 4, 10});
    int x_offset = networkWidth / (shape.size() + 1);
    vector<int> y_offsets(shape.size());
    for (int i = 0; i < shape.size(); i++) {
        y_offsets[i] = networkHeight / (shape[i] + 1);
    }
    vector<Vector2> circlePos(sum(shape));
    vector<int> circleSize(circlePos.size());
    for (int i = 0; i < shape.size(); ++i) {
        for (int j = 1; j <= shape[i]; j++) {
            int index = (sum(vector<int>(shape.begin(), shape.begin() + i))) + j - 1;
            circlePos[index].x = (i + 1) * x_offset;
            circlePos[index].y = y_offsets[i] * j;
            circleSize[index] = (min(x_offset, y_offsets[i]) * 2) / 5;
        }
    };
    int lines = 0;
    for (int i = 0; i < shape.size() - 1; i++) {
        lines += shape[i] * shape[i + 1];
    }
    vector<vector<Vector2>> linePos = vector<vector<Vector2>>(lines, vector<Vector2>(2));
    int index = 0;
    for (int i = 0; i < shape.size() - 1; i++) {
        cout << sum(vector<int>(shape.begin(), shape.begin() + i)) << endl;
        for (int j = 0; j < shape[i]; j++) {
            for (int k = 0; k < shape[i + 1]; k++, index++) {
                int startIndex = sum(vector<int>(shape.begin(), shape.begin() + i)) + j;
                int endIndex = sum(vector<int>(shape.begin(), shape.begin() + i + 1)) + k;
                linePos[index][0] = circlePos[startIndex];
                linePos[index][1] = circlePos[endIndex];
                cout << "index: " << index << endl;
                cout << "startIndex: " << startIndex << endl;
                cout << "endIndex: " << endIndex << endl;
            }
        }
    }

    Vector2 dividerStart, dividerEnd;
    dividerStart.x = dividerEnd.x = screenWidth / 2;
    dividerStart.y = 0;
    dividerEnd.y = screenHeight;
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(DARKGRAY);

            for (int i = 0; i < linePos.size(); i++) {
                DrawLineEx(linePos[i][0], linePos[i][1], 2, LIGHTGRAY);
            }
            for (int i = 0; i < circlePos.size(); i++) {
                DrawCircle(circlePos[i].x, circlePos[i].y, circleSize[i], GRAY);
            }
            
            DrawLineEx(dividerStart, dividerEnd, 10, BEIGE);

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

int sum(const vector<int>& vec) {
    int sum = 0;
    for (const auto &num : vec) {
        sum += num;
    }
    return sum;
}

int sum(const vector<int>&& vec) {
    int sum = 0;
    for (const auto &num : vec) {
        sum += num;
    }
    return sum;
}