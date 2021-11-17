#include <ncurses.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>
#include <time.h>


#define DELAY 100000

int pkt = 0;

//Function drawing borders in terminal
void draw_borders(WINDOW *screen)
{
  int x, y;

  getmaxyx(screen, y, x);

  //4 corners
  mvwprintw(screen, 0, 0, "+");
  mvwprintw(screen, y - 1, 0, "+");
  mvwprintw(screen, 0, x - 1, "+");
  mvwprintw(screen, y - 1, x - 1, "+");

  //sides
  for (int i = 1; i < (y - 1); i++) {
    mvwprintw(screen, i, 0, "|");
    mvwprintw(screen, i, x - 1, "|");
  }

  //top and bottom
  for (int i = 1; i < (x - 1); i++) {
    mvwprintw(screen, 0, i, "-");
    mvwprintw(screen, y - 1, i, "-");
  }
}

//Function actualizing window borders
void new_draw_borders(WINDOW *field, WINDOW *score,int new_x,int new_y,int parent_x,int parent_y,int score_size)
{
 getmaxyx(stdscr, new_y, new_x);

    if (new_y != parent_y || new_x != parent_x)
    {
      parent_x = new_x;
      parent_y = new_y;

      wresize(field, new_y - score_size, new_x);
      wresize(score, score_size, new_x);
      mvwin(score, new_y - score_size, 0);

      wclear(stdscr);
      wclear(field);
      wclear(score);

      draw_borders(field);
      draw_borders(score);
    }
}

//Function drawing snake tail
void tail(WINDOW *field, int x, int y, int snksize, int tailx[], int taily[])
{
    for(int i=snksize;i >= 0;--i)
    {
        tailx[i]=tailx[i-1];
        taily[i]=taily[i-1];
    }
    tailx[snksize]=x;
    taily[snksize]=y;

    for(int i=0;i<snksize;++i)
    {
        mvwprintw(field, taily[i], tailx[i], "o");
    }
}

//Function removing tail for simulation of motion
void removing(WINDOW *field, int snksize, int tailx[], int taily[])
{
    mvwprintw(field, taily[snksize-1], tailx[snksize-1], " ");
}

//Function checking collisions
bool collision(WINDOW *field, int snksize, int x, int y, int tailx[], int taily[], int parent_x, int parent_y, int score_size)
{
 if (snksize>2)
 {
    for(int i=1;i<snksize;i++)
    {
        if(x==tailx[i] && y==taily[i])
        {
         mvwprintw(field, (parent_y - score_size)/2, (parent_x-sizeof("Fail!"))/2, "Fail!");
         mvwprintw(field,((parent_y - score_size)/2)+1, (parent_x-sizeof("Try again? [y/n]"))/2, "Try again? [y/n]");
         int a = wgetch(field);
         if(a == 'y' || a == 'Y')
         {
          for(int i=0;i<snksize;++i)
          {
            mvwprintw(field, taily[i], tailx[i], " ");
          }
          snksize = 0;
          wrefresh(field);
          return 1;
         }else
         {
          clear();
          refresh();
          exit(0);
          return 1;
         }
        }
    }//for
    return 0;
 }else return 0;
}

