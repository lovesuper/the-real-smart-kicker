// the_real_smart_kicker.ino

// Бибилиотека для работы с последовательным интерфейсом
#include <SoftwareSerial.h>

/*
    Фиксация гола
*/

#define LDR_BLUE_PIN A0 // Фоторезистор синих ворот
#define LDR_RED_PIN A1 // Фоторезистор красных ворот

/*
    Проигрывание звуков
*/

#define SND_TX 3 // RX для MP3-плеера
#define SND_RX 4 // TX для MP3-плеера

/*
    Управление игрой
*/


/*
    Настройки игры
*/

#define BLUE_GOAL_THRESHOLD 800
#define RED_GOAL_THRESHOLD 800

// Мячи подряд
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

// Спец-ситуации на поле
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

SoftwareSerial soundPlayer(SND_TX, SND_RX);
static uint8_t cmdbuf[8] = {0}; // буфер для хранения команды mp3 плееру

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

void sound_player_command(int8_t cmd, int16_t data) {
    /*
        Билдер команд для mp3-плеера
    */

    delay(20);
    cmdbuf[0] = 0x7e; // стартовый байт команды

    cmdbuf[1] = 0xFF; // информация о версии API
    cmdbuf[2] = 0x06; // длина команды (в байтах)
    cmdbuf[3] = cmd; // команда (hex)
    cmdbuf[4] = 0x00; // 0x00 = без фидбэка, 0x01 = с фидбэком
    cmdbuf[5] = (int8_t)(data >> 8); // DATA1
    cmdbuf[6] = (int8_t)(data); // DATA2

    cmdbuf[7] = 0xef; // конечный байт команды

    for (uint8_t i = 0; i < 8; i++) {
      soundPlayer.write(cmdbuf[i]);
    }
}

void setup() {
    // Выставляем общение с serial портом на частоте 9600 бод
    Serial.begin(9600);

    Serial.print("\nSTARTING SERIAL DEBUG MODE\n");
    // Даем плееру загрузиться
    delay(500);
    soundPlayer.begin(9600);
    // set up SD Card
    sound_player_command(0x09, 0x0002);
    delay(200);

    // set up volume
    sound_player_command(0x06, 0x1D);

    // set up start song
    sound_player_command(0x0F, 0x0118);

}

void play_sound(int goals_count){
    switch (goals_count) {
        case 1:
            sound_player_command(0x0F, 0x0101);
            break;
        case 2:
            sound_player_command(0x0F, 0x0102);
            break;
        case 3:
            sound_player_command(0x0F, 0x0103);
            break;
        case 4:
            sound_player_command(0x0F, 0x0104);
            break;
        case 5:
            sound_player_command(0x0F, 0x0105);
            break;
        case 6:
            sound_player_command(0x0F, 0x0106);
            break;
        case 7:
            sound_player_command(0x0F, 0x0107);
            break;
        case 8:
            sound_player_command(0x0F, 0x0108);
            break;
        case 9:
            sound_player_command(0x0F, 0x0109);
            break;
        case FIRST_BLOOD:
            sound_player_command(0x0F, 0x010B);
            break;
        case TWO_GOALS_IN_ROW:
            sound_player_command(0x0F, 0x010C);
            break;
        case THREE_GOALS_IN_ROW:
            sound_player_command(0x0F, 0x010D);
            break;
        case FOUR_GOALS_IN_ROW:
            sound_player_command(0x0F, 0x010E);
            break;
        case FIVE_GOALS_IN_ROW:
            sound_player_command(0x0F, 0x010F);
            break;
        case SIX_GOALS_IN_ROW:
            sound_player_command(0x0F, 0x0110);
            break;
        case SEVEN_GOALS_IN_ROW:
            sound_player_command(0x0F, 0x0111);
            break;
        case EIGHT_GOALS_IN_ROW:
            sound_player_command(0x0F, 0x0112);
            break;
        case NINE_GOALS_IN_ROW:
            sound_player_command(0x0F, 0x0113);
            break;
        case FLAWLESS_VICTORY:
            sound_player_command(0x0F, 0x0115);
            break;
        case RED_TEAM_WIN:
            sound_player_command(0x0F, 0x011E);
            break;
        case BLUE_TEAM_WIN:
            sound_player_command(0x0F, 0x011F);
            break;
        case THREE_FAST_GOALS:
            sound_player_command(0x0F, 0x0114);
            break;
        case LONG_TIME_NO_GOALS:
            sound_player_command(0x0F, 0x011D);
            break;
        case HOPELESS:
            sound_player_command(0x0F, 0x0120);
            break;
        default:
            sound_player_command(0x0F, 0x0110);
    }
}
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
    if((blueGoals + redGoals) == 1) {
        play_sound(FIRST_BLOOD);
        return true;
    } else if (redFastGoalsCount == 3 || blueFastGoalsCount == 3) {
        play_sound(THREE_FAST_GOALS);
        if(redFastGoalsCount == 3)
            redFastGoalsCount = 0;
        else
            blueFastGoalsCount = 0;

        return true;
    } else if (blueGoaledInRow == 2 || redGoaledInRow == 2) {
        play_sound(TWO_GOALS_IN_ROW);
        return true;
    } else if (blueGoaledInRow == 3 || redGoaledInRow == 3) {
        play_sound(THREE_GOALS_IN_ROW);
        return true;
    } else if (blueGoaledInRow == 4 || redGoaledInRow == 4) {
        play_sound(FOUR_GOALS_IN_ROW);
        return true;
    } else if (blueGoaledInRow == 5 || redGoaledInRow == 5) {
        play_sound(FIVE_GOALS_IN_ROW);
        return true;
    } else if (blueGoaledInRow == 6 || redGoaledInRow == 6) {
        play_sound(SIX_GOALS_IN_ROW);
        return true;
    } else if (blueGoaledInRow == 7 || redGoaledInRow == 7) {
        play_sound(SEVEN_GOALS_IN_ROW);
        return true;
    } else if (blueGoaledInRow == 8 || redGoaledInRow == 8) {
        play_sound(EIGHT_GOALS_IN_ROW);
        return true;
    } else if (blueGoaledInRow == 9 || redGoaledInRow == 9) {
        play_sound(NINE_GOALS_IN_ROW);
        return true;
    }
    return false;
}

void resetMatch(){
    delay(200);
    if (blueGoals > redGoals){
        play_sound(RED_TEAM_WIN);
        delay(3000);
    } else{
        play_sound(BLUE_TEAM_WIN);
        delay(3000);
    }


    if (blueGoaledInRow == 10 || redGoaledInRow == 10)
        play_sound(FLAWLESS_VICTORY);

    blueGoals = 0;
    redGoals = 0;

    blueGoaled = false;
    redGoaled = false;

    blueGoaledInRow = 0;
    redGoaledInRow = 0;

    redFastGoalsCount = 0;
    blueFastGoalsCount = 0;
}

void loop() {
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
            play_sound(blueGoals);
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
            play_sound(redGoals);
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
        play_sound(LONG_TIME_NO_GOALS);
        if(thresholdOfHopelessness == secondThresholdOfHopelessness){
            delay(1500);
            play_sound(HOPELESS);
        }

        pointsOfHopelessness = 0;
        thresholdOfHopelessness = secondThresholdOfHopelessness;
    }
}
