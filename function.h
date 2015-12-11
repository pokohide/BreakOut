#ifndef _FUNCTION_h_
#define _FUNCTION_h_

#define WALL_R (COLS - 20) //右側の壁
#define WALL_L 2           //左側の壁
#define TOP 0              //上の壁
#define UNDER (LINES - 5)  //下の壁

// ブロックの構造体
struct BLOCK{
  double X;
  double Y;      //ブロックの座標
  int life;      //何回当たれば壊れるか
  struct BLOCK *next;  //月のブロックへのポインタ
};

// ボールの構造体
struct Ball{
  double X;
  double Y;      // 座標
  double Dx;
  double Dy;     // それぞれの速度ベクトル
  int waitCount; // 速度
};

// バーの構造体
struct Bar {
  double X;     // バーの位置
  double Y;
  int width;    // バーの長さ
  int shoot;    // ボールを発射するか(1)しないか(0)
  char addBar[12];   // バーの描画部分
  char eraseBar[12]; // バーの消す部分
};

// レコードの構造体
struct Record{
  int level;      //レベル
  double score;   //総スコア
  double score1;  //ボール一個目のスコア
  double score2;  //ボール二個目のスコア
  double score3;  //ボール三個目のスコア
  int time;       //合計生存タイム
  int time1;      //ボール一個目の生存タイム
  int time2;      //ボール二個目の生存タイム
  int time3;      //ボール三個目の生存タイム
};

void freeBlocks(struct BLOCK *block);
void makeBlock(struct BLOCK **block,double x,double y);
void makeBlocks(struct BLOCK **block);
void showBlocks(struct BLOCK *block);
int breakBlock(struct BLOCK **block,double x,double y,double *Dx,double *Dy,int *count);
int CollisionDetection(struct Ball *ball, struct Bar *bar);
void moveBar(struct Bar *bar, struct Ball *ball, int ch);
void Level(struct Bar *bar, int level);
void printRecord(struct Record rec);
void printScore(int time, int level, double score, int life);
void printGameOver();
void printBreakOut();
void printWall();

#endif
