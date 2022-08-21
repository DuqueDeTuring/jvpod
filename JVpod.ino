/* ---------------------------------------------------------------------
   JVpod: a simple mp3 player for podcasts.

   The complete project uses several components:
   - a DFMiniPlayer module with a micro SD card
   - an Arduino Uno R3
   - an LCD Key shield (2 rows, 16 characters per row) for the display
   - a potentiometer for volume control
   - a push button to play a random episode
   - a toggle switch to turn on/off the autoplay feature after and episode ends

   There is nothing special here about podcasts (they are just mp3 files), is 
   just the original purpose of the project.
   
  -------
  
  JVpod: un tocador de mp3 simple para podcasts.

  El proyecto completo usa varios componentes:
  - un módulo DFMiniPlayer con tarjeta micro SD
  - un Arduino Uno R3
  - un LCD Key Shield (2 líneas, 16 columnas por línea) como pantalla
  - un potenciómetro para control de volumen
  - un botón para tocar un episodio aleatorio
  - un switch para apagar/encender la funcionalidad del modo para continuar tocando
    luego de que un episodio terminó.

  En realidad no hay nada especial aquí con respecto a podcasts (son sólo archivos mp3),
  simplemente es el objetivo personal para el proyecto.
  
--------------------------------------------------------------------- */

#include "Arduino.h"
#include <DFMiniMp3.h>
#include "SoftwareSerial.h"
#include <LiquidCrystal.h>
#include "jvpod.h"


void setup() {
  Serial.begin(115200);
  Serial.println("initializing...");
  randomSeed(analogRead(FREE_ANALOG_PIN));
  
  player.begin();
  player.reset();
  playerSerial.begin(9600);

  lcd.begin(16, 2);
  createCustomCharacters();

  splash();
  lcd.setCursor(15, 1);
  lcd.blink();
  countPerFolder();
  lcd.noBlink();
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Se encontraron ");
  lcd.setCursor(0, 1);
  lcd.print(String(totalPodcasts) + " podcasts con un total de " + String(totalEpisodios) + " episodios");
  delay(1000);
  for (int positionCounter = 0; positionCounter < 38; positionCounter++) {
    lcd.scrollDisplayLeft();
    delay(400);
  }

  player.setVolume(INIT_VOLUME);

  // pin for the random play button
  pinMode(RANDOM_PLAY_PIN, INPUT_PULLUP);
}

