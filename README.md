# Joc de tip TypeRacer

## Descrierea Task-ului

Acest proiect simulează un joc asemănător cu TypeRacer utilizând un Arduino Uno. Jocul oferă feedback vizual prin intermediul unui LED RGB, care luminează alb în starea de repaus, devine verde când utilizatorul scrie corect cuvintele și roșu în caz de eroare (atunci când se introduce o literă greșită sau se depășește lungimea cuvântului). Două butoane sunt folosite, cel de start-stop pentru a controla începutul și sfârșitul jocului, și cel de schimbare al dificultății. Cuvintele sunt generate dintr-un dicționar și sunt afișate aleatoriu în terminal.

## Componente Utilizate

- Arduino Uno
- Breadboard
- LED RGB
- Buton START-STOP
- Buton de schimbare al dificultății
- Rezistențe (3x 330ohm, 2x 1kOhm)
- Cabluri de conectare

## Principiul de Funcționare

Jocul începe la apăsarea butonului de Start, cu un countdown de 3-2-1, după care un cuvânt este afișat în Serial Monitor, iar jucătorul are un timp de 30 de secunde pentru a scrie cât mai multe cuvinte corect. Dificultatea jocului afectează timpul de afișare a fiecărui cuvânt: în modul Easy, cuvântul este vizibil timp de 7000 ms, în modul Medium pentru 5000 ms, iar în modul Hard doar 2000 ms. Feedback-ul este oferit prin culoarea LED-ului RGB: verde semnalează un input corect, iar LED-ul roșu indică un input incorect. Jucătorii pot tasta cuvântul afișat, având posibilitatea de a corecta inputul folosind tasta backspace, iar după introducerea unui cuvânt, jocul verifică corectitudinea acestuia și oferă feedback corespunzător prin iluminarea LED-urilor și mesaje în terminal. La finalul celor 30 de secunde dintr-o rundă se afișează căte cuvinte corecte au fost introduse.

### LED RGB – Indicator de stare

- **Alb**: Jocul este în repaus.
- **Verde**: Litera/ cuvântul introdus sunt corecte.
- **Roșu**: Litera/ cuvântul introdus sunt greșite.

### Butoane

- **Buton START-STOP**: La apăsarea acestui buton atunci când jocul este în repaus va porni o rundă de joc pe dificultatea selectată după o numărătoare inversă. Atunci când se desfășoară o rundă apăsarea butonului va duce la oprirea acestei runde și afișarea rezultatului de până in acel moment.
- **Buton de Schimbare a Dificultății**: Atunci când jocul este în repaus acest buton permite schimbarea dificultății între cele trei dificultăți predefinite (easy, medium și high) care sunt anunțate printr-un text în terminal (ex: Medium mode on!).

## Explicația Codului

### Setarea pinilor și definirea intervalelor de timp și a variabilelor

```cpp
// LED-ul RGB este setat ulterior ca ieșire
#define RED_LED 4
#define GREEN_LED 6
#define BLUE_LED 5

//Butoanele sunt setate ulterior ca intrări
#define START_STOP_BUTTON 2
#define DIFFICULTY_BUTTON 3

//Definirea intervalelor de timp pentru o rundă și pentru cele trei dificultăți
#define ROUND_TIME 3000
#define EASY_INTERVAL 7000
#define MEDIUM_INTERVAL 5000
#define HARD_INTERVAL 2000

bool isGameRunning = false; // Indică dacă jocul este activ sau nu
bool isStartStopButtonPressed = false; // Indică dacă butonul de Start/Stop a fost apăsat
bool isDifficultyButtonPressed = false; // Indică dacă butonul de schimbare a dificultății a fost apăsat
unsigned long roundStartTime = 0; // Timpul de început al rundei curente, în milisecunde
unsigned long currentWordStartTime = 0; // Timpul de început al cuvântului curent, în milisecunde
int correctWordCount = 0; // Numărul de cuvinte corecte introduse de jucător

unsigned long lastDifficultyChangeTime = 0; // Timpul ultimei schimbări a dificultății, în milisecunde
const unsigned long debounceInterval = 300; // Intervalul de debounce pentru butoane, în milisecunde, pentru a evita citirile multiple

enum Difficulty { EASY, MEDIUM, HARD };
Difficulty currentDifficultyLevel = EASY; // Setează nivelul de dificultate curent la cel EASY ca valoare inițială
unsigned long wordDisplayTime = EASY_INTERVAL; // Timpul de afișare a cuvântului curent pentru dificultatea inițială

const char* wordList[] = {"sneaker", "laces", "sole", "high", "low", "colorway", "mid", "boost", "air", "retro"}; // Lista de cuvinte disponibile pentru joc
const int wordCount = sizeof(wordList) / sizeof(wordList[0]); // Numărul total de cuvinte din lista de cuvinte
String currentWord = ""; // Variabilă pentru a stoca cuvântul curent care trebuie tastat de jucător.
String userInputWord = ""; // Variabilă pentru a stoca cuvântul introdus de jucător.

```

