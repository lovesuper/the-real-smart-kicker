#include <Arduino.h>
#include <SoftwareSerial.h>
#include <JQ6500_Serial.h>

#define LDR_BLUE_PIN A0
#define LDR_RED_PIN A1
#define POT_VALUE_CONTROL A2

#define BLUE_GOAL_THRESHOLD 350
#define RED_GOAL_THRESHOLD 350

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
#define COW_SOUND 43
#define CARRY_ON 44
#define FLO 45
#define APL 46
#define ZOID 47

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
  mp3.reset();
  mp3.setVolume(18);
  mp3.playFileByIndexNumber(GAME_STARTED);
}

boolean isRedGoaled() {
    int ldrRedLightness = analogRead(LDR_RED_PIN);

    boolean result = ldrRedLightness < RED_GOAL_THRESHOLD;
    if(result) {
        printDebugInfo("Red", redGoals + 1, ldrRedLightness);
    }

    return result;
}
void printDebugInfo(char* team, int goals, int ldrValue) {
    char msg[40];
    sprintf(msg, "%s team goaled %d with LDR volume %d\n", team, goals, ldrValue);
    Serial.print(msg);
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
    int goalsDelay = abs(redLastGoalMills - blueLastGoalMills);
    if(goalsDelay < 10000 && redLastGoalMills != 0 && blueLastGoalMills !=0){
        mp3.playFileByIndexNumber(RAPID_EXCHANGE);
        return true; 
     }
    
    if((blueGoals + redGoals) == 1) {
        mp3.playFileByIndexNumber(FIRST_BLOOD);
        delay(2000);
        mp3.playFileByIndexNumber(APL);
        return true;
    } else if (redFastGoalsCount == 3 || blueFastGoalsCount == 3) {
        mp3.playFileByIndexNumber(THREE_FAST_GOALS);
        if(redFastGoalsCount == 3)
            redFastGoalsCount = 0;
        else
            blueFastGoalsCount = 0;

        return true;
    } else if (blueGoaledInRow == 2 || redGoaledInRow == 2) {
        mp3.playFileByIndexNumber(TWO_GOALS_IN_ROW);
        delay(2000);
        mp3.playFileByIndexNumber(APL);
        return true;
    } else if (blueGoaledInRow == 3 || redGoaledInRow == 3) {
        mp3.playFileByIndexNumber(THREE_GOALS_IN_ROW);
        return true;
    } else if (blueGoaledInRow == 4 || redGoaledInRow == 4) {
        mp3.playFileByIndexNumber(FOUR_GOALS_IN_ROW);
        return true;
    } else if (blueGoaledInRow == 5 || redGoaledInRow == 5) {
        mp3.playFileByIndexNumber(FIVE_GOALS_IN_ROW);
        return true;
    } else if (blueGoaledInRow == 6 || redGoaledInRow == 6) {
        mp3.playFileByIndexNumber(SIX_GOALS_IN_ROW);
        delay(2000);
        mp3.playFileByIndexNumber(CARRY_ON);
        delay(2000);
        return true;
    } else if (blueGoaledInRow == 7 || redGoaledInRow == 7) {
        mp3.playFileByIndexNumber(SEVEN_GOALS_IN_ROW);
        return true;
    } else if (blueGoaledInRow == 8 || redGoaledInRow == 8) {
        mp3.playFileByIndexNumber(EIGHT_GOALS_IN_ROW);
        return true;
    } else if (blueGoaledInRow == 9 || redGoaledInRow == 9) {
        mp3.playFileByIndexNumber(NINE_GOALS_IN_ROW);
        return true;
    }
    return false;
}

void resetMatch(){
    delay(200);
    if (blueGoals > redGoals){
        mp3.playFileByIndexNumber(RED_TEAM_WIN);
        delay(2000);
        mp3.playFileByIndexNumber(ZOID);
        delay(3000);
    } else{
        mp3.playFileByIndexNumber(BLUE_TEAM_WIN);
        delay(1000);
        mp3.playFileByIndexNumber(ZOID);
        delay(3000);
    }

    if (blueGoaledInRow == 10 || redGoaledInRow == 10){
        mp3.playFileByIndexNumber(FLAWLESS_VICTORY);
        delay(2000);
        mp3.playFileByIndexNumber(COW_SOUND);
    }
    blueGoals = 0;
    redGoals = 0;

    blueGoaled = false;
    redGoaled = false;

    blueGoaledInRow = 0;
    redGoaledInRow = 0;

    redFastGoalsCount = 0;
    blueFastGoalsCount = 0;

    mp3.playFileByIndexNumber(GAME_STARTED);
}

void loop() {
    int soundValue = map(analogRead(POT_VALUE_CONTROL), 0, 1023, 0, 30);
    mp3.setVolume(soundValue);
    if(isBlueGoaled()){
        blueGoaled = true;
        blueGoals++;
        blueGoaledInRow++;
        redGoaledInRow = 0;
        pointsOfHopelessness = 0;
        if((millis() - redLastGoalMills) < 6000){
            redFastGoalsCount++;
            Serial.print(">Red fast goal number ");
            Serial.print(redFastGoalsCount);
            Serial.print("\n");
        } else {
            redFastGoalsCount = 0;
        }

        if (!checkSpecial()) {
            mp3.playFileByIndexNumber(blueGoals);
        }
        redLastGoalMills = millis();

        delay(1000);
    }

    if(isRedGoaled()){
        pointsOfHopelessness = 0;
        redGoaled = true;
        redGoals++;
        redGoaledInRow++;
        blueGoaledInRow = 0;
        if((millis() - blueLastGoalMills) < 6000){
            blueFastGoalsCount++;
            Serial.print(">Blue fast goal number ");
            Serial.print(blueFastGoalsCount);
            Serial.print("\n");
        } else {
            blueFastGoalsCount = 0;
        }
        blueLastGoalMills = millis();

        if (!checkSpecial()) {
            mp3.playFileByIndexNumber(redGoals);
        }

        delay(1000);
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

