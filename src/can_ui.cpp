#include "can_ui.hpp"
#include <ncurses.h>
#include <iomanip>
#include <sstream>

CANUI::CANUI(CANLogger& src) : logger_(src) {}

void CANUI::start() { th_ = std::thread(&CANUI::uiThread,this); }

void CANUI::stop()  { running_ = false; th_.join(); }

void CANUI::uiThread() {
    initscr(); cbreak(); noecho(); nodelay(stdscr,true); curs_set(0);
    while(running_) {
        CANFrame f;
        while(logger_.pop(f)) {
            clear();
            mvprintw(0,0,"ID: 0x%03X",f.id);
            for(int i=0;i<8;++i)
                mvprintw(1,i*3," %02X",f.data[i]);
            mvprintw(3,0,"q para salir");
            refresh();
        }
        int ch = getch();
        if(ch=='q') running_=false;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    endwin();
}
