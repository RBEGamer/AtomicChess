# AtomicChess - The most advanced self playing chess table


![prod](https://user-images.githubusercontent.com/9280991/220172644-81fd00ce-cb8f-4056-8e39-720ec7858f19.png)

# FEATURES

This automated chess table allows the user to play chess virtually over the Internet with other chess tables or via a web client.
The chess table acts as an opponent and can move the pieces automatically and recognizes the player's moves and sends them to the opponent (chess table or virtual).


|                                               | Atomic Chess Table                        |
|:----------------------------------------------|------------------------------------------:|
| Recognition figure position                   | yes, using NFC tags under figures         |
| Connectivity                                  | LAN, WLAN                                 |
| Automatic movement of the figures             | yes                                       |
| Game Livestream                               | yes                                       |
| Cloud connection (online games)               | yes, central gameserver and matchmaking   |
| Parking position for eliminated figures       | yes                                       |
| Stand-alone functionality                     | yes, using stockfish-11                   |


## Integration of an embedded system into a cloud infrastructure using the example of an autonomous chess table

**NOTE**This project is my bachelor thesis in computer science. The thesis with details is also available in this repository.

![ATC_PROD_TABLE_FINAL_IMG_orig](https://user-images.githubusercontent.com/9280991/220172093-60a4af1e-1d08-4d89-8e21-97ade77841fa.jpg)

### Abstract

The goal of the work is to develop an autonomous chess table, which is able to move chess pieces autonomously and to react to user interactions. The core question of the thesis thus relates to the verification of the feasibility including the creation and implementation of an embedded system and a cloud infrastructure. The focus is especially on the programming of the embedded system and the interaction of this with a server accessible from the Internet, which serves as an intermediary between different manholes and other end devices.

First, the approaches existing at the time and their implementation are examined. Here, particular attention was paid to illustrating the limitations of existing systems and comparing them to functions that only apply to this project. From the results, the requirements of the autonomous shaft table are derived, which are to be implemented in this work.

Subsequently, two prototypes were implemented iteratively one after the other, which differ strongly from the mechanical design as well as the electrical one. The developed software, on the other hand, was developed modularly so that it can be used on both prototypes. The different designs are due to the fact that in the first iteration of the autonomous chess table, after an endurance test, numerous potential improvements were identified. This led to a complete redesign in the second iteration and thus an autonomous shaft table was developed, which fulfills all requirements.

Basically, it should be noted that the result of the work is not a finalized product, but a structural prototype. Further tests, such as usage statistics or safety tests, would have to be carried out before the shaft table can be considered a commercial product.

The system and especially the implemented cloud service can be accessed and extended online. Among other things, this allows building your own table using the AtomicChess system, but also integrating other components. Experienced developers can thus expand the game at will or even add to other games. The mechanics and game management designed for the project can accordingly also be used for various other table board games.

### PAPER PDF

`./atomicchess_thesis.pdf`


### CITE

```tex
@software{Ochsendorf_Marcel_AtomicChessBA_2021,
author = {Ochsendorf, Marcel},
license = {GPL-2.0},
month = {9},
title = {{Integration of an embedded system into a cloud infrastructure using the example of an autonomous chess table}},
url = {https://github.com/RBEGamer/AtomicChess},
version = {0.9.0},
year = {2021}
}
```

