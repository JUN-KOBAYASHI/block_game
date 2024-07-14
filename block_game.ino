#include <M5Unified.h>

// ゲームの設定
const int screenWidth = 128;
const int screenHeight = 128;
const int paddleWidth = 20;
const int paddleHeight = 5;
const int ballSize = 3;
const int brickRows = 3;
const int brickCols = 8;
const int brickWidth = screenWidth / brickCols;
const int brickHeight = 10;

// ゲームの状態
int paddleX = screenWidth - paddleWidth; // 右端に初期化
int ballX, ballY, ballSpeedX, ballSpeedY;
bool bricks[brickRows][brickCols];
int score = 0; // 得点の追加

// IMUの設定
float ax, ay, az;

lgfx::LGFX_Sprite sprite(&M5.Display);

void setup() {
  Serial.println("## setup");
  auto cfg = M5.config();
  cfg.internal_imu = true;
  M5.begin(cfg);
  M5.Display.begin();
  M5.Imu.begin();
  sprite.createSprite(screenWidth, screenHeight);
  resetGame();
}

void resetGame() {
  // 初期設定
  ballX = screenWidth / 2;
  ballY = screenHeight / 2;
  ballSpeedX = 1;
  ballSpeedY = 1;
  score = 0; // 得点のリセット
  paddleX = screenWidth - paddleWidth; // 右端に初期化
  
  // ブロックを配置
  for (int i = 0; i < brickRows; i++) {
    for (int j = 0; j < brickCols; j++) {
      bricks[i][j] = true;
    }
  }
}

void showGameOver() {
  sprite.fillSprite(BLACK);
  sprite.setTextSize(2);
  sprite.setTextColor(WHITE, BLACK); // 背景色を指定して見やすくする
  sprite.setCursor(10, 50);
  sprite.printf("Game Over");
  sprite.setCursor(10, 70);
  sprite.printf("Score: %d", score);
  sprite.pushSprite(0, 0);
  delay(3000); // 3秒間表示
}

void loop() {
  M5.update();
  M5.Imu.getAccel(&ax, &ay, &az);
  
  // パドルの移動
  float sensitivity = 5.0; // 感度を調整
  paddleX -= (int)(ax * sensitivity);  // 符号を反転
  if (paddleX < 0) paddleX = 0;
  if (paddleX > screenWidth - paddleWidth) paddleX = screenWidth - paddleWidth;

  // ボールの移動
  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // 壁との衝突
  if (ballX <= 0 || ballX >= screenWidth - ballSize) ballSpeedX = -ballSpeedX;
  if (ballY <= 0) ballSpeedY = -ballSpeedY;
  if (ballY >= screenHeight) {
    showGameOver();
    resetGame();
    return;
  }

  // パドルとの衝突
  if (ballY >= screenHeight - paddleHeight - ballSize && ballX >= paddleX && ballX <= paddleX + paddleWidth) {
    ballSpeedY = -ballSpeedY;
  }

  // ブロックとの衝突
  int brickX = ballX / brickWidth;
  int brickY = ballY / brickHeight;
  if (brickX >= 0 && brickX < brickCols && brickY >= 0 && brickY < brickRows && bricks[brickY][brickX]) {
    bricks[brickY][brickX] = false;
    ballSpeedY = -ballSpeedY;
    score++; // 得点の加算
  }

  // 画面の描画
  sprite.fillSprite(BLACK);
  sprite.fillRect(paddleX, screenHeight - paddleHeight, paddleWidth, paddleHeight, WHITE);
  sprite.fillRect(ballX, ballY, ballSize, ballSize, WHITE);
  for (int i = 0; i < brickRows; i++) {
    for (int j = 0; j < brickCols; j++) {
      if (bricks[i][j]) {
        sprite.fillRect(j * brickWidth, i * brickHeight + 10, brickWidth - 1, brickHeight - 1, WHITE); // ブロック位置を調整
        sprite.drawRect(j * brickWidth, i * brickHeight + 10, brickWidth - 1, brickHeight - 1, BLACK); // 境界線の追加
      }
    }
  }
  sprite.setTextSize(2);
  sprite.setTextColor(WHITE, BLACK); // 背景色を指定して見やすくする
  sprite.setCursor(0, 0);
  sprite.printf("Score: %d", score); // 得点の表示
  sprite.pushSprite(0, 0);

  delay(10);
}
