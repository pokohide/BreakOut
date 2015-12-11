#include "function.h"
#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// 全てのブロックのメモリを解放
void freeBlocks(struct BLOCK *block)
{
  struct BLOCK *p;
  while(p != NULL){
    p = block->next;
    free(block);
    block = p;
  }
}

// ブロック生成
void makeBlock(struct BLOCK **block,double x,double y)
{
  struct BLOCK *ptr,*new;
  
  new = (struct BLOCK *)malloc(sizeof(struct BLOCK));
  if(*block == NULL){
    *block = new;
  }else{
    ptr = *block;
    while(ptr->next != NULL){
      ptr = ptr->next;
    }
    ptr->next = new;
  }

  new->X = x;
  new->Y = y;
  new->life = 1;
  new->next = NULL;
}

void makeBlocks(struct BLOCK **block)
{
  double x, y;

  for(y = 2*LINES/15; y < 5*LINES/15; y += 5){
    for(x = ((WALL_R - 8)%9)/2 + 6; x <= WALL_R-10; x += 11){
      makeBlock(block, x, y);
    }
  }
}

// ブロックを表示
void showBlocks(struct BLOCK *block)
{
  while(block != NULL){
    mvaddstr(block->Y,block->X,"#####");
    block = block->next;
  }
}

//ブロックに触れたかを判定→当たったらぶっ壊す
int breakBlock(struct BLOCK **block,double x,double y,double *Dx,double *Dy,int *count)
{
  struct BLOCK *ptr,*tmp;
  ptr = *block;
  tmp = ptr;

  if(ptr == NULL){
    return 1;
  }

  while(ptr != NULL){
    if(ptr->Y == y){
      //もしブロックの真ん中あたりに当たったら
      if((ptr->X < x) && (x < ptr->X+4)){
        *Dy *= -1;
        break;
      }
      
      //もしブロックの左端にあたったら
      if(ptr->X == x){
        //左側から来たボールが左端に当たった場合
        if(*Dx == 1){
          *Dx *= -1;
          break;
        //右側から来たボールが左端に当たった場合
        }else{
          *Dy *= -1;
          break;
        }
      }
  
      //もしブロックの右端に当たったら
      if(x == ptr->X + 4){
        //左側から来たボールが右端に当たった場合
        if(*Dx == 1){
          *Dy *= -1;
          break;
        //右側から来たボールが左端に当たった場合
        }else{
          *Dx *= -1;
          break;
        }
      }
    }
    ptr = ptr->next;
  }
  /* while終了 */

  /* 当たっていなければptrはNULLのはず。当たっていたら当たったところでbreakしているはず */
  if(ptr != NULL){

    if (ptr->life == 1){
      *count += 1;

      //一番初めのブロックだった場合
      if(ptr == *block){
        *block = ptr->next;
        mvaddstr(ptr->Y,ptr->X,"     ");
        free(ptr);

      //一番初めブロック以外だった場合
      }else{
        //ptrの手前まで移動
        while(tmp->next != ptr) tmp=tmp->next;

        mvaddstr(ptr->Y,ptr->X,"     ");
        tmp->next = ptr->next;
        free(ptr);
      }
      clear();
    }
  }
  return 0;
}

// ボールとバーの当たり判定(下に落ちた時だけ1を返す)
int CollisionDetection(struct Ball *ball, struct Bar *bar)
{
  //ボールが下に落ちたら1を返す
  if((*ball).Y > LINES-1) return 1;

  //左にぶつかったら
  if((*ball).X < WALL_L + 2){
    (*ball).X = WALL_L+2;
    (*ball).Dx = 1;
  }

  //右にぶつかったら
  else if((*ball).X >= WALL_R - 1){
    (*ball).X = WALL_R - 2;
    (*ball).Dx = -1;
  }

  //上にぶつかったら
  else if((*ball).Y < TOP){
    (*ball).Y = 1;
    (*ball).Dy = 1;
  }

  //バーにボールが当たった時
  else if(((*ball).Y == (*bar).Y) && ((*ball).X >= (*bar).X) && ((*ball).X <= (*bar).X + ((*bar).width-1))){
    (*ball).Y = (*bar).Y - 1;
    (*ball).Dy = -1;
  }
  return 0;
}

//バーを動かす
void moveBar(struct Bar *bar, struct Ball *ball, int ch)
{
  mvaddstr((*bar).Y, (*bar).X, (*bar).eraseBar);
  switch(ch){
  case KEY_LEFT:
    if((*bar).X <= WALL_L+3) (*bar).X = WALL_L+3;
    (*bar).X -= 1;
    break;

  case KEY_RIGHT:
    if((*bar).X >= WALL_R - ((*bar).width+1)) (*bar).X = WALL_R - ((*bar).width+1);
    (*bar).X += 1;
    break;

  case KEY_UP:
    if((*ball).waitCount > 500) (*ball).waitCount -= 500;
    break;

  case KEY_DOWN:
    if((*ball).waitCount < 9000) (*ball).waitCount += 500;
    break;

  default:
    break;
  }
  mvaddstr((*bar).Y, (*bar).X, (*bar).addBar);
}

