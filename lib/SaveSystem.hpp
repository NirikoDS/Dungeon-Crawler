using namespace std;
// Function to save data to a file
void Adventurer::Save (ofstream& outFile) {
    outFile << name << endl;
    outFile << CLASS << endl;
    outFile << Level << " " << nextLevelXP << endl;
    outFile << maxHP << " " << HP << " " << maxTP << " " << TP << endl;
    outFile << STR << " " << VIT << " " << AGI << " " << TEC << endl;
    outFile << SkillPoints << endl;
    for (int i = 0; i < 10; i++)
        outFile << skills[i] << " ";
    outFile << endl;
    outFile << imageIndex << endl;
}

void saveData() {
    ofstream outFile("src/savefile.txt");

    // Save team members
    for (int i = 0; i < 6; ++i) {
        if (team[i] != NULL) {
            outFile << "TeamMember" << endl;
            outFile << i << endl; //position
            team[i]->Save(outFile);
        }
    }

    // Save guild members
    for (int i = 0; i < 20; ++i) {
        if (guild[i] != NULL) {
            continue;
        }
    }

    outFile.close();
}