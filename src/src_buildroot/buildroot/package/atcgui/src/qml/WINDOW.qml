import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
//IMPORT THE C++ BACKEND
import MenuManager 1.0



Rectangle {
    id: window
    objectName: "window"

    width: 800
    height: 480
    color: "#07a8a5"

    z: -1

    MenuManager{
        id:main_menu
        objectName: "mainmenu"
    }


    Rectangle {
        id: hb_container
        objectName: "hb_container"
        x: 0
        y: 0
        width: 800
        height: 69
        color: "#dbdbdb"
        visible: true

        Text {
            id: hb_headline_text
            objectName: "hb_headline_text"
            x: 318
            y: 18
            width: 164
            height: 34
            text: "Atomic Chess OS"
            horizontalAlignment: Text.AlignHCenter
            visible: true
            lineHeightMode: Text.ProportionalHeight
            fontSizeMode: Text.FixedSize
            font.pixelSize: 30
        }

        Text {
            id: hb_atc_text
            x: 83
            y: 18
            width: 164
            height: 34
            text: qsTr("ATC")
            visible: false
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 30
            fontSizeMode: Text.FixedSize
            lineHeightMode: Text.ProportionalHeight
        }

        Button {
            id: hb_settings_button
            x: 731
            y: 4
            width: 61
            height: 61

            Image {
                id: hb_settings_icon
                x: 0
                y: 0
                width: 61
                height: 61
                fillMode: Image.PreserveAspectFit
                source: "noun_Settings_56313.png"
            }
            Connections {
                target: hb_settings_button
                function onClicked(_mouse){
                    //ss_container.visible = true
                    main_menu.lb_settings_btn()
                }
            }
        }

        Image {
            id: hb_connection_icon
            objectName: "hb_connection_icon"
            x: 671
            y: 4
            width: 61
            height: 61
            fillMode: Image.PreserveAspectFit
            source: "noun_Cloud_offline.png"


        }

        Button {
            id: hb_atc_button
            x: 8
            y: 4
            width: 61
            height: 61

            Image {
                id: hb_atc_icon
                x: 0
                y: 0
                width: 61
                height: 61
                fillMode: Image.PreserveAspectFit
                source: "info_icon_2.png"

            }

            Connections {
                target: hb_atc_button
                function onClicked(_mouse){
                    main_menu.is_open_is_screen_btn()
                }
            }

        }

        Image {
            id: hb_is_player_move_icon
            objectName: "hb_is_player_move_icon"
            x: 84
            y: 6
            width: 61
            height: 61
            source: "player_turn_unknown.png"
            fillMode: Image.PreserveAspectFit
        }

        Image {
            id: hb_player_color_icon
            objectName: "hb_player_color_icon"
            x: 611
            y: 4
            width: 61
            height: 61
            source: "player_color_unknown.png"
            fillMode: Image.PreserveAspectFit
        }

    }

    Rectangle {
        id: mm_container
        objectName: "mm_container"
        property var headline_bar_name:"Main Menu"
        x: 0
        y: 69
        width: 800
        height: 411
        color: "#07a8a5"
        visible: false
        Rectangle {
            id: mm_searching_for_players_box
            x: 47
            y: 28
            width: 691
            height: 79
            visible: true
            color: "#189694"



            Button {
                id: mm_dis_sfg_btn
                x: 468
                y: 12
                width: 207
                height: 55
                text: qsTr("STOP SEARCH FOR GAME")
                Connections {
                    target: mm_dis_sfg_btn
                    function onClicked(_mouse){
                        main_menu.mm_search_for_players_toggled(false);
                        sfp_indicator.visible = false;
                    }
                }
            }


            Button {
                id: mm_start_random_btn
                x: 40
                y: 183
                width: 207
                height: 55
                visible: false
                text: qsTr("START AI MATCH")
                Connections {
                    target: mm_start_random_btn
                    function onClicked(_mouse){
                        main_menu.mm_search_for_players_toggled(true) //DONT KNOW WHY ITS INVERTED...
                        sfp_indicator.visible = false;
                    }
                }
            }

            Button {
                id: mm_en_sfg_btn
                x: 29
                y: 12
                width: 207
                height: 55
                text: qsTr("START SEARCH FOR GAME")
                Connections {
                    target: mm_en_sfg_btn
                    function onClicked(_mouse){
                        main_menu.mm_search_for_players_toggled(true);
                        sfp_indicator.visible = true;
                    }
                }
            }

            Button {
                id: mm_show_playerlist_btn
                x: 40
                y: 151
                width: 207
                height: 55
                visible: false
                text: qsTr("SHOW AVARIABLE PLAYERS")
                Connections {
                    target: mm_show_playerlist_btn
                    function onClicked(_mouse){
                        main_menu.mm_search_for_players_toggled(false) //DONT KNOW WHY ITS INVERTED...
                        sfp_indicator.visible = true;
                    }
                }
            }



        }

        BusyIndicator {
            id: sfp_indicator
            objectName: "sfp_indicator"
            x: 300
            y: 163
            width: 171
            height: 171
            visible: false
            Connections {
                target: sfp_indicator
            }
            wheelEnabled: true
        }
    }

    Rectangle {
        id: ls_container
        objectName: "ls_container"
        property var headline_bar_name:"Login"
        x: 0
        y: 69
        width: 800
        height: 411
        color: "#07a8a5"
        visible: false
        Button {
            id: ls_login_btn
            x: 272
            y: 147
            width: 257
            height: 118
            text: qsTr("BEGIN")
            font.pointSize: 20
            Connections {
                target: ls_login_btn
                function onClicked(_mouse){
                    main_menu.ls_login_btn()
                }
            }
        }
    }

    Rectangle {
        id: ss_container
        objectName: "ss_container"
        property var headline_bar_name:"Settings"
        x: 0
        y: 68
        width: 800
        height: 411
        color: "#07a8a5"
        visible: false

        Button {
            id: sm_back_btn
            x: 571
            y: 323
            width: 200
            height: 80
            text: qsTr("BACK")
            Connections {
                target: sm_back_btn
                function onClicked(_mouse){
                    main_menu.go_menu_back()
                }
            }
        }

        Button {
            id: sm_dbg_btn
            x: 571
            y: 8
            width: 200
            height: 80
            text: qsTr("ADV OPTIONS")
            Connections {
                target: sm_dbg_btn
                function onClicked(_mouse){
                    main_menu.open_debug_menu()
                }
            }
        }

        Button {
            id: ss_log_out_btn
            x: 17
            y: 313
            width: 200
            height: 80
            text: qsTr("LOGOUT")
            Connections {
                target: ss_log_out_btn
                function onClicked(_mouse){
                    main_menu.sm_logout_btn()

                }
            }
        }

        Button {
            id: ss_init_board_btn
            x: 17
            y: 165
            width: 200
            height: 80
            text: qsTr("INIT BOARD")
            Connections {
                target: ss_init_board_btn
                function onClicked(_mouse){
                    main_menu.sm_init_btn()
                }
            }
        }

        Button {
            id: ss_scan_board_btn
            x: 17
            y: 17
            width: 200
            height: 80
            text: qsTr("CALIBRATION WIZARD")
            Connections {
                target: ss_scan_board_btn
                function onClicked(_mouse){
                    main_menu.sm_scan_board_btn()
                }
            }
        }


    }

    Rectangle {
        id: is_container
        x: 0
        y: 68
        width: 800
        height: 411
        color: "#07a8a5"
        visible: false
        objectName: "is_container"
        property var headline_bar_name:"Information"

        Button {
            id: is_back_btn
            x: 686
            y: 363
            text: qsTr("BACK")
            Connections {
                target: is_back_btn
                function onClicked(_mouse){
                    main_menu.go_menu_back()
                }
            }
        }

        Text {
            id: is_hwid_label
            objectName: "is_hwid_label"
            x: 57
            y: 14
            width: 680
            height: 75
            color: "#ffffff"
            text: qsTr("--- HWID PLACEHOLDER ---")
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 30
        }

        Text {
            id: is_sessionid_label
            objectName: "is_sessionid_label"
            x: 57
            y: 103
            width: 680
            height: 78
            color: "#ffffff"
            text: qsTr("-- SESSION_ID PLACEHOLDER --")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 30
        }

        Text {
            id: is_version_label
            objectName: "is_version_label"
            x: 57
            y: 196
            width: 680
            height: 102
            color: "#ffffff"
            text: qsTr("-- VERSION INFO PLACEHOLDER --")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 30
        }
        Text {
            id: is_playerinfo_label
            objectName: "is_playerinfo_label"
            x: 57
            y: 298
            width: 680
            height: 102
            color: "#ffffff"
            text: qsTr("-- PLAYER INFO PLACEHOLDER --")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 30
        }
    }

    Rectangle {
        id: es_container
        x: 0
        y: 69
        width: 800
        height: 411
        color: "#07a8a5"
        objectName: "es_container"
        property var headline_bar_name:"System Error"
        Button {
            id: es_back_btn
            x: 686
            y: 363
            text: qsTr("BACK")
            Connections {
                target: es_back_btn
                function onClicked(_mouse){
                    main_menu.go_menu_back()
                }
            }
        }

        Text {
            id: es_lasterr_label
            x: 97
            y: 54
            width: 652
            height: 246
            color: "#ffffff"
            text: qsTr("ERROR")
            horizontalAlignment: Text.AlignHCenter
            objectName: "es_lasterr_label"
            font.pixelSize: 30
        }
        visible: false
    }

    Rectangle {
        id: msgta_container
        objectName: "msgta_container"
        property var headline_bar_name:"Alert Message"
        x: 2
        y: 70
        width: 800
        height: 411
        color: "#07a8a5"

        Text {
            id: msgta_headline_label
            x: 358
            y: 54
            color: "#ffffff"
            text: qsTr("MESSAGE")
            horizontalAlignment: Text.AlignHCenter
            objectName: "es_lasterr_label"
            font.pixelSize: 30
        }

        Text {
            id: msgta_message_label
            objectName: "msgta_message_label"
            x: 56
            y: 102
            width: 695
            height: 142
            color: "#ffffff"
            text: qsTr("MESSAGE")
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 20
        }

        Button {
            id: msgta_button_cancel
            x: 134
            y: 295
            width: 175
            height: 70
            text: qsTr("CANCEL")
            Connections {
                target: msgta_button_cancel
                function onClicked(_mouse){
                    main_menu.message_screen_cancel_btn()
                }
            }
        }

        Button {
            id: msgta_button_ok
            x: 487
            y: 295
            width: 175
            height: 70
            text: qsTr("OK")
            Connections {
                target: msgta_button_ok
                function onClicked(_mouse){
                    main_menu.message_screen_ok_btn()
                }
            }
        }
        visible: false
    }

    Rectangle {
        id: msgtb_container
        objectName: "msgtb_container"
        property var headline_bar_name:"Alert Message"
        x: 2
        y: 70
        width: 800
        height: 411
        color: "#07a8a5"
        Text {
            id: msgtb_headline_label
            x: 358
            y: 54
            color: "#ffffff"
            text: qsTr("MESSAGE")
            horizontalAlignment: Text.AlignHCenter
            objectName: "msgtb_headline_label"
            font.pixelSize: 30
        }

        Text {
            id: msgtb_message_label
            x: 56
            y: 102
            width: 695
            height: 142
            color: "#ffffff"
            text: qsTr("MESSAGE")
            objectName: "msgtb_message_label"
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            id: msgtb_button_ok
            x: 316
            y: 279
            width: 175
            height: 70
            text: qsTr("OK")
            Connections {
                target: msgtb_button_ok
                function onClicked(_mouse){
                    main_menu.message_screen_ok_btn()
                }
            }
        }
        visible: false
    }

    Rectangle {
        id: processing_container
        objectName: "processing_container"
        property var headline_bar_name:"-- PLEASE WAIT --"
        x: 0
        y: 70
        width: 800
        height: 411
        color: "#07a8a5"
        Text {
            id: processing_headline_label
            x: 308
            y: 34
            color: "#ffffff"
            text: qsTr("PLEASE WAIT")
            objectName: "processing_headline_label"
            font.pixelSize: 30
        }

        BusyIndicator {
            id: busyIndicator
            objectName: "pc_indicator"
            x: 223
            y: 85
            width: 355
            height: 293
            wheelEnabled: false
        }
        visible: false
    }

    Rectangle {
        id: debug_container
        x: -1
        y: 70
        width: 800
        height: 411
        color: "#07a8a5"
        visible: false
        objectName: "debug_container"
        property var headline_bar_name:"Advanced Options"
        Text {
            id: debug_headline_label
            x: 351
            y: 31
            color: "#ffffff"
            text: qsTr("DEBUG")
            objectName: "debug_headline_label"
            font.pixelSize: 30
        }

        Button {
            id: debug_back_btn
            x: 685
            y: 363
            text: qsTr("BACK")
            Connections {
                target: debug_back_btn
                function onClicked(_mouse){
                    main_menu.lb_settings_btn()
                }
            }
        }

        Rectangle {
            id: debug_bg_rect
            x: 46
            y: 31
            width: 727
            height: 315
            color: "#189694"

            Button {
                id: debug_fkt_a_btn
                x: 13
                y: 14
                width: 209
                height: 60
                text: qsTr("RESET_GUI")
                Connections {
                    target: debug_fkt_a_btn
                    function onClicked(_mouse){
                        main_menu.debug_screen_fkt(0)
                    }
                }
            }

            Button {
                id: debug_fkt_b_btn
                x: 259
                y: 14
                width: 209
                height: 60
                text: qsTr("LOAD_DEFAULT_CONFIG")
                Connections {
                    target: debug_fkt_b_btn
                    function onClicked(_mouse){
                        main_menu.debug_screen_fkt(1)
                    }
                }
            }

            Button {
                id: debug_fkt_c_btn
                x: 510
                y: 14
                width: 209
                height: 60
                text: qsTr("MAKE_MOVE_TEST")
                Connections {
                    target: debug_fkt_c_btn
                    function onClicked(_mouse){
                        main_menu.debug_screen_fkt(2)
                    }
                }
            }

            Button {
                id: debug_fkt_d_btn
                x: 13
                y: 121
                width: 209
                height: 60
                text: qsTr("G5 -> A2")
                Connections {
                    target: debug_fkt_d_btn
                    function onClicked(_mouse){
                        main_menu.debug_screen_fkt(3)
                    }
                }
            }

            Button {
                id: debug_fkt_e_btn
                x: 259
                y: 121
                width: 209
                height: 60
                text: qsTr("UPLOAD CONFIG FILE")
                Connections {
                    target: debug_fkt_e_btn
                    function onClicked(_mouse){
                        main_menu.debug_screen_fkt(4)
                    }
                }
            }

            Button {
                id: debug_fkt_f_btn
                x: 510
                y: 121
                width: 209
                height: 60
                text: qsTr("UPLOAD LOGFILE")
                Connections {
                    target: debug_fkt_f_btn
                    function onClicked(_mouse){
                        main_menu.debug_screen_fkt(5)
                    }
                }
            }

            Button {
                id: debug_fkt_g_btn
                x: 13
                y: 218
                width: 209
                height: 60
                text: qsTr("DOWNLOAD CONFIG FILE")
                Connections {
                    target: debug_fkt_g_btn
                    function onClicked(_mouse){
                        main_menu.debug_screen_fkt(6)
                    }
                }
            }

            Button {
                id: debug_fkt_h_btn
                x: 259
                y: 218
                width: 209
                height: 60
                text: qsTr("MAKE MOVE MANUAL")
                Connections {
                    target: debug_fkt_h_btn
                    function onClicked(_mouse){
                        main_menu.debug_screen_fkt(7)
                    }
                }

            }
        }
    }

    Rectangle {
        id: game_container
        x: 1
        y: 69
        width: 800
        height: 411
        color: "#07a8a5"
        border.color: "#000100"
        visible: false
        objectName: "game_container"
        property var headline_bar_name:"Game Status"
        Text {
            id: game_headline_label
            x: 308
            y: 34
            color: "#ffffff"
            text: qsTr("PLAYER TURN")
            horizontalAlignment: Text.AlignHCenter
            objectName: "game_headline_label"
            font.pixelSize: 30
        }


        Text {
            id: game_possible_move_label
            x: 8
            y: 85
            color: "#ffffff"
            text: qsTr("POSSIBLE MOVES")
            horizontalAlignment: Text.AlignHCenter
            objectName: "game_possible_move_label"
            font.pixelSize: 24
        }


        Button {

            id: game_abortgame_btn
            objectName: "game_abortgame_btn"
            x: 286
            y: 305
            width: 266
            height: 89
            text: qsTr("ABORT GAME")
            focusPolicy: Qt.ClickFocus
            autoRepeat: false
            Connections {
                target: game_abortgame_btn
                function onClicked(_mouse){
                    main_menu.gs_abort_game()
                }
            }
        }

        Text {
            id: game_statistics_headline_label1
            x: 602
            y: 85
            color: "#ffffff"
            text: qsTr("YOUR COLOR")
            visible: true
            horizontalAlignment: Text.AlignHCenter
            objectName: "game_possible_move_label"
            font.pixelSize: 24
        }

        Image {
            id: gs_my_turn_color_image
            objectName: "gs_my_turn_color_image"
            x: 605
            y: 119
            width: 140
            height: 140
            fillMode: Image.PreserveAspectFit
            source: "player_color_unknown.png"
        }

        Image {
            id: gs_current_turn_color_image
            objectName: "gs_current_turn_color_image"
            x: 306
            y: 82
            width: 200
            height: 200
            fillMode: Image.PreserveAspectFit
            source: "player_color_unknown.png"
        }

        Text {
            id: game_possible_move_text
            objectName: "game_possible_move_text"
            x: 30
            y: 119
            width: 124
            height: 15
            text: qsTr("---")
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 24
        }

    }

    Rectangle {
        id: showavariableplayer_container
        objectName: "showavariableplayer_container"
        property var headline_bar_name:"Player Lobby"
        x: 0
        y: 70
        width: 800
        height: 411
        color: "#07a8a5"
        visible: false

        Button {
            id: showavariableplayer_back_btn
            x: 686
            y: 363
            text: qsTr("BACK")
            visible: true
            Connections {
                target: is_back_btn
                function onClicked(_mouse){
                    main_menu.go_menu_back()
                }
            }
        }
    }

    Rectangle {
        id: mmem_container
        objectName: "mmem_container"
        property var headline_bar_name:"Enter Move Manually"
        x: 0
        y: 70
        width: 800
        height: 411
        color: "#07a8a5"
        visible: false

        Text {
            id: mmem_chosen_move_label
            objectName: "mmem_chosen_move_label"
            x: 304
            y: 109
            width: 192
            height: 57
            text: qsTr("----")
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 50
        }

        Text {
            id: mmem_move_headline
            x: 315
            y: 68
            width: 173
            height: 35
            text: qsTr("Entered Move")
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            id: memm_mi_a_btn
            x: 28
            y: 25
            width: 70
            height: 70
            text: qsTr("A")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_a_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("a")
                }
            }
        }

        Button {
            id: memm_mi_b_btn
            x: 182
            y: 25
            width: 70
            height: 70
            text: qsTr("B")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_b_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("b")
                }
            }
        }

        Button {
            id: memm_mi_d_btn
            x: 182
            y: 122
            width: 70
            height: 70
            text: qsTr("D")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_d_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("d")
                }
            }
        }

        Button {
            id: memm_mi_f_btn
            x: 182
            y: 219
            width: 70
            height: 70
            text: qsTr("F")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_f_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("f")
                }
            }
        }

        Button {
            id: memm_mi_c_btn
            x: 28
            y: 122
            width: 70
            height: 70
            text: qsTr("C")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_c_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("c")
                }
            }
        }

        Button {
            id: memm_mi_e_btn
            x: 28
            y: 219
            width: 70
            height: 70
            text: qsTr("E")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_e_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("e")
                }
            }
        }

        Button {
            id: memm_mi_g_btn
            x: 28
            y: 316
            width: 70
            height: 70
            text: qsTr("G")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_g_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("g")
                }
            }
        }

        Button {
            id: memm_mi_h_btn
            x: 182
            y: 316
            width: 70
            height: 70
            text: qsTr("H")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_h_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("h")
                }
            }
        }

        Button {
            id: memm_mi_1_btn
            x: 538
            y: 25
            width: 70
            height: 70
            text: qsTr("1")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_1_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("1")
                }
            }
        }

        Button {
            id: memm_mi_2_btn
            x: 696
            y: 25
            width: 70
            height: 70
            text: qsTr("2")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_2_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("2")
                }
            }
        }

        Button {
            id: memm_mi_4_btn
            x: 696
            y: 122
            width: 70
            height: 70
            text: qsTr("4")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_4_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("4")
                }
            }
        }

        Button {
            id: memm_mi_6_btn
            x: 696
            y: 219
            width: 70
            height: 70
            text: qsTr("6")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_6_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("6")
                }
            }
        }

        Button {
            id: memm_mi_3_btn
            x: 538
            y: 122
            width: 70
            height: 70
            text: qsTr("3")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_3_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("3")
                }
            }
        }

        Button {
            id: memm_mi_5_btn
            x: 538
            y: 219
            width: 70
            height: 70
            text: qsTr("5")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_5_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("5")
                }
            }
        }

        Button {
            id: memm_mi_7_btn
            x: 538
            y: 316
            width: 70
            height: 70
            text: qsTr("7")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_7_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("7")
                }
            }
        }

        Button {
            id: memm_mi_8_btn
            x: 696
            y: 316
            width: 70
            height: 70
            text: qsTr("8")
            font.pointSize: 20
            font.bold: true
            Connections {
                target: memm_mi_8_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_user_input("8")
                }
            }
        }

        Button {
            id: memm_mi_ok_btn
            x: 304
            y: 244
            width: 184
            height: 69
            text: qsTr("OK")
            font.pointSize: 15
            font.bold: true
            Connections {
                target: memm_mi_ok_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_ok()
                }
            }
        }

        Button {
            id: memm_mi_rest_btn
            x: 304
            y: 341
            width: 184
            height: 45
            text: qsTr("RESET")
            font.pointSize: 15
            Connections {
                target: memm_mi_rest_btn
                function onClicked(_mouse){
                    main_menu.memm_enter_move_reset()
                }
            }
        }


    }


}




/*##^##
Designer {
    D{i:0;active3dScene:"-1"}
}
##^##*/
