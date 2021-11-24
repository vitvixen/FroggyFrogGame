#define _DEFAULT_SOURCE

#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>


int init_menu();
int init_levels();
void print_in_middle(WINDOW *, int, int, int, char *, chtype);
void show_credits();
void draw_logo();
void init_lvl1(int, int );
void init_lvl2(int, int );
void init_lvl3(int, int );
void init_endless(int, int);
void print_message(int, int, char *, chtype);


int main(){
    initscr();

    start_color();
    noecho();
    keypad(stdscr, TRUE);
    
    init_pair(1, COLOR_WHITE, COLOR_BLACK);   
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK); 
    init_pair(3, COLOR_GREEN, COLOR_BLACK);   
    init_pair(4, COLOR_RED, COLOR_BLACK);    
    init_pair(5, COLOR_BLUE, COLOR_BLACK);  
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_BLACK);
    init_pair(8, COLOR_BLACK, COLOR_GREEN);
    init_pair(9, COLOR_BLACK, COLOR_YELLOW);
    init_pair(10, COLOR_BLUE, COLOR_BLUE);
    init_pair(11, COLOR_BLACK, COLOR_BLUE);


    //drawing the logo
    draw_logo();
    curs_set(TRUE);
    
    //menu stuff
    int choice = -1, lvl = -1;
    while (1) {
        choice = init_menu();

        if (choice == 2) {
            break;
        } else if (choice == 0) {

            while(1){ //picking the lvl
                lvl = init_levels();

                if (lvl == 4){
                    break;
                } else if (lvl == 0){
                    init_lvl1(LINES, COLS); 
                } else if (lvl == 1){
                    init_lvl2(LINES, COLS);
                } else if (lvl == 3){
                    init_endless(LINES, COLS);
                } else if (lvl == 2){
                    init_lvl3(LINES, COLS);
                }
            }
        } else if (choice == 1) {
            show_credits();
            refresh();
        }
    }

    clear();
    refresh();
    endwin();
    printf("See you again!\n");

    return 0;
}

