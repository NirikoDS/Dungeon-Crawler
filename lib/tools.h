#include <functional>
#include <vector>
#include <algorithm>
#include <fstream>

// Definitions
#define HEIGHT_MONSTER 100.0f
#define BLOCK_SIZE 1.0f     // Size of Block
#define ROTATION_SPEED 4.0f // Velocidad de rotación en radianes por segundo
#define MOVE_SPEED 0.045f    // Velocity of movement
#define SKY (Color){187,234,244} // Color of the background

#define ENCOUNTER_MIN 7
#define ENCOUNTER_MAX 10

enum ItemType {
    USABLE,
    EQUIPMENT,
    KEY,
    MATERIAL
};

enum CLASSES {
    WARRIOR
};

struct Item {
    std::string name;
    std::string description;
    int value;
    ItemType type;
};

#define InventoryMaxSize 60
std::vector <Item> Inventory;

bool CompareItems(const Item &a, const Item &b) {
    if (a.type == b.type) {
        return a.value > b.value;
    }
    return a.type < b.type;
}

void SortInventory() {
    std::sort(Inventory.begin(), Inventory.end(), CompareItems);
}

void RemoveInventory(const std::string& itemName) {
    auto it = std::find_if(Inventory.begin(), Inventory.end(), [&itemName](const Item& item) {
        return item.name == itemName;
    });

    // Remove the first matching item
    Inventory.erase(it);
}

// Define the Turns structure
struct Turns {
    std::function<void()> Action; // No argument needed here
    Turns* Next;
    int speed;
};

struct Skill {
    std::string name;
    std::string definition;
    int cost;
};

int monsterNum = 0;
int turnsCount = 0;
int partyIndex = 0;
int actionCount = 1;
std::string message1, message2;

int CalculateDamageSTR(float ATK, float DEF);
void Dying(std::string name);
void DrawTextMessage();

class TurnsStack {
    private:
        std::vector <Turns*> stack;
    
    public:

        void push(Turns* turn) {
            if (stack.size() < 5) {
                stack.push_back(turn);
            }
        }

        void pop() {
            if (!stack.empty()) {
                stack.pop_back();
            }
        }

        Turns* top() const {
            if (!stack.empty()) {
                return stack.back();
            } 
            else return nullptr;
        }

        bool isEmpty() const {
            return stack.empty();
        }

        int size() const {
            return stack.size();
        }
};

TurnsStack orderStack;

// Define the Order class to manage the Turns linked list
class TurnOrder {
private:
    Turns* head;

public:
    TurnOrder() : head(nullptr) {}

    void insert(Turns* newTurn) {
        if (!head || newTurn->speed > head->speed) {
            newTurn->Next = head;
            head = newTurn;
        } else {
            Turns* current = head;
            while (current->Next && current->Next->speed >= newTurn->speed) {
                current = current->Next;
            }
            newTurn->Next = current->Next;
            current->Next = newTurn;
        }
    }

    void insertAtBeginning(Turns* newTurn) {
        newTurn->Next = head->Next;
        head->Next = newTurn;
    }

    void execute() {
        if (monsterNum == 0) return;
        if (head == nullptr) return;
        head->Action();  // Execute the action
        head = head->Next;
    }

    void reset(){
        if (head) head->Next = nullptr;
    }

    void fullReset(){
        if (head) head = nullptr;
    }

    bool isEmpty(){
        return (head == nullptr);
    }

    int size() {
        int count = 0;
        Turns* current = head;
        while (current) {
            count++;
            current = current->Next;
        }
        return count;
    }
    
    ~TurnOrder() {
        while (head) {
            Turns* temp = head;
            head = head->Next;
            delete temp;
        }
    }
};

// Global order instance
TurnOrder order;

// Global Variables
bool FULLSCREEN = false;
int scale = 1;

float battleExperience = 0;

Texture2D Textures[3];
Texture2D UI_Swordsman[5];
Texture2D EXP_Swordsman[5];
Texture2D CityTextures[7];

Texture2D portrait;
Texture2D selectedPortrait;

class Adventurer;

Adventurer* team[6] = { NULL };
Adventurer* guild[20] = { NULL };
int money = 0;

std::vector <Adventurer*> frontline;
std::vector <Adventurer*> backline;

void UseObject(std::string name, Adventurer* target, std::string objectName);

Font font;
Font numbers;
Font names;

int action = 0;

int hour = 80;

void Die(int num);

// Enums
enum Damages {
    SLASH,
    PIERCE,
    SMASH,
    FIRE,
    ICE,
    ELECTRIC
};

enum Monsters {
    CHIMON,
    SLAIER,
    CATERLY
};

enum Direction {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};

enum GameState {
    MENU,
    MAZE,
    COMBAT,
    EXPERIENCE,
    ESCAPE
};

const char* Options[] = {
    "ATTACK",
    "DEFEND",
    "SKILL",
    "ITEM",
    "BURST",
    "SWITCH",
    "ESCAPE"
};

const char* CityOptions[] = {
    "Eternal Refuge",
    "Bazaar of Secrets",
    "Minotaur's Breath",
    "Knowledge Chamber",
    "Hall of Heroes",
    "Supply Post",
    "Gate of Oblivion"
};

void ActionEscape(GameState* State);
// Utility Functions
float AngleDifference(float target, float current) {
    float diff = target - current;
    while (diff > PI) diff -= 2.0f * PI;
    while (diff < -PI) diff += 2.0f * PI;
    return diff;
}

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

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
    return (Vector3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

Vector3 gameToWorld(Vector2 gamePos) {
    return (Vector3){gamePos.x, 0.4f, gamePos.y};
}

void FillWindow(Color color, float fade) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight() + (float)FULLSCREEN * 100, (Color){color.r, color.g, color.b, (unsigned char)fade});
}

void LoadResources() {
    Textures[0] = LoadTexture("resources/chimon.png");
    Textures[1] = LoadTexture("resources/slaier.png");
    Textures[2] = LoadTexture("resources/caterly.png");
    font = LoadFont("resources/font/dungeon.otf");
    numbers = LoadFont("resources/font/numbers.ttf");
    names = LoadFont("resources/font/names.ttf");
    UI_Swordsman[0] = LoadTexture("resources/Adventurers/Swordsman/UI_Swordsman1.png");
    UI_Swordsman[1] = LoadTexture("resources/Adventurers/Swordsman/UI_Swordsman2.png");
    UI_Swordsman[2] = LoadTexture("resources/Adventurers/Swordsman/UI_Swordsman3.png");
    UI_Swordsman[3] = LoadTexture("resources/Adventurers/Swordsman/UI_Swordsman4.png");
    UI_Swordsman[4] = LoadTexture("resources/Adventurers/Swordsman/UI_Swordsman5.png");

    EXP_Swordsman[0] = LoadTexture("resources/Adventurers/Swordsman/ExpSwordsman1.png");
    EXP_Swordsman[1] = LoadTexture("resources/Adventurers/Swordsman/ExpSwordsman2.png");
    EXP_Swordsman[2] = LoadTexture("resources/Adventurers/Swordsman/ExpSwordsman3.png");
    EXP_Swordsman[3] = LoadTexture("resources/Adventurers/Swordsman/ExpSwordsman4.png");
    EXP_Swordsman[4] = LoadTexture("resources/Adventurers/Swordsman/ExpSwordsman5.png");

    CityTextures[0] = LoadTexture("resources/UIselector/scenarios/dorm.png");
    CityTextures[1] = LoadTexture("resources/UIselector/scenarios/bazar.png");
    CityTextures[2] = LoadTexture("resources/UIselector/scenarios/bar.png");
    CityTextures[3] = LoadTexture("resources/UIselector/scenarios/guilmaster.png");
    CityTextures[4] = LoadTexture("resources/UIselector/scenarios/guild.png");
    CityTextures[5] = LoadTexture("resources/UIselector/scenarios/frontline.png");
    CityTextures[6] = LoadTexture("resources/UIselector/scenarios/maze.png");

    portrait = LoadTexture("resources/Adventurers/portrait.png");
    selectedPortrait = LoadTexture("resources/Adventurers/SelectedPortrait.png");
}

