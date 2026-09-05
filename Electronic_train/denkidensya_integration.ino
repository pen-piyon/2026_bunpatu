//denkidensyaの統合版プログラム(実際の動作用)

//モータドライブ回路
int Motor01_in1 = 0;
int Motor01_in2 = 1;
int PWM = 6;

// センサ回路
int lightsensor = A0;

// スイッチ,status回路
int SW_input = 3;
int white_LED = 8;
int buzzer = 9;
int red_LED = 10;

//PIN_3を外部割り込みのピンに設定する
volatile const byte interruptPin2 = SW_input;

// スイッチstatus回路の変数
volatile int go_check = 0;
volatile int before_SW = 0, after_SW = 0;

//変数
volatile int change = 0;

void setup() {
  // モータドライブ部
  pinMode(Motor01_in1, OUTPUT);
  pinMode(Motor01_in2, OUTPUT);
  digitalWrite(Motor01_in2, LOW);

  // センサ部
  pinMode(lightsensor, INPUT);

  // スイッチ,status部
  pinMode(white_LED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(red_LED, OUTPUT);

  digitalWrite(white_LED, LOW);
  digitalWrite(red_LED, LOW);
  digitalWrite(buzzer, LOW);

  //外部割り込みのピンにプルアップ抵抗をつける
  pinMode(interruptPin2, INPUT);

  //外部割り込みを許可する
  attachInterrupt(digitalPinToInterrupt(interruptPin2), interrupt_SW, RISING);

  //チャタリング防止する
  PCIFR = _BV(PCIF1);

  // sensor debug
  //Serial.begin(9600);
}
  int i=0;
void loop() {
  // 変数定義

  float x = 0, voltage = 0;

  // センサ入力
  x = analogRead(lightsensor);
  voltage = x * (5.0 / 1024.0);

  // スイッチ
  go_check = 1;

  // スイッチON時
  if (after_SW == 1) {
    detachInterrupt(digitalPinToInterrupt(interruptPin2));
    
    // 白LEDを消灯
    digitalWrite(white_LED, LOW);
    // スイッチ入力ができないように、赤LEDをONにする
    digitalWrite(red_LED, HIGH);

    // 処理(スイッチON時の)
    if (change == 0) {
      // モータ正転
      digitalWrite(Motor01_in1, HIGH);
      analogWrite(PWM, 255);  //PWM設定値:0～255　255=5V
      delay(100);
      //analogWrite(PWM, 100);  //PWM設定値:0～255　255=5V
      delay(2500);
      // モータ正転を停止
      digitalWrite(Motor01_in1, LOW);
      analogWrite(PWM, 0);
      delay(1000);
      change = 1;
    } else if (change == 1) {
      // モータ反転
      digitalWrite(Motor01_in2, HIGH);
      analogWrite(PWM, 255);  //128=2.5V
                              // delay(2500);
      delay(500);

      //debug
      // Serial.print(voltage);
      //Serial.println("V");

      // フォトカプラの光を遮蔽したとき
      if (voltage < 1.0) {
        // モータ反転を停止
        digitalWrite(Motor01_in2, LOW);
        analogWrite(PWM, 0);
        delay(1000);
        change = 0;

        // 終了して、赤LEDをOFFにする
        digitalWrite(red_LED, LOW);
        delay(100);
        go_check = 0;
        after_SW = 0;   
        attachInterrupt(digitalPinToInterrupt(interruptPin2), interrupt_SW, RISING);
      }
    }

  } else if (after_SW == 0) {
    //特になし
  } else {
    //エラー処理
  }

  // 待機タイム(いつでもスタート可能)
  if (go_check == 1) {

    if(i==0)
    {
      //digitalWrite(white_LED, HIGH);
    }
    else
    {}

    delay(20);
    //delay(6000);
  } else {
    // 特になし
  }

  delay(20);
}

// 外部割り込み(スイッチ)
void interrupt_SW() {
  delay(20);

  if (go_check == 0) {
    after_SW = 0;
  } else if (go_check == 1) {
    after_SW = 1;
    digitalWrite(white_LED, LOW);
  } else {
    // エラー処理
  }
}