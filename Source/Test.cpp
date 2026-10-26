//#include "Processor.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include "..\Raylib\raylib.h"

int main() {
    InitWindow(800, 600, "My Processor");

    BeginDrawing();

    int i = 0;

    while (!WindowShouldClose()) {

        BeginDrawing();

        Color gray = {i, i, i++, 255}; // Серый, непрозрачный
        DrawRectangle(100, 100, 50, 50, gray);

        WaitTime(0.1); // Пауза на 100 мс

        EndDrawing();
    }


    getchar();
    CloseWindow();
    return 0;
}
