#include "raylib.h"
#include "matrixl.h"
#include "MNumber.h"
#include "network.h"
#include <Profiler.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <cassert>
#include <random>
#include <functional>

using namespace std;

//----------------------------------------------------------------------------------------
// Typedef and Structures Definition
//----------------------------------------------------------------------------------------

typedef struct Node {
    Vector2 position;
    int radius;
    Color value;
} Node;

typedef struct Line {
    Vector2 startPos;
    Vector2 endPos;
    int thickness;
    Color value;
} Line;

typedef struct Button {
    //Triangle
    Vector2 p1, p2, p3;
    //Rounded Rectangle
    Rectangle rect;
    float roundness;
    int segments;
    Color colorF, colorB;
} Button;

//----------------------------------------------------------------------------------------
// Helper Function Declarations
//----------------------------------------------------------------------------------------

int min(int val1, int val2);
void createLines(const Network &n, Line *lines, Node *nodes);
void createNodes(const Network &n, Node *nodes, int networkWidth, int networkHeight);
void createShownNetwork(Network &sn, const Network &an, const MNumbers &nums, Line *lines, Node *nodes, int networkWidth, int networkHeight);

//----------------------------------------------------------------------------------------
// Global Variables
//----------------------------------------------------------------------------------------

const int screenWidth = 1920;
const int screenHeight = 1080;
const int networkWidth = screenWidth / 2;
const int networkHeight = screenHeight;

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "Neural Network Visualizer");

    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    Network actualNetwork({784, 16, 8, 10});
    MNumbers nums("res/t10k-images.idx3-ubyte", numberBase);
    nums.load(0);
    actualNetwork.forward((float*)nums.values);
    Network shownNetwork({10, 16, 8, 10});
    Node node[shownNetwork.shape().sum()];
    createNodes(shownNetwork, node, networkWidth, networkHeight);
    int lines = 0;
    for (int i = 0; i < shownNetwork.shape().length() - 1; i++) {
        lines += shownNetwork.shape()[i].val() * shownNetwork.shape()[i + 1].val();
    }
    Line line[lines];
    createLines(shownNetwork, line, node);

    Line divider = (Line) {
        (Vector2) {
            screenWidth / 2,
            0
        },
        (Vector2) {
            screenWidth / 2,
            screenHeight
        },
        10,
        BEIGE
    };

    Rectangle numberBase;
    numberBase.x = (screenWidth / 8) * 5;
    numberBase.y = screenHeight / 4;
    numberBase.width = screenWidth / 4;
    numberBase.height = screenHeight / 2;

    Button button[2];
    button[0].p1 = (Vector2) { (screenWidth / 4) * 3 - 20, (screenHeight / 5) + 20 };
    button[0].p2 = (Vector2) { (screenWidth / 4) * 3 - 20, (screenHeight / 5) - 20 };
    button[0].p3 = (Vector2) { (screenWidth / 4) * 3 - 50, (screenHeight / 5) };
    button[0].rect = (Rectangle) {
        (screenWidth / 4) * 3 - 57,
        (screenHeight / 5) - 25,
        50,
        50
    };
    button[0].roundness = 0.5;
    button[0].segments = 3;
    button[0].colorF = BROWN;
    button[0].colorB = DARKBROWN;

    button[1].p1 = (Vector2) { (screenWidth / 4) * 3 + 20, (screenHeight / 5) + 20 };
    button[1].p2 = (Vector2) { (screenWidth / 4) * 3 + 50, (screenHeight / 5) };
    button[1].p3 = (Vector2) { (screenWidth / 4) * 3 + 20, (screenHeight / 5) - 20 };
    button[1].rect = (Rectangle) {
        (screenWidth / 4) * 3 + 7,
        (screenHeight / 5) - 25,
        50,
        50
    };
    button[1].roundness = 0.5;
    button[1].segments = 3;
    button[1].colorF = BEIGE;
    button[1].colorB = DARKBROWN;
    
    Profiler prof;
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (nums.index != 0 && CheckCollisionPointRec( { GetMouseX(), GetMouseY() }, button[0].rect)) {
                nums.loadPrev();
                if (nums.index == 0) {
                    button[0].colorF = BROWN;
                }
                else if (nums.index == 59998) {
                    button[1].colorF = BEIGE;
                }
            }
            else if (nums.index != 59999 && CheckCollisionPointRec( { GetMouseX(), GetMouseY() }, button[1].rect)) {
                nums.loadNext();
                if (nums.index == 1) {
                    button[0].colorF = BEIGE;
                }
                else if (nums.index == 59999) {
                    button[1].colorF = BROWN;
                }
            }
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(DARKGRAY);

            for (int i = 0; i < lines; i++) {
                DrawLineV(line[i].startPos, line[i].endPos, line[i].value);
            }
            DrawLineEx(divider.startPos, divider.endPos, divider.thickness, divider.value);
            for (int i = 0; i < shownNetwork.shape().sum(); i++) {
                DrawCircle(node[i].position.x, node[i].position.y, node[i].radius, node[i].value);
            }

            for (int i = 0; i < 784; i++) {
                DrawRectangleRec(nums.rects[i], nums.colors[i]);
            }

            DrawRectangleRounded(button[0].rect, button[0].roundness, button[0].segments, button[0].colorB);
            DrawRectangleRounded(button[1].rect, button[1].roundness, button[1].segments, button[1].colorB);
            DrawTriangle(button[0].p1, button[0].p2, button[0].p3, button[0].colorF);
            DrawTriangle(button[1].p1, button[1].p2, button[1].p3, button[1].colorF);

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
void createLines(const Network &n, Line *lines, Node *nodes) {
    int startSum = 0, endSum = 0, index = 0;
    for (int i = 0; i < n.shape().length() - 1; i++) {
        endSum += n.shape()[i].val();
        for (int j = 0; j < n.shape()[i].val(); j++) {
            for (int k = 0; k < n.shape()[i + 1].val(); k++, index++) {
                lines[index].startPos = (Vector2) {nodes[startSum + j].position};
                lines[index].endPos = (Vector2) {nodes[endSum + k].position};
                lines[index].value = LIGHTGRAY;
            }
        }
        startSum += n.shape()[i].val();
    }
}

