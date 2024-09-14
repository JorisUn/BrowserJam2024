#include <bits/types/stack_t.h>
#include <endian.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "raylib.h"

#define MAX_TEXT_ROW 1024
#define MAX_TEXT_COLS 1024
#define MAX_PARAMS_COUNT 32
#define MAX_PARAMS_LENGTH 1024

typedef enum FontTypes{
    FONTS_REGULAR = 0,
    FONTS_BOLD,
    FONTS_ITALIC,
    FONTS_BOLD_ITALIC,
    FONTS_TEST,
    FONTS_MAX,
} FontTypes;


typedef enum {
    STATE_FindStartOfData,
    STATE_FindStartOfToken,
    STATE_ParseData,
    STATE_ParseTagName,
    STATE_ParseTagParam,
    STATE_ParseTagValue,
    STATE_EndOfData
} ParserState;

typedef enum {
    TAG_nil,
    TAG_html,
    TAG_head,
    TAG_header,
    TAG_meta,
    TAG_body,
    TAG_title,
    TAG_nextid,
    TAG_h1,
    TAG_h2,
    TAG_h3,
    TAG_h4,
    TAG_h5,
    TAG_h6,
    TAG_a,
    TAG_p,
    TAG_dl,
    TAG_dt,
    TAG_dd,

} TagState;

typedef struct {
    char name[32];
    char params[MAX_PARAMS_COUNT][MAX_PARAMS_LENGTH];
    char values[MAX_PARAMS_COUNT][MAX_PARAMS_LENGTH];
    size_t params_count;
    bool is_end;
    bool search_for_next;
} Tag;
typedef struct {
    char text[MAX_TEXT_ROW][MAX_TEXT_COLS];
    size_t length;
    const size_t max_length;
} Text;

/* function prototypes */

void PrintMousePosition();
void StringCopy(Text *input_text, char input[]);
void DrawHTMLText(Text text);
void LoadAllFonts();
void UnloadAllFonts();
void RemoveTagsLine(char line[]);
void AddCharToCurrentToken(char c);
void PrintCurrentToken();
void ParseText(Text input_text);

/* constants */
Font global_fonts[FONTS_MAX];
const uint font_regular_size = 24;

char current_token[4096];
size_t current_token_length;
ParserState parser_state = STATE_FindStartOfData;
TagState tag_state = TAG_nil;