void init_endless( const int lines, const int cols){
    int combo = 1, breaker = 0, distance =0;
    while(1){
        int rndm = rand();
        if (rndm%3 == 0) {
            //randomly choosing lvl1
            WINDOW *win = newwin (lines, cols, 0, 0);
            srand(time(NULL));
            char ghosts[lines-8];
            int cords[lines-8];
            int colors[lines-8];
            nodelay(win, TRUE);
            curs_set(FALSE);
            keypad(win,TRUE);
            int burrowx = rand()%cols;

            //building the exterior
            wattron(win, A_BOLD | COLOR_PAIR(3));    
            mvwaddch(win, 1, 0, ACS_LTEE);
            mvwhline(win, 1, 1, ACS_HLINE, cols - 1);
            mvwaddch(win, 1, cols - 1, ACS_RTEE);
            mvwprintw (win, 0, 1, "[P]ause | [Q]uit | [H]elp " );
            mvwprintw (win, lines-1, 1, "Distance: %d", distance);
            mvwprintw (win, lines-1, 25, "Combo: %d", combo);
            wattroff(win, A_BOLD | COLOR_PAIR(3));    
            //the burrow
            wattron(win, A_BOLD | COLOR_PAIR(9));    
            mvwprintw(win, lines -2, burrowx, "O");
            wattroff(win, A_BOLD | COLOR_PAIR(9));    
            
            //randomizing the ghosts
            for (int i = 0; i<lines-8; i++){
                int tmp = rand();
                if (combo < 10){
                    if (tmp%5==0) {
                        ghosts[i] = ' ';
                    } else ghosts[i] = '@';
                } else if (combo >=10 && combo <20 ){
                    if (tmp%8==0) {
                        ghosts[i] = ' ';
                    } else ghosts[i] = '@';
                } else if (combo>=20){
                    ghosts[i] = '@';
                }
            }

            //randomizing the color for the ghosts
            for (int i = 0; i<lines-8; i++){
                int clr = rand() % 7;
                colors[i] = clr;  
            }

            struct timespec ts = {
                    .tv_sec = 0,                    // nr of secs
                    .tv_nsec = 0.01 * 1000000000L  // nr of nanosecs
            };


            ///unleashing the ghosts
            for (int i = 0; i<lines-8; i++){
                    int tmp = rand();
                    cords[i] = tmp%cols;
                    if (ghosts[i] == '@'){
                        wattron(win, COLOR_PAIR(colors[i]));
                        mvwprintw(win, 4+i, cords[i], "@");
                        wattroff(win, COLOR_PAIR(colors[i]));

                    }   
            }

            //placing froggy
            int x = cols/2, y = 3;
            wattron (win, COLOR_PAIR(8));
            mvwprintw(win, y, x, "F");
            wattroff (win, COLOR_PAIR(8));

            wrefresh(win);

            ts.tv_nsec = 0.1 * 1000000000L;


            char dir[lines-8];
            //picking the direction of the ghosts
            for (int i = 0; i<lines-8; i++){
                int tmp = rand();
                if (tmp% 2 == 0){
                    dir[i] ='r';
                } else {
                    dir[i] = 'l';
                }
            }
            //game loop
            while(1){
                //moving the ghosts
                for (int i = 0; i<lines-8; i++){
                    if (dir[i]== 'r'){
                        if (cords[i]==cols){
                            dir[i]= 'l';
                            mvwprintw(win, i+4, cords[i], " ");
                            cords[i]--;
                            continue;
                        } else{
                            mvwprintw(win, i+4, cords[i], " ");
                            cords[i]++;
                        }
                    } else if (dir[i] == 'l'){
                        if (cords[i]== 0){
                            dir[i] = 'r';
                            mvwprintw(win, i+4, cords[i], " ");
                            cords[i]++;
                            continue;
                        } else {
                            mvwprintw(win, i+4, cords[i], " ");
                            cords[i]--;
                        }
                    }
                }
                
                nanosleep(&ts, NULL);

                //placing the ghost
                for (int i = 0; i<lines-8; i++){
                    if (ghosts[i] == '@'){
                        wattron(win, COLOR_PAIR(colors[i]));
                        mvwprintw(win, 4+i, cords[i], "@");
                        wattroff(win, COLOR_PAIR(colors[i]));
                        //wrefresh(win);
                    }   
                }

                int input = wgetch(win);
                wrefresh(win);
                switch(input){
                    case 'q': case 'Q' : {
                        print_message(4, 25, "QUIT", COLOR_PAIR(2));
                        wrefresh(win);
                        return;
                        
                    }
                    case 'h' : case 'H' : {
                        WINDOW *cwin = newwin(15, 23, lines / 2 - 15 / 2, cols / 2 - 23 / 2);
                        do{
                            
                            wattrset(cwin, A_BOLD | COLOR_PAIR(6));
                            box(cwin, 0, 0);
                            print_in_middle(cwin, 2, 0, 23, "Hopefully you are", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 3, 0, 23, "familliar with the", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 4, 0, 23, "mechanics already.", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 5, 0, 23, "Here you can play", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 6, 0, 23, "as long as you don't", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 7, 0, 23, "die.", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 8, 0, 23, "You can view the log", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 9, 0, 23, "of all your attempts", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 10, 0, 23, "in the <endless.txt>", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 11, 0, 23, "file.", A_BOLD | COLOR_PAIR(6));
                            wattrset(cwin, ~A_BOLD & COLOR_PAIR(6));
                            print_in_middle(cwin, 15 - 2, 0, 23, "Press ENTER to resume", COLOR_PAIR(1));
                        } while (wgetch(cwin) != 10);
                        //destroying the window
                        wborder(cwin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
                        for (int j = 0; j<15; j++){
                            for (int o = 0; o<24; o++){
                                mvwaddch(cwin, j, o, ' ');
                            }
                        }
                        wrefresh(cwin);
                        wrefresh(win);
                        delwin(cwin);
                        
                        break;
                    }
                    case 'p' : case 'P' :{
                        print_message(4, 25, "PAUSE", COLOR_PAIR(6));
                        wrefresh(win);
                        break;
                    }
                    case KEY_DOWN :{
                        flushinp();
                        if (y!=lines-1){
                            mvwprintw (win, y,x, " ");
                            y++;
                            distance++;
                        }
                        break;
                    }
                    case KEY_UP : {
                        flushinp();
                        if (y!=0){
                            mvwprintw (win, y,x, " ");
                            y--;
                            distance++;
                        }
                        break;
                    }
                    case KEY_RIGHT : {
                        flushinp();
                        if (x!=cols-1){
                            mvwprintw (win, y,x, " ");
                            x++;
                            distance++;
                        }
                        break;
                    }
                    case KEY_LEFT : {
                        flushinp();
                        if (x!=0){
                            mvwprintw (win, y,x, " ");
                            x--;
                            distance++;
                        }
                        break;
                    }
                }

                //building the exterior
                wattron(win, A_BOLD | COLOR_PAIR(3));    
                mvwaddch(win, 1, 0, ACS_LTEE);
                mvwhline(win, 1, 1, ACS_HLINE, cols - 1);
                mvwaddch(win, 1, cols - 1, ACS_RTEE);
                mvwprintw (win, 0, 1, "[P]ause | [Q]uit | [H]elp " );
                mvwprintw (win, lines-1, 1, "Distance: %d", distance);
                mvwprintw (win, lines-1, 25, "Combo: %d", combo);
                wattroff(win, A_BOLD | COLOR_PAIR(3));  
                //the burrow
                wattron(win, A_BOLD | COLOR_PAIR(9));    
                mvwprintw(win, lines -2, burrowx, "O");
                wattroff(win, A_BOLD | COLOR_PAIR(9));
                //wrefresh(win);  

                //placing the ghost
                for (int i = 0; i<lines-8; i++){
                    if (ghosts[i] == '@'){
                        wattron(win, COLOR_PAIR(colors[i]));
                        mvwprintw(win, 4+i, cords[i], "@");
                        wattroff(win, COLOR_PAIR(colors[i]));
                        //wrefresh(win);
                    }   
                }

                //placing froggy
                wattron (win, COLOR_PAIR(8));
                mvwprintw(win, y, x, "F");
                wattroff (win, COLOR_PAIR(8));

                wrefresh(win);

                //checking for the ghosts
                if (ghosts[y-4] == '@' && cords[y-4] == x){
                    print_message(4, 25, "YOU WERE EATEN!", COLOR_PAIR(4));
                    wrefresh(win);
                    breaker = 1;
                    break;
                }

                //checking for the burrow
                if (y==lines-2 && x == burrowx) {
                    print_message(4, 25, "YOU'RE HOME!", COLOR_PAIR(3));
                    wrefresh(win);
                    combo++;
                    break;
                }
                wrefresh(win);

            }
            
            wrefresh(win);
            delwin(win);
            curs_set(TRUE);
        } else if (rndm%3 == 1) {
            //randomly picking lvl2
            WINDOW *win = newwin (lines, cols, 0, 0);
            srand(time(NULL));
            char ghosts[lines-8];
            int cords[lines-8];
            int colors[lines-8];
            nodelay(win, TRUE);
            curs_set(FALSE);
            keypad(win,TRUE);
            int burrowx = rand()%cols;

            //building the exterior
            wattron(win, A_BOLD | COLOR_PAIR(3));    
            mvwaddch(win, 1, 0, ACS_LTEE);
            mvwhline(win, 1, 1, ACS_HLINE, cols - 1);
            mvwaddch(win, 1, cols - 1, ACS_RTEE);
            mvwprintw (win, 0, 1, "[P]ause | [Q]uit | [H]elp " );
            mvwprintw (win, lines-1, 1, "Distance: %d", distance);
            mvwprintw (win, lines-1, 25, "Combo: %d", combo);
            wattroff(win, A_BOLD | COLOR_PAIR(3));    
            //the burrow
            wattron(win, A_BOLD | COLOR_PAIR(9));    
            mvwprintw(win, lines -2, burrowx, "O");
            wattroff(win, A_BOLD | COLOR_PAIR(9));    
            
            //randomizing the ghosts and water
            for (int i = 0; i<lines-8; i++){
                int tmp = rand();
                if (combo < 10){
                    if (tmp%6==0) {
                        ghosts[i] = ' ';
                    } else if (tmp%7 ==0){
                        ghosts[i] = 'w';
                    } else ghosts[i] = '@';
                } else if (combo >=10 && combo<20){
                    if (tmp%8==0) {
                        ghosts[i] = ' ';
                    } else if (tmp%6 ==0){
                        ghosts[i] = 'w';
                    } else ghosts[i] = '@';
                } else if (combo >= 20){
                    if (tmp%6 ==0){
                        ghosts[i] = 'w';
                    } else ghosts[i] = '@';
                }
            }

            //initializing the directions of the rivers
            char stream[lines-8];
            for (int i = 0; i<lines-8; i++){
                if (i%2 ==0){
                    stream[i] = 'r';
                } else stream[i] = 'l';
            }

            //randomizing the color for the ghosts
            for (int i = 0; i<lines-8; i++){
                int clr = rand() % 7;
                colors[i] = clr;  
            }

            struct timespec ts = {
                    .tv_sec = 0,                    // nr of secs
                    .tv_nsec = 0.001 * 1000000000L  // nr of nanosecs
            };


            ///unleashing the ghosts
            for (int i = 0; i<lines-8; i++){
                    int tmp = rand();
                    cords[i] = tmp%cols;
                    if (ghosts[i] == '@'){
                        wattron(win, COLOR_PAIR(colors[i]));
                        mvwprintw(win, 4+i, cords[i], "@");
                        wattroff(win, COLOR_PAIR(colors[i]));
                    }   
            }

            //generating the water
            char water[lines-8][cols];
            for (int i =0; i<lines-8; i++){
                if (ghosts[i]== 'w'){
                    for (int j = 0; j<cols; j++){
                        int tmp = rand();
                        if (tmp%2 == 0){
                            water[i][j] = '#';
                        } else water[i][j] = 'w';
                    }
                }
            }

            //place the water
            for (int i = 0; i<cols-8; i++){
                if (ghosts[i] == 'w'){
                    for (int j = 0; j<cols; j++){
                        char tmp = water[i][j];
                        if (tmp == 'w') {
                            wattron (win, COLOR_PAIR(10));
                            mvwprintw (win, i+4, j, "w" );
                            wattroff (win, COLOR_PAIR(10));
                        }
                        if (tmp == '#') {
                            wattron (win, COLOR_PAIR(11));
                            mvwprintw (win, i+4, j, "#" );
                            wattroff (win, COLOR_PAIR(11));
                        }
                    }
                }
            }

            //placing froggy
            int x = cols/2, y = 3;
            wattron (win, COLOR_PAIR(8));
            mvwprintw(win, y, x, "F");
            wattroff (win, COLOR_PAIR(8));

            wrefresh(win);

            ts.tv_nsec = 0.1 * 1000000000L;


            char dir[lines-8];
            //picking the direction of the ghosts
            for (int i = 0; i<lines-8; i++){
                int tmp = rand();
                if (tmp% 2 == 0){
                    dir[i] ='r';
                } else {
                    dir[i] = 'l';
                }
            }
            //game loop
            while(1){
                //moving the ghosts
                for (int i = 0; i<lines-8; i++){
                    if (dir[i]== 'r'){
                        if (cords[i]==cols){
                            dir[i]= 'l';
                            mvwprintw(win, i+4, cords[i], " ");
                            cords[i]--;
                            continue;
                        } else{
                            mvwprintw(win, i+4, cords[i], " ");
                            cords[i]++;
                        }
                    } else if (dir[i] == 'l'){
                        if (cords[i]== 0){
                            dir[i] = 'r';
                            mvwprintw(win, i+4, cords[i], " ");
                            cords[i]++;
                            continue;
                        } else {
                            mvwprintw(win, i+4, cords[i], " ");
                            cords[i]--;
                        }
                    }
                }

                //moving the water
                for (int i = 0; i<lines-8; i++){
                    if (ghosts[i]== 'w'){
                        if (stream[i] == 'r') {
                            for (int j = cols-2; j>-1; j--){
                                if (water[i][j] == 'w'){
                                    water[i][j+1] = 'w';
                                }
                                if (water[i][j] == '#'){
                                    water[i][j+1] = '#';
                                }
                            }
                            char tmp = rand();
                            if (tmp%2==0){
                                water[i][0] = '#';
                            } else water[i][0] = 'w';
                        } else if (stream[i] == 'l'){
                            for (int j = 0; j<cols; j++){
                                if (water[i][j] == 'w'){
                                    water[i][j-1] = 'w';
                                }
                                if (water[i][j] == '#'){
                                    water[i][j-1] = '#';
                                }
                            }
                            char tmp = rand();
                            if (tmp%2==0){
                                water[i][cols-1] = '#';
                            } else water[i][cols-1] = 'w';
                        }
                    }
                }

                //removing single # ocurrences
                for (int i =0; i<lines-8; i++){
                    if (ghosts[i]== 'w'){
                        if (stream[i] == 'r') {
                            for (int j = cols-3; j>0; j--){
                                if (water[i][j] == '#' && water[i][j-1] == 'w' && water[i][j+1] == 'w'){
                                    water[i][j] = 'w';
                                }
                            }
                        } else if (stream[i] == 'l'){
                            for (int j = 1; j<cols-1; j++){
                                if (water[i][j] == '#' && water[i][j+1]=='w'&& water[i][j-1]=='w'){
                                    water[i][j] = 'w';
                                }
                            }
                        }
                    }
                }
                
                nanosleep(&ts, NULL);

                //placing the ghost
                for (int i = 0; i<lines-8; i++){
                    if (ghosts[i] == '@'){
                        wattron(win, COLOR_PAIR(colors[i]));
                        mvwprintw(win, 4+i, cords[i], "@");
                        wattroff(win, COLOR_PAIR(colors[i]));
                        //wrefresh(win);
                    }   
                }

                int input = wgetch(win);
                wrefresh(win);
                switch(input){
                    case 'q': case 'Q' : {
                        print_message(4, 25, "QUIT", COLOR_PAIR(2));
                        wrefresh(win);
                        return;
                        
                    }
                    case 'h' : case 'H' : {
                        WINDOW *cwin = newwin(16, 23, lines / 2 - 15 / 2, cols / 2 - 23 / 2);
                        do{
                            
                            wattrset(cwin, A_BOLD | COLOR_PAIR(6));
                            box(cwin, 0, 0);
                            print_in_middle(cwin, 2, 0, 23, "Hopefully you are", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 3, 0, 23, "familliar with the", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 4, 0, 23, "mechanics already.", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 5, 0, 23, "Here you can play", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 6, 0, 23, "as long as you don't", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 7, 0, 23, "die.", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 8, 0, 23, "You can view the log", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 9, 0, 23, "of all your attempts", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 10, 0, 23, "in the <endless.txt>", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 11, 0, 23, "file.", A_BOLD | COLOR_PAIR(6));                   
                            wattrset(cwin, ~A_BOLD & COLOR_PAIR(6));
                            print_in_middle(cwin, 15 - 1, 0, 23, "Press <ENTER> to play", COLOR_PAIR(1));
                        } while (wgetch(cwin) != 10);
                        //destroying the window
                        wborder(cwin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
                        for (int j = 0; j<15; j++){
                            for (int o = 0; o<24; o++){
                                mvwaddch(cwin, j, o, ' ');
                            }
                        }
                        wrefresh(cwin);
                        wrefresh(win);
                        delwin(cwin);
                        
                        break;
                    }
                    case 'p' : case 'P' :{
                        print_message(4, 25, "PAUSE", COLOR_PAIR(6));
                        wrefresh(win);
                        break;
                    }
                    case KEY_DOWN :{
                        flushinp();
                        if (y!=lines-1){
                            mvwprintw (win, y,x, " ");
                            y++;
                            distance++;
                        }
                        break;
                    }
                    case KEY_UP : {
                        flushinp();
                        if (y!=0){
                            mvwprintw (win, y,x, " ");
                            y--;
                            distance++;
                        }
                        break;
                    }
                    case KEY_RIGHT : {
                        flushinp();
                        if (x!=cols-1){
                            mvwprintw (win, y,x, " ");
                            x++;
                            distance++;
                        }
                        break;
                    }
                    case KEY_LEFT : {
                        flushinp();
                        if (x!=0){
                            mvwprintw (win, y,x, " ");
                            x--;
                            distance++;
                        }
                        break;
                    }
                }

                //building the exterior
                wattron(win, A_BOLD | COLOR_PAIR(3));    
                mvwaddch(win, 1, 0, ACS_LTEE);
                mvwhline(win, 1, 1, ACS_HLINE, cols - 1);
                mvwaddch(win, 1, cols - 1, ACS_RTEE);
                mvwprintw (win, 0, 1, "[P]ause | [Q]uit | [H]elp " );
                mvwprintw (win, lines-1, 1, "Distance: %d", distance);
                mvwprintw (win, lines-1, 25, "Combo: %d", combo);
                wattroff(win, A_BOLD | COLOR_PAIR(3));  
                //the burrow
                wattron(win, A_BOLD | COLOR_PAIR(9));    
                mvwprintw(win, lines -2, burrowx, "O");
                wattroff(win, A_BOLD | COLOR_PAIR(9));

                //placing the ghost
                for (int i = 0; i<lines-8; i++){
                    if (ghosts[i] == '@'){
                        wattron(win, COLOR_PAIR(colors[i]));
                        mvwprintw(win, 4+i, cords[i], "@");
                        wattroff(win, COLOR_PAIR(colors[i]));
                    }   
                }

                //place the water
                for (int i = 0; i<cols-8; i++){
                    if (ghosts[i] == 'w'){
                        for (int j = 0; j<cols; j++){
                            char tmp = water[i][j];
                            if (tmp == 'w') {
                                wattron (win, COLOR_PAIR(10));
                                mvwprintw (win, i+4, j, "w" );
                                wattroff (win, COLOR_PAIR(10));
                            }
                            if (tmp == '#') {
                                wattron (win, COLOR_PAIR(11));
                                mvwprintw (win, i+4, j, "#" );
                                wattroff (win, COLOR_PAIR(11));
                            }
                        }
                    }
                }

                //placing froggy
                wattron (win, COLOR_PAIR(8));
                mvwprintw(win, y, x, "F");
                wattroff (win, COLOR_PAIR(8));

                wrefresh(win);

                //checking for the ghosts
                if (ghosts[y-4] == '@' && cords[y-4] == x){
                    print_message(4, 25, "YOU WERE EATEN!", COLOR_PAIR(4));
                    wrefresh(win);
                    breaker = 1;
                    break;
                }

                //hopping on a branch
                if (ghosts[y-4]=='w' && water[y-4][x] == '#'){
                    if (stream[y-4]=='r') {
                        x++;
                    } else x--;
                    if (x==cols || x==-1){
                        print_message(4, 25, "YOUR BURROW IS LOST!", COLOR_PAIR(5));
                        wrefresh(win);
                        breaker = 1;
                        break;
                    }
                }


                //checking for water
                if (ghosts[y-4]=='w' && water[y-4][x] == 'w'){
                    print_message(4, 25, "YOU DIED!", COLOR_PAIR(5));
                    wrefresh(win);
                    breaker = 1;
                    break;
                }
                

                //checking for the burrow
                if (y==lines-2 && x == burrowx) {
                    print_message(4, 25, "YOU'RE HOME!", COLOR_PAIR(3));
                    wrefresh(win);
                    combo++;
                    break;
                }
                wrefresh(win);

            }
            
            wrefresh(win);
            delwin(win);
        } else {
            //randomizing level 3
            WINDOW *win = newwin (lines, cols, 0, 0);
            srand(time(NULL));
            char ghosts[lines-8];
            int cords[lines-8];
            int colors[lines-8];
            nodelay(win, TRUE);
            curs_set(FALSE);
            keypad(win,TRUE);
            int distance = 0;
            int burrowx = rand()%cols;

            //building the exterior
            wattron(win, A_BOLD | COLOR_PAIR(3));    
            mvwaddch(win, 1, 0, ACS_LTEE);
            mvwhline(win, 1, 1, ACS_HLINE, cols - 1);
            mvwaddch(win, 1, cols - 1, ACS_RTEE);
            mvwprintw (win, 0, 1, "[P]ause | [Q]uit | [H]elp " );
            mvwprintw (win, lines-1, 1, "Distance: %d", distance);
            wattroff(win, A_BOLD | COLOR_PAIR(3));    
            //the burrow
            wattron(win, A_BOLD | COLOR_PAIR(9));    
            mvwprintw(win, lines -2, burrowx, "O");
            wattroff(win, A_BOLD | COLOR_PAIR(9));    
            
            //randomizing the ghosts and water and barriers
            for (int i = 0; i<lines-8; i++){
                int tmp = rand();
                if (tmp%5==0) {
                    ghosts[i] = ' ';
                } else if (tmp%7 ==0){
                    ghosts[i] = 'w';
                } else if (tmp%4 == 0){
                    ghosts[i] = 'b';
                } else ghosts[i] = '@';
            }

            //removing two barriers side by side
            for (int i =0; i<lines-8; i++){
                    if (ghosts[i]== 'b' && ghosts[i+1]=='b'){
                        ghosts[i+1]= '@';
                    }
            }

            //initializing the directions of the rivers
            char stream[lines-8];
            for (int i = 0; i<lines-8; i++){
                if (i%2 ==0){
                    stream[i] = 'r';
                } else stream[i] = 'l';
            }

            //randomizing the color for the ghosts
            for (int i = 0; i<lines-8; i++){
                int clr = rand() % 7;
                colors[i] = clr;  
            }

            struct timespec ts = {
                    .tv_sec = 0,                    // nr of secs
                    .tv_nsec = 0.001 * 1000000000L  // nr of nanosecs
            };


            ///unleashing the ghosts
            for (int i = 0; i<lines-8; i++){
                    int tmp = rand();
                    cords[i] = tmp%cols;
                    if (ghosts[i] == '@'){
                        wattron(win, COLOR_PAIR(colors[i]));
                        mvwprintw(win, 4+i, cords[i], "@");
                        wattroff(win, COLOR_PAIR(colors[i]));
                    }   
            }

            //generating the water and walls
            char water[lines-8][cols];
            for (int i =0; i<lines-8; i++){
                if (ghosts[i]== 'w'){
                    for (int j = 0; j<cols; j++){
                        int tmp = rand();
                        if (tmp%2 == 0){
                            water[i][j] = '#';
                        } else water[i][j] = 'w';
                    }
                } else if (ghosts[i]== 'b'){
                    for (int j = 0; j<cols; j++){
                        int tmp = rand();
                        if (tmp%6 == 0){
                            water[i][j] = ' ';
                        } else water[i][j] = '=';
                    }
                }
            }
            
            //place the water and barriers
            for (int i = 0; i<cols-8; i++){
                if (ghosts[i] == 'w'){
                    for (int j = 0; j<cols; j++){
                        char tmp = water[i][j];
                        if (tmp == 'w') {
                            wattron (win, COLOR_PAIR(10));
                            mvwprintw (win, i+4, j, "w" );
                            wattroff (win, COLOR_PAIR(10));
                        }
                        if (tmp == '#') {
                            wattron (win, COLOR_PAIR(11));
                            mvwprintw (win, i+4, j, "#" );
                            wattroff (win, COLOR_PAIR(11));
                        }
                    }
                }
                if (ghosts[i] == 'b'){
                    for (int j = 0; j<cols; j++){
                        char tmp = water[i][j];
                        if (tmp == '=') {
                            wattron (win, COLOR_PAIR(1));
                            mvwprintw (win, i+4, j, "=" );
                            wattroff (win, COLOR_PAIR(1));
                        }
                    }
                }
            }

            //placing froggy
            int x = cols/2, y = 3;
            wattron (win, COLOR_PAIR(8));
            mvwprintw(win, y, x, "F");
            wattroff (win, COLOR_PAIR(8));

            wrefresh(win);

            ts.tv_nsec = 0.1 * 1000000000L;


            char dir[lines-8];
            //picking the direction of the ghosts
            for (int i = 0; i<lines-8; i++){
                int tmp = rand();
                if (tmp% 2 == 0){
                    dir[i] ='r';
                } else {
                    dir[i] = 'l';
                }
            }
            //game loop
            while(1){
                //moving the ghosts
                for (int i = 0; i<lines-8; i++){
                    if (dir[i]== 'r'){
                        if (cords[i]==cols){
                            dir[i]= 'l';
                            mvwprintw(win, i+4, cords[i], " ");
                            cords[i]--;
                            continue;
                        } else{
                            mvwprintw(win, i+4, cords[i], " ");
                            cords[i]++;
                        }
                    } else if (dir[i] == 'l'){
                        if (cords[i]== 0){
                            dir[i] = 'r';
                            mvwprintw(win, i+4, cords[i], " ");
                            cords[i]++;
                            continue;
                        } else {
                            mvwprintw(win, i+4, cords[i], " ");
                            cords[i]--;
                        }
                    }
                }

                //moving the water
                for (int i = 0; i<lines-8; i++){
                    if (ghosts[i]== 'w'){
                        if (stream[i] == 'r') {
                            for (int j = cols-2; j>-1; j--){
                                if (water[i][j] == 'w'){
                                    water[i][j+1] = 'w';
                                }
                                if (water[i][j] == '#'){
                                    water[i][j+1] = '#';
                                }
                            }
                            char tmp = rand();
                            if (tmp%2==0){
                                water[i][0] = '#';
                            } else water[i][0] = 'w';
                        } else if (stream[i] == 'l'){
                            for (int j = 0; j<cols; j++){
                                if (water[i][j] == 'w'){
                                    water[i][j-1] = 'w';
                                }
                                if (water[i][j] == '#'){
                                    water[i][j-1] = '#';
                                }
                            }
                            char tmp = rand();
                            if (tmp%2==0){
                                water[i][cols-1] = '#';
                            } else water[i][cols-1] = 'w';
                        }
                    }
                }

                //removing single # ocurrences
                for (int i =0; i<lines-8; i++){
                    if (ghosts[i]== 'w'){
                        if (stream[i] == 'r') {
                            for (int j = cols-3; j>0; j--){
                                if (water[i][j] == '#' && water[i][j-1] == 'w' && water[i][j+1] == 'w'){
                                    water[i][j] = 'w';
                                }
                            }
                        } else if (stream[i] == 'l'){
                            for (int j = 1; j<cols-1; j++){
                                if (water[i][j] == '#' && water[i][j+1]=='w'&& water[i][j-1]=='w'){
                                    water[i][j] = 'w';
                                }
                            }
                        }
                    }
                }
                
                nanosleep(&ts, NULL);

                //placing the ghost
                for (int i = 0; i<lines-8; i++){
                    if (ghosts[i] == '@'){
                        wattron(win, COLOR_PAIR(colors[i]));
                        mvwprintw(win, 4+i, cords[i], "@");
                        wattroff(win, COLOR_PAIR(colors[i]));
                    }   
                }

                int input = wgetch(win);
                wrefresh(win);
                switch(input){
                    case 'q': case 'Q' : {
                        print_message(4, 25, "QUIT", COLOR_PAIR(2));
                        wrefresh(win);
                        return;
                        
                    }
                    case 'h' : case 'H' : {
                        WINDOW *cwin = newwin(16, 23, lines / 2 - 15 / 2, cols / 2 - 23 / 2);
                        do{
                            
                            wattrset(cwin, A_BOLD | COLOR_PAIR(6));
                            box(cwin, 0, 0);
                            print_in_middle(cwin, 2, 0, 23, "Hopefully you are", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 3, 0, 23, "familliar with the", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 4, 0, 23, "mechanics already.", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 5, 0, 23, "Here you can play", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 6, 0, 23, "as long as you don't", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 7, 0, 23, "die.", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 8, 0, 23, "You can view the log", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 9, 0, 23, "of all your attempts", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 10, 0, 23, "in the <endless.txt>", A_BOLD | COLOR_PAIR(6));
                            print_in_middle(cwin, 11, 0, 23, "file.", A_BOLD | COLOR_PAIR(6));                      
                            wattrset(cwin, ~A_BOLD & COLOR_PAIR(6));
                            print_in_middle(cwin, 15 - 1, 0, 23, "Press <ENTER> to play", COLOR_PAIR(1));
                        } while (wgetch(cwin) != 10);
                        //destroying the window
                        wborder(cwin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
                        for (int j = 0; j<15; j++){
                            for (int o = 0; o<24; o++){
                                mvwaddch(cwin, j, o, ' ');
                            }
                        }
                        wrefresh(cwin);
                        wrefresh(win);
                        delwin(cwin);
                        
                        break;
                    }
                    case 'p' : case 'P' :{
                        print_message(4, 25, "PAUSE", COLOR_PAIR(6));
                        wrefresh(win);
                        break;
                    }
                    case KEY_DOWN :{
                        flushinp();
                        if (ghosts[y-3] == 'b' && water [y-3][x]== '='){
                            break;
                        }
                        if (y!=lines-1){
                            mvwprintw (win, y,x, " ");
                            y++;
                            distance++;
                        }
                        break;
                    }
                    case KEY_UP : {
                        flushinp();
                        if (ghosts[y-5] == 'b' && water [y-5][x]== '='){
                            break;
                        }
                        if (y!=0 ){
                            mvwprintw (win, y,x, " ");
                            y--;
                            distance++;
                        }
                        break;
                    }
                    case KEY_RIGHT : {
                        flushinp();
                        if (ghosts[y-4] == 'b' && water [y-4][x+1]== '='){
                            break;
                        }
                        if (x!=cols-1 ){
                            mvwprintw (win, y,x, " ");
                            x++;
                            distance++;
                        }
                        break;
                    }
                    case KEY_LEFT : {
                        flushinp();
                        if (ghosts[y-4] == 'b' && water [y-4][x-1]== '='){
                            break;
                        }
                        if (x!=0 ){
                            mvwprintw (win, y,x, " ");
                            x--;
                            distance++;
                        }
                        break;
                    }
                }

                //building the exterior
                wattron(win, A_BOLD | COLOR_PAIR(3));    
                mvwaddch(win, 1, 0, ACS_LTEE);
                mvwhline(win, 1, 1, ACS_HLINE, cols - 1);
                mvwaddch(win, 1, cols - 1, ACS_RTEE);
                mvwprintw (win, 0, 1, "[P]ause | [Q]uit | [H]elp " );
                mvwprintw (win, lines-1, 1, "Distance: %d", distance);
                wattroff(win, A_BOLD | COLOR_PAIR(3));  
                //the burrow
                wattron(win, A_BOLD | COLOR_PAIR(9));    
                mvwprintw(win, lines -2, burrowx, "O");
                wattroff(win, A_BOLD | COLOR_PAIR(9));

                //placing the ghost
                for (int i = 0; i<lines-8; i++){
                    if (ghosts[i] == '@'){
                        wattron(win, COLOR_PAIR(colors[i]));
                        mvwprintw(win, 4+i, cords[i], "@");
                        wattroff(win, COLOR_PAIR(colors[i]));
                    }   
                }

                //place the water and barriers
            for (int i = 0; i<cols-8; i++){
                if (ghosts[i] == 'w'){
                    for (int j = 0; j<cols; j++){
                        char tmp = water[i][j];
                        if (tmp == 'w') {
                            wattron (win, COLOR_PAIR(10));
                            mvwprintw (win, i+4, j, "w" );
                            wattroff (win, COLOR_PAIR(10));
                        }
                        if (tmp == '#') {
                            wattron (win, COLOR_PAIR(11));
                            mvwprintw (win, i+4, j, "#" );
                            wattroff (win, COLOR_PAIR(11));
                        }
                    }
                }
                if (ghosts[i] == 'b'){
                    for (int j = 0; j<cols; j++){
                        char tmp = water[i][j];
                        if (tmp == '=') {
                            wattron (win, COLOR_PAIR(1));
                            mvwprintw (win, i+4, j, "=" );
                            wattroff (win, COLOR_PAIR(1));
                        }
                    }
                }
            }

                //placing froggy
                wattron (win, COLOR_PAIR(8));
                mvwprintw(win, y, x, "F");
                wattroff (win, COLOR_PAIR(8));

                wrefresh(win);

                //checking for the ghosts
                if (ghosts[y-4] == '@' && cords[y-4] == x){
                    print_message(4, 25, "YOU WERE EATEN", COLOR_PAIR(4));
                    wrefresh(win);
                    breaker = 1;
                    break;
                }

                //hopping on a branch
                if (ghosts[y-4]=='w' && water[y-4][x] == '#'){
                    if (stream[y-4]=='r') {
                        x++;
                    } else x--;
                    if (x==cols || x==-1){
                        print_message(4, 25, "YOUR BURROW IS LOST!", COLOR_PAIR(5));
                        wrefresh(win);
                        breaker = 1;
                        break;
                    }
                }


                //checking for water
                if (ghosts[y-4]=='w' && water[y-4][x] == 'w'){
                    print_message(4, 25, "YOU DIED!", COLOR_PAIR(5));
                    wrefresh(win);
                    breaker = 1;
                    break;
                }
                

                //checking for the burrow
                if (y==lines-2 && x == burrowx) {
                    print_message(4, 25, "YOU'RE HOME!", COLOR_PAIR(3));
                    wrefresh(win);
                    combo ++;
                    break;
                }
                wrefresh(win);

            }
            
            wrefresh(win);
            delwin(win);
            curs_set(TRUE);
        }
        
        if (breaker == 1){
            //saving the score
            FILE * fscore = fopen ("endless.txt", "a");
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            fseek(fscore, -1, SEEK_END);
            fprintf (fscore, "DATE AND TIME %s MAX SCORE %d\n", asctime(tm), combo);
            fclose(fscore);
            print_message(4, 25, "GOOD GAME!", COLOR_PAIR(3));
                    curs_set(TRUE);
                    break;
        }
    }
}

void init_lvl3(const int lines, const int cols){
    WINDOW *win = newwin (lines, cols, 0, 0);
    srand(time(NULL));
    char ghosts[lines-8];
    int cords[lines-8];
    int colors[lines-8];
    nodelay(win, TRUE);
    curs_set(FALSE);
    keypad(win,TRUE);
    int distance = 0;
    int burrowx = rand()%cols;

    //building the exterior
    wattron(win, A_BOLD | COLOR_PAIR(3));    
    mvwaddch(win, 1, 0, ACS_LTEE);
    mvwhline(win, 1, 1, ACS_HLINE, cols - 1);
    mvwaddch(win, 1, cols - 1, ACS_RTEE);
    mvwprintw (win, 0, 1, "[P]ause | [Q]uit | [H]elp " );
    mvwprintw (win, lines-1, 1, "Distance: %d", distance);
    wattroff(win, A_BOLD | COLOR_PAIR(3));    
    //the burrow
    wattron(win, A_BOLD | COLOR_PAIR(9));    
    mvwprintw(win, lines -2, burrowx, "O");
    wattroff(win, A_BOLD | COLOR_PAIR(9));    
    
    //randomizing the ghosts and water and barriers
    for (int i = 0; i<lines-8; i++){
        int tmp = rand();
        if (tmp%5==0) {
            ghosts[i] = ' ';
        } else if (tmp%7 ==0){
            ghosts[i] = 'w';
        } else if (tmp%4 == 0){
            ghosts[i] = 'b';
        } else ghosts[i] = '@';
    }

    //removing two barriers side by side
    for (int i =0; i<lines-8; i++){
            if (ghosts[i]== 'b' && ghosts[i+1]=='b'){
                ghosts[i+1]= '@';
            }
    }

    //initializing the directions of the rivers
    char stream[lines-8];
    for (int i = 0; i<lines-8; i++){
        if (i%2 ==0){
            stream[i] = 'r';
        } else stream[i] = 'l';
    }

    //randomizing the color for the ghosts
    for (int i = 0; i<lines-8; i++){
        int clr = rand() % 7;
        colors[i] = clr;  
    }

    struct timespec ts = {
            .tv_sec = 0,                    // nr of secs
            .tv_nsec = 0.001 * 1000000000L  // nr of nanosecs
    };


    ///unleashing the ghosts
    for (int i = 0; i<lines-8; i++){
            int tmp = rand();
            cords[i] = tmp%cols;
            if (ghosts[i] == '@'){
                wattron(win, COLOR_PAIR(colors[i]));
                mvwprintw(win, 4+i, cords[i], "@");
                wattroff(win, COLOR_PAIR(colors[i]));
            }   
    }

    //generating the water and walls
    char water[lines-8][cols];
    for (int i =0; i<lines-8; i++){
        if (ghosts[i]== 'w'){
            for (int j = 0; j<cols; j++){
                int tmp = rand();
                if (tmp%2 == 0){
                    water[i][j] = '#';
                } else water[i][j] = 'w';
            }
        } else if (ghosts[i]== 'b'){
            for (int j = 0; j<cols; j++){
                int tmp = rand();
                if (tmp%6 == 0){
                    water[i][j] = ' ';
                } else water[i][j] = '=';
            }
        }
    }
    
    //place the water and barriers
    for (int i = 0; i<cols-8; i++){
        if (ghosts[i] == 'w'){
            for (int j = 0; j<cols; j++){
                char tmp = water[i][j];
                if (tmp == 'w') {
                    wattron (win, COLOR_PAIR(10));
                    mvwprintw (win, i+4, j, "w" );
                    wattroff (win, COLOR_PAIR(10));
                }
                if (tmp == '#') {
                    wattron (win, COLOR_PAIR(11));
                    mvwprintw (win, i+4, j, "#" );
                    wattroff (win, COLOR_PAIR(11));
                }
            }
        }
        if (ghosts[i] == 'b'){
            for (int j = 0; j<cols; j++){
                char tmp = water[i][j];
                if (tmp == '=') {
                    wattron (win, COLOR_PAIR(1));
                    mvwprintw (win, i+4, j, "=" );
                    wattroff (win, COLOR_PAIR(1));
                }
            }
        }
    }

    //placing froggy
    int x = cols/2, y = 3;
    wattron (win, COLOR_PAIR(8));
    mvwprintw(win, y, x, "F");
    wattroff (win, COLOR_PAIR(8));

    wrefresh(win);

    //about the game
    WINDOW *cwin = newwin(10, 23, lines / 2 - 10 / 2, cols / 2 - 23 / 2);
    do{
                    
        wattrset(cwin, A_BOLD | COLOR_PAIR(6));
        box(cwin, 0, 0);
        print_in_middle(cwin, 2, 0, 23, "And now you also have", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 3, 0, 23, "to get through the", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 4, 0, 23, "barbed wire (===).", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 5, 0, 23, "Look for the holes!", A_BOLD | COLOR_PAIR(6));
        wattrset(cwin, ~A_BOLD & COLOR_PAIR(6));
        print_in_middle(cwin, 9 - 1, 0, 23, "Press <ENTER> to play", COLOR_PAIR(1));
    } while (wgetch(cwin) != 10);
    //destroying the window
    wborder(cwin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    for (int j = 0; j<15; j++){
        for (int o = 0; o<24; o++){
            mvwaddch(cwin, j, o, ' ');
        }
    }
    wrefresh(cwin);
    wrefresh(win);
    delwin(cwin);            

    ts.tv_nsec = 0.1 * 1000000000L;


    char dir[lines-8];
    //picking the direction of the ghosts
    for (int i = 0; i<lines-8; i++){
        int tmp = rand();
        if (tmp% 2 == 0){
            dir[i] ='r';
        } else {
            dir[i] = 'l';
        }
    }
    //game loop
    while(1){
        //moving the ghosts
        for (int i = 0; i<lines-8; i++){
            if (dir[i]== 'r'){
                if (cords[i]==cols){
                    dir[i]= 'l';
                    mvwprintw(win, i+4, cords[i], " ");
                    cords[i]--;
                    continue;
                } else{
                    mvwprintw(win, i+4, cords[i], " ");
                    cords[i]++;
                }
            } else if (dir[i] == 'l'){
                if (cords[i]== 0){
                    dir[i] = 'r';
                    mvwprintw(win, i+4, cords[i], " ");
                    cords[i]++;
                    continue;
                } else {
                    mvwprintw(win, i+4, cords[i], " ");
                    cords[i]--;
                }
            }
        }

        //moving the water
        for (int i = 0; i<lines-8; i++){
            if (ghosts[i]== 'w'){
                if (stream[i] == 'r') {
                    for (int j = cols-2; j>-1; j--){
                        if (water[i][j] == 'w'){
                            water[i][j+1] = 'w';
                        }
                        if (water[i][j] == '#'){
                            water[i][j+1] = '#';
                        }
                    }
                    char tmp = rand();
                    if (tmp%2==0){
                        water[i][0] = '#';
                    } else water[i][0] = 'w';
                } else if (stream[i] == 'l'){
                    for (int j = 0; j<cols; j++){
                        if (water[i][j] == 'w'){
                            water[i][j-1] = 'w';
                        }
                        if (water[i][j] == '#'){
                            water[i][j-1] = '#';
                        }
                    }
                    char tmp = rand();
                    if (tmp%2==0){
                        water[i][cols-1] = '#';
                    } else water[i][cols-1] = 'w';
                }
            }
        }

        //removing single # ocurrences
        for (int i =0; i<lines-8; i++){
            if (ghosts[i]== 'w'){
                if (stream[i] == 'r') {
                    for (int j = cols-3; j>0; j--){
                        if (water[i][j] == '#' && water[i][j-1] == 'w' && water[i][j+1] == 'w'){
                            water[i][j] = 'w';
                        }
                    }
                } else if (stream[i] == 'l'){
                    for (int j = 1; j<cols-1; j++){
                        if (water[i][j] == '#' && water[i][j+1]=='w'&& water[i][j-1]=='w'){
                            water[i][j] = 'w';
                        }
                    }
                }
            }
        }
        
        nanosleep(&ts, NULL);

        //placing the ghost
        for (int i = 0; i<lines-8; i++){
            if (ghosts[i] == '@'){
                wattron(win, COLOR_PAIR(colors[i]));
                mvwprintw(win, 4+i, cords[i], "@");
                wattroff(win, COLOR_PAIR(colors[i]));
            }   
        }

        int input = wgetch(win);
        wrefresh(win);
        switch(input){
            case 'q': case 'Q' : {
                print_message(4, 25, "QUIT", COLOR_PAIR(2));
                wrefresh(win);
                return;
                
            }
            case 'h' : case 'H' : {
                WINDOW *cwin = newwin(10, 23, lines / 2 - 10 / 2, cols / 2 - 23 / 2);
                do{
                    
                    wattrset(cwin, A_BOLD | COLOR_PAIR(6));
                    box(cwin, 0, 0);
                    print_in_middle(cwin, 2, 0, 23, "And now you also have", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 3, 0, 23, "to get through the", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 4, 0, 23, "barbed wire (===).", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 5, 0, 23, "Look for the holes!", A_BOLD | COLOR_PAIR(6));                   
                    wattrset(cwin, ~A_BOLD & COLOR_PAIR(6));
                    print_in_middle(cwin, 9 - 1, 0, 23, "Press <ENTER> to play", COLOR_PAIR(1));
                } while (wgetch(cwin) != 10);
                //destroying the window
                wborder(cwin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
                for (int j = 0; j<15; j++){
                    for (int o = 0; o<24; o++){
                        mvwaddch(cwin, j, o, ' ');
                    }
                }
                wrefresh(cwin);
                wrefresh(win);
                delwin(cwin);
                
                break;
            }
            case 'p' : case 'P' :{
                print_message(4, 25, "PAUSE", COLOR_PAIR(6));
                wrefresh(win);
                break;
            }
            case KEY_DOWN :{
                flushinp();
                if (ghosts[y-3] == 'b' && water [y-3][x]== '='){
                    break;
                }
                if (y!=lines-1){
                    mvwprintw (win, y,x, " ");
                    y++;
                    distance++;
                }
                break;
            }
            case KEY_UP : {
                flushinp();
                if (ghosts[y-5] == 'b' && water [y-5][x]== '='){
                    break;
                }
                if (y!=0 ){
                    mvwprintw (win, y,x, " ");
                    y--;
                    distance++;
                }
                break;
            }
            case KEY_RIGHT : {
                flushinp();
                if (ghosts[y-4] == 'b' && water [y-4][x+1]== '='){
                    break;
                }
                if (x!=cols-1 ){
                    mvwprintw (win, y,x, " ");
                    x++;
                    distance++;
                }
                break;
            }
            case KEY_LEFT : {
                flushinp();
                if (ghosts[y-4] == 'b' && water [y-4][x-1]== '='){
                    break;
                }
                if (x!=0 ){
                    mvwprintw (win, y,x, " ");
                    x--;
                    distance++;
                }
                break;
            }
        }

        //building the exterior
        wattron(win, A_BOLD | COLOR_PAIR(3));    
        mvwaddch(win, 1, 0, ACS_LTEE);
        mvwhline(win, 1, 1, ACS_HLINE, cols - 1);
        mvwaddch(win, 1, cols - 1, ACS_RTEE);
        mvwprintw (win, 0, 1, "[P]ause | [Q]uit | [H]elp " );
        mvwprintw (win, lines-1, 1, "Distance: %d", distance);
        wattroff(win, A_BOLD | COLOR_PAIR(3));  
        //the burrow
        wattron(win, A_BOLD | COLOR_PAIR(9));    
        mvwprintw(win, lines -2, burrowx, "O");
        wattroff(win, A_BOLD | COLOR_PAIR(9));

        //placing the ghost
        for (int i = 0; i<lines-8; i++){
            if (ghosts[i] == '@'){
                wattron(win, COLOR_PAIR(colors[i]));
                mvwprintw(win, 4+i, cords[i], "@");
                wattroff(win, COLOR_PAIR(colors[i]));
            }   
        }

        //place the water and barriers
    for (int i = 0; i<cols-8; i++){
        if (ghosts[i] == 'w'){
            for (int j = 0; j<cols; j++){
                char tmp = water[i][j];
                if (tmp == 'w') {
                    wattron (win, COLOR_PAIR(10));
                    mvwprintw (win, i+4, j, "w" );
                    wattroff (win, COLOR_PAIR(10));
                }
                if (tmp == '#') {
                    wattron (win, COLOR_PAIR(11));
                    mvwprintw (win, i+4, j, "#" );
                    wattroff (win, COLOR_PAIR(11));
                }
            }
        }
        if (ghosts[i] == 'b'){
            for (int j = 0; j<cols; j++){
                char tmp = water[i][j];
                if (tmp == '=') {
                    wattron (win, COLOR_PAIR(1));
                    mvwprintw (win, i+4, j, "=" );
                    wattroff (win, COLOR_PAIR(1));
                }
            }
        }
    }

        //placing froggy
        wattron (win, COLOR_PAIR(8));
        mvwprintw(win, y, x, "F");
        wattroff (win, COLOR_PAIR(8));

        wrefresh(win);

        //checking for the ghosts
        if (ghosts[y-4] == '@' && cords[y-4] == x){
            print_message(4, 25, "YOU WERE EATEN", COLOR_PAIR(4));
            wrefresh(win);
            break;
        }

        //hopping on a branch
        if (ghosts[y-4]=='w' && water[y-4][x] == '#'){
            if (stream[y-4]=='r') {
                x++;
            } else x--;
            if (x==cols || x==-1){
                print_message(4, 25, "YOUR BURROW IS LOST!", COLOR_PAIR(5));
                wrefresh(win);
                break;
            }
        }


        //checking for water
        if (ghosts[y-4]=='w' && water[y-4][x] == 'w'){
            print_message(4, 25, "YOU DIED!", COLOR_PAIR(5));
            wrefresh(win);
            break;
        }
        

        //checking for the burrow
        if (y==lines-2 && x == burrowx) {
            print_message(4, 25, "YOU'RE HOME!", COLOR_PAIR(3));
            wrefresh(win);
            break;
        }
        wrefresh(win);

    }
    
    wrefresh(win);
    delwin(win);
    curs_set(TRUE);
}

void init_lvl2(const int lines, const int cols){
    WINDOW *win = newwin (lines, cols, 0, 0);
    srand(time(NULL));
    char ghosts[lines-8];
    int cords[lines-8];
    int colors[lines-8];
    nodelay(win, TRUE);
    curs_set(FALSE);
    keypad(win,TRUE);
    int distance = 0;
    int burrowx = rand()%cols;

    //building the exterior
    wattron(win, A_BOLD | COLOR_PAIR(3));    
    mvwaddch(win, 1, 0, ACS_LTEE);
    mvwhline(win, 1, 1, ACS_HLINE, cols - 1);
    mvwaddch(win, 1, cols - 1, ACS_RTEE);
    mvwprintw (win, 0, 1, "[P]ause | [Q]uit | [H]elp " );
    mvwprintw (win, lines-1, 1, "Distance: %d", distance);
    wattroff(win, A_BOLD | COLOR_PAIR(3));    
    //the burrow
    wattron(win, A_BOLD | COLOR_PAIR(9));    
    mvwprintw(win, lines -2, burrowx, "O");
    wattroff(win, A_BOLD | COLOR_PAIR(9));    
    
    //randomizing the ghosts and water
    for (int i = 0; i<lines-8; i++){
        int tmp = rand();
        if (tmp%6==0) {
            ghosts[i] = ' ';
        } else if (tmp%7 ==0){
            ghosts[i] = 'w';
        } else ghosts[i] = '@';
    }

    //initializing the directions of the rivers
    char stream[lines-8];
    for (int i = 0; i<lines-8; i++){
        if (i%2 ==0){
            stream[i] = 'r';
        } else stream[i] = 'l';
    }

    //randomizing the color for the ghosts
    for (int i = 0; i<lines-8; i++){
        int clr = rand() % 7;
        colors[i] = clr;  
    }

    struct timespec ts = {
            .tv_sec = 0,                    // nr of secs
            .tv_nsec = 0.001 * 1000000000L  // nr of nanosecs
    };


    ///unleashing the ghosts
    for (int i = 0; i<lines-8; i++){
            int tmp = rand();
            cords[i] = tmp%cols;
            if (ghosts[i] == '@'){
                wattron(win, COLOR_PAIR(colors[i]));
                mvwprintw(win, 4+i, cords[i], "@");
                wattroff(win, COLOR_PAIR(colors[i]));
            }   
    }

    //generating the water
    char water[lines-8][cols];
    for (int i =0; i<lines-8; i++){
        if (ghosts[i]== 'w'){
            for (int j = 0; j<cols; j++){
                int tmp = rand();
                if (tmp%2 == 0){
                    water[i][j] = '#';
                } else water[i][j] = 'w';
            }
        }
    }

    //place the water
    for (int i = 0; i<cols-8; i++){
        if (ghosts[i] == 'w'){
            for (int j = 0; j<cols; j++){
                char tmp = water[i][j];
                if (tmp == 'w') {
                    wattron (win, COLOR_PAIR(10));
                    mvwprintw (win, i+4, j, "w" );
                    wattroff (win, COLOR_PAIR(10));
                }
                if (tmp == '#') {
                    wattron (win, COLOR_PAIR(11));
                    mvwprintw (win, i+4, j, "#" );
                    wattroff (win, COLOR_PAIR(11));
                }
            }
        }
    }

    //placing froggy
    int x = cols/2, y = 3;
    wattron (win, COLOR_PAIR(8));
    mvwprintw(win, y, x, "F");
    wattroff (win, COLOR_PAIR(8));

    wrefresh(win);

    //about the game
    WINDOW *cwin = newwin(16, 23, lines / 2 - 15 / 2, cols / 2 - 23 / 2);
    do{
                    
        wattrset(cwin, A_BOLD | COLOR_PAIR(6));
        box(cwin, 0, 0);
        print_in_middle(cwin, 2, 0, 23, "The rules are just", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 3, 0, 23, "like in level 1,", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 4, 0, 23, "except that the", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 5, 0, 23, "rivers have been", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 6, 0, 23, "poisoned by the", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 7, 0, 23, "factories, as our", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 8, 0, 23, "capitalist society", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 9, 0, 23, "continues to near its", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 10, 0, 23, "own demise. Hop onto", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 11, 0, 23, "branches (#) that", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 12, 0, 23, "swim by to get across", A_BOLD | COLOR_PAIR(6));                    
        wattrset(cwin, ~A_BOLD & COLOR_PAIR(6));
        print_in_middle(cwin, 15 - 1, 0, 23, "Press <ENTER> to play", COLOR_PAIR(1));
    } while (wgetch(cwin) != 10);
    //destroying the window
    wborder(cwin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    for (int j = 0; j<15; j++){
        for (int o = 0; o<24; o++){
            mvwaddch(cwin, j, o, ' ');
        }
    }
    wrefresh(cwin);
    wrefresh(win);
    delwin(cwin);            

    ts.tv_nsec = 0.1 * 1000000000L;


    char dir[lines-8];
    //picking the direction of the ghosts
    for (int i = 0; i<lines-8; i++){
        int tmp = rand();
        if (tmp% 2 == 0){
            dir[i] ='r';
        } else {
            dir[i] = 'l';
        }
    }
    //game loop
    while(1){
        //moving the ghosts
        for (int i = 0; i<lines-8; i++){
            if (dir[i]== 'r'){
                if (cords[i]==cols){
                    dir[i]= 'l';
                    mvwprintw(win, i+4, cords[i], " ");
                    cords[i]--;
                    continue;
                } else{
                    mvwprintw(win, i+4, cords[i], " ");
                    cords[i]++;
                }
            } else if (dir[i] == 'l'){
                if (cords[i]== 0){
                    dir[i] = 'r';
                    mvwprintw(win, i+4, cords[i], " ");
                    cords[i]++;
                    continue;
                } else {
                    mvwprintw(win, i+4, cords[i], " ");
                    cords[i]--;
                }
            }
        }

        //moving the water
        for (int i = 0; i<lines-8; i++){
            if (ghosts[i]== 'w'){
                if (stream[i] == 'r') {
                    for (int j = cols-2; j>-1; j--){
                        if (water[i][j] == 'w'){
                            water[i][j+1] = 'w';
                        }
                        if (water[i][j] == '#'){
                            water[i][j+1] = '#';
                        }
                    }
                    char tmp = rand();
                    if (tmp%2==0){
                        water[i][0] = '#';
                    } else water[i][0] = 'w';
                } else if (stream[i] == 'l'){
                    for (int j = 0; j<cols; j++){
                        if (water[i][j] == 'w'){
                            water[i][j-1] = 'w';
                        }
                        if (water[i][j] == '#'){
                            water[i][j-1] = '#';
                        }
                    }
                    char tmp = rand();
                    if (tmp%2==0){
                        water[i][cols-1] = '#';
                    } else water[i][cols-1] = 'w';
                }
            }
        }

        //removing single # ocurrences
        for (int i =0; i<lines-8; i++){
            if (ghosts[i]== 'w'){
                if (stream[i] == 'r') {
                    for (int j = cols-3; j>0; j--){
                        if (water[i][j] == '#' && water[i][j-1] == 'w' && water[i][j+1] == 'w'){
                            water[i][j] = 'w';
                        }
                    }
                } else if (stream[i] == 'l'){
                    for (int j = 1; j<cols-1; j++){
                        if (water[i][j] == '#' && water[i][j+1]=='w'&& water[i][j-1]=='w'){
                            water[i][j] = 'w';
                        }
                    }
                }
            }
        }
        
        nanosleep(&ts, NULL);

        //placing the ghost
        for (int i = 0; i<lines-8; i++){
            if (ghosts[i] == '@'){
                wattron(win, COLOR_PAIR(colors[i]));
                mvwprintw(win, 4+i, cords[i], "@");
                wattroff(win, COLOR_PAIR(colors[i]));
                //wrefresh(win);
            }   
        }

        int input = wgetch(win);
        wrefresh(win);
        switch(input){
            case 'q': case 'Q' : {
                print_message(4, 25, "QUIT", COLOR_PAIR(2));
                wrefresh(win);
                return;
                
            }
            case 'h' : case 'H' : {
                WINDOW *cwin = newwin(16, 23, lines / 2 - 15 / 2, cols / 2 - 23 / 2);
                do{
                    
                    wattrset(cwin, A_BOLD | COLOR_PAIR(6));
                    box(cwin, 0, 0);
                    print_in_middle(cwin, 2, 0, 23, "The rules are just", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 3, 0, 23, "like in level 1,", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 4, 0, 23, "except that the", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 5, 0, 23, "rivers have been", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 6, 0, 23, "poisoned by the", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 7, 0, 23, "factories, as our", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 8, 0, 23, "capitalist society", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 9, 0, 23, "continues to near its", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 10, 0, 23, "own demise. Hop onto", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 11, 0, 23, "branches (#) that", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 12, 0, 23, "swim by to get across", A_BOLD | COLOR_PAIR(6));                    
                    wattrset(cwin, ~A_BOLD & COLOR_PAIR(6));
                    print_in_middle(cwin, 15 - 1, 0, 23, "Press <ENTER> to play", COLOR_PAIR(1));
                } while (wgetch(cwin) != 10);
                //destroying the window
                wborder(cwin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
                for (int j = 0; j<15; j++){
                    for (int o = 0; o<24; o++){
                        mvwaddch(cwin, j, o, ' ');
                    }
                }
                wrefresh(cwin);
                wrefresh(win);
                delwin(cwin);
                
                break;
            }
            case 'p' : case 'P' :{
                print_message(4, 25, "PAUSE", COLOR_PAIR(6));
                wrefresh(win);
                break;
            }
            case KEY_DOWN :{
                flushinp();
                if (y!=lines-1){
                    mvwprintw (win, y,x, " ");
                    y++;
                    distance++;
                }
                break;
            }
            case KEY_UP : {
                flushinp();
                if (y!=0){
                    mvwprintw (win, y,x, " ");
                    y--;
                    distance++;
                }
                break;
            }
            case KEY_RIGHT : {
                flushinp();
                if (x!=cols-1){
                    mvwprintw (win, y,x, " ");
                    x++;
                    distance++;
                }
                break;
            }
            case KEY_LEFT : {
                flushinp();
                if (x!=0){
                    mvwprintw (win, y,x, " ");
                    x--;
                    distance++;
                }
                break;
            }
        }

        //building the exterior
        wattron(win, A_BOLD | COLOR_PAIR(3));    
        mvwaddch(win, 1, 0, ACS_LTEE);
        mvwhline(win, 1, 1, ACS_HLINE, cols - 1);
        mvwaddch(win, 1, cols - 1, ACS_RTEE);
        mvwprintw (win, 0, 1, "[P]ause | [Q]uit | [H]elp " );
        mvwprintw (win, lines-1, 1, "Distance: %d", distance);
        wattroff(win, A_BOLD | COLOR_PAIR(3));  
        //the burrow
        wattron(win, A_BOLD | COLOR_PAIR(9));    
        mvwprintw(win, lines -2, burrowx, "O");
        wattroff(win, A_BOLD | COLOR_PAIR(9));
        //wrefresh(win);  

        //placing the ghost
        for (int i = 0; i<lines-8; i++){
            if (ghosts[i] == '@'){
                wattron(win, COLOR_PAIR(colors[i]));
                mvwprintw(win, 4+i, cords[i], "@");
                wattroff(win, COLOR_PAIR(colors[i]));
                //wrefresh(win);
            }   
        }

        //place the water
    for (int i = 0; i<cols-8; i++){
        if (ghosts[i] == 'w'){
            for (int j = 0; j<cols; j++){
                char tmp = water[i][j];
                if (tmp == 'w') {
                    wattron (win, COLOR_PAIR(10));
                    mvwprintw (win, i+4, j, "w" );
                    wattroff (win, COLOR_PAIR(10));
                }
                if (tmp == '#') {
                    wattron (win, COLOR_PAIR(11));
                    mvwprintw (win, i+4, j, "#" );
                    wattroff (win, COLOR_PAIR(11));
                }
            }
        }
    }

        //placing froggy
        wattron (win, COLOR_PAIR(8));
        mvwprintw(win, y, x, "F");
        wattroff (win, COLOR_PAIR(8));

        wrefresh(win);

        //checking for the ghosts
        if (ghosts[y-4] == '@' && cords[y-4] == x){
            print_message(4, 25, "YOU WERE EATEN", COLOR_PAIR(4));
            wrefresh(win);
            break;
        }

        //hopping on a branch
        if (ghosts[y-4]=='w' && water[y-4][x] == '#'){
            if (stream[y-4]=='r') {
                x++;
            } else x--;
            if (x==cols || x==-1){
                print_message(4, 25, "YOUR BURROW IS LOST!", COLOR_PAIR(5));
                wrefresh(win);
                break;
            }
        }


        //checking for water
        if (ghosts[y-4]=='w' && water[y-4][x] == 'w'){
            print_message(4, 25, "YOU DIED!", COLOR_PAIR(5));
            wrefresh(win);
            break;
        }
        

        //checking for the burrow
        if (y==lines-2 && x == burrowx) {
            print_message(4, 25, "YOU'RE HOME!", COLOR_PAIR(3));
            wrefresh(win);
            break;
        }
        wrefresh(win);

    }
    
    wrefresh(win);
    delwin(win);
    curs_set(TRUE);
}

void init_lvl1(const int lines, const int cols){
    WINDOW *win = newwin (lines, cols, 0, 0);
    srand(time(NULL));
    char ghosts[lines-8];
    int cords[lines-8];
    int colors[lines-8];
    nodelay(win, TRUE);
    curs_set(FALSE);
    keypad(win,TRUE);
    int distance = 0;
    int burrowx = rand()%cols;

    //building the exterior
    wattron(win, A_BOLD | COLOR_PAIR(3));    
    mvwaddch(win, 1, 0, ACS_LTEE);
    mvwhline(win, 1, 1, ACS_HLINE, cols - 1);
    mvwaddch(win, 1, cols - 1, ACS_RTEE);
    mvwprintw (win, 0, 1, "[P]ause | [Q]uit | [H]elp " );
    mvwprintw (win, lines-1, 1, "Distance: %d", distance);
    wattroff(win, A_BOLD | COLOR_PAIR(3));    
    //the burrow
    wattron(win, A_BOLD | COLOR_PAIR(9));    
    mvwprintw(win, lines -2, burrowx, "O");
    wattroff(win, A_BOLD | COLOR_PAIR(9));    
    
    //randomizing the ghosts
    for (int i = 0; i<lines-8; i++){
        int tmp = rand();
        if (tmp%5==0) {
            ghosts[i] = ' ';
        } else ghosts[i] = '@';
    }

    //randomizing the color for the ghosts
    for (int i = 0; i<lines-8; i++){
        int clr = rand() % 7;
        colors[i] = clr;  
    }

    struct timespec ts = {
            .tv_sec = 0,                    // nr of secs
            .tv_nsec = 0.001 * 1000000000L  // nr of nanosecs
    };


    ///unleashing the ghosts
    for (int i = 0; i<lines-8; i++){
            int tmp = rand();
            cords[i] = tmp%cols;
            if (ghosts[i] == '@'){
                wattron(win, COLOR_PAIR(colors[i]));
                mvwprintw(win, 4+i, cords[i], "@");
                wattroff(win, COLOR_PAIR(colors[i]));

            }   
    }

    //placing froggy
    int x = cols/2, y = 3;
    wattron (win, COLOR_PAIR(8));
    mvwprintw(win, y, x, "F");
    wattroff (win, COLOR_PAIR(8));

    wrefresh(win);

    //about the game
    WINDOW *cwin = newwin(15, 23, lines / 2 - 15 / 2, cols / 2 - 23 / 2);
    do{
                    
        wattrset(cwin, A_BOLD | COLOR_PAIR(6));
        box(cwin, 0, 0);
        print_in_middle(cwin, 2, 0, 23, "You are a frog,", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 3, 0, 23, "your mark is 'F'.", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 4, 0, 23, "Your task is to", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 5, 0, 23, "get to your", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 6, 0, 23, "burrow (O).", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 7, 0, 23, "Birds (@) are evil", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 8, 0, 23, "and work for the", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 9, 0, 23, "bourgeoisie, so", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 10, 0, 23, "AVOID THEM!!!", A_BOLD | COLOR_PAIR(6));
        print_in_middle(cwin, 11, 0, 23, "AT ALL COSTS!!!!", A_BOLD | COLOR_PAIR(4));
        wattrset(cwin, ~A_BOLD & COLOR_PAIR(6));
        print_in_middle(cwin, 15 - 2, 0, 23, "Press <ENTER> to play", COLOR_PAIR(1));
    } while (wgetch(cwin) != 10);
    //destroying the window
    wborder(cwin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    for (int j = 0; j<15; j++){
        for (int o = 0; o<24; o++){
            mvwaddch(cwin, j, o, ' ');
        }
    }
    wrefresh(cwin);
    wrefresh(win);
    delwin(cwin);


    ts.tv_nsec = 0.01 * 10000000000L;


    char dir[lines-8];
    //picking the direction of the ghosts
    for (int i = 0; i<lines-8; i++){
        int tmp = rand();
        if (tmp% 2 == 0){
            dir[i] ='r';
        } else {
            dir[i] = 'l';
        }
    }
    //game loop
    while(1){
        //moving the ghosts
        for (int i = 0; i<lines-8; i++){
            if (dir[i]== 'r'){
                if (cords[i]==cols){
                    dir[i]= 'l';
                    mvwprintw(win, i+4, cords[i], " ");
                    cords[i]--;
                    continue;
                } else{
                    mvwprintw(win, i+4, cords[i], " ");
                    cords[i]++;
                }
            } else if (dir[i] == 'l'){
                if (cords[i]== 0){
                    dir[i] = 'r';
                    mvwprintw(win, i+4, cords[i], " ");
                    cords[i]++;
                    continue;
                } else {
                    mvwprintw(win, i+4, cords[i], " ");
                    cords[i]--;
                }
            }
        }
        
        nanosleep(&ts, NULL);

        //placing the ghost
        for (int i = 0; i<lines-8; i++){
            if (ghosts[i] == '@'){
                wattron(win, COLOR_PAIR(colors[i]));
                mvwprintw(win, 4+i, cords[i], "@");
                wattroff(win, COLOR_PAIR(colors[i]));
            }   
        }

        int input = wgetch(win);
        wrefresh(win);
        switch(input){
            case 'q': case 'Q' : {
                print_message(4, 25, "QUIT", COLOR_PAIR(2));
                wrefresh(win);
                return;
                
            }
            case 'h' : case 'H' : {
                WINDOW *cwin = newwin(15, 23, lines / 2 - 15 / 2, cols / 2 - 23 / 2);
                do{
                    
                    wattrset(cwin, A_BOLD | COLOR_PAIR(6));
                    box(cwin, 0, 0);
                    print_in_middle(cwin, 2, 0, 23, "You are a frog,", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 3, 0, 23, "your mark is 'F'.", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 4, 0, 23, "Your task is to", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 5, 0, 23, "get to your", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 6, 0, 23, "burrow (O).", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 7, 0, 23, "Birds (@) are evil", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 8, 0, 23, "and work for the", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 9, 0, 23, "bourgeoisie, so", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 10, 0, 23, "AVOID THEM!!!", A_BOLD | COLOR_PAIR(6));
                    print_in_middle(cwin, 11, 0, 23, "AT ALL COSTS!!!!", A_BOLD | COLOR_PAIR(4));
                    wattrset(cwin, ~A_BOLD & COLOR_PAIR(6));
                    print_in_middle(cwin, 15 - 2, 0, 23, "Press ENTER to resume", COLOR_PAIR(1));
                } while (wgetch(cwin) != 10);
                //destroying the window
                wborder(cwin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
                for (int j = 0; j<15; j++){
                    for (int o = 0; o<24; o++){
                        mvwaddch(cwin, j, o, ' ');
                    }
                }
                wrefresh(cwin);
                wrefresh(win);
                delwin(cwin);
                
                break;
            }
            case 'p' : case 'P' :{
                print_message(4, 25, "PAUSE", COLOR_PAIR(6));
                wrefresh(win);
                break;
            }
            case KEY_DOWN :{
                flushinp();
                if (y!=lines-1){
                    mvwprintw (win, y,x, " ");
                    y++;
                    distance++;
                }
                break;
            }
            case KEY_UP : {
                flushinp();
                if (y!=0){
                    mvwprintw (win, y,x, " ");
                    y--;
                    distance++;
                }
                break;
            }
            case KEY_RIGHT : {
                flushinp();
                if (x!=cols-1){
                    mvwprintw (win, y,x, " ");
                    x++;
                    distance++;
                }
                break;
            }
            case KEY_LEFT : {
                flushinp();
                if (x!=0){
                    mvwprintw (win, y,x, " ");
                    x--;
                    distance++;
                }
                break;
            }
        }

        //building the exterior
        wattron(win, A_BOLD | COLOR_PAIR(3));    
        mvwaddch(win, 1, 0, ACS_LTEE);
        mvwhline(win, 1, 1, ACS_HLINE, cols - 1);
        mvwaddch(win, 1, cols - 1, ACS_RTEE);
        mvwprintw (win, 0, 1, "[P]ause | [Q]uit | [H]elp " );
        mvwprintw (win, lines-1, 1, "Distance: %d", distance);
        wattroff(win, A_BOLD | COLOR_PAIR(3));  
        //the burrow
        wattron(win, A_BOLD | COLOR_PAIR(9));    
        mvwprintw(win, lines -2, burrowx, "O");
        wattroff(win, A_BOLD | COLOR_PAIR(9));

        //placing the ghost
        for (int i = 0; i<lines-8; i++){
            if (ghosts[i] == '@'){
                wattron(win, COLOR_PAIR(colors[i]));
                mvwprintw(win, 4+i, cords[i], "@");
                wattroff(win, COLOR_PAIR(colors[i]));
                //wrefresh(win);
            }   
        }

        //placing froggy
        wattron (win, COLOR_PAIR(8));
        mvwprintw(win, y, x, "F");
        wattroff (win, COLOR_PAIR(8));

        wrefresh(win);

        //checking for the ghosts
        if (ghosts[y-4] == '@' && cords[y-4] == x){
            print_message(4, 25, "YOU WERE EATEN!", COLOR_PAIR(4));
            wrefresh(win);
            break;
        }

        //checking for the burrow
        if (y==lines-2 && x == burrowx) {
            print_message(4, 25, "YOU'RE HOME!", COLOR_PAIR(3));
            wrefresh(win);
            break;
        }
        wrefresh(win);

    }
    
    wrefresh(win);
    delwin(win);
    curs_set(TRUE);
}

void print_message(int height, int width, char *msg, chtype color ){
    WINDOW *pwin = newwin(height, width, LINES / 2 - height / 2, COLS / 2 - width / 2);
            do{
                wattrset(pwin, A_BOLD | color);
                box(pwin, 0, 0);
                print_in_middle(pwin, 1, 0, width, msg , A_BOLD | color);
                wattrset(pwin, ~A_BOLD & color);
                print_in_middle(pwin, height - 2, 0, width, "Press <ENTER>", COLOR_PAIR(1));
            } while (wgetch(pwin) !=10);
            //destroying the window
            wborder(pwin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
            for (int j = 0; j<4; j++){
                for (int o = 0; o<25; o++){
                    mvwaddch(pwin, j, o, ' ');
                }
            }
            wrefresh(pwin);
            delwin(pwin);
}
    
void show_credits() {
    int lines = LINES-6, cols = COLS-8;
    curs_set(FALSE);

    WINDOW *cwin = newwin(LINES-6, COLS-8, 3, 4);
    wattrset(cwin, A_BOLD | COLOR_PAIR (6));
    box(cwin, 0, 0);
    print_in_middle(cwin, 2, 0, cols, "CREDITS", A_BOLD | COLOR_PAIR(4));

    print_in_middle(cwin, 4, 0, cols, "Made possible by:", COLOR_PAIR(1));
    print_in_middle(cwin, 5, 0, cols, "hours of googling", COLOR_PAIR(1));
    print_in_middle(cwin, 6, 0, cols, "Stack Overflow", COLOR_PAIR(1));
    print_in_middle(cwin, 7, 0, cols, "random dudes on YouTube", COLOR_PAIR(1));
    print_in_middle(cwin, 8, 0, cols, "KPI of TUKE", COLOR_PAIR(1));
    print_in_middle(cwin, 9, 0, cols, "Emilia Pietrikova and Samuel Migas", COLOR_PAIR(1));
    print_in_middle(cwin, 10, 0, cols, "GNU Project", COLOR_PAIR(1));
    print_in_middle(cwin, 11, 0, cols, "Ken Arnold", COLOR_PAIR(1));
    print_in_middle(cwin, 12, 0, cols, "Dennis Ritchie & Bell Labs", COLOR_PAIR(1));
    print_in_middle(cwin, 13, 0, cols, "and many people before that", COLOR_PAIR(1));
    print_in_middle(cwin, 13, 0, cols, "and many people before that", COLOR_PAIR(1));
    print_in_middle(cwin, 15, 0, cols, "Vaguely inspired by FROGGER released in 1981", COLOR_PAIR(1));

    print_in_middle(cwin, lines-5, 0, cols, "Intellectual property of vitvixen inc.", COLOR_PAIR(4));
    print_in_middle(cwin, lines-4, 0, cols, "DECEMBER 2020", COLOR_PAIR(1));
    wattrset(cwin, ~A_BOLD & COLOR_PAIR (5));
    print_in_middle(cwin, lines - 2, 0, cols, "press <ENTER> to close", COLOR_PAIR(1));
    while (wgetch(cwin) != 10);
    delwin(cwin);
}

int init_levels(){
    ITEM **my_items;
    int c, choice = -1, width, height;
    MENU *my_menu;
    WINDOW *my_menu_win;
    char menu_choices[5][13] = {
        "  Level 1  ",
        "  Level 2  ",
        "  Level 3  ",
        "  Endless  ",
        "  BACK  "
    };

    char froggo[][43] = {
        "                        ..                ",
        "                      .' @`._             ",
        "       ~       ...._.'  ,__.-;            ",
        "    _..------/`           .-'    ~        ",
        "   :     __./'       ,  .'-'--.._         ",
        "~   `---(.-'''---.    |`._       `.   ~   ",
        "  _.--'(  .______.'.-' `-.`        `.     ",
        " :      `-..____`-.                  ;    ",
        " `.             ````                 ;   ~",
        "   `-.__                        __.-'     ",
        "        ````-----.......-----'''    ~     ",
        "     ~                   ~                "
    };
        //allocates contiguous space in memory for 4 elements of type  ITEM**
    my_items = (ITEM **) calloc(6, sizeof(ITEM *));
    for (int i = 0; i < 5; i++) {
        my_items[i] = new_item(menu_choices[i], "");
        item_opts_off(my_items[i], O_SHOWDESC);
    }
    my_items[5] = (ITEM *) NULL;


    clear();
    wrefresh(stdscr);

    // Create menu 
    my_menu = new_menu((ITEM **) my_items);

    width = COLS - 8;
    height = LINES - 6;

    my_menu_win = newwin(LINES - 6, COLS - 8, 3, 4);
    keypad(my_menu_win, TRUE);

    set_menu_win(my_menu, my_menu_win);
    set_menu_sub(my_menu, derwin(my_menu_win, 5, 30, 7, width - 30));

    set_menu_mark(my_menu, " -> ");

    // making a border 
    box(my_menu_win, 0, 0);
    print_in_middle(my_menu_win, 1, 0, width, "-=*! FROGGY FROG !*=-", A_BOLD | COLOR_PAIR(4));
    mvwaddch(my_menu_win, 2, 0, ACS_LTEE);
    mvwhline(my_menu_win, 2, 1, ACS_HLINE, width - 1);
    mvwaddch(my_menu_win, 2, width - 1, ACS_RTEE);

    //adding the frog image
    int i;
    for (i = 0; i < 12; i++) {
        mvwprintw(my_menu_win, 3 + i, 5, "%s", froggo[i]);
    }
    refresh();

    set_menu_fore(my_menu, COLOR_PAIR(1) | A_REVERSE);
    set_menu_back(my_menu, COLOR_PAIR(4));
    set_menu_grey(my_menu, COLOR_PAIR(4));

    print_in_middle(my_menu_win, height - 2, 1, width, "press <ENTER> to select an option, use Up and Down keys to navigate", COLOR_PAIR(1));
    post_menu(my_menu);
    wrefresh(my_menu_win);

    while ((c = wgetch(my_menu_win)) != -1) {
        switch (c) {
            case KEY_DOWN:
                menu_driver(my_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(my_menu, REQ_UP_ITEM);
                break;
            case 10: // Enter 
                move(0, 0);
                clrtoeol();
                choice = item_index(current_item(my_menu));
                break;
        }

        if (choice > -1) break;
    }
    free(my_items);
    unpost_menu(my_menu);
    delwin(my_menu_win);

    return choice;
}

int init_menu() {
    ITEM **my_items;
    int c, choice = -1, width, height;
    MENU *my_menu;
    WINDOW *my_menu_win;
    char menu_choices[3][13] = {
        "  New game  ",
        "  Credits  ",
        "  Exit  "
    };

    char froggo[][43] = {
        "                        ..                ",
        "                      .' @`._             ",
        "       ~       ...._.'  ,__.-;            ",
        "    _..------/`           .-'    ~        ",
        "   :     __./'       ,  .'-'--.._         ",
        "~   `---(.-'''---.    |`._       `.   ~   ",
        "  _.--'(  .______.'.-' `-.`        `.     ",
        " :      `-..____`-.                  ;    ",
        " `.             ````                 ;   ~",
        "   `-.__                        __.-'     ",
        "        ````-----.......-----'''    ~     ",
        "     ~                   ~                "
    };
    //allocates contiguous space in memory for 4 elements of type  ITEM**
    my_items = (ITEM **) calloc(4, sizeof(ITEM *));
    for (int i = 0; i < 3; i++) {
        my_items[i] = new_item(menu_choices[i], "");
        item_opts_off(my_items[i], O_SHOWDESC);
    }
    my_items[3] = (ITEM *) NULL;


    clear();
    wrefresh(stdscr);

    // Create menu 
    my_menu = new_menu((ITEM **) my_items);

    width = COLS - 8;
    height = LINES - 6;

    my_menu_win = newwin(LINES - 6, COLS - 8, 3, 4);
    keypad(my_menu_win, TRUE);

    set_menu_win(my_menu, my_menu_win);
    set_menu_sub(my_menu, derwin(my_menu_win, 3, 30, 7, width - 30));

    set_menu_mark(my_menu, " -> ");

    // making a border 
    box(my_menu_win, 0, 0);
    print_in_middle(my_menu_win, 1, 0, width, "-=*! FROGGY FROG !*=-", A_BOLD | COLOR_PAIR(4));
    mvwaddch(my_menu_win, 2, 0, ACS_LTEE);
    mvwhline(my_menu_win, 2, 1, ACS_HLINE, width - 1);
    mvwaddch(my_menu_win, 2, width - 1, ACS_RTEE);

    //adding the frog image
    int i;
    for (i = 0; i < 12; i++) {
        mvwprintw(my_menu_win, 3 + i, 5, "%s", froggo[i]);
    }
    refresh();

    set_menu_fore(my_menu, COLOR_PAIR(1) | A_REVERSE);
    set_menu_back(my_menu, COLOR_PAIR(4));
    set_menu_grey(my_menu, COLOR_PAIR(4));

    print_in_middle(my_menu_win, height - 2, 1, width, "press <ENTER> to select an option, use Up and Down keys to navigate", COLOR_PAIR(1));
    post_menu(my_menu);
    wrefresh(my_menu_win);

    while ((c = wgetch(my_menu_win)) != -1) {
        switch (c) {
            case KEY_DOWN:
                menu_driver(my_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(my_menu, REQ_UP_ITEM);
                break;
            case 10: // Enter 
                move(0, 0);
                clrtoeol();
                choice = item_index(current_item(my_menu));
                break;
        }

        if (choice > -1) break;
    }
    free(my_items);
    unpost_menu(my_menu);
    delwin(my_menu_win);

    return choice;
}

void draw_logo(){
    int pos = LINES-6;
    WINDOW * logo_win = newwin(LINES - 6, COLS - 8, 3, 4);
    wattrset(logo_win, A_BOLD | COLOR_PAIR (4));
    keypad(logo_win, TRUE);
    clear();
    const char logo[9][42] = {
        {"                                          "},
        {" _____  _____  _____  _____  _____  __ __ "},
        {"|   __|| __  ||     ||   __||   __||  |  |"},
        {"|   __||    -||  |  ||  |  ||  |  ||_   _|"},
        {"|__|   |__|__||_____||_____||_____|  |_|  "},
        {"       _____  _____  _____  _____         "},
        {"      |   __|| __  ||     ||   __|        "},
        {"      |   __||    -||  |  ||  |  |        "},
        {"      |__|   |__|__||_____||_____|        "}     
    };

    curs_set(FALSE);
    int target = pos/2+4;
    wattrset(logo_win, A_BOLD | COLOR_PAIR (4));
    for( int row_count = 8; row_count >= 1; row_count-- ){
        char txt[42];
        for( int y = 1; y <= target; y++ ){
            for (int j =0; j<42; j++){
                txt[j] = logo[row_count][j];
            }
            print_in_middle(logo_win, y, 0, COLS-4, txt, COLOR_PAIR(4));
            wrefresh(logo_win);
            usleep( 1 * 100000 );

            // clear
            for (int j =0; j<42; j++){
                txt[j] = logo[0][j];
            }
            print_in_middle(logo_win, y, 0, COLS-4, txt, COLOR_PAIR(4));
        }
        for (int j =0; j<42; j++){
                txt[j] = logo[row_count][j];
            }
        print_in_middle(logo_win, target, 0, COLS-4, txt, COLOR_PAIR(4));

        target--;
        wrefresh(logo_win);
    }
    usleep( 2 * 100000 );
    delwin(logo_win);
    flushinp(); 
}

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color) {
    int length, x, y;
    float temp;

    if (win == NULL)
        win = stdscr;
    getyx(win, y, x);
    if (startx != 0)
        x = startx;
    if (starty != 0)
        y = starty;
    if (width == 0)
        width = COLS;

    length = strlen(string);
    temp = (width - length) / 2;
    x = startx + (int) temp;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    refresh();
}