### Funcția de setare a culorii LED-ului RGB

```cpp
// primeste ca input intensitatea culorii și aprinde led-ul specific prin analogWrite
void setLEDColor(int red, int green, int blue) {
    analogWrite(RED_LED, red);
    analogWrite(GREEN_LED, green);
    analogWrite(BLUE_LED, blue);
}
```

### Funcție pentru generarea unui cuvânt aleatoriu

```cpp
// Returnează un cuvânt aleatoriu din lista de cuvinte
String getRandomWord() {
    return wordList[random(0, wordCount)]; // Alege un index aleatoriu între 0 și numărul total de cuvinte și returnează cuvântul corespunzător din array-ul wordList
}
```

### Funcția pentru start-ul unei runde de joc

Această funcție este folosită pentru a porni jocul la apăsarea butonului de Start.

```cpp
void startNewRound() {
    isGameRunning = true; // Setează starea jocului ca fiind true
    roundStartTime = millis(); // Salvează timpul curent ca moment de început al rundei
    currentWordStartTime = millis(); // Salvează timpul curent ca moment de început al cuvântului curent
    correctWordCount = 0; // Resetează numărul de cuvinte corecte la zero
    userInputWord = ""; // Resetează inputul utilizatorului la un șir gol

    // Efectuează un countdown de la 3 la 1, aprinzând și stingând LED-ul de fiecare dată
    for (int i = 3; i > 0; i--) {
        setLEDColor(255, 255, 255);
        delay(500);
        setLEDColor(0, 0, 0);
        delay(500);
        Serial.println(i);
    }

    Serial.println("Round started!"); // Mesaj începutul rundei
    setLEDColor(0, 255, 0); // Aprinde LED-ul verde

    currentWord = getRandomWord(); // Cuvânt aleatoriu din lista de cuvinte
    Serial.print("Word: ");
    Serial.println(currentWord);
}
```

### Funcția pentru oprirea rundei curente

Această funcție este folosită atunci când butonul de Stop este apăsat sau când durata rundei curente este egală cu ROUND_TIME (30 s).

```cpp
void stopCurrentRound() {
    isGameRunning = false; // Oprește jocul
    setLEDColor(255, 255, 255); // Aprinde LED-ul RGB în culoarea alb
    Serial.print("\n");
    Serial.print("Finish line! Correct words: ");
    Serial.println(correctWordCount); // afișeaza numărul de cuvinte scrise corect
}
```

### Funcția de schimbare a dificultății

```cpp
void changeGameDifficulty() {
    if (isGameRunning) return; //Dacă jocul rulează

    if (millis() - lastDifficultyChangeTime > debounceInterval) { //verific intervalul de timp de la ultima apăsare să fie mai mare decât cel de debouncing definit la început
        currentDifficultyLevel = static_cast<Difficulty>((currentDifficultyLevel + 1) % 3); // reține dificultatea aleasă pentru a putea schimba timpul de afișare al cuvintelor și pentru a afișa ulterior mesajul cu dificultatea aleasă (prin switch-ul de mai jos)

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
        lastDifficultyChangeTime = millis(); //actualizează timpul ultimei schimbări de dificultate
    }
}
```

### Set-up:

În set-up sunt setate: LED-ul RGB drept output:

```cpp
pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);

    pinMode(START_STOP_BUTTON, INPUT_PULLUP);
    pinMode(DIFFICULTY_BUTTON, INPUT_PULLUP);
```

și butoanele drept input și este aprins LED-ul RGB in culoarea alb:

```cpp
    setLEDColor(255, 255, 255);
```

### Funcția Loop:

