//GY-61で測定した数値を重力加速度１Gとして標準偏差をだすようにした

#define led_PIN_yellow 11 // 黄色のLEDを11番ピン
#define led_PIN_green 12  // 緑色のLEDを12番ピン
#define led_PIN_red 13    // 赤色のLEDを13番ピン
#define button_yellow 2   // 黄色のボタンを2番ピン
#define button_green 3    // 緑色のボタンを3番ピン
#define button_red 4      // 赤色のボタンを4番ピン
#define accele_xout A0    // X軸の加速度計ピン
#define accele_yout A1    // Y軸の加速度計ピン
#define accele_zout A2    // Z軸の加速度計ピン

int button;               // この変数に各色のボタンピンを代入
int mode = 0;             // 機器の状態を表す。最初は待機状態の0をしてい
int random_number;        // LEDを点灯の待ち時間の基礎数値
int random_led;           
int val_byte = 0;         // 入力された文字の最初をASCIIコードで受け取る
int max_count_num;        // val_byteの数値を一桁の整数に変換
int max_count;            // max_count_numから測る回数を決定
int count = 0;
int count_yellow = 0;
int count_green = 0;
int count_red = 0;
unsigned int start;
unsigned int end;

unsigned int led_random_list[30] = {0};
unsigned int time;
unsigned int time_list[30];
unsigned int time_list_yellow[10];
unsigned int time_list_green[10];
unsigned int time_list_red[10];
unsigned int average_total = 0;
unsigned int average_yellow = 0;
unsigned int average_green = 0;
unsigned int average_red = 0;
double sd_total = 0;
double sd_yellow = 0;
double sd_green = 0;
double sd_red = 0;
//加速度を一時的に保管する
int accele_count = 0;
unsigned int accele_x_axis[100];
unsigned int accele_y_axis[100];
unsigned int accele_z_axis[100];
//加速度の標準偏差をいれる
double accele_x_axis_sd;
double accele_y_axis_sd;
double accele_z_axis_sd;
//各軸の標準偏差をボタンに応じて配列に入れる
double accele_yellow_xout[10];
double accele_yellow_yout[10];
double accele_yellow_zout[10];
double accele_green_xout[10];
double accele_green_yout[10];
double accele_green_zout[10];
double accele_red_xout[10];
double accele_red_yout[10];
double accele_red_zout[10];

double accele_x_axis_yellow_sd_list[10];
double accele_y_axis_yellow_sd_list[10];
double accele_z_axis_yellow_sd_list[10];
double accele_x_axis_green_sd_list[10];
double accele_y_axis_green_sd_list[10];
double accele_z_axis_green_sd_list[10];
double accele_x_axis_red_sd_list[10];
double accele_y_axis_red_sd_list[10];
double accele_z_axis_red_sd_list[10];



void setup() {
  pinMode(led_PIN_yellow, OUTPUT);
  pinMode(led_PIN_green, OUTPUT);
  pinMode(led_PIN_red, OUTPUT);
  pinMode(button_yellow, INPUT);
  pinMode(button_green, INPUT);
  pinMode(button_red, INPUT);
  Serial.begin( 9600 );
  delay(600);
  //Serial.println("数字を入力して下さい");
  //Serial.println("1:12回, 2:15回, 3:18回, 4:21回, 5:24回");
  //Serial.println("-------------------------------------");
}

//LEDを点灯させ、押すボタンを決定する
int ledon_and_buttoncheck(){
   for (int i=0; i <= 1; i++ ){ // 被験者に検査を始める合図をさせる
     digitalWrite(led_PIN_yellow, HIGH);
     digitalWrite(led_PIN_green, HIGH);
     digitalWrite(led_PIN_red, HIGH);
     delay(400);
     digitalWrite(led_PIN_yellow, LOW);
     digitalWrite(led_PIN_green, LOW);
     digitalWrite(led_PIN_red, LOW);
     delay(400);
   }
   random_number = random(1, 4); // 1から4秒の間で点灯
   random_led = (led_random_list[count] % 3) + 11; // ランダムなリストから点灯するLEDのピンを計算
   
  //光るLEDによって反応するボタンを決定する
   if (random_led == 11){        
     button = button_yellow;
   }else if (random_led == 12){
     button = button_green; 
   }else{
     button = button_red;
   }
   delay(random_number * 1000);
   digitalWrite(random_led, HIGH); // ランダムにLEDを点灯させる
   start = millis();
   mode = 1;      // 機器の状態を1にして次の工程に移る
 }

