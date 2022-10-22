# JVpod, a simple mp3 player for podcasts

A small personal project: a simple mp3 player for podcasts.

The core of the functionality is provided by a DFMiniPlayer module.

[descripción en Español aquí](README_ES.md)

## Motivation

Years ago at work, my father used applications like Photoshop, Illustrator and other programs but since then he doesn’t want anything like a computer, no smartphone or tablet nor mac/pc in his life: he has only a landline. The only exception he has made was for my old iPod (80Gb hdd): for podcasts.

Sadly after years and years of use, a few months ago the iPod finally died and I decided to find an acceptable solution because he really, really enjoys listening to podcasts related to science and biographies.

This is a small project that takes an Arduino and a handful other components to implement a simple mp3 player loaded with podcasts (no network connectivity) with a printed index of all the podcasts and episodes organized by numbers.

## Constraints

+ The DFMiniPlayer reads mp3 files but has no functionality to get file metadata or reading files (for example for a small database) and has a strict format for file names...and plenty of bugs and weird behaviors (maybe the market is full of cheap clones of cheap clones).
+ To get the mp3 player file structure requirements, my [podcasts2jvpod](https://github.com/DuqueDeTuring/podcasts2jvpod) project receives a list of podcasts rss files, downloads all episodes and outputs:
++ podcasts episodes organized by folder (podcast)
++ a markdown formatted text file with a complete index with podcasts number-name and corresponding episodes


## Features
+ Display shows podcast and episode number
+ Display shows a little graph for volume level and icon for play/pause state
+ Buttons to change podcasts and episodes, pause/play and reset (using the LCD shield provided keys)
+ Autoplay
+ A button that plays a random episode
+ A toggle switch that turns autoplay on and off
+ Volume control


## List of components / lista de componentes
+ Arduino R3
+ 1602 LCD Keypad Shield for Arduino
+ Potentiometer
+ Toggle switch
+ Push button
+ DFMiniPlayer  (in my case the one I found is based on a MH2024K-24SS chip)
+ 1K resistor (between Arduino's TX pin and the DFMiniPlayer RX pin)
+ Audio Jack (3.5mm) for speakers
+ 3.3 voltage regulator (LD1117V33): the DFMiniPlayer is very sensitive to voltage drops. I used the Arduino's 5v pin to power it through this one.
+ Micro SD cards


## Some images

![](/images/1.jpg)
![](/images/2.jpg)
![](/images/3.jpg)
![](/images/4.jpg)
