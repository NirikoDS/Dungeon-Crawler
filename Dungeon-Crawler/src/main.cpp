// Include the libraries
#include <raylib.h>     //library for graphics
#include <cmath>        // library for math tools
#include <iostream>     // library for input output
#include <stdlib.h>     // library for rand() 
#include <time.h>       // library for time()
#include <string>

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

        void Draw(float deltaMonster, float pos) {
            DrawTextureEx(image, (Vector2){pos, (float)(GetScreenHeight()/3 - image.height/4)}, 0.0f, 1.0f, WHITE);
        }
};

class Slime : public Monster {
    public:
        Slime() : Monster("Slime", 10, 5, 5, 5, LoadTexture("resources/slime.png")) {}
};

class Calabaza : public Monster {
    public:
        Calabaza() : Monster("Calabaza", 20, 7, 8, 3, LoadTexture("resources/calabaza.png")) {}
};

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
        int monsterNumber;
        Monster* monsters[5];
        int type;
    };
    
void DrawEncounter(Encounter* encounter, Camera camera, float deltaMonster)  {
    switch(encounter->type){ 
        case 0: {  
            encounter->monsters[0]->Draw(deltaMonster, GetScreenWidth()/2 - encounter->monsters[0]->image.width/2 - encounter->monsters[0]->image.width - 20);
            encounter->monsters[1]->Draw(deltaMonster, GetScreenWidth()/2 - encounter->monsters[1]->image.width/2);
            encounter->monsters[2]->Draw(deltaMonster, GetScreenWidth()/2 - encounter->monsters[2]->image.width/2 + encounter->monsters[2]->image.width + 20);
            break;
        }

        case 1: {  
            encounter->monsters[0]->Draw(deltaMonster, GetScreenWidth()/2 - encounter->monsters[0]->image.width/2);
            break;
        }

        case 2: {  
            encounter->monsters[0]->Draw(deltaMonster, GetScreenWidth()/2 - encounter->monsters[0]->image.width - 10);
            encounter->monsters[1]->Draw(deltaMonster, GetScreenWidth()/2 + 10);
            break;
        }

        case 3: {  
            encounter->monsters[0]->Draw(deltaMonster, GetScreenWidth()/2 - encounter->monsters[0]->image.width/2);
            break;
        }

    }
};

void GenerateEncounter(Encounter* encounter) {
    int random = 2+rand() % 1;

    switch (random) {
        case 0: {
            encounter->monsters[0] = {new Slime()};
            encounter->monsters[1] = {new Slime()};
            encounter->monsters[2] = {new Slime()};
            encounter->monsters[3] = {NULL};
            encounter->monsters[4] = {NULL};
            encounter->monsterNumber = 3;
            encounter->type = random;
            break;
        }

        case 1: {
            encounter->monsters[0] = {new Slime()};
            encounter->monsters[1] = {NULL};
            encounter->monsters[2] = {NULL};
            encounter->monsters[3] = {NULL};
            encounter->monsters[4] = {NULL};
            encounter->monsterNumber = 1;
            encounter->type = random;
            break;
        }

        case 2: {
            encounter->monsters[0] = {new Slime()};
            encounter->monsters[1] = {new Calabaza()};
            encounter->monsters[2] = {NULL};
            encounter->monsters[3] = {NULL};
            encounter->monsters[4] = {NULL};
            encounter->monsterNumber = 2;
            encounter->type = random;
            break;
        }

        case 3: {
            encounter->monsters[0] = {new Calabaza()};
            encounter->monsters[1] = {NULL};
            encounter->monsters[2] = {NULL};
            encounter->monsters[3] = {NULL};
            encounter->monsters[4] = {NULL};
            encounter->monsterNumber = 1;
            encounter->type = random;
            break;
        }
    }
};


int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
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
    enum Monsters {
        SLIME
    };

    Image imMap = LoadImage("resources/cubicmap.png");
    Texture2D cubicmap = LoadTextureFromImage(imMap);
    Mesh mesh = GenMeshCubicmap(imMap, (Vector3){1.0f, 1.0f, 1.0f});
    Model model = LoadModelFromMesh(mesh);

    Texture2D texture = LoadTexture("resources/cubicmap_atlas.png");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Texture2D ramas = LoadTexture("resources/ramas.png");

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

    while (!WindowShouldClose()) {
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
                    DrawTextureEx(ramas, (Vector2){-ramas.width/8.0f, ramas.height/8.0f}, 90-deltaBattle, 1.0f, WHITE);
                    DrawTextureEx(ramas, (Vector2){-ramas.width/8.0f, GetScreenHeight()+ramas.height/2.0f}, -deltaBattle, 1.0f, WHITE);
                    DrawTextureEx(ramas, (Vector2){GetScreenWidth()+ramas.width/8.0f, ramas.height/8.0f}, deltaBattle, 1.0f, WHITE);
                    DrawTextureEx(ramas, (Vector2){GetScreenWidth()+ramas.width/8.0f, GetScreenHeight()+ramas.height/2.0f}, 90+deltaBattle, 1.0f, WHITE);
                    DrawRectangle(0,0,GetScreenWidth(), GetScreenHeight(),(Color){0,0,0,(unsigned char)fade});
                    
                    if(deltaBattle >= 160.0f){
                        deltaBattle = 0.0f;
                        battle = false;
                        heigh = 0.9f;
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
                    if (heigh < 0.7f)
                        deltaMonster += 0.015f;
                     
                }

                else if (deltaMonster < 0.5f) {
                    deltaMonster = deltaMonster + 0.015f > 0.5f ? 0.5f : deltaMonster + 0.015f;
                }

                else {
                    camera.position = (Vector3){16.0f,0.4,5.0f};
                    deltaMonster = 0.5f;
                    if(IsKeyPressed(KEY_ENTER))
                        currentState = MAZE;
                }
                

                BeginDrawing();
                    ClearBackground(SKY);
                BeginMode3D(camera);
                    DrawModel(model, mapPosition, 1.0f, WHITE); // Dibujar mapa del laberinto
                EndMode3D();
                
                DrawEncounter(&encounter, camera, deltaMonster);
                if (heigh > 0.4f)
                    DrawRectangle(0,0,GetScreenWidth(), GetScreenHeight(),(Color){0,0,0,(unsigned char)fade});
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