//平均を求める関数
unsigned int time_average(int local_count, unsigned int local_time_list[100]){
    long sum = 0;
    for (int i = 0; i < (local_count + 1); i++){
      sum += local_time_list[i];
    }
    unsigned int local_average = sum / (local_count + 1);
    return local_average;
}

//標準偏差を求める関数
double time_sd(int local_count, unsigned int local_time_list[100]){
    unsigned int sd_average = time_average(local_count, local_time_list);
    long sum = 0;
    for(int i = 0; i < local_count + 1; i++){
        sum += ( local_time_list[i] - sd_average ) * ( local_time_list[i] - sd_average );
    }
    double local_sd = sqrt( sum / (count + 1) );
    return local_sd;
}


//すべてと色違いの平均と標準偏差を出す関数
int average_sd_result(int button, int count, unsigned int time, unsigned int time_list[30]){
  time_list[count] = time;
  average_total = time_average(count, time_list);
  sd_total = time_sd(count, time_list);
  //加速度の標準偏差を出す
  accele_x_axis_sd = time_sd(accele_count, accele_x_axis);
  accele_y_axis_sd = time_sd(accele_count, accele_y_axis);
  accele_z_axis_sd = time_sd(accele_count, accele_z_axis);
  //色別のリストにタイムを入れる
  //色別リストと色別カウントから平均値と標準偏差を出す
  //色別カウントを1増やす
  //これを順番に行って最後に返す
  if (button == 2){     //黄色の統計処理
    time_list_yellow[count_yellow] = time;
    average_yellow = time_average(count_yellow, time_list_yellow);
    sd_yellow = time_sd(count_yellow, time_list_yellow);
    accele_x_axis_yellow_sd_list[count_yellow] = accele_x_axis_sd;
    accele_y_axis_yellow_sd_list[count_yellow] = accele_y_axis_sd;
    accele_z_axis_yellow_sd_list[count_yellow] = accele_z_axis_sd;
    count_yellow += 1;
    //return average, sd, average_yellow, sd_yellow;

  }else if (button == 3){//緑の統計処理
    time_list_green[count_green] = time;
    average_green = time_average(count_green, time_list_green);
    sd_green = time_sd(count_green, time_list_green);
    accele_x_axis_green_sd_list[count_green] = accele_x_axis_sd;
    accele_y_axis_green_sd_list[count_green] = accele_y_axis_sd;
    accele_z_axis_green_sd_list[count_green] = accele_z_axis_sd;
    count_green += 1;
    //return average, average_green, sd_green;
    
  }else{                 //赤の統計処理
    time_list_red[count_red] = time;
    average_red = time_average(count_red, time_list_red);
    sd_red = time_sd(count_red, time_list_red);
    accele_x_axis_red_sd_list[count_red] = accele_x_axis_sd;
    accele_y_axis_red_sd_list[count_red] = accele_y_axis_sd;
    accele_z_axis_red_sd_list[count_red] = accele_z_axis_sd;
    count_red += 1;
    //return average, average_red, sd_red;
  }
}

// 計測する回数が決定した後、ランダムなリストを作る。
void create_number_list(){
  for (int i=0; i<max_count; i++){
    led_random_list[i] = i+1;
  }
  shuffle();
}

//Fisher-Yatesアルゴリズムを適用してリストをランダムシャッフルする
void shuffle() {
    for(int i = 0; i < max_count; i++) {
        int t = led_random_list[i];
        int j = random(i, max_count);
        led_random_list[i] = led_random_list[j];
        led_random_list[j] = t;
    }
}

double list_print(int local_count, double local_list[10]){
  for (int i = 0; i < local_count; i++){
    Serial.println(local_list[i]);
  }
}


