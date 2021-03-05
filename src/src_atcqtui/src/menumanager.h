#ifndef MAINMENU_H
#define MAINMENU_H

#include <QObject>
#include <QQuickItem>
#include <QDebug> //FOR WRITING TO THE APPLICATION OUTPUT CONSOLE WITH qInfo
#include <QTimer> // FOR REFRESH INTERVAL
#include <QThread>
#include <QStack>
#include <QString>
#include <QMap>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>



#include "SHARED/guicommunicator/guicommunicator.h"
class MenuManager: public QObject
{
    Q_OBJECT
   // Q_PROPERTY(float best READ best WRITE setBest NOTIFY setBestChanged)
public:
    MenuManager(QObject* _parent);
    MenuManager();
    ~MenuManager();

    static QQuickView* v;

signals:

    //void setBestChanged();
public slots:
    // ----- HEADLINE BAR ---- //
    void lb_settings_btn();
    void lb_info_btn();
    // ----- LOGIN/START ----//
    void ls_login_btn();
    //------ MAIN MENU ---- //
    void mm_start_ai_btn();
    void mm_player_list_btn();
    void mm_search_for_players_toggled(bool _state);
    
    //------ SETTINGS MENU ---- //
    void sm_open_settings_btn();

    void sm_init_btn();
    void sm_logout_btn();
    void sm_scan_board_btn();


    //----- GENERAL ----//
    void go_menu_back();

    //----- INFO SCREEN ----/
    void is_open_is_screen_btn();


    //---- MESSAGE SCREEN ----//
    void message_screen_ok_btn();
    void message_screen_cancel_btn();


    void open_debug_menu();
    void debug_screen_fkt(int _id);

    //---- GAME SCREEN---- //
    void gs_abort_game();

   //----- PLAYER ENTER MOVE MANUAL SCREEN --- //
    void memm_enter_move_reset();
    void memm_enter_move_ok();
    void memm_enter_move_user_input(QString _charakter);
    //---- GENERAL FUNCTIONS --- //
    void general_ui_reset(); //RESET THE UI TO INITIAL STATE
private:
    void show_error(QString _err);
    guicommunicator guiconnection;
    void switch_menu(guicommunicator::GUI_VALUE_TYPE _screen);
    void switch_menu(QString _screen);
    void set_visible_element(QString _name, bool _state);
    void set_headline_text(QString _name);
    void updateProgress();
    QString last_menu_opened = "";
    QString current_menu_opened = "";
    QString user_entered_move ="";
    QStack<QString> menu_visist_history;
    QMap<QString, int> menu_levels;
    void set_label_text(QString _container_name, QString _labelname,QString _text);
    void set_icon_image(QString _container_name, QString _image_name, QString _path);
    void set_progress_indicator(QString _container_name, QString _ic_name, bool _state);
    bool is_number(QChar _char);
    bool is_alpha(QChar _char);
};

#endif // MAINMENU_H