int main(int argc, char *argv[])
{
    InitWindow(960, 640, "Ooga booga browser ;)");
    Text input_text = {
        .max_length = 1024,
        .length = 0,
    };
    LoadAllFonts();
    /*
    FILE *input = fopen("../input.html", "r");
    char buffer[528];
    while(fgets(buffer, 527, input) != NULL){
        StringCopy(&input_text, buffer);
    }
    */
    ParseText(input_text);
    /*
    for(int i=0;i<input_text.length;i++){
        printf("%s", input_text.text[i]);
    }
    */  
    /*
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
    */
}
void PrintMousePosition(){
    printf("MOUSE POSITION: %d, %d\n", GetMouseX(), GetMouseY());
}
void StringCopy(Text *input_text, char input[]){
    size_t len = strlen(input);
    for(size_t i=0;i<len;i++){
        input_text->text[input_text->length][i] = input[i];
    }
    input_text->length++;
}
void DrawHTMLText(Text text){
    for(int i=0;i<text.length;i++){
        RemoveTagsLine(text.text[i]);
        if(strcmp(text.text[i], "")!=0)
            DrawTextEx(global_fonts[FONTS_REGULAR], 
                    text.text[i], 
                    (Vector2){10, 10+(font_regular_size+2)*i}, 
                    font_regular_size, 1, BLACK);
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
    if(strcmp(line, "\n")=='\n')
        return;
    char arr[1024] = "";
    size_t len = 0, count = 0;
    bool inside_tag = false;
    len = strlen(line);
    for(size_t i=0;i<len;i++){
        if(line[i]=='<'){
            inside_tag = true;
        }
        if(!inside_tag)
            arr[count++] = line[i];
        if(line[i]=='\n')
            continue;
        if(line[i]=='>'){
            if(line[i+1]=='\n')
                line[i+1]='\0';
            inside_tag = false;
        }
    }
    strncpy(line, arr, len+1);
}

void AddCharToCurrentToken(char c){
    if(current_token_length < sizeof(current_token))
        current_token[current_token_length++] = c;
}
void PrintCurrentToken(){
    AddCharToCurrentToken('\n');
    printf("%s", current_token);
    for(size_t i=0;i<4096; i++)
        current_token[i] = 0;
    current_token_length = 0;
}

void ParseText(Text input_text){
    FILE *input_file = fopen("../input.html", "r");
    Tag tag_current = {
        .name = {0}, 
        .params = {{0}}, 
        .values = {{0}}, 
        .params_count = 0,
        .is_end = 0,
        .search_for_next = 0,};
    while(parser_state != STATE_EndOfData) {
        char c = fgetc(input_file);
        if(c == -1){
            PrintCurrentToken();
            return;
        }
        switch(parser_state){
            case STATE_FindStartOfData:
                if(c == '\n' || c == '\r') break;

            case STATE_FindStartOfToken:
                // Skip whitespace
                if(c==' ' || c == '\t' || c=='\n' || c=='\r') break;
                if(c == '<'){
                    tag_current = (Tag){
                        .name = {0}, 
                        .params = {{0}}, 
                        .values = {{0}}, 
                        .params_count = 0,
                        .is_end = 0,
                        .search_for_next = 0,
                    };
                    parser_state = STATE_ParseTagName;
                    AddCharToCurrentToken(c);
                    break;
                }
                if((c >= 33 && c <= 126)){
                    parser_state = STATE_ParseData;
                    AddCharToCurrentToken(c);
                    break;
                }
                printf("Unexpected ASCII code!: %c, in state:%d\n", c, STATE_FindStartOfToken);
                break;
            case STATE_ParseData:
                // End of data
                if(c=='<'){
                    parser_state = STATE_ParseTagName;
                    PrintCurrentToken();
                    AddCharToCurrentToken(c);
                    break;
                }
                if(c=='\n' || c=='\r') break;
                if(c>=32 && c<=126){
                    AddCharToCurrentToken(c);
                    break;
                }
                printf("Unexpected ASCII code!: %c, in state:%d\n", c, STATE_ParseData);
                break;
            case STATE_ParseTagName:
                if(c=='<') break;
                if(c=='>'){
                    AddCharToCurrentToken(c);
                    PrintCurrentToken();
                    parser_state = STATE_FindStartOfToken;
                    break;
                }
                if(c==' '){
                    parser_state = STATE_ParseTagParam;
                }
                if(c=='/')
                    tag_current.is_end = true;
                if(c>=32 && c<=126){
                    AddCharToCurrentToken(c);
                    size_t len = strlen(tag_current.name);
                    if(len>=0 && len<32){
                        tag_current.name[len] = c;
                    }
                    break;
                }
                printf("Unexpected ASCII code!: %c, in state:%d\n", c, STATE_ParseTagName);
                break;
            case STATE_ParseTagParam:
                if(c==' ') break;
                if(c=='='){
                    AddCharToCurrentToken(c);
                    break;
                }
                if(c=='>'){
                    AddCharToCurrentToken(c);
                    PrintCurrentToken();
                    parser_state = STATE_FindStartOfToken;
                    break;
                }
                if(c=='"'){
                    parser_state = STATE_ParseTagParam;
                    break;
                }
                if(c>=32 && c<=126){
                    AddCharToCurrentToken(c);
                    size_t len = strlen(tag_current.params[tag_current.params_count]);
                    if(len<MAX_PARAMS_LENGTH){
                        tag_current.params[tag_current.params_count][len] = c;
                    }
                    break;
                }
                printf("Unexpected ASCII code!: %c, in state:%d\n", c, STATE_ParseTagParam);
                break;
            case STATE_ParseTagValue:
                if(c=='"'){
                    tag_current.search_for_next = true;
                    tag_current.params_count++;
                    break;
                }
                if(c==' ' && tag_current.search_for_next){
                    AddCharToCurrentToken(c);
                    parser_state = STATE_ParseTagParam;
                    break;
                }
                if(c=='>' && tag_current.search_for_next){
                    AddCharToCurrentToken(c);
                    PrintCurrentToken();
                    parser_state = STATE_FindStartOfToken;
                    break;
                }
                if(c>=32 && c<=126){
                    AddCharToCurrentToken(c);
                    size_t len = strlen(tag_current.values[tag_current.params_count]);
                    if(len<MAX_PARAMS_LENGTH){
                        tag_current.values[tag_current.params_count][len] = c;
                    }
                    break;
                }
                printf("Unexpected ASCII code!: %d, in state:%d\n", c, STATE_ParseTagValue);
                break;
            case STATE_EndOfData:
                return;
        }
    }
}