void UnloadResources() {
    for (int i = 0; i < 3; i++)
        UnloadTexture(Textures[i]);
    UnloadFont(font);
}

void ChangeMessage(std::string m1, std::string m2){
    message1 = m1;
    message2 = m2;
}

void FillQueue();

bool HaveItems(){
    for (int i = 0; i < (int)Inventory.size(); i++){
        if (Inventory[i].type == USABLE) return true;
    }
    return false;
}
// Class Definitions
class Monster {
    protected:
        std::string name;
        int level;
        int maxHP;
        int HP;

        int STR;
        int AGI;
        int TEC;
        int VIT;

        int index;
        int resistances[6];

        int fade = 255;
        float height = 0;
        float pos;

        float defBoost = 1;
        float PhyAtkBoost = 1.0f;
        
        int experience;
    public:

        Texture image;
        
        Monster(std::string N, int L, int H, int A, int S, int T, int V, Texture I)
        : name(N), level(L), maxHP(H), STR(A), AGI(S), TEC(T), VIT(V),  image(I) {
            HP = maxHP;  
        }

        // Destructor
        ~Monster() {
            monsterNum--;
        }

        void Draw(float deltaMonster, float fade, float pos) {
            this->pos = GetScreenWidth()/2 + (pos + image.width/2) * scale;
            
            if (HP > 0)
            DrawTextureEx(
                image,
                (Vector2) {GetScreenWidth()/2 + pos * scale,
                           (float)GetScreenHeight()/3 - ((image.height * scale)/3) - (HEIGHT_MONSTER * scale) + (deltaMonster * scale)},
                           0.0f,
                           scale,
                           (Color){255, 255, 255, (unsigned char)fade}
            );

            else if(this->fade > 0){
                this->fade = this->fade-10>0 ? this->fade - 10 : 0;
                height += 6.5f;
                DrawTextureEx(
                image,
                (Vector2) {GetScreenWidth()/2 + pos * scale,
                           (float)GetScreenHeight()/3 - ((image.height - height) * scale)/3 - (HEIGHT_MONSTER * scale) + (deltaMonster * scale)},
                           0.0f,
                           scale,
                           (Color){255, 255, 255, (unsigned char)this->fade}
                );
            }

            else Die(index);
        }

        void DrawUI(){
            DrawRectangle(pos - 53 * scale, 123 * scale, 106 * scale, 16 * scale, GRAY);
            DrawRectangle(pos - 50 * scale, 126 * scale, 100 * scale, 10 * scale, DARKGRAY);
            DrawRectangle(pos - 50 * scale, 126 * scale, (float(HP)/float(maxHP))*100 * scale, 10 * scale, GREEN);
        }

        void DrawTarget(){
            static Texture2D target;
            static float rotation;
            if (!IsTextureReady(target)) target = LoadTexture("resources/UIselector/target.png");

            DrawTextureEx(target, Vector2{pos, (270.0f) * scale } , rotation - 90, scale, WHITE);
            DrawTextureEx(target, Vector2{pos, (270.0f) * scale } , rotation, scale, WHITE);
            DrawTextureEx(target, Vector2{pos, (270.0f) * scale } , rotation + 90, scale, WHITE);
            DrawTextureEx(target, Vector2{pos, (270.0f) * scale } , rotation + 180, scale, WHITE);
            rotation = (rotation >= 90)? 0 : rotation + 0.75f;

        }

        bool isAlive() {
            return HP > 0;
        }

        int GetAGI(){
            return AGI;
        }

        void SetIndex(){
            index = monsterNum++;
        }

        int GetLevel(){
            return level;
        }

        std::string GetName(){
            return name;
        }

        float getElemRes(Damages type){
            return resistances[type];
        }
        
        int getStrDef(){
            return 0.50 * (VIT) * defBoost;
        }

        void Death() {
            HP = 0;
            battleExperience += experience;
        }

        //monster 0 bug
        void TakeDamage(int damage) {
            if(HP - damage <= 0){
                order.insertAtBeginning(new Turns{std::bind(&Monster::Death, this), nullptr, 0});
                order.insertAtBeginning(new Turns{std::bind(Dying, name), nullptr, 0});
            }
            else HP -= damage;
        }

        void attack();
        
};

class Chimon : public Monster {
    public:
        Chimon() : Monster("Chimon", 3, 10, 7, /*s*/7, 5, 5, Textures[CHIMON]) {
            experience = 3;
            
            resistances[SLASH] = 1.0f;
            resistances[PIERCE] = 1.0f;
            resistances[SMASH] = 1.0f;
            resistances[FIRE] = 1.0f;
            resistances[ICE] = 1.0f;
            resistances[ELECTRIC] = 1.0f;
        }
};

class Caterly : public Monster {
    public:
        Caterly() : Monster("Caterly", 4, 20, 7, /*s*/5, 5, 5, Textures[CATERLY]) {
            experience = 8;
            
            resistances[SLASH] = 1.0f;
            resistances[PIERCE] = 1.0f;
            resistances[SMASH] = 1.0f;
            resistances[FIRE] = 1.0f;
            resistances[ICE] = 1.0f;
            resistances[ELECTRIC] = 1.0f;
        }
};

class Slaier : public Monster {
    public:
        Slaier() : Monster("Slaier", 5, 20, 7, /*s*/3, 5, 5, Textures[SLAIER]) {
            experience = 7;
            
            resistances[SLASH] = 1.0f;
            resistances[PIERCE] = 1.0f;
            resistances[SMASH] = 1.0f;
            resistances[FIRE] = 1.0f;
            resistances[ICE] = 1.0f;
            resistances[ELECTRIC] = 1.0f;
        }
};

// encounter struct and functions
struct Encounter {
    int type;
    int monsterNumber;
    Monster* monsters[5];
};

Encounter encounter;

void Die(int index){
    delete encounter.monsters[index];
    encounter.monsters[index] = NULL;
};

void Dying(std::string name){
    std::cout<<name<<" die.\n";
    ChangeMessage(name + " die.", "");
}

void DrawEncounter(Encounter* encounter, Camera camera, float deltaMonster, float fade) {
    switch (encounter->type) { 
        case 0: 
            if(encounter->monsters[0] != NULL) encounter->monsters[0]->Draw(deltaMonster, fade, -encounter->monsters[0]->image.width / 2 - encounter->monsters[0]->image.width - 10);
            if(encounter->monsters[1] != NULL) encounter->monsters[1]->Draw(deltaMonster, fade, -encounter->monsters[1]->image.width / 2);
            if(encounter->monsters[2] != NULL) encounter->monsters[2]->Draw(deltaMonster, fade, -encounter->monsters[2]->image.width / 2 + encounter->monsters[2]->image.width + 10);
            break;
        case 1: 
            if(encounter->monsters[0] != NULL) encounter->monsters[0]->Draw(deltaMonster, fade, -encounter->monsters[0]->image.width / 2);
            break;
        case 2: 
            if(encounter->monsters[0] != NULL) encounter->monsters[0]->Draw(deltaMonster, fade, -encounter->monsters[0]->image.width - 5);
            if(encounter->monsters[1] != NULL) encounter->monsters[1]->Draw(deltaMonster, fade, +5);
            break;
        case 3: 
            if(encounter->monsters[0] != NULL) encounter->monsters[0]->Draw(deltaMonster, fade, -encounter->monsters[0]->image.width / 2);
            break;
        case 4: 
            if(encounter->monsters[0] != NULL) encounter->monsters[0]->Draw(deltaMonster, fade, -encounter->monsters[0]->image.width / 2 - encounter->monsters[0]->image.width - 10);
            if(encounter->monsters[1] != NULL) encounter->monsters[1]->Draw(deltaMonster, fade, -encounter->monsters[1]->image.width / 2);
            if(encounter->monsters[2] != NULL) encounter->monsters[2]->Draw(deltaMonster, fade, -encounter->monsters[2]->image.width / 2 + encounter->monsters[2]->image.width + 10);
            break;
    }
}