//LEVEL別にバーの長さを変更
void Level(struct Bar *bar, int level)
{
  if(level == 1){
    strcpy((*bar).addBar, "========");
    strcpy((*bar).eraseBar, "        ");
    (*bar).width = 8;
  }
  else if(level == 2){
    strcpy((*bar).addBar, "=======");
    strcpy((*bar).eraseBar, "       ");
    (*bar).width = 7;
  }
  else if(level== 3){
    strcpy((*bar).addBar, "======");
    strcpy((*bar).eraseBar, "      ");
    (*bar).width = 6;
  }
  else if(level== 4){
    strcpy((*bar).addBar, "=====");
    strcpy((*bar).eraseBar, "     ");
    (*bar).width = 5;
  }
  else{
    strcpy((*bar).addBar, "====");
    strcpy((*bar).eraseBar, "    ");
    (*bar).width = 4;
  }
}

// レコードを描画
void printRecord(struct Record rec)
{
  clear();
  mvprintw(LINES/2 - 3,2*COLS/7,"1st ball survived for  %d[s]",rec.time1);
  mvprintw(LINES/2 - 2,2*COLS/7,"2st ball survived for  %d[s]",rec.time2);
  mvprintw(LINES/2 - 1,2*COLS/7,"3st ball survived for  %d[s]",rec.time3);
  mvprintw(LINES/2 - 3,4*COLS/7,"Summary, You survived for  %d[s]",rec.time);
  mvprintw(LINES/2    ,2*COLS/7,"Your 1st Score  %6.1f",rec.score1);
  mvprintw(LINES/2 + 1,2*COLS/7,"Your 2st Score  %6.1f",rec.score2);
  mvprintw(LINES/2 + 2,2*COLS/7,"Your 3st Score  %6.1f",rec.score3);
  mvprintw(LINES/2    ,4*COLS/7,"Summary, Your total Score  %6.1f",rec.score);
  refresh();
  sleep(5);
}

// Scoreを表示
void printScore(int timer, int level, double score, int life)
{
  mvprintw(5,WALL_R+1,"Time %d[s]",timer);
  mvprintw(6,WALL_R+1,"Level %d",level);
  mvprintw(7,WALL_R+1,"SCORE %5.1f",score);
  mvprintw(8,WALL_R+1,"Ball %d",life);
}

// GAMEOVERを表示
void printGameOver(struct Record rec)
{
  /* 端末の幅によって表示方法変更 */
  if(COLS > 150){
    mvprintw(LINES/2-4,(COLS-70)/2," GGGG    AAA   M     M  EEEEE        OOOO   V     V  EEEEE  RRRR       ");
    mvprintw(LINES/2-3,(COLS-70)/2,"G    G  A   A  MM   MM  E           O    O  V     V  E      R   R      ");
    mvprintw(LINES/2-2,(COLS-70)/2,"G       A   A  M M M M  E           O    O  V     V  E      R    R     ");
    mvprintw(LINES/2-1,(COLS-70)/2,"G       AAAAA  M  M  M  EEEEE       O    O  V     V  EEEEE  RRRRR      ");
    mvprintw(LINES/2  ,(COLS-70)/2,"G       A   A  M     M  E           O    O  V     V  E      R  R       ");
    mvprintw(LINES/2+1,(COLS-70)/2,"G   GGG A   A  M     M  E           O    O   V   V   E      R   R      ");
    mvprintw(LINES/2+2,(COLS-70)/2,"G    G  A   A  M     M  E           O    O    V V    E      R    R     ");
    mvprintw(LINES/2+3,(COLS-70)/2," GGGG   A   A  M     M  EEEEE        OOOO      V     EEEEE  R     R    ");
  }else{
    mvprintw(LINES/2,(WALL_R-10)/2,"GAME OVER");
  }  
  refresh();
  sleep(3);
  clear();

  /* レコード表示 */
  printRecord(rec);
  /* THANK YOU FOR PLAYING */
  clear();
  mvprintw(LINES/2  ,(COLS-20)/2,"Thank you for playing");
  mvprintw(LINES/2+2,(COLS-10)/2,"[r]: restart");
  mvprintw(LINES/2+4,(COLS-10)/2,"[q]: quit");
}

// BreakOutを表示
void printBreakOut()
{
  clear();
  mvprintw(LINES/2-4,(COLS-72)/2,"BBBB     RRRRR    EEEE   AAAAA   K   K          OOOO    U    U  TTTTTTT  ");
  mvprintw(LINES/2-3,(COLS-72)/2,"B   B    R   R    E      A   A   K  K          O    O   U    U     T     ");
  mvprintw(LINES/2-2,(COLS-72)/2,"B   B    R   R    E      A   A   K K           O    O   U    U     T     ");
  mvprintw(LINES/2-1,(COLS-72)/2,"BBBB     RRRR     EEEE   AAAAA   KK            O    O   U    U     T     ");
  mvprintw(LINES/2  ,(COLS-72)/2,"B   B    R  R     E      A   A   K K           O    O   U    U     T     ");
  mvprintw(LINES/2+1,(COLS-72)/2,"B    B   R   R    E      A   A   K  K          O    O   U    U     T     ");
  mvprintw(LINES/2+2,(COLS-72)/2,"B    B   R    R   E      A   A   K   K         O    O   U    U     T     ");
  mvprintw(LINES/2+3,(COLS-72)/2,"BBBBB    R     R  EEEE   A   A   K    K         OOOO     UUUU      T     ");
}

// 両側の壁を生成
void printWall()
{
  int i = 0;
  //枠の作成
  for(i=0;i<LINES;i++){
    mvaddch(i,WALL_L,' ');
    mvaddch(i,WALL_R,' ');
    mvaddch(i,WALL_L,'|');
    mvaddch(i,WALL_R,'|');
  }
}
