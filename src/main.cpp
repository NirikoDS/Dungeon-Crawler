// Include the libraries
#include <raylib.h>     //library for graphics
#include <cmath>        // library for math tools
#include <iostream>     // library for input output
#include <stdlib.h>     // library for rand() 
#include <time.h>       // library for time()


// definition of constants
#define BLOCK_SIZE 1.0f     // Size of Block
#define ROTATION_SPEED 4.0f // Velocidad de rotación en radianes por segundo
#define MOVE_SPEED 0.05f    // Velocity of movement

#define SKY (Color){187,234,244} // Color of the background

// Range till encounter
#define ENCOUNTER_MIN 7
#define ENCOUNTER_MAX 10

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
    Texture2D monsters[] = {LoadTexture("resources/slime.png")};

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
    float fade = 0.0f;

    GameState currentState = MAZE;

    int stepCounter = 0;
    int encounter = ENCOUNTER_MIN + rand() % (ENCOUNTER_MAX - ENCOUNTER_MIN + 1);

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

                        if (stepCounter == encounter) {
                            stepCounter = 0;
                            encounter = ENCOUNTER_MIN + rand() % (ENCOUNTER_MAX - ENCOUNTER_MIN + 1);
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
                DrawText(TextFormat("Encounter: (%d)", encounter), 10, 50, 20, DARKGRAY);
                
                DrawFPS(10, 10);
                
                EndDrawing();
                break;
            }


            case COMBAT: {
                camera.target = Vector3{17.5f,0.0,5.0f};
                if (heigh > 0.4f) {
                    camera.position = (Vector3){16.0f, heigh, 5.0f};
                    heigh = heigh - 0.01f ;
                    fade -= 5.1f;
                }

                else {
                    camera.position = (Vector3){16.0f,0.4,5.0f};

                    if(IsKeyPressed(KEY_ENTER))
                        currentState = MAZE;
                }
                

                BeginDrawing();
                    ClearBackground(SKY);
                BeginMode3D(camera);
                    DrawModel(model, mapPosition, 1.0f, WHITE); // Dibujar mapa del laberinto
                    DrawBillboard(camera, monsters[SLIME], (Vector3){camera.position.x + 1.0f, 0.25f, camera.position.z}, 0.5f, WHITE);
                EndMode3D();

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