void GenerateEncounter(Encounter* encounter) {
    Encounter setups;
    
    //int random = rand() % 5;
    int random = 4;

    switch (random) {
        case 0: setups = Encounter{0, 3, {new Chimon(), new Chimon(), new Chimon(), NULL, NULL}}; break;
        case 1: setups = Encounter{1, 1, {new Chimon(), NULL, NULL, NULL, NULL}}; break;
        case 2: setups = Encounter{2, 2, {new Chimon(), new Caterly, NULL, NULL, NULL}}; break;
        case 3: setups = Encounter{3, 1, {new Caterly(), NULL, NULL, NULL, NULL}}; break;
        case 4: setups = Encounter{4, 3, {new Slaier(), new Chimon(), new Caterly(), NULL, NULL}}; break;
    }

    encounter->type = setups.type;
    encounter->monsterNumber = setups.monsterNumber;
    for (int i = 0; i < 5; i++) {
        encounter->monsters[i] = setups.monsters[i];
        if (encounter->monsters[i]!=NULL) encounter->monsters[i]->SetIndex();
    }

    turnsCount = 1;
}



// while (encounter.monsters[target] != NULL) target = target > 0 ? target - 1 : encounter.monsterNumber - 1;


// adventurer class and funtions
class Adventurer{
    protected:
        std::string name;
        CLASSES CLASS;

        int Level = 1;
        int nextLevelXP = 20;

        int maxHP;
        int HP;
        int maxTP;
        int TP;

        int STR;
        int VIT;
        int AGI;
        int TEC;

        int BurstPoints = 0;
        int SkillPoints = 3;

        int skills[10] = { 0 };
        std::vector<Skill> skillList;

        float PhyAtkBoost = 1;
        float defBoost = 1;
        Damages Elem;

        Texture2D selfPortait = portrait;

    public:
        Texture2D image;
        int imageIndex;

        Adventurer(int H, int M, int S, int V, int A, int T, Texture2D I, int im) : maxHP(H), maxTP(M), STR(S), VIT(V), AGI(A), TEC(T), image(I), imageIndex(im){
            HP = maxHP;
            TP = maxTP;
        }
        
        bool isAlive(){
            return HP > 0;
        }

        void Save(std::ofstream& outFile);

        void DrawUI(Vector2 pos){
            DrawTextureEx(image, pos, 0.0f, scale, WHITE);
        }

        int GetLevel(){
            return Level;
        }
        
        int GetEXP(){
            return nextLevelXP;
        }
        
        void GetHeal(int heal){
            HP += heal;
            if (HP>maxHP) HP = maxHP;
        }

        bool gotSkills(){ return skillList.size()>0;}

        bool gotBurst(){ return BurstPoints==100;}

        int getAGI() {return AGI;}

        float GetLifePercentage() {
            return (float(HP) / maxHP);
        }

        float GetManaPercentage() {
            return (float(TP) / maxTP);
        }

        float GetLevelPercentage() {
            int full = int(20 * (pow(1.4 , Level-1)));
            return (float(full - nextLevelXP) / full);
        }
        
        void TakeDamage(int damage) {
            HP -= damage;
        }

        std::string GetName(){
            return name;
        }

        Texture2D GetPortrait(){
            return selfPortait;
        }

        void SetPortrait(Texture2D t){
            selfPortait = t;
        }

        float getStrDef(){
            return 0.50 * (VIT) * defBoost;
        }

        std::vector<Skill> getSkills(){
            return skillList;
        }

        std::string getName(){
            return name;
        }

        void GainExp(int EXP){
            if (EXP == 0) EXP = 1;

            if (nextLevelXP > EXP){
                nextLevelXP -= EXP;
            }
            
            else if (nextLevelXP == EXP){
                Level++;
                nextLevelXP = int(20 * (pow(1.4 , Level-1)));
            }

            else {
                EXP -= nextLevelXP;
                Level++;
                nextLevelXP = int(20 * (pow(1.4 , Level-1)));
                this->GainExp(EXP);
            }
        }
};

class Warrior : public Adventurer {

    public:
        Warrior(std::string n, Texture2D t, int im) : Adventurer(30, 10, 5, 5, 5, 5, t, im){
            name = n;
            Elem = SLASH;
            CLASS = WARRIOR;
            skillList.push_back(Skill{"SLASH", "", 3});
            skillList.push_back(Skill{"FLAMING CUT", "", 7});
            skillList.push_back(Skill{"GRAND CROSS", "", 10});
        }

        void attack(int target){
            if (!this->isAlive()) {actionCount = 0; return;}

            while (encounter.monsters[target] == NULL) target = (target+1==encounter.monsterNumber)? 0: target + 1;
            Monster& monster = *encounter.monsters[target];

            int ATK = (STR + 1/*weaponATK*/) * 1.0 * monster.getElemRes(SLASH) * PhyAtkBoost;
            int damage = CalculateDamageSTR(ATK, monster.getStrDef());
            monster.TakeDamage(damage);
            ChangeMessage(name + " dealt " + std::to_string(damage) + " damage to " + monster.GetName(), "");
        }
};

