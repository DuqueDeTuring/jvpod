#include "jvpod.h"


SoftwareSerial secondarySerial(11, 13); // RX, TX
DfMp3 player(secondarySerial);


const int MAX_FOLDERS = 99;
int filesPerFolder[MAX_FOLDERS];
int totalPodcasts = 0;
int currentFolder = 1;
int playingFileNumber = 1;
int lastFolder = -1;

const int INIT_VOLUME = 12;
const int MIN_VOLUME = 1;
const int MAX_VOLUME = 29;
int volume = INIT_VOLUME;
boolean playing = false;
boolean restartPlay = true;



const int NO_KEY = -1;
const int KEY_UP = 1;
const int KEY_DOWN = 2;
const int KEY_LEFT = 3;
const int KEY_RIGHT = 4;
const int KEY_SELECT = 5;
int prevKey = NO_KEY;
const int MIN_KEY_PRESS_TIME = 50;
long lastKeyPressed = MIN_KEY_PRESS_TIME;

const int pauseChar = 0;
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

const int playChar = 1;
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


const int UPDATE_INTERVAL = 50;
int prevUpdate = 0;
boolean shouldUpdateScreen = true;

//LCD to Arduino
const int pin_RS = 8;
const int pin_EN = 9;
const int pin_d4 = 4;
const int pin_d5 = 5;
const int pin_d6 = 6;
const int pin_d7 = 7;
const int pin_BL = 10;
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);


void splash() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Bienvenido al JVpod 1.0!");
  for (int positionCounter = 0; positionCounter < 18; positionCounter++) {
    lcd.scrollDisplayLeft();
    delay(250);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("JVpod 1.0!");
  lcd.setCursor(0, 1);
  lcd.print("por favor espere");
}

void setup() {
  Serial.begin(115200);
  Serial.println("initializing...");

  player.begin();
  player.reset();
  secondarySerial.begin(9600);

  lcd.begin(16, 2);
  lcd.createChar(pauseChar, pauseSymbol);
  lcd.createChar(playChar, playSymbol);

  splash();
  countPerFolder();
  lcd.clear();
  player.setVolume(INIT_VOLUME);
}

void loop() {
  updateVolume();
  int k = readFromKeypad();
  if (k == NO_KEY && prevKey != NO_KEY) {
    actOnKey(prevKey);
  }
  prevKey = k;

  if (shouldUpdateScreen && ((millis() - prevUpdate) > UPDATE_INTERVAL)) {
    prevUpdate = millis();
    //    int currentFileNumber = player.readCurrentFileNumber();
    //    if (playing && (currentFileNumber != playingFileNumber)) {
    //      Serial.println("Actruali fn" + String(playingFileNumber));
    //      playingFileNumber = currentFileNumber;
    //      shouldUpdateScreen = true;
    //    }
    updateScreen();
  }
}

void countPerFolder() {
  for (int i = 0; i < MAX_FOLDERS; i++) {
    filesPerFolder[i] = 0;
  }

  int folder = 1;
  int count = -1;
  while (count != 0) {
    Serial.println("contando en folder:" + String(folder));
    count = player.getFolderTrackCount(folder);

    Serial.println("\tfiles:" + String(count));
    if (count > 0) {
      filesPerFolder[folder - 1] = count;
      folder++;
    }
  }

  totalPodcasts = folder - 1;
  Serial.println("Total folders:" + String(totalPodcasts));
  Serial.println("Reset...");
  player.reset();
}


int readFromKeypad() {
  int v = analogRead(0);
  //  Serial.println(v);
  //  if (  v > 740 &&   v < 745) {
  //    return KEY_SELECT;
  //  } else if (  v > 500 &&   v < 510) {
  //    return KEY_LEFT;
  //  } else if (  v < 10) {
  //    return KEY_RIGHT;
  //  } else if (  v > 140 &&   v < 150) {
  //    return KEY_UP;
  //  } else if (  v > 320 &&   v < 365) {
  //    return KEY_DOWN;
  //  } else {
  //    return NO_KEY;
  //  }

  if (v < 60) {
    return KEY_RIGHT;
  }
  else if (v < 200) {
    return KEY_UP;
  }
  else if (v < 400) {
    return KEY_DOWN;
  }
  else if (v < 600) {
    return KEY_LEFT;
  }
  else if (v < 800) {
    return KEY_SELECT;
  } else {
    return NO_KEY;
  }
}

