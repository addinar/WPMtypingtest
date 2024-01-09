#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <map>

using namespace std;

string texts[] = {"Peter_Rabbit.txt", "The_Little_Match_Girl.txt","Peter_Pan.txt",
                "The_Ugly_Duckling.txt"};

string titles[] = {"'The Tale of Peter Rabbit' by Beatrice Potter (1901)", 
                "'The Little Match Girl' by Hans Christian Andersen (1845)",
                "'Peter Pan' by J.M. Barrie (1911)",
                "'The Ugly Duckling' by Hans Christian Andersen (1843)"};

char wordBreak[] = {' ', '\n'};

bool newWord(char toFind) {
    auto it = find(begin(wordBreak), end(wordBreak), toFind);
    if (it != end(wordBreak)) {
        return true;
    }
    else {
        return false;
    }
}

int wordCount(const string& currLine) {
    int count = 0;
    for(int i = 0; i < currLine.size(); ++i) {
        if (newWord(currLine.at(i))) {
            count++;
        }
    }
    return count;
}

int typosCount(string line1, string line2) { // line1 must be the USER inputted line
    int typos = 0;
    int i = 0;
    
    while (i < min(line1.size(), line2.size())) {
        if (line1.at(i) != line2.at(i)) {
            typos++;
        }
        if (i + 1 < min(line1.size(), line2.size())) {
            if (line1.at(i) == line2.at(i+1)) {
                if (i - 1 > 0 && line1.at(i-1) != line2.at(i-1)) {
                    line1 = line1.substr(i, line1.size() - i);
                    line2 = line2.substr(i + 1, line2.size() - i);
                }
            }
            else if (line1.at(i+1) == line2.at(i)) {
                if (i - 1 > 0 && line1.at(i-1) != line2.at(i-1)) {
                    line1 = line1.substr(i + 1, line1.size() - i);
                    line2 = line2.substr(i, line2.size() - i);
                }
            }
        }
        i++;
    }

    if (line1.size() > line2.size()) {
        typos += (line1.size() - line2.size());
    }
    
    return typos;
}

float CalculatePercentTypos(int numTypos, int numTotalChars) {
    return static_cast<float>(numTypos) / numTotalChars;
}

int CalculateWPM(int elapsedTime, int numUserWords) {
    float minutes = elapsedTime / 60.00;
    int WPM = numUserWords / minutes;
    return WPM;
}

int main() {
    // declarations
    srand(static_cast<unsigned>(time(0)));
    int num = rand() % 4;
    ifstream chosenText;
    vector<string> lines;
    vector<string> userLines;
    char userChar;
    int i = 1;

    cout << "The purpose of this test is to count the number of words you are able to type\nwithin a minute. You will be provided an excerpt from the text: \n" << titles[num] << ".\n To account for an accurate result, please refrain from typos and type to \nthe best of your ability. Please be as precise as possible when typing.\n" << endl << endl;
    cout << "The timer starts as soon as you press [s]. Press [s] to begin." << endl;
    cin >> userChar;

    while (userChar != 's') {
        cout << "Incorrect input. Press [s] to begin." << endl;
        cin >> userChar;
    }
    
    chosenText.open(texts[num], ios::in);
    if (chosenText.is_open()) {
        string line;
        while (getline(chosenText, line)) {
             lines.push_back(line);
        }
        chosenText.close();
    }
    else {
        cerr << "Error opening the file." << endl;
    }

    auto duration = chrono::seconds(60);
    auto startTime = chrono::system_clock::now();

    string userInput;
    cout << lines.at(0) << endl;

    while (userInput == "\0") {
        getline(cin, userInput);
    }
    userLines.push_back(userInput);

    while (chrono::system_clock::now() - startTime <= duration && i < lines.size()) {
        cout << lines.at(i) << endl;

        getline(cin, userInput);
        userLines.push_back(userInput);

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        i++;
        // cout << endl;
    }

    auto endTime = chrono::system_clock::now();

    char tempChar;
    int numWords = 0;
    int numUserWords = 0;
    int typos = 0;
    int totalChars = 0;

    for(int i = 0; i < userLines.size(); ++i) {
        string temp1 = userLines.at(i);
        string temp2 = lines.at(i);

        numUserWords += wordCount(temp1); // count num words user inputted
        numWords += wordCount(temp2); // count num words in original text

        if (temp1.size() == temp2.size()) {
            numUserWords++;
            numWords++;
        }

        // count num typos
        typos += typosCount(temp1, temp2);

        // count total chars
        totalChars += temp2.size();
    }

    int extra = numUserWords - numWords;
    if (extra != 0) {
        typos += abs(extra);
    }

    float percentTypos = CalculatePercentTypos(typos, totalChars);
    int elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
    int rawWPM = CalculateWPM(elapsedTime, numUserWords);
    float accurary = 1 - percentTypos;

    cout << "Result: " << rawWPM << " words per minute x " << accurary*100 << 
    "% accurary = " << static_cast<int>(rawWPM * accurary) << " words per minute." << endl;

    return 0;
}