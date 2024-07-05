// Include the libraries
#include <raylib.h>     //library for graphics
#include <cmath>        // library for math tools
#include <iostream>     // library for input output
#include <stdlib.h>     // library for rand() 
#include <time.h>       // library for time()
#include <string>

#include "lib/tools.h"
#include "lib/SaveSystem.hpp"
#include "lib/City.hpp"

// Define the height to enter combat
int main(void)
{
    const int screenWidth = 960;
    const int screenHeight = 540;
    InitWindow(screenWidth, screenHeight, "Forgotten Echoes");
    InitAudioDevice();

    // Load the MP3 sound
    Sound sound = LoadSound("resources/sound.mp3");

    // Set the volume for the sound (optional)
    SetSoundVolume(sound, 0.5f);

    srand(time(NULL));

    //camera
    Camera camera = { 0 };
    camera.position = (Vector3) {0.2f, 0.4f, 0.2f};
    camera.target = (Vector3) {0.2f, 0.4f, 0.0f};
    camera.up = (Vector3) {0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    LoadResources();

    Image imMap = LoadImage("resources/cubicmap.png");
    Texture2D cubicmap = LoadTextureFromImage(imMap);
    Mesh mesh = GenMeshCubicmap(imMap, (Vector3){1.0f, 1.0f, 1.0f});
    Model model = LoadModelFromMesh(mesh);

    Texture2D texture1 = LoadTexture("resources/atlasDay.png");
    Texture2D BattleResults = LoadTexture("resources/Scenarios/BattleResults.png");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture1;

    Texture2D ramas;
    
    Image imMap2 = LoadImage("resources/cubicmapLocations.png");
    Color *mapPixels = LoadImageColors(imMap2);
    int pixelArray[16][32];

    // Convert to a binary pixel array
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 32; x++) {
            Color color = mapPixels[y * (32 + 4) + x];
            pixelArray[y][x] = (color.r > 200 && color.g > 200 && color.b > 200) ? 0 : 
                                (color.r > 200 ) ? 2 : 1; // Simplified thresholding for B/W
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
    Vector2 playerPos = {1,1}; //con referencia al mapa

    Vector2 DIRECTIONS[] = {
        (Vector2) { 0,-1}, //arriba
        (Vector2) { 0, 1}, //abajo
        (Vector2) {-1, 0}, //izquierda
        (Vector2) { 1, 0}, //derecha
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
    float monsterFade = 0.0f;
    float ExperienceFade = 0.0f;
    int fadeEntrance = 255;

    GameState currentState = MENU;

    int stepCounter = 0;
    int encounter_steps = ENCOUNTER_MIN + rand() % (ENCOUNTER_MAX - ENCOUNTER_MIN + 1);

    team[0] = new Warrior("Thalorin", UI_Swordsman[0], 0);
    team[2] = new Warrior("Elara", UI_Swordsman[1], 1);
    team[3] = new Warrior("Gorak", UI_Swordsman[2], 2);
    team[4] = new Warrior("Lyria", UI_Swordsman[3], 3);
    team[5] = new Warrior("Darine", UI_Swordsman[4], 4);

    team[3]->GainExp(10000);
    team[2]->GainExp(100);
    team[0]->GainExp(25);

    Inventory.push_back(Item{"Potion", "Heals 10 HP", 50, USABLE});
    Inventory.push_back(Item{"Sword", "A basic weapon for a warrrior", 75, EQUIPMENT});
    Inventory.push_back(Item{"Potion", "Heals 10 HP", 50, USABLE});
    Inventory.push_back(Item{"Sword", "A basic weapon for a warrrior", 75, EQUIPMENT});
    Inventory.push_back(Item{"Potion", "Heals 10 HP", 50, USABLE});
    Inventory.push_back(Item{"Sword", "A basic weapon for a warrrior", 75, EQUIPMENT});
    Inventory.push_back(Item{"Potion", "Heals 10 HP", 50, USABLE});
    Inventory.push_back(Item{"Sword", "A basic weapon for a warrrior", 75, EQUIPMENT});
    Inventory.push_back(Item{"Potion", "Heals 10 HP", 50, USABLE});
    Inventory.push_back(Item{"Large Potion", "Heals 50 HP", 70, USABLE});

    SortInventory();

    //for (const auto &item : Inventory) std::cout << "Name: " << item.name << ", Value: " << item.value << std::endl;

    TeamDivider();

    float r = 255, g = 255, b = 255;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_F)) {
			
            if (!FULLSCREEN){SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0)), ToggleFullscreen();}
            else {ToggleFullscreen(); SetWindowSize(screenWidth, screenHeight);}
            FULLSCREEN = !FULLSCREEN;
            scale = 1 + FULLSCREEN;
		}

        // Cambiar dirección objetivo al presionar teclas
        switch (currentState) {
            case MAZE: {
                static bool entrance = true;
                static bool exit = false;
                camera.fovy = 65.0f;
                if (!IsSoundPlaying(sound)) PlaySound(sound);

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

                tile = pixelArray[(int)(playerPos.y + DIRECTIONS[direction].y)][(int)(playerPos.x + DIRECTIONS[direction].x)];

                if (!rotating && moving==0 and !battle) {
                    if (IsKeyDown(KEY_W)){ 
                        if (tile != 0 && tile != 2) {
                            nextGamePos = gamePos + currentDir;
                            playerPos = playerPos + DIRECTIONS[direction];
                            moving = 1;
                            hour = hour==239? 0 : hour + 1;

                            if (tile == 1)
                                stepCounter++;
                        }
                    }

                    else if (IsKeyDown(KEY_S)){
                        tile = pixelArray[(int)(playerPos.y - DIRECTIONS[direction].y)][(int)(playerPos.x - DIRECTIONS[direction].x)];
                        if (tile != 0 && tile != 2) {
                            nextGamePos = gamePos - currentDir;
                            playerPos = playerPos - DIRECTIONS[direction];
                            moving = -1;
                            hour = hour==239? 0 : hour + 1;

                            if (tile == 1)
                                stepCounter++;

                        }
                    }
                }

                if (IsKeyPressed(KEY_ENTER) && tile == 2) {
                    exit = true;
                }

                if (moving!=0){
                    gamePos = gamePos + ((moving==1? currentDir : currentDir*-1) * MOVE_SPEED);
                    
                    if (hour==180) {
                        if (int(r) != 26) r = (r > 26)? r - 12.06 : 26;
                        if (int(g) != 78) g = (g > 78)? g - 9.32 : 78;
                        if (int(b) != 129) b = (b > 129)? b - 6.64 : 129;
                    }

                    if (hour==60) {
                        if (int(r) != 255) r = (r < 255)? r + 12.06 : 255;
                        if (int(g) != 255) g = (g < 255)? g + 9.32 : 255;
                        if (int(b) != 255) b = (b < 255)? b + 6.64 : 255;
                    }

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
                
                //230 215 130
                
                BeginMode3D(camera);
                    DrawModel(model, mapPosition, 1.0f, {(unsigned char)r, (unsigned char)g, (unsigned char)b, 255}); // Dibujar mapa del laberinto
                EndMode3D();

                    DrawClock();
                if (entrance) {
                    fadeEntrance -= 10;
                    if(fadeEntrance < 0) {fadeEntrance = 0; entrance = false;}
                    FillWindow((Color){0,0,0}, fadeEntrance);
                }

                if(exit) {
                    StopSound(sound);
                    fadeEntrance += 10;
                    if(fadeEntrance > 255) {fadeEntrance = 255; entrance = true; exit = false; currentState = MENU;}
                    
                    FillWindow((Color){0,0,0}, fadeEntrance);
                }

                if (battle) {
                    deltaBattle += 2.5f;
                    fade = (fade+5.5>255)?255:fade+5.5;
                    if (!IsTextureReady(ramas)) ramas = LoadTexture("resources/ramas.png");
                    DrawTextureEx(ramas, (Vector2){-ramas.width/8.0f, ramas.height/8.0f}, 90-deltaBattle, 1.0f + float(FULLSCREEN), WHITE);
                    DrawTextureEx(ramas, (Vector2){-ramas.width/8.0f, GetScreenHeight()+ramas.height/2.0f}, -deltaBattle, 1.0f + float(FULLSCREEN), WHITE);
                    DrawTextureEx(ramas, (Vector2){GetScreenWidth()+ramas.width/8.0f, ramas.height/8.0f}, deltaBattle, 1.0f + float(FULLSCREEN), WHITE);
                    DrawTextureEx(ramas, (Vector2){GetScreenWidth()+ramas.width/8.0f, GetScreenHeight()+ramas.height/2.0f}, 90+deltaBattle, 1.0f + float(FULLSCREEN), WHITE);
                    
                    FillWindow((Color){0,0,0}, fade);
                    
                    //EXIT MAZE - ENTER COMBAT
                    if(deltaBattle >= 160.0f){
                        StopSound(sound);
                        monsterFade = 0.0f;
                        deltaMonster = 0.0f;
                        deltaBattle = 0.0f;
                        battle = false;

                        heigh = 0.9f;
                        index = 0;
                        action = 0;
                        partyIndex = 0;

                        order.fullReset();
                        battleExperience = 0;
                        GenerateEncounter(&encounter);
                        currentState = COMBAT;
                    }
                }

                DrawTextureEx(cubicmap, (Vector2){GetScreenWidth() - cubicmap.width * 4.0f - 20, 20.0f}, 0.0f, 4.0f, WHITE);
                DrawRectangleLines(GetScreenWidth() - cubicmap.width * 4 - 20, 20, cubicmap.width * 4, cubicmap.height * 4, GREEN);

                // Dibujar posición del jugador en el radar
                DrawRectangle(GetScreenWidth() - cubicmap.width * 4 - 20 + playerCellX * 4, 20 + playerCellY * 4, 4, 4, RED);
                
                //lineas para probar
                //DrawText(TextFormat("Encounter EXP: (%g)", battleExperience), 10, 10, 20, DARKGRAY);

                //DrawText(TextFormat("1 EXP: (%d)", team[0]->GetEXP()), 10, 30, 20, DARKGRAY);
                //DrawText(TextFormat("2 EXP: (%d)", team[2]->GetEXP()), 10, 50, 20, DARKGRAY);
                //DrawText(TextFormat("3 EXP: (%d)", team[3]->GetEXP()), 10, 70, 20, DARKGRAY);
                //DrawText(TextFormat("4 EXP: (%d)", team[4]->GetEXP()), 10, 90, 20, DARKGRAY);
                //DrawText(TextFormat("5 EXP: (%d)", team[5]->GetEXP()), 10, 110, 20, DARKGRAY);

                //DrawText(TextFormat("Game Position: (%.2f, %.2f)", gamePos.x, gamePos.y), 10, 70, 20, DARKGRAY);
                //DrawText(TextFormat("Player Position: (%.2f, %.2f)", playerPos.x, playerPos.y), 10, 30, 20, DARKGRAY);
                //DrawText(TextFormat("CurrentDir: (%.2f, %.2f)", currentDir.x, currentDir.y), 10, 50, 20, DARKGRAY);
                //DrawText(TextFormat("Array: (%d)", pixelArray[(int)playerPos.y][(int)playerPos.x]), 10, 70, 20, DARKGRAY);
                //DrawText(TextFormat("Direction: (%d)", direction), 10, 90, 20, DARKGRAY);
/*
                DrawText(TextFormat("CounterStep: (%d)", stepCounter), 10, 30, 20, DARKGRAY);
                DrawText(TextFormat("Encounter: (%d)", encounter_steps), 10, 50, 20, DARKGRAY);

                DrawText(TextFormat("Size: (%d), (%d)", GetMonitorWidth(GetCurrentMonitor()),GetMonitorHeight(GetCurrentMonitor())), 10, 70, 20, DARKGRAY);
                DrawText(TextFormat("Time: (%d)", hour/10), 10, 90, 20, DARKGRAY);
                DrawFPS(10, 10);
*/               
                EndDrawing();
                break;
            }

            case COMBAT: {
                camera.target = Vector3{17.0f,0.0,5.0f};
                camera.fovy = 70.0f;
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
                }
                

                BeginDrawing();
                    ClearBackground(SKY);
                BeginMode3D(camera);
                    DrawModel(model, mapPosition, 1.0f, {(unsigned char)r, (unsigned char)g, (unsigned char)b, 255}); // Dibujar mapa del laberinto
                EndMode3D();

                    DrawClock();
                    DrawTurn();
                    DrawMessages();
                    DrawTextMessage();

                DrawEncounter(&encounter, camera, deltaMonster, monsterFade);

                switch(action){
                    default: action = 0;
                    case 0: DrawUI(&index, &currentState); break;
                    case 1: ActionAttack(&encounter, &index); break;
                    case 3: ActionSkill(team[partyIndex]); break;
                    case 4: ActionItem(&index); break;
                    case 9: ExecuteTurn(&currentState); break;
                }
                
                DrawBoxes();

                if (heigh > 0.4f)
                    FillWindow((Color){0,0,0}, fade);
                //DrawTextureEx(cubicmap, (Vector2){GetScreenWidth() - cubicmap.width * 4.0f - 20, 20.0f}, 0.0f, 4.0f, WHITE);
                //DrawText(TextFormat("NUMERO: (%d)", monsterNum), 10, 50, 20, DARKGRAY);
                //DrawText(TextFormat("STACK: (%d)", orderStack.size()), 10, 30, 20, DARKGRAY);
                //DrawText(TextFormat("QUEUE: (%d)", order.size()), 10, 50, 20, DARKGRAY);
                //DrawFPS(10, 10);
                EndDrawing();
                break;
            }

            case ESCAPE: {
                static int state = 1;
                if(state == 1) {
                    ExperienceFade += 10;
                    if(ExperienceFade > 255) {ExperienceFade = 255; state = 2;};
                    FillWindow((Color){0,0,0}, ExperienceFade);
                }

                else if (state == 2){
                    camera.fovy = 65.0f;
                    PlaySound(sound);
                    Vector2 currentDir = {(float)cos(currentAngle), (float)sin(currentAngle)};
                    camera.target = gameToWorld(gamePos + currentDir);
                    camera.position = gameToWorld(gamePos);

                    ClearBackground(SKY);

                    BeginMode3D(camera);
                        DrawModel(model, mapPosition, 1.0f, {(unsigned char)r, (unsigned char)g, (unsigned char)b, 255}); // Dibujar mapa del laberinto
                    EndMode3D();
                    DrawClock();

                    ExperienceFade -= 10;
                    if(ExperienceFade < 0) {ExperienceFade = 0; state = 3;}
                    FillWindow((Color){0,0,0}, ExperienceFade);
                }

                else if (state == 3) {
                    currentState = MAZE;
                    state = 1;
                }

                EndDrawing();
                break;
            }

            case EXPERIENCE: {
                static int state = 1;
                BeginDrawing();
                if(state == 1) {
                    ExperienceFade += 10;
                    if(ExperienceFade > 255) {ExperienceFade = 255; state = 2;};
                    FillWindow((Color){0,0,0}, ExperienceFade);
                    }

                else if (state == 2) {
                    DrawTextureEx(BattleResults, Vector2{0,0}, 0, scale, WHITE);
                    DrawTextEx(names, "EXP", Vector2{649, 48}*scale, 22 * scale, 0, {24,207,206,255});
                    DrawTextEx(names, std::to_string((int)battleExperience).c_str(), Vector2{781, 45} * scale, 25 * scale, 0, {186,255,255,255});
                    DrawTextEx(names, "FOUND", Vector2{125, 396}*scale, 23*scale ,0, {24,207,206,255});

                    for (int i = 0, j = 0; i < 6; i++){
                        if(team[i] != NULL){
                            DrawExpPortrait(j, i);
                            j++;
                        }
                    }

                    ExperienceFade -= 10;
                    if(ExperienceFade < 0) {ExperienceFade = 0; state = 3;}
                    FillWindow((Color){0,0,0}, ExperienceFade);
                }

                else if (state == 3) {
                    DrawTextureEx(BattleResults, Vector2{0,0}, 0, scale, WHITE);
                    DrawTextEx(names, "EXP", Vector2{649, 48}*scale, 22 * scale, 0, {24,207,206,255});
                    DrawTextEx(names, std::to_string((int)battleExperience).c_str(), Vector2{781, 45} * scale, 25 * scale, 0, {186,255,255,255});
                    DrawTextEx(names, "FOUND", Vector2{125, 396}*scale, 23*scale ,0, {24,207,206,255});

                    for (int i = 0, j = 0; i < 6; i++){
                        if(team[i] != NULL){
                            DrawExpPortrait(j, i);
                            j++;
                        }
                    }

                    if(IsKeyPressed(KEY_ENTER)){
                        state = 4;
                        int alive = 0;
                        for(int i = 0; i < 6; i++){
                            if (team[i] != NULL && team[i]->isAlive()){
                                alive++;
                            }
                        }
                        
                        for(int i = 0; i < 6; i++){
                            if (team[i] != NULL && team[i]->isAlive()){
                                team[i]->GainExp(round(battleExperience/alive));
                            }
                        }
                    }
                }

                else if (state == 4) {
                    DrawTextureEx(BattleResults, Vector2{0,0}, 0, scale, WHITE);
                    DrawTextEx(names, "EXP", Vector2{649, 48}*scale, 22 * scale, 0, {24,207,206,255});
                    DrawTextEx(names, std::to_string((int)battleExperience).c_str(), Vector2{781, 45} * scale, 25 * scale, 0, {186,255,255,255});
                    DrawTextEx(names, "FOUND", Vector2{125, 396}*scale, 23*scale ,0, {24,207,206,255});

                    for (int i = 0, j = 0; i < 6; i++){
                        if(team[i] != NULL){
                            DrawExpPortrait(j, i);
                            j++;
                        }
                    }

                    if(IsKeyPressed(KEY_ENTER)){
                        state = 5;
                    }
                }

                else if (state == 5) {
                    ExperienceFade += 10;
                    if(ExperienceFade > 255) {ExperienceFade = 255; state = 6;}
                    FillWindow((Color){0,0,0}, ExperienceFade);
                }

                else if (state == 6) {
                    camera.fovy = 65.0f;
                    PlaySound(sound);
                    Vector2 currentDir = {(float)cos(currentAngle), (float)sin(currentAngle)};
                    camera.target = gameToWorld(gamePos + currentDir);
                    camera.position = gameToWorld(gamePos);

                    ClearBackground(SKY);

                    BeginMode3D(camera);
                        DrawModel(model, mapPosition, 1.0f, {(unsigned char)r, (unsigned char)g, (unsigned char)b, 255}); // Dibujar mapa del laberinto
                    EndMode3D();
                    DrawClock();

                    ExperienceFade -= 10;
                    if(ExperienceFade < 0) {ExperienceFade = 0; state = 7;}
                    FillWindow((Color){0,0,0}, ExperienceFade);
                }

                else if (state == 7) {
                    currentState = MAZE;
                    state = 1;
                }

                //DrawText(TextFormat("STACK: (%d)", orderStack.size()), 10, 30, 20, DARKGRAY);
                //DrawText(TextFormat("QUEUE: (%d)", order.size()), 10, 50, 20, DARKGRAY);
                EndDrawing();
                break;
            }

            case MENU: {
                BeginDrawing();
                    ClearBackground(RAYWHITE);
                    switch(menuAction) {
                        case 0: DrawCityUI(&currentState); break;
                        case 1: DrawRefugeUI(); break;
                        case 2: DrawBazarUI(); break;
                        case 3: DrawBarUI(); break;
                        case 4: DrawChamberUI(); break;
                        case 5: DrawHallUI(); break;
                        case 6: DrawPostUI(); break;

                    }
                EndDrawing();
                break;
            }
        }
        // Renderizado

        
    }

    // Desinicialización
    UnloadImageColors(mapPixels);
    UnloadTexture(cubicmap);
    UnloadModel(model);
    UnloadResources();

    CloseWindow();

    return 0;
}