void DrawUI(int* index, GameState* currentState) {
    while (partyIndex != 6 && (team[partyIndex] == NULL or !team[partyIndex]->isAlive())) partyIndex++;
    if (partyIndex == 6) {action = 9; actionCount = 1; FillQueue(); return;} 

    static Texture2D UITexture; 
    if (!IsTextureReady(UITexture)) UITexture = LoadTexture("resources/UIselector/UIBorder.png");
    
    static Sound selectionSound;
    if (!IsSoundReady(selectionSound)) {selectionSound = LoadSound("resources/selection.wav"); SetSoundVolume(selectionSound, 0.5f);}

    if (IsKeyPressed(KEY_W)){
        PlaySound(selectionSound);
        *index = *index - 1 == -1 ? 7 : *index - 1 == 0 ? 7 : *index - 1;
        if (*index == 5 && !team[partyIndex]->gotBurst()) (*index)--;
        if (*index == 4 && !HaveItems()) (*index)--;
        if (*index == 3 && !team[partyIndex]->gotSkills()) (*index)--;
    }

    else if (IsKeyPressed(KEY_S)){
        PlaySound(selectionSound);
        *index = *index + 1 == 8 ? 1 : *index + 1;
        if (*index == 3 && !team[partyIndex]->gotSkills()) (*index)++;
        if (*index == 4 && !HaveItems()) (*index)++;
        if (*index == 5 && !team[partyIndex]->gotBurst()) (*index)++;
    }

    team[partyIndex]->SetPortrait(selectedPortrait);
    team[partyIndex]->DrawUI((Vector2){0, 113.0f * scale});
    
    DrawRectangle(137*scale, 120*scale, 178 * scale, 37 * scale, (*index==1)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(152*scale, 159*scale, 171 * scale, 37 * scale, (*index==2)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(159*scale, 198*scale, 168 * scale, 37 * scale, (!team[partyIndex]->gotSkills())? Color{20,20,20,166} : (*index==3)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(162*scale, 237*scale, 165 * scale, 37 * scale, (!HaveItems())? Color{20,20,20,166} : (*index==4)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(159*scale, 276*scale, 168 * scale, 37 * scale, (!team[partyIndex]->gotBurst())? Color{20,20,20,166} : (*index==5)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(152*scale, 315*scale, 171 * scale, 37 * scale, (*index==6)?Color{83,209,235,166}:(Color){66,139,187,166});
    DrawRectangle(137*scale, 354*scale, 178 * scale, 37 * scale, (*index==7)?Color{83,209,235,166}:(Color){66,139,187,166});

    DrawTextureEx(UITexture, (Vector2){0, 113.0f * scale}, 0.0f, scale, WHITE);

    for (int i = 0; i < 7; i++) {
        if (i + 1 == *index)
            DrawTextEx(font, TextFormat(Options[i]), (Vector2){180.0f, 116.0f + 39.0f * i} * scale, 47 * scale, 0.0f, (Color){0, 0, 102, 230});
        else{
            if((i+1 == 3 && !team[partyIndex]->gotSkills()) || (i+1 == 4 && !HaveItems()) || (i+1 == 5 && !team[partyIndex]->gotBurst())) DrawTextEx(font, TextFormat(Options[i]), (Vector2){180.0f, 116.0f + 39.0f * i} * scale, 47 * scale, 0.0f, (Color){150, 150, 150, 230});
            else DrawTextEx(font, TextFormat(Options[i]), (Vector2){180.0f, 116.0f + 39.0f * i} * scale, 47 * scale, 0.0f, (Color){255, 255, 255, 230});
        }
    }

    switch(*index){
        case 0: ChangeMessage("What will you do?", ""); break; // default
        case 1: ChangeMessage("A basic attack.", ""); break; // attack
        case 2: ChangeMessage("Defend yourself.", ""); break; // defense
        case 3: ChangeMessage("Use a powerful skill.", ""); break; // skill
        case 4: ChangeMessage("Use an item from your", "inventory."); break; // item
        case 5: ChangeMessage("Release all your", "power."); break; // burst
        case 6: ChangeMessage("Switch to another", "character."); break; // switch
        case 7: ChangeMessage("Attempt to escape", "the battle."); break; // escape
    }

    if(IsKeyPressed(KEY_ENTER)){
        action = *index;
        if (*index == 7) {
            *index = 0;
            team[partyIndex]->SetPortrait(portrait);
            ActionEscape(currentState);
        }
    }
}

void ActionAttack(Encounter* encounter, int* index) {
    static int target = 0;
    int limit = encounter->monsterNumber - 1;
    while (encounter->monsters[target] == NULL) target = (target+1==encounter->monsterNumber)? 0 : target + 1;

    for (int i=0; i<5; i++) {if (encounter->monsters[i] != NULL && encounter->monsters[i]->isAlive()) encounter->monsters[i]->DrawUI();}
    
    if(encounter->monsters[target] != NULL && encounter->monsters[target]->isAlive()) encounter->monsters[target]->DrawTarget();
    
    if (IsKeyPressed(KEY_ENTER)) {
        orderStack.push(new Turns{std::bind(&Warrior::attack, static_cast<Warrior*>(team[partyIndex]), target), nullptr, team[partyIndex]->getAGI()});
        *index = 0;
        action = 0;
        target = 0;
        team[partyIndex]->SetPortrait(portrait);
        partyIndex++;
    }

    else if (IsKeyPressed(KEY_A)) {
        do {
            target = (target - 1 < 0) ? limit : target - 1;
        } while (encounter->monsters[target] == NULL);
    }
    
    else if (IsKeyPressed(KEY_D)) {
        do {
            target = (target + 1 > limit) ? 0 : target + 1;
        } while (encounter->monsters[target] == NULL);
    }

    else if (IsKeyPressed(KEY_BACKSPACE)){
        target = 0;
        action = 0;
    }

}

void ActionSkill(Adventurer* adventurer){
    static int index = 1;
    static int subindex = 1;
    static Sound selectionSound;
    if (!IsSoundReady(selectionSound)) {selectionSound = LoadSound("resources/selection.wav"); SetSoundVolume(selectionSound, 0.5f);}

    std::vector<Skill> skills = adventurer->getSkills();
    int skillsNum = skills.size();

    static Texture2D UITexture; 
    if (!IsTextureReady(UITexture)) UITexture = LoadTexture("resources/UIselector/UIBorder.png");
    team[partyIndex]->DrawUI((Vector2){0, 113.0f * scale});

    DrawRectangle(137*scale, 120*scale, 178 * scale, 37 * scale, {66,139,187,166});
    DrawRectangle(152*scale, 159*scale, 171 * scale, 37 * scale, {66,139,187,166});
    DrawRectangle(159*scale, 198*scale, 168 * scale, 37 * scale, {83,209,235,166});
    DrawRectangle(162*scale, 237*scale, 165 * scale, 37 * scale, (!HaveItems())? Color{20,20,20,166} : (Color){66,139,187,166});
    DrawRectangle(159*scale, 276*scale, 168 * scale, 37 * scale, (!team[partyIndex]->gotBurst())? Color{20,20,20,166} : Color{66,139,187,166});
    DrawRectangle(152*scale, 315*scale, 171 * scale, 37 * scale, {66,139,187,166});
    DrawRectangle(137*scale, 354*scale, 178 * scale, 37 * scale, {66,139,187,166});

    static Texture2D UISkillTexture;
    if (!IsTextureReady(UISkillTexture)) UISkillTexture = LoadTexture("resources/UIselector/Skills.png");

    if (skillsNum <= 6){
        if (IsKeyPressed(KEY_W)){
            if(index != 1)PlaySound(selectionSound);
            index = index - 1 <= 0 ? 1 : index - 1;
        }

        else if (IsKeyPressed(KEY_S)){
            if(index != skillsNum) PlaySound(selectionSound);
            index = index + 1 >= skillsNum+1 ? skillsNum : index + 1;
        }

        for (int i = 0; i< 6; i++){
            DrawRectangle(368*scale, (149 + 36*i)*scale, 287 * scale, 34 * scale, (i>=skillsNum)? Color{20,20,20,166} : (i+1 != index)? Color{66,139,187,166}: Color{253, 239, 78, 255});
            DrawRectangle(597*scale, (154 + 36*i)*scale, 51 * scale, 23 * scale, {5,31,52,255});
        }

        for (int i = 0; i< skillsNum; i++){
            DrawTextEx(font, TextFormat(skills[i].name.c_str()), (Vector2){385.0f, 152.0f + 36.0f * i} * scale, 30 * scale, 1.0f * scale, (i+1 == index)? (Color){0, 0, 102, 230} : (Color){255, 255, 255, 230});
            DrawTextEx(numbers, TextFormat("%2d", skills[i].cost), (Vector2){620.0f, (147.0f + 36.0f * i)} * scale, 40 * scale, 0.0f,(Color){255, 255, 255, 230});
        }
    }

    else {
        if (IsKeyPressed(KEY_W)){
            if(subindex != 1)PlaySound(selectionSound);
            index = index - 1 <= 0 ? 1 : index - 1;
            subindex = (subindex != 1)? subindex - 1: subindex;
        }

        else if (IsKeyPressed(KEY_S)){
            if(subindex != skillsNum) PlaySound(selectionSound);
            index = index + 1 >= 7 ? 6 : index + 1;
            subindex = (subindex != skillsNum)? subindex + 1: subindex;
        }

        for (int i = 0; i< 6; i++){
            DrawRectangle(368*scale, (149 + 36*i)*scale, 287 * scale, 34 * scale, (i>=skillsNum)? Color{20,20,20,166} : (i+1 != index)? Color{66,139,187,166}: Color{253, 239, 78, 255});
            DrawRectangle(597*scale, (154 + 36*i)*scale, 51 * scale, 23 * scale, {5,31,52,255});
        }

        for (int i = 0; i< 6; i++){
            DrawTextEx(font, TextFormat(skills[i + subindex - index].name.c_str()), (Vector2){385.0f, 152.0f + 36.0f * i} * scale, 30 * scale, 1.0f * scale, (i+1 == index)? (Color){0, 0, 102, 230} : (Color){255, 255, 255, 230});
            DrawTextEx(numbers, TextFormat("%2d", skills[i  + subindex - index].cost), (Vector2){620.0f, (147.0f + 36.0f * i)} * scale, 40 * scale, 0.0f,(Color){255, 255, 255, 230});
        }


        DrawText(TextFormat("INDEX: (%d)", index), 300, 20, 20, DARKGRAY);
        DrawText(TextFormat("SUBINDEX: (%d)", subindex), 300, 50, 20, DARKGRAY);
    }
    DrawRectangle(658*scale, 150*scale, 5 * scale, 22 * scale, {255,255,52,255});
    DrawRectangle(662*scale, 159*scale, 4 * scale, 13 * scale, {255,255,52,255});
    
    DrawTextureEx(UITexture, (Vector2){0, 113.0f * scale}, 0.0f, scale, WHITE);
    DrawTextureEx(UISkillTexture, (Vector2){346.0f * scale, 146.0f * scale}, 0.0f, scale, WHITE);

    if (IsKeyPressed(KEY_BACKSPACE)){
        action = 0;
        index = 1;
        subindex = 1;
    }
    
    for (int i = 0; i < 7; i++) {
        if (i + 1 == 3)
            DrawTextEx(font, TextFormat(Options[i]), (Vector2){180.0f, 116.0f + 39.0f * i} * scale, 47 * scale, 0.0f, (Color){0, 0, 102, 230});
        else{
            if(i+1 == 5 && !team[partyIndex]->gotBurst()) DrawTextEx(font, TextFormat(Options[i]), (Vector2){180.0f, 116.0f + 39.0f * i} * scale, 47 * scale, 0.0f, (Color){150, 150, 150, 230});
            else DrawTextEx(font, TextFormat(Options[i]), (Vector2){180.0f, 116.0f + 39.0f * i} * scale, 47 * scale, 0.0f, (Color){255, 255, 255, 230});
        }
    }
}

void ActionItem (int* ActionIndex) {
    static int state = 0;
    static int index = 1;
    static int subindex = 1;
    static int target = 0;
    static int front = true;

    static Sound selectionSound;
    if (!IsSoundReady(selectionSound)) {selectionSound = LoadSound("resources/selection.wav"); SetSoundVolume(selectionSound, 0.5f);}

    std::vector <Item> UsableItems; 
    for (int i = 0; i < (int)Inventory.size(); i++) {
        if (Inventory[i].type == USABLE) UsableItems.push_back(Inventory[i]);
    }

    

    static Texture2D UITexture; 
    if (!IsTextureReady(UITexture)) UITexture = LoadTexture("resources/UIselector/UIBorder.png");
    team[partyIndex]->DrawUI((Vector2){0, 113.0f * scale});

    DrawRectangle(137*scale, 120*scale, 178 * scale, 37 * scale, {66,139,187,166});
    DrawRectangle(152*scale, 159*scale, 171 * scale, 37 * scale, {66,139,187,166});
    DrawRectangle(159*scale, 198*scale, 168 * scale, 37 * scale, (!team[partyIndex]->gotSkills())?  Color{20,20,20,166} : Color{66,139,187,166});
    DrawRectangle(162*scale, 237*scale, 165 * scale, 37 * scale, {83,209,235,166});
    DrawRectangle(159*scale, 276*scale, 168 * scale, 37 * scale, (!team[partyIndex]->gotBurst())? Color{20,20,20,166} : Color{66,139,187,166});
    DrawRectangle(152*scale, 315*scale, 171 * scale, 37 * scale, {66,139,187,166});
    DrawRectangle(137*scale, 354*scale, 178 * scale, 37 * scale, {66,139,187,166});

    static Texture2D UISkillTexture;
    if (!IsTextureReady(UISkillTexture)) UISkillTexture = LoadTexture("resources/UIselector/Skills.png");

    int limit = UsableItems.size();
    if ((int)UsableItems.size() <= 6){
        if (IsKeyPressed(KEY_W) && state == 0){
            if(index != 1)PlaySound(selectionSound);
            index = index - 1 <= 0 ? 1 : index - 1;
            subindex = (subindex != 1)? subindex - 1: subindex;
        }

        else if (IsKeyPressed(KEY_S) && state == 0){
            if(index != (int)UsableItems.size()) PlaySound(selectionSound);
            index = index + 1 >= (int)UsableItems.size()+1 ? UsableItems.size() : index + 1;
            subindex = (subindex != limit)? subindex + 1: subindex;
        }

        for (int i = 0; i< 6; i++){
            DrawRectangle(368*scale, (149 + 36*i)*scale, 287 * scale, 34 * scale, (i>=(int)UsableItems.size())? Color{20,20,20,166} : (i+1 != index)? Color{66,139,187,166}: Color{253, 239, 78, 255});
        }

        for (int i = 0; i< (int)UsableItems.size(); i++){
            if(UsableItems[i].type == USABLE) {
                DrawTextEx(font, TextFormat(UsableItems[i].name.c_str()), (Vector2){385.0f, 152.0f + 36.0f * i} * scale, 30 * scale, 1.0f * scale, (i+1 == index)? (Color){0, 0, 102, 230} : (Color){255, 255, 255, 230});
            }
        }
    }

    else {
        if (IsKeyPressed(KEY_W) && state == 0){
            if(subindex != 1)PlaySound(selectionSound);
            index = index - 1 <= 0 ? 1 : index - 1;
            subindex = (subindex != 1)? subindex - 1: subindex;
        }

        else if (IsKeyPressed(KEY_S) && state == 0){
            if(subindex != limit) PlaySound(selectionSound);
            index = index + 1 >= 7 ? 6 : index + 1;
            subindex = (subindex != limit)? subindex + 1: subindex;
        }

        for (int i = 0; i< 6; i++){
            DrawRectangle(368*scale, (149 + 36*i)*scale, 287 * scale, 34 * scale, (i>=limit)? Color{20,20,20,166} : (i+1 != index)? Color{66,139,187,166}: Color{253, 239, 78, 255});
        }

        for (int i = 0; i< 6; i++){
            DrawTextEx(font, TextFormat(UsableItems[i + subindex - index].name.c_str()), (Vector2){385.0f, 152.0f + 36.0f * i} * scale, 30 * scale, 1.0f * scale, (i+1 == index)? (Color){0, 0, 102, 230} : (Color){255, 255, 255, 230});
        }

        DrawText(TextFormat("INDEX: (%d)", index), 300, 20, 20, DARKGRAY);
        DrawText(TextFormat("SUBINDEX: (%d)", subindex), 300, 50, 20, DARKGRAY);
    }

    DrawRectangle(658*scale, 150*scale, 5 * scale, 22 * scale, {255,255,52,255});
    DrawRectangle(662*scale, 159*scale, 4 * scale, 13 * scale, {255,255,52,255});
    
    DrawTextureEx(UITexture, (Vector2){0, 113.0f * scale}, 0.0f, scale, WHITE);
    DrawTextureEx(UISkillTexture, (Vector2){346.0f * scale, 146.0f * scale}, 0.0f, scale, WHITE);

/*
    else if(state == 1){
        continue;
    }
*/
    
    for (int i = 0; i < 7; i++) {
        if (i + 1 == 4)
            DrawTextEx(font, TextFormat(Options[i]), (Vector2){180.0f, 116.0f + 39.0f * i} * scale, 47 * scale, 0.0f, (Color){0, 0, 102, 230});
        else{
            if(( i+1 == 5 && !team[partyIndex]->gotBurst() )|| ( i+1 == 3 && !team[partyIndex]->gotSkills() )) DrawTextEx(font, TextFormat(Options[i]), (Vector2){180.0f, 116.0f + 39.0f * i} * scale, 47 * scale, 0.0f, (Color){150, 150, 150, 230});
            else DrawTextEx(font, TextFormat(Options[i]), (Vector2){180.0f, 116.0f + 39.0f * i} * scale, 47 * scale, 0.0f, (Color){255, 255, 255, 230});
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) && state == 0){
        action = 0;
        index = 1;
        subindex = 1;
    }

    else if (IsKeyPressed(KEY_ENTER) && state == 0){
        ChangeMessage("Select a Target.", "");
        target = 0;
        team[partyIndex]->SetPortrait(portrait);
        frontline[target]->SetPortrait(selectedPortrait);
        state = 1;
    }

    else if (IsKeyPressed(KEY_BACKSPACE) && state == 1){
        ChangeMessage("Use an item from your", "inventory.");
        if (front) frontline[target]->SetPortrait(portrait);
        else backline[target]->SetPortrait(portrait);
        team[partyIndex]->SetPortrait(selectedPortrait);
        state = 0;
    }

    else if (IsKeyPressed(KEY_ENTER) && state == 1){
        if (front){
            order.insert(new Turns{std::bind(UseObject, team[partyIndex]->getName(), frontline[target], UsableItems[subindex-1].name), nullptr, 1000});
            frontline[target]->SetPortrait(portrait);
        }
        else {
            order.insert(new Turns{std::bind(UseObject, team[partyIndex]->getName(), backline[target], UsableItems[subindex-1].name), nullptr, 1000});
            backline[target]->SetPortrait(portrait);
        }

        RemoveInventory(UsableItems[subindex-1].name);
        *ActionIndex = 0;
        partyIndex++;
        action = 0;
        state = 0;
        target = 0;
    }

    else if (state == 1){
        if (IsKeyPressed(KEY_W)){
            if (!front) { //frontline
                backline[target]->SetPortrait(portrait);
                front = true;
                while (target > (int)frontline.size()-1) target--;
                frontline[target]->SetPortrait(selectedPortrait);
            }
        }
        
        else if (IsKeyPressed(KEY_S) && !backline.empty()){
            if (front) { //frontline
                frontline[target]->SetPortrait(portrait);
                front = false;
                while (target > (int)backline.size()-1) target--;
                backline[target]->SetPortrait(selectedPortrait);
            }
        }
        
        else if (IsKeyPressed(KEY_A)){
            if (front) { //frontline
                frontline[target]->SetPortrait(portrait);
                target = (target > 0)? target-1 : target;
                frontline[target]->SetPortrait(selectedPortrait);
            }
            else {
                backline[target]->SetPortrait(portrait);
                target = (target > 0)? target-1 : target;
                backline[target]->SetPortrait(selectedPortrait);
            }
        }

        else if (IsKeyPressed(KEY_D)){
            if (front) { //frontline
                frontline[target]->SetPortrait(portrait);
                target = (target < (int)frontline.size()-1)? target+1 : target;
                frontline[target]->SetPortrait(selectedPortrait);
            }
            else{
                backline[target]->SetPortrait(portrait);
                target = (target < (int)backline.size()-1)? target+1 : target;
                backline[target]->SetPortrait(selectedPortrait);
            }
        }

    }
}

void EscapeBattle(GameState* State){
    *State = ESCAPE;
    monsterNum = 0;
    order.reset();
}

void Escape(GameState* State, int escape, int ratio, std::string name){
    if (escape >= 50 + ratio){
        ChangeMessage(name + " managed to escape.", "");
        order.insertAtBeginning(new Turns{std::bind(EscapeBattle, State)});
    }

    else {
        ChangeMessage(name + " tried to escape", "but failed.");
        }
}

void ActionEscape(GameState* State){
    int levelM = 0;
    int levelA = 0;
    for(int i = 0; i<5; i++) if( encounter.monsters[i] != NULL) levelM += encounter.monsters[i]->GetLevel();
    for(int i = 0; i<6; i++) if( team[i] != NULL && team[i]->isAlive()) levelA += team[i]->GetLevel();

    int escape = 1 + rand()%100;

    orderStack.push(new Turns{std::bind(Escape, State, escape, levelM - levelA, team[partyIndex]->getName()), nullptr, team[partyIndex]->getAGI()});
    partyIndex++;
    action = 0;
}

void TeamDivider(){
    for(int i = 0; i<3; i++) if(team[i] != NULL) frontline.push_back(team[i]);
    for(int i = 3; i<6; i++) if(team[i] != NULL) backline.push_back(team[i]);
}

void DrawBoxes() {

    switch (frontline.size()) {
        case 1:
            
            DrawRectangle(392 * scale, 412 * scale, 176 * scale, 59 * scale, (frontline[0]->isAlive())? Color{32, 58, 129, 225} : Color{29, 33, 43, 225});
            DrawTextureEx(frontline[0]->GetPortrait(), Vector2{390, 410}*scale, 0, scale, WHITE);
            DrawTextEx(names, TextFormat(frontline[0]->getName().c_str()), Vector2{435, 414}*scale, 22 * scale, 0.0f, (Color){113, 227, 217, 230});
            DrawRectangle(398 * scale, 461 * scale, 78 * frontline[0]->GetLifePercentage() * scale, 5*scale, {255,166,4,255});
            DrawRectangle(484 * scale, 461 * scale, 78 * frontline[0]->GetManaPercentage() * scale, 5*scale, {78,246,249,255});
            
            break;
        case 2:
            
            DrawRectangle(301 * scale, 412 * scale, 176 * scale, 59 * scale, (frontline[0]->isAlive())? Color{32, 58, 129, 225} : Color{29, 33, 43, 225});
            DrawTextureEx(frontline[0]->GetPortrait(), Vector2{299, 410}*scale, 0, scale, WHITE);
            DrawTextEx(names, TextFormat(frontline[0]->getName().c_str()), Vector2{344, 414}*scale, 22 * scale, 0, (Color){113, 227, 217, 230});
            DrawRectangle(307 * scale, 461 * scale, 78 * frontline[0]->GetLifePercentage() * scale, 5*scale, {255,166,4,255});
            DrawRectangle(393 * scale, 461 * scale, 78 * frontline[0]->GetManaPercentage() * scale, 5*scale, {78,246,249,255});
            
            
            DrawRectangle(483 * scale, 412 * scale, 176 * scale, 59 * scale, (frontline[1]->isAlive())? Color{32, 58, 129, 225} : Color{29, 33, 43, 225});
            DrawTextureEx(frontline[1]->GetPortrait(), Vector2{481, 410}*scale, 0, scale, WHITE);
            DrawTextEx(names, TextFormat(frontline[1]->getName().c_str()), Vector2{526, 414}*scale, 22 * scale, 0, (Color){113, 227, 217, 230});
            DrawRectangle(489 * scale, 461 * scale, 78 * frontline[1]->GetLifePercentage() * scale, 5*scale, {255,166,4,255});
            DrawRectangle(575 * scale, 461 * scale, 78 * frontline[0]->GetManaPercentage() * scale, 5*scale, {78,246,249,255});
            break;
        case 3:
            
            DrawRectangle(210 * scale, 412 * scale, 176 * scale, 59 * scale, (frontline[0]->isAlive())? Color{32, 58, 129, 225} : Color{29, 33, 43, 225});
            DrawTextureEx(frontline[0]->GetPortrait(), Vector2{208, 410}*scale, 0, scale, WHITE);
            DrawTextEx(names, TextFormat(frontline[0]->getName().c_str()), Vector2{253, 414}*scale, 22 * scale, 0, (Color){113, 227, 217, 230});
            DrawRectangle(216 * scale, 461 * scale, 78 * frontline[0]->GetLifePercentage() * scale, 5*scale, {255,166,4,255});
            DrawRectangle(302 * scale, 461 * scale, 78 * frontline[0]->GetManaPercentage() * scale, 5*scale, {78,246,249,255});

            
            DrawRectangle(392 * scale, 412 * scale, 176 * scale, 59 * scale, (frontline[1]->isAlive())? Color{32, 58, 129, 225} : Color{29, 33, 43, 225});
            DrawTextureEx(frontline[1]->GetPortrait(), Vector2{390, 410}*scale, 0, scale, WHITE);
            DrawTextEx(names, TextFormat(frontline[1]->getName().c_str()), Vector2{435, 414}*scale, 22 * scale, 0.0f, (Color){113, 227, 217, 230});
            DrawRectangle(398 * scale, 461 * scale, 78 * frontline[1]->GetLifePercentage() * scale, 5*scale, {255,166,4,255});
            DrawRectangle(484 * scale, 461 * scale, 78 * frontline[1]->GetManaPercentage() * scale, 5*scale, {78,246,249,255});

            
            DrawRectangle(574 * scale, 412 * scale, 176 * scale, 59 * scale, (frontline[2]->isAlive())? Color{32, 58, 129, 225} : Color{29, 33, 43, 225});
            DrawTextureEx(frontline[2]->GetPortrait(), Vector2{572, 410}*scale, 0, scale, WHITE);
            DrawTextEx(names, TextFormat(frontline[2]->getName().c_str()), Vector2{617, 414}*scale, 22 * scale, 0, (Color){113, 227, 217, 230});
            DrawRectangle(580 * scale, 461 * scale, 78 * frontline[2]->GetLifePercentage() * scale, 5*scale, {255,166,4,255});
            DrawRectangle(666 * scale, 461 * scale, 78 * frontline[0]->GetManaPercentage() * scale, 5*scale, {78,246,249,255});
            break;
    }

    switch (backline.size()) {
        case 1:
            
            DrawRectangle(392 * scale, 477 * scale, 176 * scale, 59 * scale, (backline[0]->isAlive())? Color{32, 58, 129, 225} : Color{29, 33, 43, 225});
            DrawTextureEx(backline[0]->GetPortrait(), Vector2{390, 475}*scale, 0, scale, WHITE);
            DrawTextEx(names, TextFormat(backline[0]->getName().c_str()), Vector2{435, 479}*scale, 22 * scale, 0, (Color){113, 227, 217, 230});
            DrawRectangle(398 * scale, 526 * scale, 78 * backline[0]->GetLifePercentage() * scale, 5*scale, {255,166,4,255});
            DrawRectangle(484 * scale, 526 * scale, 78 * frontline[0]->GetManaPercentage() * scale, 5*scale, {78,246,249,255});
            break;
        case 2:
            
            DrawRectangle(301 * scale, 477 * scale, 176 * scale, 59 * scale, (backline[0]->isAlive())? Color{32, 58, 129, 225} : Color{29, 33, 43, 225});
            DrawTextureEx(backline[0]->GetPortrait(), Vector2{299, 475}*scale, 0, scale, WHITE);
            DrawTextEx(names, TextFormat(backline[0]->getName().c_str()), Vector2{344, 479}*scale, 22 * scale, 0, (Color){113, 227, 217, 230});
            DrawRectangle(307 * scale, 526 * scale, 78 * backline[0]->GetLifePercentage() * scale, 5*scale, {255,166,4,255});
            DrawRectangle(393 * scale, 526 * scale, 78 * frontline[0]->GetManaPercentage() * scale, 5*scale, {78,246,249,255});

            
            DrawRectangle(483 * scale, 477 * scale, 176 * scale, 59 * scale, (backline[1]->isAlive())? Color{32, 58, 129, 225} : Color{29, 33, 43, 225});
            DrawTextureEx(backline[1]->GetPortrait(), Vector2{481, 475}*scale, 0, scale, WHITE);
            DrawTextEx(names, TextFormat(backline[1]->getName().c_str()), Vector2{526, 479}*scale, 22 * scale, 0, (Color){113, 227, 217, 230});
            DrawRectangle(489 * scale, 526 * scale, 78 * backline[1]->GetLifePercentage() * scale, 5*scale, {255,166,4,255});
            DrawRectangle(575 * scale, 526 * scale, 78 * frontline[0]->GetManaPercentage() * scale, 5*scale, {78,246,249,255});
            break;
        case 3:
            
            DrawRectangle(210 * scale, 477 * scale, 176 * scale, 59 * scale, (backline[0]->isAlive())? Color{32, 58, 129, 225} : Color{29, 33, 43, 225});
            DrawTextureEx(backline[0]->GetPortrait(), Vector2{208, 475}*scale, 0, scale, WHITE);
            DrawTextEx(names, TextFormat(backline[0]->getName().c_str()), Vector2{253, 479}*scale, 22 * scale, 0, (Color){113, 227, 217, 230});
            DrawRectangle(216 * scale, 526 * scale, 78 * backline[0]->GetLifePercentage() * scale, 5*scale, {255,166,4,255});
            DrawRectangle(302 * scale, 526 * scale, 78 * frontline[0]->GetManaPercentage() * scale, 5*scale, {78,246,249,255});

            
            DrawRectangle(392 * scale, 477 * scale, 176 * scale, 59 * scale, (backline[1]->isAlive())? Color{32, 58, 129, 225} : Color{29, 33, 43, 225});
            DrawTextureEx(backline[1]->GetPortrait(), Vector2{390, 475}*scale, 0, scale, WHITE);
            DrawTextEx(names, TextFormat(backline[1]->getName().c_str()), Vector2{435, 479}*scale, 22 * scale, 0, (Color){113, 227, 217, 230});
            DrawRectangle(398 * scale, 526 * scale, 78 * backline[1]->GetLifePercentage() * scale, 5*scale, {255,166,4,255});
            DrawRectangle(484 * scale, 526 * scale, 78 * frontline[0]->GetManaPercentage() * scale, 5*scale, {78,246,249,255});

            
            DrawRectangle(574 * scale, 477 * scale, 176 * scale, 59 * scale, (backline[2]->isAlive())? Color{32, 58, 129, 225} : Color{29, 33, 43, 225});
            DrawTextureEx(backline[2]->GetPortrait(), Vector2{572, 475}*scale, 0, scale, WHITE);
            DrawTextEx(names, TextFormat(backline[2]->getName().c_str()), Vector2{617, 479}*scale, 22 * scale, 0, (Color){113, 227, 217, 230});
            DrawRectangle(580 * scale, 526 * scale, 78 * backline[2]->GetLifePercentage() * scale, 5*scale, {255,166,4,255});
            DrawRectangle(666 * scale, 526 * scale, 78 * frontline[0]->GetManaPercentage() * scale, 5*scale, {78,246,249,255});
            break;
    }
}

int CalculateDamageSTR(float ATK, float DEF){
    float baseDamage = ATK - DEF;
    float damage;

    if (ATK <= DEF) damage = 1 + rand() % 5;
    else {
        if (ATK <= (DEF * 1.1))
            baseDamage = (8 + rand()% 5) * ATK / 100;

        if (baseDamage <= 100)
            damage = baseDamage + rand() % 6;
        
        else damage = baseDamage + ( 5 * (baseDamage/100));
    }

    return int(damage);
}

void ExecuteTurn(GameState* State){
    if ((IsKeyPressed(KEY_ENTER)||actionCount == 1) && order.isEmpty()) {
        partyIndex = 0;
        action = 0;
        actionCount = 1;

        if (monsterNum == 0){
            *State = EXPERIENCE;
        }

        else {turnsCount++; order.reset();}
    }

    else if (monsterNum == 0) {
        if (IsKeyPressed(KEY_ENTER)) {
            order.reset();
            action = 0;
            actionCount = 1;
            *State = EXPERIENCE;
        }
    }

    else {
        if (actionCount == 1 || IsKeyPressed(KEY_ENTER)) {
            order.execute();
            actionCount++;
        }
    }
}

void DrawClock(){
    static Texture2D clock1;
    static Texture2D clock2;
    static Texture2D clock3;
    static Texture2D clock4;

    static Texture2D clockframe;

    if (!IsTextureReady(clock1)) clock1 = LoadTexture("resources/UIselector/clock/clock1.png");
    if (!IsTextureReady(clock2)) clock2 = LoadTexture("resources/UIselector/clock/clock2.png");
    if (!IsTextureReady(clock3)) clock3 = LoadTexture("resources/UIselector/clock/clock3.png");
    if (!IsTextureReady(clock4)) clock4 = LoadTexture("resources/UIselector/clock/clock4.png");

    if (!IsTextureReady(clockframe)) clockframe = LoadTexture("resources/UIselector/clockframe.png");

    float rotation = 1.5f * hour - 165.0f;

    DrawTextureEx(clock4, Vector2{0, 43} * scale, 0.0f+rotation, scale, WHITE);
    DrawTextureEx(clock1, Vector2{0, 43} * scale, 180.0f+rotation, scale, WHITE);
    DrawTextureEx(clock2, Vector2{0, 43} * scale, -90.0f+rotation, scale, WHITE);
    DrawTextureEx(clock3, Vector2{0, 43} * scale, 90.0f+rotation, scale, WHITE);

    DrawTextureEx(clockframe, Vector2{0,0}, 0, scale, WHITE);

    std::string midday = (hour<120)?"am":"pm";
    DrawTextEx(names, midday.c_str(), Vector2{37, -1} * scale, 25*scale, 0, {255,255,255,240});

    int intHour = (hour/10)%12;
    if (intHour == 0) intHour = 12;
    std::string stringHour = ((intHour<10)?"0":"") + std::to_string(intHour);

    DrawTextEx(names, stringHour.c_str(), Vector2{77, -3} * scale, 50*scale, scale, {255,255,255,240});
}
    
void DrawTurn() {
    static Texture2D turnPortrait;
    if (!IsTextureReady(turnPortrait)) turnPortrait = LoadTexture("resources/UIselector/turnPortait.png");

    DrawTextureEx(turnPortrait, Vector2{0,0}, 0, scale, WHITE);
}

void DrawMessages() {
    static Texture2D messagePortrait;
    if (!IsTextureReady(messagePortrait)) messagePortrait = LoadTexture("resources/UIselector/messagePortrait.png");

    DrawTextureEx(messagePortrait, Vector2{0,0}, 0, scale, WHITE);
}

void DrawTextMessage() {
    DrawTextEx(names, message1.c_str(), Vector2{180, 10} * scale, 27*scale, 0, WHITE);
    DrawTextEx(names, message2.c_str(), Vector2{180, 37} * scale, 27*scale, 0, WHITE);
}

void Monster::attack() {
    if (!this->isAlive()){actionCount = 0; return;}
    int target = rand() % 6;

    while (team[target] == NULL || !team[target]->isAlive()) target = (target+1==6)? 0: target + 1;
    Adventurer& adventurer = *team[target];

    int ATK = (STR) * 1.0 * PhyAtkBoost;
    int damage = CalculateDamageSTR(ATK, adventurer.getStrDef());
    adventurer.TakeDamage(damage);
    ChangeMessage(name + " dealt " + std::to_string(damage) + " damage to " + adventurer.GetName(), "");
}

void FillQueue() {
    while (!orderStack.isEmpty()) {
        Turns* turn = orderStack.top();
        order.insert(turn);
        orderStack.pop();
    }

    for (const auto &monster : encounter.monsters){
        if (monster != NULL) order.insert(new Turns{std::bind(&Monster::attack, static_cast<Monster*>(monster)), nullptr, monster->GetAGI()});
    }
}

void UseObject(std::string name, Adventurer* target, std::string objectName){
    if (objectName == "Potion") {
        if(target->isAlive()){
            ChangeMessage(name + " used a potion.", target->getName() + " was healed.");
            target->GetHeal(10);
        }
        else ChangeMessage(name + " used a potion.", "But it didn't work.");
    }
}

void DrawExpPortrait(int i, int teamIndex) {
    static Texture2D EXPportrait;
    if (!IsTextureReady(EXPportrait)) EXPportrait = LoadTexture("resources/Adventurers/ExpPortrait.png");

    DrawTextureEx(EXP_Swordsman[team[teamIndex]->imageIndex], Vector2{0, 103.0f + 54 * i}*scale, 0, scale, WHITE);
    DrawTextureEx(EXPportrait, Vector2{0, 103.0f + 54 * i}*scale, 0, scale, WHITE);
    DrawTextEx(names, team[teamIndex]->getName().c_str(), Vector2{275, 112.0f + 54 * i} * scale, 30 * scale, 0, {210,255,255,255});

    std::string level = (team[teamIndex]->GetLevel()>10)?std::to_string(team[teamIndex]->GetLevel()):"  " + std::to_string(team[teamIndex]->GetLevel());
    DrawTextEx(names, level.c_str(), Vector2{495, 112.0f + 54 * i} * scale, 30 * scale, 0, {0,252,221,255});
    
    int exp = team[teamIndex]->GetEXP();
    std::string expstr = (exp>9999)? std::to_string(team[teamIndex]->GetEXP()) :
                        (exp>999)? "  " + std::to_string(team[teamIndex]->GetEXP()) :
                        (exp>99)?  "    " + std::to_string(team[teamIndex]->GetEXP()):
                        (exp>9)? "      " + std::to_string(team[teamIndex]->GetEXP()) :
                        "        " + std::to_string(team[teamIndex]->GetEXP());

    DrawTextEx(names, expstr.c_str(), Vector2{775, 107.0f + 54 * i} * scale, 25 * scale, 0, {0,252,221,255});

    DrawRectangle(618*scale, (135 + 54 * i) * scale, 216 * team[teamIndex]->GetLevelPercentage() * scale, 6 * scale, {40, 235, 255, 255});
}