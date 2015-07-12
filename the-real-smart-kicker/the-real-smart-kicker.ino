#include <Arduino.h>
#include <SoftwareSerial.h>
#include <JQ6500_Serial.h>

// LEDS [next version]

//#define RED_LED_DATA 11
//#define RED_LED_LATCH 10
//#define RED_LED_CLOCK 9
//
//#define BLUE_LED_DATA 8
//#define BLUE_LED_LATCH 7
//#define BLUE_LED_CLOCK 6

//

#define LDR_BLUE_PIN A0
#define LDR_RED_PIN A1

// Mp3 value control
#define POT_VALUE_CONTROL A2

//Real
#define BLUE_GOAL_THRESHOLD 600
#define RED_GOAL_THRESHOLD 600

////Test
//#define BLUE_GOAL_THRESHOLD 620
//#define RED_GOAL_THRESHOLD 620

#define GOALS_REVENGE_DELAY 10000

JQ6500_Serial mp3(12,13);

// (Комментарии) Мячи подряд
#define FIRST_BLOOD 11
#define TWO_GOALS_IN_ROW 12
#define THREE_GOALS_IN_ROW 13
#define FOUR_GOALS_IN_ROW 14
#define FIVE_GOALS_IN_ROW 15
#define SIX_GOALS_IN_ROW 16
#define SEVEN_GOALS_IN_ROW 17
#define EIGHT_GOALS_IN_ROW 18
#define NINE_GOALS_IN_ROW 19
#define RED_TEAM_WIN 30
#define BLUE_TEAM_WIN 31
#define HOPELESS 32

// (Комментарии) Спец-ситуации на поле
#define THREE_FAST_GOALS 20
#define FLAWLESS_VICTORY 21
#define FAST_REVENGE_GOAL 22
#define FIVE_NINE 23
#define GAME_STARTED 24
#define EIGHT_NINE 25
#define NO_FLAWLESS_VICTORY 26
#define FAST_GOAL 27
#define RAPID_EXCHANGE 28
#define LONG_TIME_NO_GOALS 29

#define GOAL_SOUNDS_START 33
#define GOAL_SOUNDS_END 42

#define APPL_1 43
#define APPL_2 44
#define CHICKEN 47
#define TROMBONE 48

boolean blueGoaled = false; // синим забили
boolean redGoaled = false; // красным забили

int blueGoaledInRow = 0; // забитых синим подряд мячей
int redGoaledInRow = 0; // забитых красным подряд мячей

int blueGoals = 0; // всего забитых синим мячей
int redGoals = 0; // всего забитых красным мячей

int redFastGoalsCount = 0;
int blueFastGoalsCount = 0;

int redLastGoalMills = 0;
int blueLastGoalMills = 0;

long pointsOfHopelessness = 0;
long firstThresholdOfHopelessness = 200000;
long secondThresholdOfHopelessness = 100000;
long thresholdOfHopelessness = firstThresholdOfHopelessness;

void setup() {
  mp3.begin(9600);
  Serial.begin(230400);
  mp3.reset();
  mp3.setVolume(17);
  mp3.playFileByIndexNumber(GAME_STARTED);

//[next version]
//  pinMode(RED_LED_DATA, OUTPUT);
//  pinMode(RED_LED_LATCH, OUTPUT);
//  pinMode(RED_LED_CLOCK, OUTPUT);
//  pinMode(BLUE_LED_DATA, OUTPUT);
//  pinMode(BLUE_LED_LATCH, OUTPUT);
//  pinMode(BLUE_LED_CLOCK, OUTPUT);
//  showBlueNumber(0);
//  showRedNumber(0);

  Serial.print("Debugging TRSK starts here:");
  Serial.print("\n");
}

//[next version]
//byte segments[10] = {
//  0b00001000, 0b01101011, 0b01000100, 0b01000001, 0b00100011,
//  0b00010001, 0b00010000, 0b01001011, 0b00000000, 0b00000001
//};

//[next version]
//void showRedNumber(int number){
//  digitalWrite(RED_LED_LATCH, LOW);
//  shiftOut(RED_LED_DATA, RED_LED_CLOCK, LSBFIRST, segments[number]);
//  digitalWrite(RED_LED_LATCH, HIGH);
//}
//
//[next version]
//void showBlueNumber(int number){
//  digitalWrite(BLUE_LED_LATCH, LOW);
//  shiftOut(BLUE_LED_DATA, BLUE_LED_CLOCK, LSBFIRST, segments[number]);
//  digitalWrite(BLUE_LED_LATCH, HIGH);
//}

void printDebugInfo(char* team, int goals, int ldrValue) {
    char msg[40];
    sprintf(msg, "%s team goaled %d with LDR volume %d\n", team, goals, ldrValue);
    Serial.print(msg);
}

