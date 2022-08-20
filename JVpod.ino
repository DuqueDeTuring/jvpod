#include "Arduino.h"
#include <DFMiniMp3.h>
#include "SoftwareSerial.h"
#include <LiquidCrystal.h>

class Mp3Notify;
typedef DFMiniMp3<SoftwareSerial, Mp3Notify> DfMp3;
SoftwareSerial secondarySerial(11, 13); // RX, TX
DfMp3 player(secondarySerial);


const int RANDOM_PLAY_PIN=A3;
int prevRandomButton = HIGH;

const int MAX_FOLDERS = 99;
int filesPerFolder[MAX_FOLDERS];
int totalPodcasts = 0;
int totalEpisodios = 0;
int currentFolder = 1;
int playingFileNumber = 1;
int lastFolder = -1;

const int INIT_VOLUME = 12;
const int MIN_VOLUME = 0;
const int MAX_VOLUME = 30;
int prevVolAnalogValue = 0;
const int analogVolumeChgThreshold = 3; // The analog reading of the potentiometer value "jitters" a little (expected)
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

const int stopChar = 2;
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

const int vol1Char = 3;
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

const int vol2Char = 4;
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


const int vol3Char = 5;
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

const int vol4Char = 6;
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

//LCD Shield
const int pin_RS = 8;
const int pin_EN = 9;
const int pin_d4 = 4;
const int pin_d5 = 5;
const int pin_d6 = 6;
const int pin_d7 = 7;
const int pin_BL = 10;
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);
String error = "";

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
  randomSeed(analogRead(2));
  Serial.begin(115200);
  Serial.println("initializing...");

  player.begin();
  player.reset();
  secondarySerial.begin(9600);

  lcd.begin(16, 2);
  createCustomCharacters();

  splash();
  countPerFolder();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Se encontraron ");
  lcd.setCursor(0, 1);
  lcd.print(String(totalPodcasts) + " podcasts con un total de " + String(totalEpisodios) + " episodios");
  delay(1000);
  for (int positionCounter = 0; positionCounter < 38; positionCounter++) {
    lcd.scrollDisplayLeft();
    delay(400);  //Scrolling speed
  }
  //delay(3000);

  player.setVolume(INIT_VOLUME);

  // pin for random play
  pinMode(RANDOM_PLAY_PIN, INPUT_PULLUP);
}

void loop() {
  player.loop();
  int randomButton = digitalRead(A3);
  if (randomButton != prevRandomButton) {
    if (randomButton == HIGH) {
      randomPlay();
    }
    prevRandomButton = randomButton;
  }
  updateVolume();
  int k = readFromKeypad();
  if (k == NO_KEY && prevKey != NO_KEY) {
    actOnKey(prevKey);
  }
  prevKey = k;

  if (shouldUpdateScreen && ((millis() - prevUpdate) > UPDATE_INTERVAL)) {
    prevUpdate = millis();
    updateScreen();
  }
}

void countPerFolder() {
  totalEpisodios = 0;
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
      totalEpisodios += count;
      folder++;
    }
  }

  totalPodcasts = folder - 1;
  Serial.println("Total folders:" + String(totalPodcasts));
  Serial.println("Reset player...");
  player.reset();
}


int readFromKeypad() {
  int v = analogRead(0);

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
  //  Serial.println("-----");
  //  Serial.println("currentFolder:" + String(currentFolder));
  //  Serial.println("play previous");
  //  Serial.println("playingFileNumber:" + String(playingFileNumber));
  if (playingFileNumber == 1) {
    playingFileNumber = filesPerFolder[currentFolder - 1];
  } else {
    playingFileNumber--;
  }
  restartPlay = true;
  //  Serial.println("nuevo:" + String(playingFileNumber));
  //  Serial.println("-----");
  //  Serial.println("");
  // player.playFolderTrack(currentFolder, playingFileNumber);
  shouldUpdateScreen = true;

}

