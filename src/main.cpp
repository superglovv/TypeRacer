#include <Arduino.h>

#define RED_LED 4
#define GREEN_LED 6
#define BLUE_LED 5
#define START_STOP_BUTTON 2
#define DIFFICULTY_BUTTON 3
#define ROUND_TIME 30000
#define EASY_INTERVAL 7000
#define MEDIUM_INTERVAL 5000
#define HARD_INTERVAL 2000

bool isGameRunning = false;
bool isStartStopButtonPressed = false;
bool isDifficultyButtonPressed = false;
unsigned long roundStartTime = 0;
unsigned long currentWordStartTime = 0;
int correctWordCount = 0;

unsigned long lastDifficultyChangeTime = 0;
const unsigned long debounceInterval = 300;

enum Difficulty { EASY, MEDIUM, HARD };
Difficulty currentDifficultyLevel = EASY;
unsigned long wordDisplayTime = EASY_INTERVAL;

const char* wordList[] = {"sneaker", "laces", "sole", "high", "low", "colorway", "mid", "boost", "air", "retro"};
const int wordCount = sizeof(wordList) / sizeof(wordList[0]);
String currentWord = "";
String userInputWord = "";

void setLEDColor(int red, int green, int blue) {
    analogWrite(RED_LED, red);
    analogWrite(GREEN_LED, green);
    analogWrite(BLUE_LED, blue);
}

String getRandomWord() {
    return wordList[random(0, wordCount)];
}

void startNewRound() {
    isGameRunning = true;
    roundStartTime = millis();
    currentWordStartTime = millis();
    correctWordCount = 0;
    userInputWord = "";
    
    for (int i = 3; i > 0; i--) {
        setLEDColor(255, 255, 255);
        delay(500);
        setLEDColor(0, 0, 0);
        delay(500);
        Serial.println(i);
    }

    Serial.println("Round started!");
    setLEDColor(0, 255, 0);
    
    currentWord = getRandomWord();
    Serial.print("Word: ");
    Serial.println(currentWord);
}

void stopCurrentRound() {
    isGameRunning = false;
    setLEDColor(255, 255, 255);
    Serial.print("\n");
    Serial.print("Finish line! Correct words: ");
    Serial.println(correctWordCount);
}

void changeGameDifficulty() {
    if (isGameRunning) return;

    if (millis() - lastDifficultyChangeTime > debounceInterval) {
        currentDifficultyLevel = static_cast<Difficulty>((currentDifficultyLevel + 1) % 3);

        switch (currentDifficultyLevel) {
            case EASY:
                wordDisplayTime = EASY_INTERVAL;
                Serial.println("Easy mode on!");
                break;
            case MEDIUM:
                wordDisplayTime = MEDIUM_INTERVAL;
                Serial.println("Medium mode on!");
                break;
            case HARD:
                wordDisplayTime = HARD_INTERVAL;
                Serial.println("Hard mode on!");
                break;
        }
        lastDifficultyChangeTime = millis();
    }
}

bool isInputWordCorrect(const String& inputWord) {
    return inputWord.equals(currentWord);
}

void setup() {
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);

    pinMode(START_STOP_BUTTON, INPUT_PULLUP);
    pinMode(DIFFICULTY_BUTTON, INPUT_PULLUP);

    Serial.begin(9600);

    setLEDColor(255, 255, 255);

    Serial.println("Initial difficulty: Easy");
}

void loop() {
    if (digitalRead(START_STOP_BUTTON) == LOW) {
        if (!isStartStopButtonPressed && millis() - lastDifficultyChangeTime > debounceInterval) {
            lastDifficultyChangeTime = millis();
            isStartStopButtonPressed = true;

            if (!isGameRunning) {
                startNewRound();
            } else {
                stopCurrentRound();
            }
        }
    } else {
        isStartStopButtonPressed = false;
    }

    if (digitalRead(DIFFICULTY_BUTTON) == LOW) {
        if (!isDifficultyButtonPressed) {
            isDifficultyButtonPressed = true;
            changeGameDifficulty();
        }
    } else {
        isDifficultyButtonPressed = false;
    }

    if (isGameRunning) {
        if ((millis() - roundStartTime) > ROUND_TIME) {
            stopCurrentRound();
        } else {
            if (millis() - currentWordStartTime > wordDisplayTime) {
                currentWord = getRandomWord();
                Serial.print("\n");
                Serial.print("Word: ");
                Serial.println(currentWord);
                currentWordStartTime = millis();
                userInputWord = "";
                setLEDColor(0, 255, 0);
            }

            while (Serial.available() > 0) {
                char ch = Serial.read();
                if (ch == '\n') {
                    userInputWord.trim();
                    Serial.print("Entered word: ");
                    Serial.println(userInputWord);

                    if (isInputWordCorrect(userInputWord)) {
                        correctWordCount++;
                        Serial.println("Correct!");
                        setLEDColor(0, 255, 0);
                        
                        currentWord = getRandomWord();
                        Serial.print("Word: ");
                        Serial.println(currentWord);
                        currentWordStartTime = millis();
                        userInputWord = "";
                    } else {
                        setLEDColor(255, 0, 0);
                        Serial.println("Incorrect!");
                    }
                } else if (ch == 8 || ch == 127) {
                    if (userInputWord.length() > 0) {
                        userInputWord.remove(userInputWord.length() - 1);
                        Serial.print("\b \b");
                    }
                } else {
                    userInputWord += ch;

                    if (userInputWord.length() > currentWord.length() || 
                        userInputWord.charAt(userInputWord.length() - 1) != currentWord.charAt(userInputWord.length() - 1)) {
                        setLEDColor(255, 0, 0);
                    } else {
                        setLEDColor(0, 255, 0);
                    }
                    Serial.print(ch);
                }
            }
        }
    }
}
