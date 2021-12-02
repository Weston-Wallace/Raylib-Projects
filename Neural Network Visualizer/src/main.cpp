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

    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    vector<int> shape({2, 16, 8, 10});
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
            }
        }
    }

    Vector2 dividerStart, dividerEnd;
    dividerStart.x = dividerEnd.x = screenWidth / 2;
    dividerStart.y = 0;
    dividerEnd.y = screenHeight;
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(DARKGRAY);

            for (int i = 0; i < linePos.size(); i++) {
                DrawLineV(linePos[i][0], linePos[i][1], LIGHTGRAY);
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
    CloseWindow();
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