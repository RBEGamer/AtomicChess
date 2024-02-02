//
// Created by marcel on 02.02.24.
//

#ifndef ATC_SERVER_GAME_STATE_H
#define ATC_SERVER_GAME_STATE_H


class game_state {


public:
    enum class GAME_STATE {
        PS_INVALID = 0,
        PS_IDLE = 1,
        PS_SEARCHING = 4,
        PS_SEARCHING_MANUAL = 5,
        PS_PREPARING_INGAME = 6,
        PS_INGAME = 7,
        PS_GAME_OVER = 8
    };
};


#endif //ATC_SERVER_GAME_STATE_H
