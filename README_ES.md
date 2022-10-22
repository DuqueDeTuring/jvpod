# JVpod, un sencillo tocador de mp3 para podcasts

Un pequeño proyecto personal: un simple tocador de mp3 dedicado a podcasts.

La base de su funcionalidad está provista por un módulo DFMiniPlayer. 


## Motivación 

Hace años mi padre usaba en su trabajo aplicaciones como Photoshop, Illustrator (entre otros programas) pero desde entonces no ha querido tener nada relacionado con móviles, tabletas o pc/macs: sólo tiene una línea telefónica fija. La única excepción que hizo hace unos años fue aceptar mi iPod (de 80Gb).

Después de años y años de uso, tristemente el iPod finalmente dejó de funcionar y decidí encontrarle una solución aceptable porque él disfruta muchísimo escuchar podcasts relacionados con ciencia y biografías.

Este es un pequeño proyecto que utiliza un Arduino y unos cuantos componentes para implementar un sencillo tocador de mp3s lleno de podcasts (sin conectividad alguna) y con un índice impreso para todos los podcasts y sus episodios organizados por número y título.


## Restricciones

+ El DFMiniplayer lee archivos mp3 pero no tiene mayor funcionalidad para leer metadata de los archivos o para leer archivos cualquiera (como para una peq. base de datos) y además tiene un estricto formato para los nombres de los ardchivos...y una buena dósis de pulgas y extraños comportamientos (tal vez se debe a un mercado saturado con clones baratos).
+ Para contar con la estructura de archivos que el módulo demanda, mi proyecto [ podcasts2jvpod](https://github.com/DuqueDeTuring/podcasts2jvpod) recibe una lista de archivos rss de podcasts, descarga todos los episodios y genera:
++ episodios de podcasts numerados y organizados por directorio (podcast)
++ un archivo de texto en formato Markdown con un índice completo de los podcasts y sus episodios

## Funcionalidad
+ La pantalla muestra el número de episodio y podcast
+ La pantalla muestra un pequeño gráfico correspondiente al nivel del volumen y un ícono según el estado tocando/en pausa
+ Botones para cambiar de episodio y podcasts, tocar/pausa y reset (usando el LCD shield con botones)
+ Tocar automáticamente el siguiente episodio al terminar el actual
+ Un botón para iniciar un episodio aleatorio
+ Un switch para activar/desactivar la función de "auto tocar"
+ Control de volumen

## Lista de componentes
+ Arduino R3
+ 1602 LCD Keypad Shield for Arduino
+ Potenciómetro (control de volumen)
+ Switch (activa/desactiva "auto tocar")
+ Botón (episodio aleatorio)
+ DFMiniPlayer (en mi caso, el que encontré está basado en el IC MH2024K-24SS)
+ Resistencia de 1K: entre el pin TX del Arduino y el pin RX del DFMiniPlayer
+ Audio Jack (3.5mm) para parlantes
+ 3.3v regulador (LD1117V33): el DFMiniPlayer es muy sensible a caídas de voltage. Usé el pin de 5v del Arduino conectado a este regulador para alimentar el DFMiniPlayer.
+ Tarjetas Micro SD


## Algunas imágenes

![](/images/1.jpg)
![](/images/2.jpg)
![](/images/3.jpg)
![](/images/4.jpg)
