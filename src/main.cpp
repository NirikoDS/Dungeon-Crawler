// Include the libraries
#include <raylib.h>     //library for graphics
#include <cmath>        // library for math tools
#include <iostream>     // library for input output
#include <stdlib.h>     // library for rand() 
#include <time.h>       // library for time()
#include <string>

bool FULLSCREEN = false;

Texture2D Textures[3];
Texture2D UITextures[8];

Font font; 

enum Monsters {
    CHIMON,
    SLAIER,
    CATERLY
};

#define HEIGHT_MONSTER 100.0f

const char* Options[] = {
    "ATTACK",
    "DEFEND",
    "SKILL",
    "ITEM",
    "BURST",
    "SWITCH",
    "ESCAPE"
    };


class Monster {
    protected:
        std::string name;
        int health;
        int attack;
        int defense;
        int speed;
    
    public:
        Texture image;
        
        Monster(std::string _name, int _health, int _attack, int _defense, int _speed, Texture _image)
            : name(_name), health(_health), attack(_attack), defense(_defense), speed(_speed), image(_image) {}

        void Draw(float deltaMonster,float fade, float pos) {
            DrawTextureEx(
                image,
                (Vector2) {GetScreenWidth()/2 + pos * (1 + (float)FULLSCREEN),
                            (float)GetScreenHeight()/3 - ((image.height * (1 + (float)FULLSCREEN))/3) - (HEIGHT_MONSTER+HEIGHT_MONSTER*FULLSCREEN) +  (deltaMonster+deltaMonster*FULLSCREEN)},
                            0.0f,
                            1.0f + float(FULLSCREEN),
                            (Color){255,255,255, (unsigned char)fade});
        }
};

class Chimon : public Monster {
    public:
        Chimon() : Monster("Chimon", 10, 5, 5, 5, Textures[CHIMON]) {}
};

class Caterly : public Monster {
    public:
        Caterly() : Monster("Caterly", 20, 7, 8, 3,Textures[CATERLY]) {}
};

class Slaier : public Monster {
    public:
        Slaier() : Monster("Slaier", 20, 7, 8, 3, Textures[SLAIER]) {}
};


void FillWindow(Color color, float fade){
    DrawRectangle(0,0, GetScreenWidth(), GetScreenHeight() + (float)FULLSCREEN * 100,(Color){color.r,color.g,color.b,(unsigned char)fade});
}

// definition of constants
#define BLOCK_SIZE 1.0f     // Size of Block
#define ROTATION_SPEED 4.0f // Velocidad de rotación en radianes por segundo
#define MOVE_SPEED 0.05f    // Velocity of movement

#define SKY (Color){187,234,244} // Color of the background

// Range till encounter
#define ENCOUNTER_MIN 1
#define ENCOUNTER_MAX 1

// Función para calcular la diferencia mínima entre dos ángulos
float AngleDifference(float target, float current) {
    float diff = target - current;
    while (diff > PI) diff -= 2.0f * PI;
    while (diff < -PI) diff += 2.0f * PI;
    return diff;
}

// Operadores para facilitar las operaciones con Vector2
Vector2 operator+(const Vector2& v1, const Vector2& v2) {
    return (Vector2){v1.x + v2.x, v1.y + v2.y};
}

Vector2 operator-(const Vector2& v1, const Vector2& v2) {
    return (Vector2){v1.x - v2.x, v1.y - v2.y};
}

Vector2 operator*(const Vector2& v, float scalar) {
    return (Vector2){v.x * scalar, v.y * scalar};
}

bool operator<(const Vector2& v1, const Vector2& v2) {
    return {v1.x < v2.x || v1.y < v2.y};
}

