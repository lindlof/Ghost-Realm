# Introduction

Ghost Realm is a mobile game demo. It takes place in augmented reality and features 3D action with innovative fighting mechanics. Development of the Ghost Realm has stopped. Therefore this demo is the final version of the game.

Ghost Realm was designed and developed during Game Project course in Metropolia University of Applied Sciences. Demo is ultimately designed for demonstration of core game concepts in the Game Connection Europe event.

Demo is made with Marmalade C++ SDK. Ghost Realm is tested and released on Android but nothing should limit compatibility with other platforms that Marmalade supports.

All material including sounds, pictures, 3D models and source code is licensed under Apache 2.0 license.

# Game concept

Ghost Realm demo was designed to be presented with a guy that explains the game concept. We can't ship the game with a marketing guy so here is a short explanation of the concept.

The game takes place in augmented reality. The default view is a real life map that player can navigate using GPS. Ghosts can be found in some area of the map.

Player fights 3D ghosts displayed in front of camera feed. Player can hit ghosts by e.g. waving their phone, drawing a spell or by yelling. Player blocks attacks e.g. by drawing a counter spell or connecting dots.

Game also includes RPG and possibly MMO elements.

Map and GPS are not implemented in this demo so the player can only stand still. Player can attack by waving their phone and block by connecting dots in the demo. RPG and MMO elements are not implemented in the demo.

# Credits

* Producer: Juri Meskanen
* Lead designer: Otso Kaukomies
* Artist: Eero Salminen
* Artist: Humberto Bampende
* Developer: Mikael Lindlöf
* Sound designer: Eero Kaukomies

# Links

* GitHub: https://github.com/Mikuz/Ghost-Realm
* Google Play: https://play.google.com/store/apps/details?id=fi.ghostrealm
* Ghost Realm blog: https://ghosthuntermobile.wordpress.com
* Trello: https://trello.com/b/MX5TF9HU/ghost-realm

# Building

Building is tested with following components:
* Marmalade 6.4, JDK 1.6.0_45 and Android SDK
* Marmalade 7.3, JDK 1.6.0_45 and Android NDK r9

To build the compiled 3D models (.group.bin files):

1. Build Simulator x86 Debug
1. Run Simulator x86 Debug
