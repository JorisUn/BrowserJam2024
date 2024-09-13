#include <endian.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "raylib.h"

#define MAX_TEXT_COLS 1024

typedef enum FontTypes{
    FONTS_REGULAR = 0,
    FONTS_TEST,
    FONTS_MAX,
} FontTypes;

typedef struct {
    char text[MAX_TEXT_COLS][MAX_TEXT_COLS];
    uint length;
    const uint max_length;
} Text;

/* function prototypes */

void PrintMousePosition();
void StringCopy(Text *input_text, char input[]);
void DrawHTMLText(Text text, Font font);
void LoadAllFonts();
void UnloadAllFonts();

Font global_fonts[FONTS_MAX];


int main(int argc, char *argv[])
{
    Font test = LoadFont("../alagard.ttf");
    Text input_text = {
        .max_length = 1024,
        .length = 0,
    };
    LoadAllFonts();
    FILE *input = fopen("../input.html", "r");
    char buffer[528];
    while(fgets(buffer, 527, input) != NULL){
        StringCopy(&input_text, buffer);
    }
    /*
    for(int i=0;i<input_text.length;i++){
        printf("%s", input_text.text[i]);
    }
    */  
    InitWindow(960, 640, "Ooga booga browser ;)");
    while(!WindowShouldClose()){
        BeginDrawing(); 
        ClearBackground(WHITE);
        DrawTextEx(test, "name", (Vector2){0,0}, 10, 1, BLACK);
        DrawHTMLText(input_text, test);
        EndDrawing();
    }

    CloseWindow();
    UnloadAllFonts();
}
void PrintMousePosition(){
    printf("MOUSE POSITION: %d, %d\n", GetMouseX(), GetMouseY());
}
void StringCopy(Text *input_text, char input[]){
    size_t len = strlen(input);
    for(int i=0;i<len;i++){
        input_text->text[input_text->length][i] = input[i];
    }
    input_text->length++;
}
void DrawHTMLText(Text text, Font font){
    for(int i=0;i<text.length;i++){
        DrawTextEx(global_fonts[FONTS_TEST], text.text[i], (Vector2){10, 10+22*i}, 20, 1, BLACK);
    }
}
void LoadAllFonts(){
    //global_fonts[FONTS_REGULAR] = LoadFont("../fonts/RobotoMonoNerdFont-Bold.ttf");
    global_fonts[FONTS_REGULAR] = LoadFontEx("../fonts/RobotoMonoNerdFont-Bold.ttf", 32, NULL, 0);
    global_fonts[FONTS_TEST] = LoadFontEx("../fonts/alagard.ttf", 32, NULL, 0);
}
void UnloadAllFonts(){
    for(int i=0;i<FONTS_MAX;i++)
        UnloadFont(global_fonts[i]);
}