void playNext() {
  pause();
  //  Serial.println("-----");
  //  Serial.println("currentFolder:" + String(currentFolder));
  //  Serial.println("playnext");
  //  Serial.println("playingFileNumber:" + String(playingFileNumber));
  if (playingFileNumber == filesPerFolder[currentFolder - 1]) {
    playingFileNumber = 1;
  } else {
    playingFileNumber++;
  }
  restartPlay = true;

  //  Serial.println("nuevo:" + String(playingFileNumber));
  //  Serial.println("-----");
  //  Serial.println("");

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

void stop() {
  player.stop();
  playing = false;
  restartPlay = true;
  shouldUpdateScreen = true;
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
  stop();
  playingFileNumber = 1;

  if (currentFolder < totalPodcasts) {
    currentFolder++;
  } else {
    currentFolder = 1;
  }
  Serial.println("Nexto podcast:" + String(currentFolder));
  //  restartPlay = true;
  //  shouldUpdateScreen = true;
}


void folderDown() {
  stop();
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

  if (error == "") {
    lcd.print("Episodio: " + String(playingFileNumber));
    lcd.setCursor(15, 0);
    if (!playing) {
      if (restartPlay) {
        lcd.write(stopChar);
      } else {
        lcd.write(pauseChar);
      }
    } else {
      lcd.write(playChar);
    }

    lcd.setCursor(0, 1);
    lcd.print("Podcast: " + String(currentFolder));

    lcd.setCursor(15, 1);
    if (volume == 0) {
      lcd.print("");
    } else {
      int v = map(volume, MIN_VOLUME+2, MAX_VOLUME - 2, vol1Char, vol4Char);
      lcd.write(v);
    }

  } else {
    lcd.print(error);
  }
}


void updateVolume() {
  int analogValue = analogRead(A5);
  // we change the volume only when there is enough change in the value
  if (abs(prevVolAnalogValue - analogValue) < analogVolumeChgThreshold) {
    return;
  }
  int selectedVolume = map(analogValue, 0, 1023, MIN_VOLUME, MAX_VOLUME);

  if (abs(selectedVolume - volume) > analogVolumeChgThreshold) {
    volume = selectedVolume;
    player.setVolume(volume);
    shouldUpdateScreen = true;
  }
  prevVolAnalogValue = analogValue;
}


// This function comes from https://github.com/ghmartin77/DFPlayerAnalyzer
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

int randomPlay() {
  stop();
  int podcast = random(totalPodcasts);
  int episode = random(filesPerFolder[podcast]);
  currentFolder = podcast + 1;
  playingFileNumber = episode + 1;

  randomAnimation(3000);
  togglePlaying();
  randomAnimation(500);
  shouldUpdateScreen = true;
}

void randomAnimation(int duration) {
  shouldUpdateScreen = false;
  // generate random chars
  for (int i = 0; i < 7; i++) {
    byte rSymbol[8] = {
      random(32),
      random(32),
      random(32),
      random(32),
      random(32),
      random(32),
      random(32),
      random(32),
    };
    lcd.createChar(i, rSymbol);
  }
  //  byte rSymbol1[8] = {
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //  };
  //  byte rSymbol2[8] = {
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //  };
  //  byte rSymbol3[8] = {
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //  };
  //  byte rSymbol4[8] = {
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //    random(32),
  //  };
  //  lcd.createChar(3, rSymbol1);
  //  lcd.createChar(4, rSymbol2);
  //  lcd.createChar(5, rSymbol3);
  //  lcd.createChar(6, rSymbol4);

  long start = millis();
  while ((millis() - start) < duration) {
    lcd.setCursor(random(16), random(2));
    lcd.write(random(7));
    waitMilliseconds(10);
  }
  waitMilliseconds(500);

  // we restore the basic set of custom characters
  createCustomCharacters();
}

// Original class comes from https://github.com/ghmartin77/DFPlayerAnalyzer
class Mp3Notify
{
  public:
    static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
    {
      if (source & DfMp3_PlaySources_Sd)
      {
        Serial.print("Tarjeta SD, ");
      }
      if (source & DfMp3_PlaySources_Usb)
      {
        Serial.print("USB Disk, ");
      }
      if (source & DfMp3_PlaySources_Flash)
      {
        Serial.print("Flash, ");
      }
      Serial.println(action);
    }
    static void OnError([[maybe_unused]] DfMp3& mp3, uint16_t errorCode)
    {
      // see DfMp3_Error for code meaning
      Serial.println();
      Serial.print("Com Error ");
      Serial.println(errorCode);
    }
    static void OnPlayFinished([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track)
    {
      Serial.print("Play finished for #");
      Serial.println(track);
      // TODO
      stop();

    }
    static void OnPlaySourceOnline([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "online");
    }
    static void OnPlaySourceInserted([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "inserted");
    }
    static void OnPlaySourceRemoved([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "removed");
      error = "Sin TARJETA!";
    }
};


void createCustomCharacters() {
  // basic set of characters for playing status
  lcd.createChar(pauseChar, pauseSymbol);
  lcd.createChar(playChar, playSymbol);
  lcd.createChar(stopChar, stopSymbol);

  // volume level
  lcd.createChar(vol1Char, vol1Symbol);
  lcd.createChar(vol2Char, vol2Symbol);
  lcd.createChar(vol3Char, vol3Symbol);
  lcd.createChar(vol4Char, vol4Symbol);
}