void loop() {
  player.loop();
  
  int randomButton = digitalRead(RANDOM_PLAY_PIN);
  if (randomButton != prevRandomButton) {
    if (randomButton == HIGH) {
      playRandomFile();
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


/* 
 *  Just a little "splash" screen when we boot the Arduino.
 *  --
 *  Una pequeña pantalla de introducción cuando iniciamos el Arduino.
 */
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


/* 
 * We count how many folders we have and how many files per folder.
 * 
 * The DFMiniMp3 has a getTotalTrackCountSd function to get how many
 * folders are in the card, so... why this one?
 * In my particular module it just didn't work (among other functions). 
 * I tried with multiple libraries, checked the hex command code 
 * against a PDF I found in a chinese website, changed timings and 
 * nothing worked with my MH2024K-24SS module.
 * 
 * What we do here is iterate from folder 0 to MAX_FOLDERS
 * asking for the track count for each one BUT we stop as soon as
 * we get 0 as the track count for a folder.
 * 
 * Now, after reaching the last folder I reset the player, why?
 * I found that asking for the track count in a non existent
 * folder (last one + 1)...gets weird behavior afterwards :-) .
 * 
 * --
 * 
 * Contamos cuántas carpetas hay y cuántos archivos por carpeta.
 * 
 * El DFMiniMp3 ya tiene una función XXXXXXX para obtener cuántas carpetas 
 * hay en la tarjeta, entonces...¿para qué ésta?
 * En el módulo que usé para el proyecto simplemente no funciona (entre otras funciones).
 * Intenté con múltiples librerías, revisé el comando en hexadecimal enviado al módulo
 * contra el documentado en un PDF que encontré en un sitio de China, cambié 
 * los tiempos/espera que hay que tener en la comunicación con el módulo y nada funcionó
 * con el módulo a base del MH2024K-24SS que usé.
 * 
 * Por otro lado, luego de llegar a la última carpeta hago un reset del módulo mp3, 
 * ¿por qué?
 * Encontré que luego de usar getFolderTrackCount para obtener el total de archivos
 * en una carpeta que NO existe (justo después de la úlima que existe)...el módulo
 * exhibe comportamiento errático a partir de ese momento :-) .
 */
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
  Serial.println("Reset mp3 player module...");
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
  if ((k == NO_KEY) || ((millis() - lastKeyPressed) < MIN_KEY_PRESS_TIME)) {
    return;
  }
  lastKeyPressed = millis();

  if (k == KEY_UP) {
    moveToNextFolder();
  } else if (k == KEY_DOWN) {
    moveToPrevFolder();
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

  if (playingFileNumber == 1) {
    // the previous to the first is the last episode
    playingFileNumber = filesPerFolder[currentFolder - 1];
  } else {
    playingFileNumber--;
  }
  restartPlay = true;
  shouldUpdateScreen = true;
}

void playNext() {
  pause();

  if (playingFileNumber == filesPerFolder[currentFolder - 1]) {
    // the next episode after the last one is the first episode
    playingFileNumber = 1;
  } else {
    playingFileNumber++;
  }
  restartPlay = true;
  shouldUpdateScreen = true;
}


void pause() {
  Serial.println("PAUSE:" + String(playing));

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

  if (playing) {
    pause();
  } else {
    playing = true;
    player.start();
  }
  waitMilliseconds(200);
}


/* 
 *  We change the current folder number to the next one.
 *  
 *  What happens if the current folder is the last one?
 *  Then we set the first folder as the current folder.
 *  (if we reach the end we return to the beginning)
 *  ---
 *  
 *  Cambiamos la carpeta actual a la siguiente.
 *  
 *  ¿Qué pasa cuando la carpeta actual es la última?
 *  Entonces ponemos la primera carpeta como la actual.
 *  (si llegamos al final, regresamos al inicio)
 *  
 */
void moveToNextFolder() {
  stop();

  // every time we change podcast we start with the first episode -- cada vez que cambiamos de podcast iniciamos con el primer episodio
  playingFileNumber = 1;

  if (currentFolder < totalPodcasts) {
    currentFolder++;
  } else {
    currentFolder = 1;
  }
}

/* 
 *  We change the current folder number to the previous one.
 *  
 *  What happens if the current folder is the last one?
 *  Then we set the first folder as the current folder.
 *  (We reach the end then we return to the beginning)
 *  ---
 *  
 *  Cambiamos la carpeta actual actual a la siguiente.
 *  
 *  ¿Qué pasa cuando la carpeta actual es la última?
 *  Entonces ponemos la primera carpeta como la actual.
 *  (llegamos al final por lo que regresamos al inicio)
 *  
 */
void moveToPrevFolder() {
  stop();
  
  // every time we change podcast we start with the first episode -- cada vez que cambiamos de podcast iniciamos con el primer episodio
  playingFileNumber = 1;
  
  if (currentFolder > 1) {
    currentFolder--;
  } else {
    currentFolder = totalPodcasts;
  }
}


void updateScreen() {
  shouldUpdateScreen = false;
  lcd.clear();
  lcd.setCursor(0, 0);

  if (error != "") {
    lcd.print(error);
    return;
  }
  
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
    int v = charForVolumeLevel(volume);
    lcd.write(v);
  }
}

/*
 * For a specific volume level we return the custom character to represent it.
 * --
 * Para un volumen específico retornamos el caracter personalizado correspondiente.
 */ 
int charForVolumeLevel(int vol) {
  // the +2 and -2 is just a little aesthethic adjustment done to show the level just like I want when we are near zero or need max volume
  // el +2 y el -2 es un pequeño ajuste estético para mostrar el nivel exactamente como quiero cuando estamos cerca de 0 o del máximo del volumen
  int customChar = map(vol, MIN_VOLUME + 2, MAX_VOLUME - 2, vol1Char, vol4Char);
  return customChar;
}

/*
 * We update the volume according to the reading of an analog PIN,
 * which maybe connected to a potentiometer.
 * The value for the volume is update and the player module volume changed 
 * but only if the change detected between the current value and a previous one
 * is above a threshold.
 * --
 * Actualizamos el volumen de acuerdo al valor leído de un pin analógico
 * que puede estar conectado a un potenciómetro.
 * El valor del volumen se actualiza y el volumen del módulo mp3 se cambia
 * pero sólo si el cambio detectado entre el valor actual y uno previo 
 * sobre pasa un límite preestablecido.
 */ 
void updateVolume() {
  int analogValue = analogRead(A5);
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

int playRandomFile() {
  stop();
  int podcast = random(totalPodcasts);

  playingFileNumber = random(filesPerFolder[podcast]) + 1; // folders and tracks start from 1
  currentFolder = podcast + 1;

  randomAnimation(3500);
  togglePlaying();
  randomAnimation(500);
  shouldUpdateScreen = true;
}

/*
 * Shows a silly random animation using custom characters
 * (an LCD Shield feature) and at the end restores the
 * initial set of custom characters.
 * Why that at the end? because the LCD shield I am using allows
 * a maximum of 7 custom characters, so for the animation
 * we use the 7 available slots and then we must restore the 
 * set created at the program setup.
 * --
 * 
 * Muestra una animación (sin mayor sentido) utilizando
 * caracteres personalizados (una función del LCD Shield) y al final
 * restauramos el conjunto original de caracteres personalizados.
 * ¿Por qué hacemos eso al final? Porque el LCD Shield que uso
 * permite definir un máximo de 7 caracteres, la animación usa 
 * todos los 7 así que debemos asegurarnos de restablecer los 
 * creados al inicio del programa (en la función setup()).
 */ 
void randomAnimation(int duration) {
  shouldUpdateScreen = false;
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
  long start = millis();
  while ((millis() - start) < duration) {
    lcd.setCursor(random(16), random(2));
    lcd.write(random(7));
    waitMilliseconds(10);
  }
  waitMilliseconds(500);

  createCustomCharacters();
}

/* 
 * The program a few custom characters to show information in 
 * the LCD Shield display, for example for the playing status:
 * pause, playing, stopped and volume level.
 * --
 * El programa utiliza varios caracteres personalziados para mostrar
 * información en la pantalla del LCD Shield, por ejemplo para el
 * estado del mp3 player: pausa, tocando, detenido y el nivel de volumen.
 */
void createCustomCharacters() {
  // basic set of characters for playing status
  lcd.createChar(pauseChar, pauseSymbol);
  lcd.createChar(playChar, playSymbol);
  lcd.createChar(stopChar, stopSymbol);

  // 4 volume level characters
  lcd.createChar(vol1Char, vol1Symbol);
  lcd.createChar(vol2Char, vol2Symbol);
  lcd.createChar(vol3Char, vol3Symbol);
  lcd.createChar(vol4Char, vol4Symbol);
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