void actOnKey(int k) {
  if (((millis() - lastKeyPressed) < MIN_KEY_PRESS_TIME) || (k == NO_KEY)) {
    return;
  }
  lastKeyPressed = millis();

  if (k == KEY_UP) {
    folderUp();
  } else if (k == KEY_DOWN) {
    folderDown();
  } else if (k == KEY_SELECT) {
    togglePlaying();
  } else if (k == KEY_LEFT) {
    playPrevious();
  } else if (k == KEY_RIGHT) {
    playNext();
  }
}

void playPrevious() {
  pause();
  Serial.println("-----");
  Serial.println("currentFolder:" + String(currentFolder));
  Serial.println("play previous");
  Serial.println("playingFileNumber:" + String(playingFileNumber));
  if (playingFileNumber == 1) {
    playingFileNumber = filesPerFolder[currentFolder - 1];
  } else {
    playingFileNumber--;
  }
  Serial.println("nuevo:" + String(playingFileNumber));
  Serial.println("-----");
  Serial.println("");
  // player.playFolderTrack(currentFolder, playingFileNumber);
  shouldUpdateScreen = true;

}

void playNext() {
  pause();
  Serial.println("-----");
  Serial.println("currentFolder:" + String(currentFolder));
  Serial.println("playnext");
  Serial.println("playingFileNumber:" + String(playingFileNumber));
  if (playingFileNumber == filesPerFolder[currentFolder - 1]) {
    playingFileNumber = 1;
  } else {
    playingFileNumber++;
  }
  Serial.println("nuevo:" + String(playingFileNumber));
  Serial.println("-----");
  Serial.println("");
  //player.playFolderTrack(currentFolder, playingFileNumber);
  shouldUpdateScreen = true;

}


void pause() {
  Serial.println("PAUSA!:" + String(playing));

  if (playing) {
    playing = false;
    player.pause();
    shouldUpdateScreen = true;
  }
}

void togglePlaying() {
  Serial.println("toggle. restartPlay:" + String(restartPlay) + ",playing:" + String(playing));
  shouldUpdateScreen = true;
  if (restartPlay)  {
    restartPlay = false;
    playing = true;
    player.playFolderTrack(currentFolder, playingFileNumber);
    return;
  }
  //  Serial.println(player.readState());
  if (playing) {
    pause();
  } else {
    playing = true;
    player.start();
  }
  waitMilliseconds(200);
}

void folderUp() {
  pause();
  playingFileNumber = 1;

  if (currentFolder < totalPodcasts) {
    currentFolder++;
  } else {
    currentFolder = 1;
  }
  Serial.println("Nexto podcast:" + String(currentFolder));
  restartPlay = true;
  shouldUpdateScreen = true;
}


void folderDown() {
  pause();
  playingFileNumber = 1;
  if (currentFolder > 1) {
    currentFolder--;
  } else {
    currentFolder = totalPodcasts;
  }
  Serial.println("Prev podcast:" + String(currentFolder));
  restartPlay = true;
  shouldUpdateScreen = true;
}


void updateScreen() {
  shouldUpdateScreen = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Episodio: " + String(playingFileNumber));
  lcd.setCursor(15, 0);
  if (!playing) {
    lcd.write(pauseChar);
  } else {
    lcd.write(playChar);
  }

  lcd.setCursor(0, 1);
  lcd.print("Podcast:" + String(currentFolder));

}


void updateVolume() {
  int analogValue = analogRead(A5);
  Serial.println("Volumne:" + String(analogValue));
  int selectedVolume = map(analogValue, 0, 1023, MIN_VOLUME, MAX_VOLUME);
  if (selectedVolume != volume) {
    volume = selectedVolume;
    player.setVolume(volume);
  }
}

void waitMilliseconds(uint16_t msWait)
{
  uint32_t start = millis();

  while ((millis() - start) < msWait)
  {
    // if you have loops with delays, its important to
    // call dfmp3.loop() periodically so it allows for notifications
    // to be handled without interrupts
    player.loop();
    delay(1);
  }
}