```cpp
void loop() {
    // Verifică dacă butonul de Start/Stop este apăsat
    if (digitalRead(START_STOP_BUTTON) == LOW) {
        // Se asigură că butonul este apăsat doar o dată (debounce)
        if (!isStartStopButtonPressed && millis() - lastDifficultyChangeTime > debounceInterval) {
            lastDifficultyChangeTime = millis(); // Actualizează timpul ultimei schimbări de dificultate
            isStartStopButtonPressed = true; // Marchează butonul ca fiind apăsat

            // Dacă jocul nu este activ, începe o nouă rundă
            if (!isGameRunning) {
                startNewRound();
            } else {
                // Dacă jocul este activ, oprește runda curentă
                stopCurrentRound();
            }
        }
    } else {
        // Resetează starea butonului când acesta nu este apăsat
        isStartStopButtonPressed = false;
    }

    // Verifică dacă butonul de schimbare a dificultății este apăsat
    if (digitalRead(DIFFICULTY_BUTTON) == LOW) {
        if (!isDifficultyButtonPressed) {
            isDifficultyButtonPressed = true; // Marchează butonul ca fiind apăsat
            changeGameDifficulty(); // Schimbă dificultatea jocului
        }
    } else {
        // Resetează starea butonului când acesta nu este apăsat
        isDifficultyButtonPressed = false;
    }

    // Verifică dacă jocul este activ
    if (isGameRunning) {
        // Dacă timpul rundei este mai mare decât cel de 30 de secunde se oprește runda
        if ((millis() - roundStartTime) > ROUND_TIME) {
            stopCurrentRound();
        } else {
            // altfel verifică dacă timpul pentru cuvântul curent a expirat pentru a știi daca se schimbă cuvântul curent
            if (millis() - currentWordStartTime > wordDisplayTime) {
                // Se ia un cuvânt aleatoriu din listă
                currentWord = getRandomWord();

                Serial.print("\n");
                Serial.print("Word: ");
                Serial.println(currentWord);

                // Resetează timpul de început pentru cuvântul curent
                currentWordStartTime = millis();
                userInputWord = ""; // Resetează inputul utilizatorului
                setLEDColor(0, 255, 0); // Aprinde LED-ul verde pentru a indica un nou cuvânt
            }

            // Verifică dacă există date disponibile în Serial pentru a citi inputul utilizatorului
            while (Serial.available() > 0) {
                char ch = Serial.read(); // Citește caracterul din Serial

                // Verifică dacă utilizatorul apasă Enter
                if (ch == '\n') {
                    userInputWord.trim(); // Elimină spațiile de la început și sfârșit

                    // Afișează cuvântul introdus
                    Serial.print("Entered word: ");
                    Serial.println(userInputWord);

                    // Verifică dacă cuvântul introdus e corect
                    if (isInputWordCorrect(userInputWord)) {
                        correctWordCount++; // Incrementează numărul de cuvinte corecte
                        Serial.println("Correct!");
                        setLEDColor(0, 255, 0);

                        // Obține un nou cuvânt aleatoriu
                        currentWord = getRandomWord();
                        Serial.print("Word: ");
                        Serial.println(currentWord);
                        currentWordStartTime = millis(); // Resetează timpul de început pentru cuvântul curent
                        userInputWord = ""; // Resetează inputul utilizatorului
                    } else {
                        // altfel afișează un mesaj pentru input incorect
                        setLEDColor(255, 0, 0);
                        Serial.println("Incorrect!");
                    }
                } else if (ch == 8 || ch == 127) { // Verifică dacă tasta Backspace este apăsată, iar dacă există caractere în input...
                    if (userInputWord.length() > 0) {
                        userInputWord.remove(userInputWord.length() - 1); // ...elimină ultimul caracter
                        Serial.print("\b \b"); // Șterge caracterul din Serial Monitor
                    }
                } else {
                    userInputWord += ch; // adaugă caracterul la inputul utilizatorului

                    // Verifică dacă inputul utilizatorului este corect față de cuvântul curent
                    if (userInputWord.length() > currentWord.length() ||
                        userInputWord.charAt(userInputWord.length() - 1) != currentWord.charAt(userInputWord.length() - 1)) {
                        // Aprinde LED-ul roșu dacă inputul este greșit
                        setLEDColor(255, 0, 0);
                    } else {
                        // Aprinde LED-ul verde dacă inputul este corect
                        setLEDColor(0, 255, 0);
                    }
                    // Afișează caracterul in Serial Monitor
                    Serial.print(ch);
                }
            }
        }
    }
}
```

## Poze

![IMG_0431](https://github.com/user-attachments/assets/ef3fb9ca-e4d6-4c33-9024-86011ad27aac)
![IMG_0432](https://github.com/user-attachments/assets/ff94093c-bfe3-47f9-99f2-e50ac18403d6)
![IMG_0433](https://github.com/user-attachments/assets/304177c5-b454-4f4c-a688-55edc2e4a3b8)
![IMG_0434](https://github.com/user-attachments/assets/3b43bb23-7408-4bff-befa-f8aaa6bfdf15)


## Video

Videoclipul poate fi văzut prin accesarea link-ului:
[Video pe YouTube](https://www.youtube.com/watch?v=Bijfz-eLBYU&ab_channel=AlexV)

## Schema Circuitului

Pentru a vizualiza schema circuitului în Tinkercad, accesați următorul link:
[Schema circuitului pe Tinkercad](https://www.tinkercad.com/things/1Rt8Drf8ZVi-tema-2-typeracer?sharecode=5dil5ZWgb8ymbeFlN0tixIzp3hs4Y5ax1k8WslOLGZ4)
