#ifndef _FUNCTION_h_
#define _FUNCTION_h_

#define WALL_R (COLS - 20) //右側の壁
#define WALL_L 2           //左側の壁
#define TOP 0              //上の壁
#define UNDER (LINES - 5)  //下の壁

//ブロックの構造体
struct BLOCK{
  double X;
  double Y;      //ブロックの座標
  int life;      //何回当たれば壊れるか
  struct BLOCK *next;  //月のブロックへのポインタ
};

//ボールやバーの位置の構造体
struct location{
  double X;
  double Y;      //座標
  double Dx;
  double Dy;     //それぞれの速度ベクトル
};

//TOP5位の記録情報
struct Record{
  int data;       //0だったらデータ未入力
  double score;   //総スコア
  double score1;  //ボール一個目のスコア
  double score2;  //ボール二個目のスコア
  double score3;  //ボール三個目のスコア
  int time;       //合計生存タイム
  int time1;      //ボール一個目の生存タイム
  int time2;      //ボール二個目の生存タイム
  int time3;      //ボール三個目の生存タイム
  char name[32];  //記録保持者の名前
};

void freeBlocks(struct BLOCK *block);
void makeBlock(struct BLOCK **block,double x,double y);
void showBlocks(struct BLOCK *block);
int breakBlock(struct BLOCK **block,double x,double y,double *Dx,double *Dy,int *count);
int CollisionDetection(struct location *ball, struct location *bar, int *delay, int bar_width);
void moveBar(struct location *bar,int ch,int bar_width,int *waitCount);
void Level(struct location *bar,int *bar_width,int level,int ch,int *waitCount);
void printRecord(struct Record rec);
void printScore(int time, int level, double score, int life);
void printGameOver();
void printBreakOut();
void printWall();

#endif
