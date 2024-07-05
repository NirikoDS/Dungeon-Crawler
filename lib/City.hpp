int menuAction = 0;

void DrawCityUI(GameState* currentState){
    static Texture2D UIborder;
    if (!IsTextureReady(UIborder)) UIborder = LoadTexture("resources/UIselector/scenarios/cityUI.png");
    static Texture2D background;
    if (!IsTextureReady(background)) background = LoadTexture("resources/Scenarios/backgrounds/city.png");

    static Texture2D title;
    if (!IsTextureReady(title)) title = LoadTexture("resources/title.png");

    static int index = 1;
    static int state = 0;
    static int fade = 255;
    static bool entrance = true;


    static Sound selectionSound;
    if (!IsSoundReady(selectionSound)) {selectionSound = LoadSound("resources/selection.wav"); SetSoundVolume(selectionSound, 0.5f);}

    if (IsKeyPressed(KEY_W) && state == 0 ){
        PlaySound(selectionSound);
        index = index - 1 == -1 ? 7 : index - 1 == 0 ? 7 : index - 1;
    }

    else if (IsKeyPressed(KEY_S) && state == 0 ){
        PlaySound(selectionSound);
        index = index + 1 == 8 ? 1 : index + 1;
    }
    
    DrawTextureEx(background, (Vector2){0, 0}, 0.0f, scale, WHITE);

    DrawRectangle(137*scale, 120*scale, 328 * scale, 37 * scale, (index==1)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(152*scale, 159*scale, 321 * scale, 37 * scale, (index==2)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(159*scale, 198*scale, 318 * scale, 37 * scale, (index==3)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(162*scale, 237*scale, 315 * scale, 37 * scale, (index==4)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(159*scale, 276*scale, 318 * scale, 37 * scale, (index==5)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(152*scale, 315*scale, 321 * scale, 37 * scale, (index==6)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(137*scale, 354*scale, 328 * scale, 37 * scale, (index==7)?Color{83,209,235,166}:(Color){66,139,187,166});

    DrawTextureEx(CityTextures[index-1], (Vector2){0, 113.0f*scale}, 0.0f, scale, WHITE);
    DrawTextureEx(UIborder, (Vector2){0, 113.0f * scale}, 0.0f, scale, WHITE);
    DrawTextureEx(title, (Vector2){535, 180} * scale, 0.0f, scale, WHITE);
    DrawClock();
    DrawMessages();
    DrawTextMessage();
    DrawBoxes();

    switch(index){
        case 1: ChangeMessage("Rest and recover", "your strength."); break; // Eternal Refuge
        case 2: ChangeMessage("Buy and sell", "various items."); break; // Bazaar of Secrets
        case 3: ChangeMessage("Gather information", "and accept quests."); break; // Minotaur's Breath
        case 4: ChangeMessage("Learn about the lore", "and get main quests."); break; // Knowledge Chamber
        case 5: ChangeMessage("Recruit adventurers", "and form your team."); break; // Hall of Heroes
        case 6: ChangeMessage("Buy medicine and", "revive fallen mates."); break; // Supply Post
        case 7: ChangeMessage("Enter the labyrinth", "and explore."); break; // Gate of Oblivion
    }

    for (int i = 0; i < 7; i++) {
        if (i + 1 == index) DrawTextEx(font, TextFormat(CityOptions[i]), (Vector2){180.0f, 121.0f + 39.0f * i} * scale, 37 * scale, scale, (Color){0, 0, 102, 230});
        else DrawTextEx(font, TextFormat(CityOptions[i]), (Vector2){180.0f, 121.0f + 39.0f * i} * scale, 37 * scale, scale, (Color){255, 255, 255, 230});
    }

    if (entrance) {
        fade -= 20;
        if(fade < 0) {fade = 0; entrance = false;};
        FillWindow((Color){0,0,0}, fade);
    }

    if (IsKeyPressed(KEY_ENTER) && state == 0){
        state = 1;
    }
    else if(state == 1) {
        fade += 20;
        if(fade > 255) {fade = 255; state = 0; menuAction = (index == 7)?0:index; entrance = true; if (index == 7) *currentState = MAZE;}
        
        FillWindow((Color){0,0,0}, fade);
    }

}

void DrawRefugeUI(){
    static Texture2D background;
    static bool entrance = true;
    static int fade = 255;
    static int index = 1;
    static int state = 0;

    const char* RefugeOptions[] = {
    "Sleep",
    "Nap",
    "Talk",
    "Save",
    "Leave",
    };

    static Texture2D UIborder;
    if (!IsTextureReady(UIborder)) UIborder = LoadTexture("resources/UIselector/UIfive.png");
    if (!IsTextureReady(background)) background = LoadTexture("resources/Scenarios/backgrounds/dorm.png");

    static Sound selectionSound;
    if (!IsSoundReady(selectionSound)) {selectionSound = LoadSound("resources/selection.wav"); SetSoundVolume(selectionSound, 0.5f);}

    if (IsKeyPressed(KEY_W) && state == 0 ){
        PlaySound(selectionSound);
        index = index - 1 == -1 ? 5 : index - 1 == 0 ? 5 : index - 1;
    }

    else if (IsKeyPressed(KEY_S) && state == 0 ){
        PlaySound(selectionSound);
        index = index + 1 == 6 ? 1 : index + 1;
    }
    

    DrawTextureEx(background, (Vector2){0, 0}, 0.0f, scale, WHITE);
    
    DrawRectangle(0, 159*scale, 291 * scale, 37 * scale, (index==1)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(0, 198*scale, 293 * scale, 37 * scale, (index==2)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(0, 237*scale, 295 * scale, 37 * scale, (index==3)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(0, 276*scale, 293 * scale, 37 * scale, (index==4)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(0, 315*scale, 291 * scale, 37 * scale, (index==5)?Color{83,209,235,166}:(Color){66,139,187,166});

    DrawTextureEx(UIborder, (Vector2){0, 113.0f * scale}, 0.0f, scale, WHITE);

    DrawClock();
    DrawMessages();
    DrawTextMessage();
    DrawBoxes();

    switch(index){
        case 1: ChangeMessage("Rest until the next day.", "Recover HP and TP."); break;
        case 2: ChangeMessage("Rest until night.", "Recover HP and TP."); break; 
        case 3: ChangeMessage("Talk to the owner.", ""); break; 
        case 4: ChangeMessage("Save your progress.", ""); break; 
        case 5: ChangeMessage("Leave the refuge.", ""); break; 
    }

    for (int i = 0; i < 5; i++) {
        if (i + 1 == index) DrawTextEx(font, TextFormat(RefugeOptions[i]), (Vector2){50.0f, 160.0f + 39.0f * i} * scale, 37 * scale, scale, (Color){0, 0, 102, 230});
        else DrawTextEx(font, TextFormat(RefugeOptions[i]), (Vector2){50.0f, 160.0f + 39.0f * i} * scale, 37 * scale, scale, (Color){255, 255, 255, 230});
    }

    if (entrance) {
        fade -= 20;
        if(fade < 0) {fade = 0; entrance = false;};
        FillWindow((Color){0,0,0}, fade);

        if(fade == 0) fade = 255;
    }

    if (IsKeyPressed(KEY_BACKSPACE) && state == 0){
        state = 1;
        fade = 0;
    }
    else if(state == 1) {
        fade += 20;
        if(fade > 255) {fade = 255; entrance = true; state = 0; menuAction = 0;}
        FillWindow((Color){0,0,0}, fade);
    }

}

void DrawBazarUI(){
    static Texture2D background;
    static bool entrance = true;
    static int fade = 255;

    if (!IsTextureReady(background)) background = LoadTexture("resources/Scenarios/backgrounds/bazar.png");

    DrawTextureEx(background, (Vector2){0, 0}, 0.0f, scale, WHITE);

    DrawClock();
    DrawMessages();
    DrawTextMessage();
    DrawBoxes();

    if (entrance) {
        fade -= 20;
        if(fade < 0) {fade = 0; entrance = false;};
        FillWindow((Color){0,0,0}, fade);

        if(fade == 0) fade = 255;
    }

    static int state = 0;
    if (IsKeyPressed(KEY_BACKSPACE) && state == 0){
        state = 1;
        fade = 0;
    }
    else if(state == 1) {
        fade += 20;
        if(fade > 255) {fade = 255; entrance = true; state = 0; menuAction = 0;}
        FillWindow((Color){0,0,0}, fade);
    }

}

void DrawBarUI(){
    static Texture2D background;
    static bool entrance = true;
    static int fade = 255;

    if (!IsTextureReady(background)) background = LoadTexture("resources/Scenarios/backgrounds/bar.png");

    DrawTextureEx(background, (Vector2){0, 0}, 0.0f, scale, WHITE);

    DrawClock();
    DrawMessages();
    DrawTextMessage();
    DrawBoxes();

    if (entrance) {
        fade -= 20;
        if(fade < 0) {fade = 0; entrance = false;};
        FillWindow((Color){0,0,0}, fade);

        if(fade == 0) fade = 255;
    }

    static int state = 0;
    if (IsKeyPressed(KEY_BACKSPACE) && state == 0){
        state = 1;
        fade = 0;
    }
    else if(state == 1) {
        fade += 20;
        if(fade > 255) {fade = 255; entrance = true; state = 0; menuAction = 0;}
        FillWindow((Color){0,0,0}, fade);
    }
}

void DrawChamberUI(){
    static Texture2D background;
    static bool entrance = true;
    static int fade = 255;

    if (!IsTextureReady(background)) background = LoadTexture("resources/Scenarios/backgrounds/guild.png");

    DrawTextureEx(background, (Vector2){0, 0}, 0.0f, scale, WHITE);

    DrawClock();
    DrawMessages();
    DrawTextMessage();
    DrawBoxes();

    if (entrance) {
        fade -= 20;
        if(fade < 0) {fade = 0; entrance = false;};
        FillWindow((Color){0,0,0}, fade);

        if(fade == 0) fade = 255;
    }

    static int state = 0;
    if (IsKeyPressed(KEY_BACKSPACE) && state == 0){
        state = 1;
        fade = 0;
    }
    else if(state == 1) {
        fade += 20;
        if(fade > 255) {fade = 255; entrance = true; state = 0; menuAction = 0;}
        FillWindow((Color){0,0,0}, fade);
    }

}

void DrawHallUI(){
    static Texture2D background;
    static bool entrance = true;
    static int fade = 255;
    static int index = 1;
    static int state = 0;

    const char* RefugeOptions[] = {
    "Register",
    "Organize",
    "Talk",
    "Formation",
    "Leave",
    };

    static Texture2D UIborder;
    if (!IsTextureReady(UIborder)) UIborder = LoadTexture("resources/UIselector/UIfive.png");
    if (!IsTextureReady(background)) background = LoadTexture("resources/Scenarios/backgrounds/guildMaster.png");

    static Sound selectionSound;
    if (!IsSoundReady(selectionSound)) {selectionSound = LoadSound("resources/selection.wav"); SetSoundVolume(selectionSound, 0.5f);}

    if (IsKeyPressed(KEY_W) && state == 0 ){
        PlaySound(selectionSound);
        index = index - 1 == -1 ? 5 : index - 1 == 0 ? 5 : index - 1;
    }

    else if (IsKeyPressed(KEY_S) && state == 0 ){
        PlaySound(selectionSound);
        index = index + 1 == 6 ? 1 : index + 1;
    }
    

    DrawTextureEx(background, (Vector2){0, 0}, 0.0f, scale, WHITE);
    
    DrawRectangle(0, 159*scale, 291 * scale, 37 * scale, (index==1)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(0, 198*scale, 293 * scale, 37 * scale, (index==2)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(0, 237*scale, 295 * scale, 37 * scale, (index==3)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(0, 276*scale, 293 * scale, 37 * scale, (index==4)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(0, 315*scale, 291 * scale, 37 * scale, (index==5)?Color{83,209,235,166}:(Color){66,139,187,166});

    DrawTextureEx(UIborder, (Vector2){0, 113.0f * scale}, 0.0f, scale, WHITE);

    DrawClock();
    DrawMessages();
    DrawTextMessage();
    DrawBoxes();

    switch(index){
        case 1: ChangeMessage("Register new adventurers", "in your guild."); break;
        case 2: ChangeMessage("Organize your guild.", ""); break; 
        case 3: ChangeMessage("Talk to the Master of Guilds.", ""); break; 
        case 4: ChangeMessage("Make your formation", "for battle."); break; 
        case 5: ChangeMessage("Leave the refuge.", ""); break; 
    }

    for (int i = 0; i < 5; i++) {
        if (i + 1 == index) DrawTextEx(font, TextFormat(RefugeOptions[i]), (Vector2){50.0f, 160.0f + 39.0f * i} * scale, 37 * scale, scale, (Color){0, 0, 102, 230});
        else DrawTextEx(font, TextFormat(RefugeOptions[i]), (Vector2){50.0f, 160.0f + 39.0f * i} * scale, 37 * scale, scale, (Color){255, 255, 255, 230});
    }

    if (entrance) {
        fade -= 20;
        if(fade < 0) {fade = 0; entrance = false;};
        FillWindow((Color){0,0,0}, fade);

        if(fade == 0) fade = 255;
    }

    if (IsKeyPressed(KEY_BACKSPACE) && state == 0){
        state = 1;
        fade = 0;
    }
    else if(state == 1) {
        fade += 20;
        if(fade > 255) {fade = 255; entrance = true; state = 0; menuAction = 0;}
        FillWindow((Color){0,0,0}, fade);
    }

}

void DrawPostUI(){
    static Texture2D background;
    static bool entrance = true;
    static int fade = 255;

    if (!IsTextureReady(background)) background = LoadTexture("resources/Scenarios/backgrounds/maze.png");

    DrawTextureEx(background, (Vector2){0, 0}, 0.0f, scale, WHITE);

    DrawClock();
    DrawMessages();
    DrawTextMessage();
    DrawBoxes();

    if (entrance) {
        fade -= 20;
        if(fade < 0) {fade = 0; entrance = false;};
        FillWindow((Color){0,0,0}, fade);

        if(fade == 0) fade = 255;
    }

    static int state = 0;
    if (IsKeyPressed(KEY_BACKSPACE) && state == 0){
        state = 1;
        fade = 0;
    }
    else if(state == 1) {
        fade += 20;
        if(fade > 255) {fade = 255; entrance = true; state = 0; menuAction = 0;}
        FillWindow((Color){0,0,0}, fade);
    }
}