// Conversión de coordenadas del juego a coordenadas del mundo
Vector3 gameToWorld(Vector2 gamePos) {
    return (Vector3){gamePos.x, 0.4f, gamePos.y};
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
    return (Vector3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

struct Encounter{
        int type;
        int monsterNumber;
        Monster* monsters[5];
    };

void DrawUI(int* index){

    if (IsKeyPressed(KEY_W))
        *index = *index - 1 == -1? 7 : *index - 1 == 0? 7 : *index - 1;
    
    else if (IsKeyPressed(KEY_S))
        *index = *index + 1 == 8? 1 : *index + 1;

    DrawTextureEx(UITextures[*index], (Vector2){ 0, 113.0f * (1 + FULLSCREEN)}, 0.0f, 1.0f + (float)FULLSCREEN, WHITE);

    for (int i = 0; i < 7; i++){
        if (i+1 == *index)
            DrawTextEx(font, TextFormat(Options[i]), (Vector2) {180.0f, 116.0f + 39.0f*i} * (1 + FULLSCREEN), 47 * (1 + FULLSCREEN), 0.0f, (Color){0,0,102,230});
        else DrawTextEx(font, TextFormat(Options[i]), (Vector2) {180.0f, 116.0f + 39.0f*i} * (1 + FULLSCREEN), 47 * (1 + FULLSCREEN), 0.0f, (Color){255,255,255,230});
    }
    
}

void DrawEncounter(Encounter* encounter, Camera camera, float deltaMonster, float fade)  {
    switch(encounter->type){ 
        case 0: {  
            encounter->monsters[0]->Draw(deltaMonster, fade, - encounter->monsters[0]->image.width/2 - encounter->monsters[0]->image.width - 10);
            encounter->monsters[1]->Draw(deltaMonster, fade, - encounter->monsters[1]->image.width/2);
            encounter->monsters[2]->Draw(deltaMonster, fade, - encounter->monsters[2]->image.width/2 + encounter->monsters[2]->image.width + 10);
            break;
        }

        case 1: {  
            encounter->monsters[0]->Draw(deltaMonster, fade, - encounter->monsters[0]->image.width/2);
            break;
        }

        case 2: {  
            encounter->monsters[0]->Draw(deltaMonster, fade, - encounter->monsters[0]->image.width - 5);
            encounter->monsters[1]->Draw(deltaMonster, fade, + 5);
            break;
        }

        case 3: {  
            encounter->monsters[0]->Draw(deltaMonster, fade, - encounter->monsters[0]->image.width/2);
            break;
        }

        case 4: {  
            encounter->monsters[0]->Draw(deltaMonster, fade, - encounter->monsters[0]->image.width/2 - encounter->monsters[0]->image.width - 10);
            encounter->monsters[1]->Draw(deltaMonster, fade, - encounter->monsters[1]->image.width/2);
            encounter->monsters[2]->Draw(deltaMonster, fade, - encounter->monsters[2]->image.width/2 + encounter->monsters[2]->image.width + 10);
            break;
        }

    }
};

void GenerateEncounter(Encounter* encounter) {

    Encounter setups[] = {
        {0, 3, {new Chimon(), new Chimon(), new Chimon(), NULL, NULL}},
        {1, 1, {new Chimon(), NULL, NULL, NULL, NULL}},
        {2, 2, {new Chimon(), new Caterly(), NULL, NULL, NULL}},
        {3, 1, {new Caterly(), NULL, NULL, NULL, NULL}},
        {4, 3, {new Slaier(), new Chimon(), new Caterly(), NULL, NULL}}
    };

    int random = rand() % 5;

    encounter->type = setups[random].type;
    encounter->monsterNumber = setups[random].monsterNumber;
    for (int i = 0; i < 5; i++) {
        encounter->monsters[i] = setups[random].monsters[i];
    }
}

int main(void)
{
    
    const int screenWidth = 960;
    const int screenHeight = 540;
    InitWindow(screenWidth, screenHeight, "Maze Core");

    srand(time(NULL));

    //camera
    Camera camera = { 0 };
    camera.position = (Vector3) {0.2f, 0.4f, 0.2f};
    camera.target = (Vector3) {0.2f, 0.4f, 0.0f};
    camera.up = (Vector3) {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    //list of monster
    

    Textures[0] = LoadTexture("resources/chimon.png");
    Textures[1] = LoadTexture("resources/slaier.png");
    Textures[2] = LoadTexture("resources/caterly.png");

    Image imMap = LoadImage("resources/cubicmap.png");
    Texture2D cubicmap = LoadTextureFromImage(imMap);
    Mesh mesh = GenMeshCubicmap(imMap, (Vector3){1.0f, 1.0f, 1.0f});
    Model model = LoadModelFromMesh(mesh);

    Texture2D texture = LoadTexture("resources/cubicmap_atlas.png");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Texture2D ramas = LoadTexture("resources/ramas.png");
    
    for (int i = 0; i<8; i++)
        UITextures[i] = LoadTexture(("resources/UIselector/UIselector" + std::to_string(i) +".png").c_str());

    font = LoadFont("resources/font/dungeon.otf");
    Color *mapPixels = LoadImageColors(imMap);
    int pixelArray[16][32];

    // Convert to a binary pixel array
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 32; x++) {
            Color color = mapPixels[y * (32 + 4) + x];
            pixelArray[y][x] = (color.r > 128 && color.g > 128 && color.b > 128) ? 0 : 1; // Simplified thresholding for B/W
            //std::cout<<pixelArray[y][x]<<" ";
        }
        //std::cout<<std::endl;
    }

    UnloadImage(imMap);

    Vector3 mapPosition = {-16.0f, 0.0f, -8.0f};

    DisableCursor();
    SetTargetFPS(60);

    Vector2 gamePos = {mapPosition.x + 1.0f, mapPosition.z + 1.0f};
    Vector2 nextGamePos = gamePos;
    Vector2 playerPos = {1,1};

    enum Direction {
        UP = 0,
        DOWN = 1,
        LEFT = 2,
        RIGHT = 3
    };
    
    enum GameState {
        MENU,
        MAZE,
        COMBAT
    };

    Encounter encounter;

    Vector2 DIRECTIONS[] = {
        (Vector2) { 0,-1},
        (Vector2) { 0, 1},
        (Vector2) {-1, 0},
        (Vector2) { 1, 0},
    };

    float angles[] = {
        -PI / 2.0f,         // UP: -90 radianes
        PI / 2.0f,           // DOWN: 90 grados
        PI,   // LEFT: 180 grados
        0.0f     // RIGHT: 0 grados
    };

    int direction = RIGHT;
    float currentAngle = angles[direction];
    float targetAngle = currentAngle;
    bool rotating = false;
    int moving = 0;
    int tile = 0;
    int index = 0;

    float heigh = 0.0f;
    bool battle = false;

    int playerCellX = (int)(camera.position.x - mapPosition.x + 0.5f);
    int playerCellY = (int)(camera.position.z - mapPosition.z + 0.5f);

    float deltaBattle = 0.0f;
    float deltaMonster = 0.0f;
    float fade = 0.0f;

    GameState currentState = MAZE;

    int stepCounter = 0;
    int encounter_steps = ENCOUNTER_MIN + rand() % (ENCOUNTER_MAX - ENCOUNTER_MIN + 1);

    float monsterFade = 0.0f;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_F)) {
			
            if (!FULLSCREEN){SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0)), ToggleFullscreen();}
            else {ToggleFullscreen(); SetWindowSize(screenWidth, screenHeight);}
            FULLSCREEN = !FULLSCREEN;
		}

        // Cambiar dirección objetivo al presionar teclas
        switch (currentState) {
            case MAZE: {
                if(!rotating && moving==0 and !battle){
                    if (IsKeyDown(KEY_A)) {
                        direction = direction == RIGHT ? UP : direction == UP ? LEFT : direction == LEFT ? DOWN : RIGHT;
                        targetAngle = angles[direction];
                        rotating = true;
                    } else if (IsKeyDown(KEY_D)) {
                        direction = direction == RIGHT ? DOWN : direction == DOWN ? LEFT : direction == LEFT ? UP : RIGHT;
                        targetAngle = angles[direction];
                        rotating = true;
                    }
                }
                if (rotating) {
                    // Interpolación angular suave hacia la dirección objetivo
                    float angleDifference = AngleDifference(targetAngle, currentAngle);
                    if (fabs(angleDifference) < ROTATION_SPEED * GetFrameTime()) {
                        currentAngle = targetAngle;
                        rotating = false;
                    } else {
                        currentAngle += ROTATION_SPEED * GetFrameTime() * (angleDifference > 0 ? 1 : -1);
                    }
                }
                
            // Actualizar dirección actual
                Vector2 currentDir = {(float)cos(currentAngle), (float)sin(currentAngle)};
                camera.target = gameToWorld(gamePos + currentDir);
                camera.position = gameToWorld(gamePos);

                if (!rotating && moving==0 and !battle) {
                    if (IsKeyDown(KEY_W)){ 
                        tile = pixelArray[(int)(playerPos.y + DIRECTIONS[direction].y)][(int)(playerPos.x + DIRECTIONS[direction].x)];
                        if (tile != 0) {
                            nextGamePos = gamePos + currentDir;
                            playerPos = playerPos + DIRECTIONS[direction];
                            moving = 1;

                            if (tile == 1)
                                stepCounter++;

                        }
                    }

                    else if (IsKeyDown(KEY_S)){
                        tile = pixelArray[(int)(playerPos.y - DIRECTIONS[direction].y)][(int)(playerPos.x - DIRECTIONS[direction].x)];
                        if (tile != 0) {
                            nextGamePos = gamePos - currentDir;
                            playerPos = playerPos - DIRECTIONS[direction];
                            moving = -1;

                            if (tile == 1)
                                stepCounter++;
                        }
                    }
                }

                if (moving!=0){
                    gamePos = gamePos + ((moving==1? currentDir : currentDir*-1) * MOVE_SPEED);

                    if (fabs(gamePos.x - nextGamePos.x) < MOVE_SPEED && fabs(gamePos.y - nextGamePos.y) < MOVE_SPEED){
                        moving = 0;
                        gamePos = nextGamePos;

                        if (stepCounter == encounter_steps) {
                            stepCounter = 0;
                            encounter_steps = ENCOUNTER_MIN + rand() % (ENCOUNTER_MAX - ENCOUNTER_MIN + 1);
                            battle = true;
                        }
                    }
                        
                }

                playerCellX = (int)(camera.position.x - mapPosition.x + 0.5f);
                playerCellY = (int)(camera.position.z - mapPosition.z + 0.5f);
                

                BeginDrawing();
                    ClearBackground(SKY);

                BeginMode3D(camera);
                    DrawModel(model, mapPosition, 1.0f, WHITE); // Dibujar mapa del laberinto
                EndMode3D();


                if (battle) {
                    deltaBattle += 2.5f;
                    fade = (fade+5.5>255)?255:fade+5.5;
                    DrawTextureEx(ramas, (Vector2){-ramas.width/8.0f, ramas.height/8.0f}, 90-deltaBattle, 1.0f + float(FULLSCREEN), WHITE);
                    DrawTextureEx(ramas, (Vector2){-ramas.width/8.0f, GetScreenHeight()+ramas.height/2.0f}, -deltaBattle, 1.0f + float(FULLSCREEN), WHITE);
                    DrawTextureEx(ramas, (Vector2){GetScreenWidth()+ramas.width/8.0f, ramas.height/8.0f}, deltaBattle, 1.0f + float(FULLSCREEN), WHITE);
                    DrawTextureEx(ramas, (Vector2){GetScreenWidth()+ramas.width/8.0f, GetScreenHeight()+ramas.height/2.0f}, 90+deltaBattle, 1.0f + float(FULLSCREEN), WHITE);
                    
                    FillWindow((Color){0,0,0}, fade);
                    
                    if(deltaBattle >= 160.0f){
                        monsterFade = 0.0f;
                        deltaMonster = 0.0f;
                        deltaBattle = 0.0f;
                        battle = false;
                        heigh = 0.9f;
                        index = 0;
                        GenerateEncounter(&encounter);
                        currentState = COMBAT;
                    }
                }

                DrawTextureEx(cubicmap, (Vector2){GetScreenWidth() - cubicmap.width * 4.0f - 20, 20.0f}, 0.0f, 4.0f, WHITE);
                DrawRectangleLines(GetScreenWidth() - cubicmap.width * 4 - 20, 20, cubicmap.width * 4, cubicmap.height * 4, GREEN);

                // Dibujar posición del jugador en el radar
                DrawRectangle(GetScreenWidth() - cubicmap.width * 4 - 20 + playerCellX * 4, 20 + playerCellY * 4, 4, 4, RED);
                
                
                //DrawText(TextFormat("Game Position: (%.2f, %.2f)", gamePos.x, gamePos.y), 10, 70, 20, DARKGRAY);
                //DrawText(TextFormat("Player Position: (%.2f, %.2f)", playerPos.x, playerPos.y), 10, 30, 20, DARKGRAY);
                //DrawText(TextFormat("CurrentDir: (%.2f, %.2f)", currentDir.x, currentDir.y), 10, 50, 20, DARKGRAY);
                //DrawText(TextFormat("Array: (%d)", pixelArray[(int)playerPos.y][(int)playerPos.x]), 10, 70, 20, DARKGRAY);
                //DrawText(TextFormat("Direction: (%d)", direction), 10, 90, 20, DARKGRAY);

                DrawText(TextFormat("CounterStep: (%d)", stepCounter), 10, 30, 20, DARKGRAY);
                DrawText(TextFormat("Encounter: (%d)", encounter_steps), 10, 50, 20, DARKGRAY);

                DrawText(TextFormat("Size: (%d), (%d)", GetMonitorWidth(GetCurrentMonitor()),GetMonitorHeight(GetCurrentMonitor())), 10, 70, 20, DARKGRAY);
                
                DrawFPS(10, 10);
                
                EndDrawing();
                break;
            }


            case COMBAT: {
                camera.fovy = 70;
                camera.target = Vector3{17.0f,0.0,5.0f};
                if (heigh > 0.4f) {
                    camera.position = (Vector3){16.0f, heigh, 5.0f};
                    heigh = heigh - 0.01f ;
                    fade -= 5.1f;
                }

                else if (deltaMonster < HEIGHT_MONSTER) {
                    deltaMonster = deltaMonster + 3.5f > HEIGHT_MONSTER? HEIGHT_MONSTER : deltaMonster + 3.5f;
                    monsterFade = monsterFade + 10.0f > 255.0f ?  255.0f : monsterFade + 10.0f;
                }

                else if (monsterFade < 254.9f) {
                    monsterFade = monsterFade + 10.0f > 255.0f ?  255.0f : monsterFade + 10.0f;
                }

                else {
                    camera.position = (Vector3){16.0f,0.4,5.0f};
                    deltaMonster = HEIGHT_MONSTER;
                    monsterFade = 255.0f;
                    if(IsKeyPressed(KEY_ENTER))
                        currentState = MAZE;
                }
                

                BeginDrawing();
                    ClearBackground(SKY);
                BeginMode3D(camera);
                    DrawModel(model, mapPosition, 1.0f, WHITE); // Dibujar mapa del laberinto
                EndMode3D();
                
                DrawEncounter(&encounter, camera, deltaMonster, monsterFade);
                
                DrawUI(&index);
                
                if (heigh > 0.4f)
                    FillWindow((Color){0,0,0}, fade);
                //DrawTextureEx(cubicmap, (Vector2){GetScreenWidth() - cubicmap.width * 4.0f - 20, 20.0f}, 0.0f, 4.0f, WHITE);
                
                DrawFPS(10, 10);
                EndDrawing();
                break;
            }


            case MENU: {

                break;
            }
        }
        // Renderizado

        
    }

    // Desinicialización
    UnloadImageColors(mapPixels);
    UnloadTexture(cubicmap);
    UnloadTexture(texture);
    UnloadModel(model);

    CloseWindow();

    return 0;
}