void createNodes(const Network &n, Node *nodes, int networkWidth, int networkHeight) {
    vector<MatrixL<float>> activations = n.getActivations();
    int x_offset = networkWidth / (n.shape().length() + 1);
    int y_offsets[n.shape().length()];
    for (int i = 0; i < n.shape().length(); i++) {
        y_offsets[i] = networkHeight / (n.shape()[i].val() + 1);
    }
    int index = 0;
    for (int i = 0; i < n.shape().length(); ++i) {
        for (int j = 1; j <= n.shape()[i].val(); j++, index++) {
            nodes[index].position = (Vector2) { (i + 1) * x_offset, y_offsets[i] * j };
            nodes[index].radius = (min(x_offset, y_offsets[i]) * 2) / 5;
            nodes[index].value = activations[i].flatAt(index);
        }
    }
}

void createShownNetwork(const Network &sn, const Network &an, Line *lines, Node *nodes, int networkWidth, int networkHeight) {
    nodes = new Node[shownNetwork.shape().sum()];
    vector<MatrixL<float>> shownActivations = actualNetwork.getActivations();
    for (int i = 0; i < sn.shape().length(); i++) {
        if (shownActivations[i].length != sn.shape()[i]) {
            shownActivations[i].reshape_in_place({sn.shape()[i]});
        }
    }
    sn.setActivations(shownActivations);
    createNodes(shownNetwork, node, networkWidth, networkHeight);

    int lines = 0;
    for (int i = 0; i < shownNetwork.shape().length() - 1; i++) {
        lines += shownNetwork.shape()[i].val() * shownNetwork.shape()[i + 1].val();
    }
    Line line[lines];
    createLines(shownNetwork, line, node);
}