void loop(){
  if (Serial.available() > 0) { // 受信したデータが存在する
   val_byte = Serial.read(); // 入力された文字の最初をASCIIコードで受け取る
   max_count_num = val_byte - 0x30; // ASCIIコードを一桁の数字に直す

   if ( 0 < max_count_num && max_count_num < 6){ // 一桁の数字を検査する回数に変化する
     switch (max_count_num) {
      case 1:
        //hoge(var);<-数値を送って何かをさせる処理
        max_count = 12;
        break;
      case 2:
        //hoge(var);
        max_count = 15;
        break;
      case 3:
        //hoge(var);
        max_count = 18;
        break;
      case 4:
        //hoge(var);
        max_count = 21;
        break;
      case 5:
        //hoge(var);
        max_count = 24;
        break;
      }
      create_number_list();  // ランダムなリストを作る          
      while(count < max_count){
       if (mode == 0){
         ledon_and_buttoncheck();
       }
       if (mode == 1 && digitalRead(button) == LOW){
         double xout = analogRead(accele_xout);
         double yout = analogRead(accele_yout);
         double zout = analogRead(accele_zout);
         
         accele_x_axis[accele_count] = ((double)xout - 327) / 67;
         accele_y_axis[accele_count] = ((double)yout - 326) / 67;
         accele_z_axis[accele_count] = ((double)zout - 350) / 66;
         accele_count += 1;
         delay(100);
       }
      
      //終了　計測結果提示
       if (mode == 1 && digitalRead(button) == HIGH){
          end = millis();
          digitalWrite(random_led, LOW);
          time = end - start;
          //Serial.print(count + 1);
          //Serial.print("回目 ");
          //Serial.println(time);
          average_sd_result(button, count, time, time_list);   //一様ここで取得データを保存
          count += 1;
          accele_count = 0;
          mode = 0;
          while (digitalRead(button) == HIGH){
            delay(100);
          }
          delay(3000);
        } // 終了計測のカッコ
      } // 回数分のループのカッコ
   // 結果をシリアルモニターに提示
   Serial.print(count);
   Serial.println("回分すべての平均");
   Serial.println(average_total);
   Serial.println("黄色の平均");
   Serial.println(average_yellow);
   Serial.println("緑の平均 ");
   Serial.println(average_green);
   Serial.println("赤の平均 ");
   Serial.println(average_red);
   
   Serial.print(count);
   Serial.println("回すべての標準偏差");
   Serial.println(sd_total);
   Serial.println("黄色の標準偏差");
   Serial.println(sd_yellow);
   Serial.println("緑の標準偏差");
   Serial.println(sd_green);
   Serial.println("赤の標準偏差");
   Serial.println(sd_red);

   //Serial.println("黄色の加速度");
   //Serial.println("X軸");
   list_print(count_yellow, accele_x_axis_yellow_sd_list);
   //Serial.println("Y軸");
   list_print(count_yellow, accele_y_axis_yellow_sd_list);
   //Serial.println("Z軸");
   list_print(count_yellow, accele_z_axis_yellow_sd_list);

   //Serial.println("緑の加速度");
   //Serial.println("X軸");
   list_print(count_green, accele_x_axis_green_sd_list);
   //Serial.println("Y軸");
   list_print(count_green, accele_y_axis_green_sd_list);
   //Serial.println("Z軸");
   list_print(count_green, accele_z_axis_green_sd_list);

   //Serial.println("赤の加速度");
   //Serial.println("X軸");
   list_print(count_red, accele_x_axis_red_sd_list);
   //Serial.println("Y軸");
   list_print(count_red, accele_y_axis_red_sd_list);
   //Serial.println("Z軸");
   list_print(count_red, accele_z_axis_red_sd_list);

   //カウント初期化
   count = 0;
   count_yellow = 0;
   count_green = 0;
   count_red = 0;

   //平均初期化
   average_total = 0;
   average_yellow = 0;
   average_green = 0;
   average_red = 0;

   //標準偏差初期化
   sd_total = 0;
   sd_yellow = 0;
   sd_green = 0;
   sd_red = 0;

   //リスト初期化
   time_list[30] = {0};
   time_list_yellow[10] = {0};
   time_list_green[10] = {0};
   time_list_red[10] = {0};
   
   } // 正しい回数を入力したあとの全体的なループのカッコ if文

    else{
        Serial.println("正しい回数を入力してください");
    }
        
   } // シリアルモニターにデータが送られたか確認するカッコ
} // void loop の終了カッコ
