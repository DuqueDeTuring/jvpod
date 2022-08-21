const int FREE_ANALOG_PIN = A2;

const int RANDOM_PLAY_PIN = A3;
int prevRandomButton = HIGH;

// Folders and episodes
const int MAX_FOLDERS = 99;
int filesPerFolder[MAX_FOLDERS];
int totalPodcasts = 0;
int totalEpisodios = 0;
int currentFolder = 1;
int playingFileNumber = 1;
int lastFolder = -1;

// Volume
const int INIT_VOLUME = 12;
const int MIN_VOLUME = 0;
const int MAX_VOLUME = 30;
int prevVolAnalogValue = 0;
const int analogVolumeChgThreshold = 3; // The analog reading of the potentiometer value "jitters" a little (expected), we will use this to compensate
int volume = INIT_VOLUME;
boolean playing = false;
boolean restartPlay = true;

// Keyboard related
const int NO_KEY = -1;
const int KEY_UP = 1;
const int KEY_DOWN = 2;
const int KEY_LEFT = 3;
const int KEY_RIGHT = 4;
const int KEY_SELECT = 5;
int prevKey = NO_KEY;
const int MIN_KEY_PRESS_TIME = 50;
long lastKeyPressed = MIN_KEY_PRESS_TIME;

// LCD Shield
const int pin_RS = 8;
const int pin_EN = 9;
const int pin_d4 = 4;
const int pin_d5 = 5;
const int pin_d6 = 6;
const int pin_d7 = 7;
const int pin_BL = 10;


enum customChar {
  pauseChar = 0,
  playChar,
  stopChar,
  vol1Char,
  vol2Char,
  vol3Char,
  vol4Char
};

byte pauseSymbol[8] = {
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
};

byte playSymbol[8] = {
  B11000,
  B11100,
  B11110,
  B11111,
  B11111,
  B11110,
  B11100,
  B11000,
};

byte stopSymbol[8] = {
  B00000,
  B00000,
  B01110,
  B01110,
  B01110,
  B01110,
  B00000,
  B00000,
};

byte vol1Symbol[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
};

byte vol2Symbol[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte vol3Symbol[8] = {
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte vol4Symbol[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};


const int UPDATE_INTERVAL = 50;
int prevUpdate = 0;
boolean shouldUpdateScreen = true;

String error = "";


class Mp3Notify;
typedef DFMiniMp3<SoftwareSerial, Mp3Notify> DfMp3;
SoftwareSerial playerSerial(11, 13); // RX, TX
DfMp3 player(playerSerial);

LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);