int main() {
  int parent_x, parent_y, new_x, new_y;
  int score_size = 3;

  srand (time(NULL));
  initscr();
  noecho();
  curs_set(FALSE);

  //Set up initial windows
  getmaxyx(stdscr, parent_y, parent_x);

  WINDOW *field = newwin(parent_y - score_size, parent_x, 0, 0);
  WINDOW *score = newwin(score_size, parent_x, parent_y - score_size, 0);

  draw_borders(field);
  draw_borders(score);
  keypad(field, TRUE);

  //head
  int x = parent_x/2;
  int y = (parent_y - score_size)/2;

  //tail
  int tailx[100],
      taily[100];
  int snksize = 0;

  //fruit objects
  int ox,
      oy;

  //moving
  int next_x = 0,
      next_y = 0;
  int direction_h = 1,
      direction_v = -1;

  int c;
  int which = 1;
  int* pntr;
  pntr = &which;

 while(1) {

    //Actualizing our windows
    new_draw_borders(field,score,new_x,new_y,parent_x,parent_y,score_size);

    //Draw to our windows
    wclear(field);
    draw_borders(field);

    if(snksize==100)
    {
        mvwprintw(field, (parent_y - score_size)/2, (parent_x-sizeof("Win!"))/2, "Win!");
        mvwprintw(field,((parent_y - score_size)/2)+1, (parent_x-sizeof("Press any key to continue..."))/2, "Press any key to continue");
        wgetch(field);
        break;
    }

    snksize++;
    ox = (rand()%(parent_x-2)) + 1;
    oy = (rand()%(parent_y-(score_size+2))) + 1;


    wrefresh(field);
    wrefresh(score);



    while(x!=ox || y!=oy)
    {
        tailx[0]=x;
        taily[0]=y;

    wattron (field, A_REVERSE);
    mvwprintw(field, oy, ox, " ");
    wattroff (field, A_REVERSE);

    mvwprintw(field, y, x, "@");

    if (collision(field,snksize,x,y,tailx,taily,parent_x,parent_y,score_size))
    {
     wclear(field);
     break;
    }

    wrefresh(field);
    wrefresh(score);

    //move
    if(kbhit())
    {
      c = wgetch(field);
        switch(c){
            case KEY_UP:
                if(*pntr!=4)
                {
                *pntr = 3;
                break;
                }else
                break;
            case KEY_DOWN:
                if(*pntr!=3)
                {
                *pntr = 4;
                break;
                }else
                break;
            case KEY_RIGHT:
                if(*pntr!=2)
                {
                *pntr = 1;
                break;
                }else
                break;
            case KEY_LEFT:
                 if(*pntr!=1)
                 {
                  *pntr = 2;
                  break;
                 }else
                 break;
            default:
                break;
        }//switch
    }else
    {
      if(*pntr == 1)
      {
        usleep(DELAY);
        next_x = x + direction_h;
        removing(field,snksize,tailx,taily);
        wrefresh(field);


        if (next_x >= parent_x-1)
        {
            x = 1;
            wclear(field);
            draw_borders(field);


        } else
        {
            tail(field,x,y,snksize,tailx,taily);
            x+= direction_h;

        }
      }else if(*pntr == 2)
      {
        usleep(DELAY);
        next_x = x - direction_h;
        removing(field,snksize,tailx,taily);
        wrefresh(field);


        if (next_x < 1)
        {
            x = parent_x-2;
            wclear(field);
            draw_borders(field);
        }else
        {
            tail(field,x,y,snksize,tailx,taily);
            x-= direction_h;
        }
      }else if(*pntr == 3)
      {
        usleep(DELAY*2);
        next_y = y + direction_v;
        removing(field,snksize,tailx,taily);
        wrefresh(field);

        if (next_y < 1)
        {
            y = parent_y - (score_size + 2);
            wclear(field);
            draw_borders(field);
        }else
        {
            tail(field,x,y,snksize,tailx,taily);
            y+=direction_v;
        }
      }else if(*pntr == 4)
      {
        usleep(DELAY*2);
        next_y = y - direction_v;
        removing(field,snksize,tailx,taily);
        wrefresh(field);

        if (next_y >= (parent_y - (score_size + 1)))
        {
            y = 1;
            wclear(field);
            draw_borders(field);
        }else
        {
            tail(field,x,y,snksize,tailx,taily);
            y-=direction_v;
        }
      }
    }
    mvwprintw(score, 1, 1, "Score:");
    mvwprintw(score, 1, sizeof("Score: "), "%d", pkt);

    //Refresh each window
    wrefresh(field);
    wrefresh(score);

    }//while obj
    pkt+=10;
    wrefresh(field);
    wrefresh(score);
  }//while

  endwin();

  return 0;
}