boolean isRedGoaled() {
    int ldrRedLightness = analogRead(LDR_RED_PIN);
    boolean result = ldrRedLightness < RED_GOAL_THRESHOLD;
    if(result) {
        printDebugInfo("Red", redGoals + 1, ldrRedLightness);
    }

    return result;
}

boolean isBlueGoaled() {
    int ldrBlueLightness = analogRead(LDR_BLUE_PIN);
    boolean result = ldrBlueLightness < BLUE_GOAL_THRESHOLD;

    if(result) {
        printDebugInfo("Blue", blueGoals + 1, ldrBlueLightness);
    }

    return result;
}

boolean checkSpecial() {
   int randNum = rand()%(GOAL_SOUNDS_END-GOAL_SOUNDS_START + 1) + GOAL_SOUNDS_START;
    mp3.playFileByIndexNumber(randNum);
    delay(2000);

    // Проверка на быстрый обмен голами
    int goalsDelay = abs(redLastGoalMills - blueLastGoalMills);
    if(goalsDelay < GOALS_REVENGE_DELAY && redLastGoalMills != 0 && blueLastGoalMills !=0){
       mp3.playFileByIndexNumber(RAPID_EXCHANGE);
       return true;
    }

    // Проверка на первый гол
    int fastGoalsCount = redFastGoalsCount + blueFastGoalsCount;
    if((blueGoals + redGoals) == 1) {
       mp3.playFileByIndexNumber(FIRST_BLOOD);
       delay(2000);
       return true;
    // Проверка на мячи подряд (12-21)
    } else if (fastGoalsCount > 0) {
       mp3.playFileByIndexNumber(10 + fastGoalsCount);
       return true;
    }

    return false;
}

void resetMatch(){
    if (blueGoals > redGoals){
       mp3.playFileByIndexNumber(RED_TEAM_WIN);
    } else{
       mp3.playFileByIndexNumber(BLUE_TEAM_WIN);
    }
    delay(2000);
    if (blueGoaledInRow + redGoaledInRow == 10){
       mp3.playFileByIndexNumber(FLAWLESS_VICTORY);
       delay(2000);
    }
    blueGoals = 0;
    redGoals = 0;
    blueGoaled = false;
    redGoaled = false;
    blueGoaledInRow = 0;
    redGoaledInRow = 0;
    redFastGoalsCount = 0;
    blueFastGoalsCount = 0;
// [next version]
//    showBlueNumber(0);
//    showRedNumber(0);
   mp3.playFileByIndexNumber(GAME_STARTED);
}

void loop() {
//    int soundValue = map(analogRead(POT_VALUE_CONTROL), 0, 1023, 0, 30);
//    mp3.setVolume(soundValue);
    if(isBlueGoaled()){
        blueGoaled = true;
        blueGoals++;
        blueGoaledInRow++;
        redGoaledInRow = 0;
        pointsOfHopelessness = 0;
        // [next version]
        // showBlueNumber(blueGoals);
        if((millis() - redLastGoalMills) < 8000){
            redFastGoalsCount++;
            Serial.print(">>> Red fast goal! number ");
            Serial.print(redFastGoalsCount);
            Serial.print("\n");
        } else {
            redFastGoalsCount = 0;
        }

        if (!checkSpecial()) {
            mp3.playFileByIndexNumber(blueGoals);
            delay(1000);
        }
        redLastGoalMills = millis();
    }

    if(isRedGoaled()){
        pointsOfHopelessness = 0;
        redGoaled = true;
        redGoals++;
        redGoaledInRow++;
        blueGoaledInRow = 0;
        // [next version]
        // showRedNumber(redGoals);
       if((millis() - blueLastGoalMills) < 8000){
           blueFastGoalsCount++;
           Serial.print(">>> Blue fast goal! number ");
           Serial.print(blueFastGoalsCount);
           Serial.print("\n");
       } else {
           blueFastGoalsCount = 0;
       }
       blueLastGoalMills = millis();

        if (!checkSpecial()) {
            mp3.playFileByIndexNumber(redGoals);
            delay(1000);
        }
    }

    if(blueGoals >= 10 || redGoals >= 10) {
       resetMatch();
    }

    if(!isRedGoaled() && !isBlueGoaled()){
       pointsOfHopelessness++;
    }

    if(pointsOfHopelessness >= thresholdOfHopelessness) {
       mp3.playFileByIndexNumber(LONG_TIME_NO_GOALS);
       if(thresholdOfHopelessness == secondThresholdOfHopelessness){
           delay(1500);
           mp3.playFileByIndexNumber(HOPELESS);
       }

       pointsOfHopelessness = 0;
       thresholdOfHopelessness = secondThresholdOfHopelessness;
    }
}
