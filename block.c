#include "function.h"
#include <curses.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

void run();

int main(int argc,char **argv)
{
  initscr();           //curses
  noecho();            //curses
  cbreak();            //curses
  keypad(stdscr,TRUE); //curses
  run();               //実行
  endwin();            //curses
  return 0;
}

///////////////////////////////////////////////////////////////////
//                                                               //
//                          run関数                               //
//                                                               //
///////////////////////////////////////////////////////////////////
void run()
{
  struct Bar bar;                //バーの位置情報
  struct Ball ball;              //ボールの位置情報
  struct Record rec;             //今回のゲームを記録と一時的な保持記録
  struct BLOCK *block = NULL;    //ブロック

  int ch;                //キーボード入力用
  int i = 0;
  int count = 0;         //ブロックがどれだけ破壊されたか
  int loop = 0;          //これによってボールの時間を制御
  time_t t1,t2,t3;       //時間
  int life = 3;          //文字通りLIFE
  double score;          //スコア
  double level_score=0;  //一時的なスコアを記録
  double tmp_score=0;  

  // 記録の設定
  rec.level = 1;         //レベル

  // バーの設定
  bar.Y = LINES - 5;     //バーの初期位置
  bar.X = WALL_R / 2;
  bar.width = 7;         //バーの長さの初期設定
  bar.shoot = 0;         //ボールを発射するか(1) or しないか(0)
  strcpy(bar.addBar, "=======");
  strcpy(bar.eraseBar, "       ");

  // ボールの設定
  ball.X = WALL_R / 2;   //ボールの初期位置
  ball.Y = LINES - 5;
  ball.Dx = 1;           //ボールの方向ベクトルの初期設定
  ball.Dy = -1;          //斜め右上45度に進む
  ball.waitCount = 3000; //bポールの速さ

  timeout(0);


  /////////////////////////////////////////////
  //                                         //
  //                イントロ                  //
  //                                         //
  /////////////////////////////////////////////
  INTRO:
    // BREAKOUTを表示
    printBreakOut();
    mvprintw(LINES/2+5,(COLS-10)/2,"[s]: start");
    mvprintw(LINES/2+7,(COLS-10)/2,"[q]: quit");

    /* キー入力待機。[s]を押したらスタート。[q]を押したら終了*/
    while(1){
      ch = getch();
      if(ch == 's'){
        clear();
        goto START;
      }
      else if(ch == 'q') return;
    }
 
  /////////////////////////////////////////////
  //                                         //
  //               ゲームスタート              //
  //                                         //
  /////////////////////////////////////////////
  START:   
    clear();
    makeBlocks(&block);   // ブロック生成
  /////////////////////////////////////////////
  //                                         //
  //               初期状態                   //
  //                                         //
  /////////////////////////////////////////////
  INIT:
    while((ch = getch()) != 'q'){
      showBlocks(block);
      mvaddch(ball.Y,ball.X,' ');
      //shootが0の時はバーと一緒に移動
      if(bar.shoot == 0){
        ball.X = bar.X + 1;
        ball.Y = bar.Y - 1;
        //スペースが入力されたら発射(shoot=1)
        if(ch == ' ') bar.shoot = 1;
      } else if(bar.shoot == 1) {
        t1 = time(NULL);
        goto GAME;
      }
      printWall();
      printScore(0, rec.level, score, life);
      moveBar(&bar, &ball, ch);
      mvaddch(ball.Y, ball.X, '@');
    }
    goto END;

  /////////////////////////////////////////////
  //                                         //
  //               メインループ                //
  //                                         //
  /////////////////////////////////////////////
  GAME:
    while((ch = getch()) != 'q'){
      //ボールの時間を進める。
      loop++;
      showBlocks(block);
      mvaddch(ball.Y, ball.X, ' ');
      printWall();

      moveBar(&bar, &ball, ch);

      t2 = time(NULL);
      score = tmp_score + (count * 1000)*(rec.level/2.0) + (100000.0/ball.waitCount)*(int)(t2-t1);

      // scoreが条件を満たしたらレベルアップ
      if((score - level_score) > 8000*rec.level){
        tmp_score = score;
        rec.level++;
        count = 0;
        mvprintw(LINES/2,WALL_R/2,"LEVEL UP!!");
        t3 = time(NULL);
        if(ball.waitCount > 500) ball.waitCount -= 500;
        level_score = score;
        Level(&bar, rec.level);
      }

      // 3秒経ったら、LEVEL UPを消す。
      if((int)(t2-t3) == 3) mvprintw(LINES/2, WALL_R/2, "          ");
      // 右側にスコア表示
      printScore((int)(t2 - t1), rec.level, score, life);

      if(loop % ball.waitCount == 0){
        ball.X += ball.Dx;
        ball.Y += ball.Dy;

        if( CollisionDetection(&ball, &bar) == 1 ){
          //ボールをバーの位置に。向きなども初期値に戻してlifeを減らす。
          bar.shoot = 0;
          ball.X = bar.X;        //ボールの位置を初期化
          ball.Y = bar.Y;
          ball.Dx = 1;           //ボールの速度ベクトルも初期化
          ball.Dy = -1;
          loop = 0;             //時間も初期化
          life--;                //ライフを減らす

          if(life == 2){
            rec.time1 = t2 - t1;
            rec.score1 = score;
            tmp_score = score;
            count = 0;
            goto INIT;
          }
          else if(life == 1){
            rec.time2 = t2 - t1;
            rec.score2 = score - rec.score1;
            tmp_score = score;
            count = 0;
            goto INIT;
          }
          else if(life == 0){
            rec.time3 = t2 - t1;
            rec.score3 = score - rec.score2 - rec.score1;
            rec.time = rec.time1 + rec.time2 + rec.time3;
            rec.score = score;
            /* ゲームオーバーしたら3秒待機して記録を表示 */
            printGameOver(rec);
            goto RESTART;
          }
        }
      }
    
      //壊れたブロックがないかの判定。そしてそれを壊す
      if(breakBlock(&block,ball.X,ball.Y,&ball.Dx,&ball.Dy,&count) == 1){
        clear();
        mvprintw(LINES/2,COLS/2,"GAME CLEAR");
        sleep(5);
        return;
      }

      mvaddch(ball.Y,ball.X,'@'|COLOR_PAIR(2));
    }/* ここでwhile文閉じている */
  goto END;

  /////////////////////////////////////////////
  //                                         //
  //               リスタート待機              //
  //                                         //
  /////////////////////////////////////////////
  RESTART:
    /* データの初期化 */
    life = 3;
    tmp_score = 0;
    count = 0;
    ball.waitCount = 3000;
    score = 0;
    rec.level = 1;
    freeBlocks(block);  //前回のブロックをすべて開放してする。
    block = NULL;
    bar.X = WALL_R / 2;
    bar.Y = LINES - 5;
    while(1){
      ch = getch();
      if(ch == 'r') goto START;
      if(ch == 'q') goto END;
    }
    
  /////////////////////////////////////////////
  //                                         //
  //                   終了                   //
  //                                         //
  /////////////////////////////////////////////
  END:
    clear();
    return;
}

