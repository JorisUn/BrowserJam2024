#include <endian.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "raylib.h"

#define MAX_TEXT_ROW 1024
#define MAX_TEXT_COLS 1024

typedef enum FontTypes{
    FONTS_REGULAR = 0,
    FONTS_BOLD,
    FONTS_ITALIC,
    FONTS_BOLD_ITALIC,
    FONTS_TEST,
    FONTS_MAX,
} FontTypes;

typedef struct {
    char text[MAX_TEXT_ROW][MAX_TEXT_COLS];
    uint length;
    const uint max_length;
} Text;

/* function prototypes */

void PrintMousePosition();
void StringCopy(Text *input_text, char input[]);
void DrawHTMLText(Text text);
void LoadAllFonts();
void UnloadAllFonts();
void RemoveTagsLine(char line[]);

Font global_fonts[FONTS_MAX];


int main(int argc, char *argv[])
{
    InitWindow(960, 640, "Ooga booga browser ;)");
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
    GenTextureMipmaps(&global_fonts[FONTS_REGULAR].texture);
    SetTextureFilter(global_fonts[FONTS_REGULAR].texture, TEXTURE_FILTER_BILINEAR);
    while(!WindowShouldClose()){
        BeginDrawing(); 
        ClearBackground(WHITE);
        DrawHTMLText(input_text);
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
void DrawHTMLText(Text text){
    for(int i=0;i<text.length;i++){
        RemoveTagsLine(text.text[i]);
        DrawTextEx(global_fonts[FONTS_REGULAR], text.text[i], (Vector2){10, 10+22*i}, 20, 1, BLACK);
    }
}
void LoadAllFonts(){
    //global_fonts[FONTS_REGULAR] = LoadFont("../fonts/RobotoMonoNerdFont-Bold.ttf");
    global_fonts[FONTS_REGULAR] = LoadFontEx("../fonts/RobotoMonoNerdFont-Bold.ttf", 92, NULL, 0);
    global_fonts[FONTS_BOLD] = LoadFontEx("../fonts/RobotoMonoNerdFont-Bold.ttf", 128, NULL, 0);
    global_fonts[FONTS_ITALIC] = LoadFontEx("../fonts/RobotoMonoNerdFont-Italic.ttf", 128, NULL, 0);
    global_fonts[FONTS_BOLD_ITALIC] = LoadFontEx("../fonts/RobotoMonoNerdFont-BoldItalic.ttf.ttf", 128, NULL, 0);
    global_fonts[FONTS_TEST] = LoadFontEx("../fonts/alagard.ttf", 32, NULL, 0);
}
void UnloadAllFonts(){
    for(int i=0;i<FONTS_MAX;i++)
        UnloadFont(global_fonts[i]);
}
void RemoveTagsLine(char line[]){
    char arr[1024] = "";
    size_t len = 0, count = 0;
    bool inside_tag = false;
    len = strlen(line);
    for(int i=0;i<len;i++){
        if(line[i]=='<')
            inside_tag = true;
        if(!inside_tag)
            arr[count++] = line[i];
        if(line[i]=='>')
            inside_tag = false;
    }
    strncpy(line, arr, len+1);
}
