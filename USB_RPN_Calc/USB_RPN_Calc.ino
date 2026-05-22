/**********************
   GPIO            用途
  D0
  D1
  D2
  D3
   0          
   2          
   4          
   5          USB SS
  12                 ※2020.03.10 書き込みや起動の問題が発生するため変更
  13          
  14          
  15          SD CS
  16          
  17          USB INT
  18          SPI CLK
  19          SPI MISO
  21          I2C SDA (OLED)
  22          I2C SCL (OLED)
  23          SPI MOSI
  25          LED(R)
  26          LED(G)
  27          LED(B)
  32          ----
  33
  34          (INPUT only)
  35          (INPUT only)
  5V          V-BUS
  EN          USB RST
  3.3V        
  GND         


ボード
  ESP32 Dev Module

Libraries
  ESP8266_and_ESP32_OLED_driver_for_SSD1306_displays
  USB_Host_Shield_Library_2.0


 2024.02.09 USBキーボードに変更
 2024.02.20 逆三角関数追加

 2024.04.16 整数積のオーバーフローエラー判定追加 

 2024.04.   実数の割り算で桁が間違うバグ修正

 2024.05.12 SDカードなしモデル作成

 2023.07.02 実数割り算の桁数間違いを修正
 2024.09.02　　同上
 
 
 */

#define SD_MODEL 1   // SDカード付きの場合は　1  そうでない場合は 0

#include <hidboot.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>
#include <Wire.h>
#include <stdlib.h>
#include <SSD1306Wire.h>
#include <FS.h>
#include <SD.h>

#include "OLED.h"
#include "BCDcalc.h"
#include "CORDIC.h"
#include "key.h"
#include "function.h"
#include "arc.h"

#define SD_CS 15

 // OLED i2c
#define SDApin 21 // 'D1' on the NodeMCU v3 baord 5
#define SCLpin 22 // 'D2' on the NodeMCU v3 baord 4
#define OLED_I2C_ADDRESS1 0x3d
#define OLED_I2C_ADDRESS2 0x3c

// Initialize the OLED display using Wire library
SSD1306Wire  display2(OLED_I2C_ADDRESS2, SDApin, SCLpin);
SSD1306Wire  display( OLED_I2C_ADDRESS1, SDApin, SCLpin);

#define LEDr   25
#define LEDg   26
#define LEDb   27

char in_key = (char)NULL;     // 入力キー
int  dsp_start = 0;   // 表示フラグ
String print_key;     // 入力キー表示
int  cal_mode  = 0;   // 0:整数計算モード  1:実数計算モード
int  error     = 0;   // 1:オーバーフロー等のエラー
int  base      = 10;  // 表示進数　2:2進数　　8:8進数   10:10進数    16:16進数
int  shift     = 0;   // 0:off   1:on
int  script    = 0;   // 1:aSin 2:aCos 3:aTan 計算中

TaskHandle_t thp[1];//マルチスレッドのタスクハンドル格納用

struct stack {
  struct stack *next;        // 次のスタックへの連結用ポインタ
  int digit;                 // 指数　10^n or 10^-n
  unsigned int numbers[4];   // BCDコードで数字を保持　2バイト * 4  = 16桁
  boolean code;              // 符号 正 = true or 負 = false
  boolean decimal_point;     // 小数点の有無 有 = true  無 = false
};

struct stack *bcd = new stack { (struct stack *)NULL, 0, 0, 0, 0, 0, true, false };      // スタックの最初の変数を宣言


class KbdRptParser : public KeyboardReportParser
{
  protected:
    void OnKeyDown	(uint8_t mod, uint8_t key);
};


void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
//  Serial.print(" ");
//  Serial.print(key, HEX);
// Serial.print(" ");
//  Serial.print(mod);
  in_key = key_cov( mod, key );
//  Serial.print(" ");
//  Serial.println( in_key );
}




USB     Usb;
//USBHub     Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);

KbdRptParser Prs;



// ******* USB 管理タスク ****************
void Task_USBkbCtr(void *pvParameters)
{
  (void) pvParameters;

  for (;;) // A Task shall never return or exit.
  {
    Usb.Task();
    delay(1);   // ウォッチドッグタイマー対策
  }
}


int debug = 0;

void setup()
{
  Serial.begin( 115200 );
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("Start");

  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");

  delay( 200 );


#if SD_MODEL    // SDカード付きの場合

  const char fname[] = "/key.txt";
  File fp;        // keyコードファイル用

  pinMode( LEDr, OUTPUT );
  pinMode( LEDg, OUTPUT );
  pinMode( LEDb, OUTPUT );
  digitalWrite( LEDr, LOW );
  digitalWrite( LEDg, HIGH );
  digitalWrite( LEDb, LOW ); 

  // SDカード初期化
  int led = 1;
  Serial.println("\nSD card Device 初期化");
  while( !SD.begin(SD_CS) ){

    Serial.println("****** Card failed *****");
    digitalWrite( LEDg, LOW );
    digitalWrite( LEDr, led );
    delay(1000);
    led = led^1;

  };

  fp = SD.open( fname, FILE_READ );
  while( !fp ){

    Serial.printf("%s file not open!\n", fname);
    digitalWrite( LEDg, LOW );
    digitalWrite( LEDr, led );
    delay(1000);
    led = led^1;
    fp = SD.open( fname, FILE_READ );
    
  }
  digitalWrite( LEDg, HIGH );
  Serial.printf("%s file open!\n", fname );
  key_code( fp );  // キーコードの読み込み

# else

  key_def();

#endif

  HidKeyboard.SetReportParser(0, &Prs);

  // OLED 管理タスク *******************
  xTaskCreatePinnedToCore(
    Task_OLED
    ,  "Task_OLED"   // A name just for humans
    ,  2048   // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  4      // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest. configMAX_PRIORITIES = 25
    ,  NULL 
    ,  tskNO_AFFINITY      // Core 0 : Process CPU   Core 1 : Application CPU． tskNO_AFFINITY : 実行コアはOS任せ
    );
    dsp_start = 1;

  // USB キーボード 管理タスク **********
  xTaskCreatePinnedToCore(
    Task_USBkbCtr
    ,  "Task_USBkbCtr"   // A name just for humans
    ,  2048   // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1      // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest. configMAX_PRIORITIES = 25
    ,  NULL 
    ,  tskNO_AFFINITY      // Core 0 : Process CPU   Core 1 : Application CPU．
    );
    Serial.println( "USB キーボード管理 タスク起動" );

  Serial.println( "Set Up END!!!" );
//  printStack();

#if SD_MODE

  digitalWrite( LEDr, LOW );
  digitalWrite( LEDg, LOW );
  digitalWrite( LEDb, LOW ); 

#endif
}




int index_input = 0;  // 入力している桁数

void loop()
{

  static int add_stack;    // 四則演算後に1を立てる。1の場合は数字キーを入力するとスタックを追加する。

  
  if ( in_key != (char)NULL || script != 0 ) {

    // 数字入力
    if ( '0' <= in_key && in_key <= '9' ) {           // ******  '0' = 48  '9' = 57 ***********************
/*
Serial.print( "index = " );
Serial.print( index_input );
Serial.print( " add_stack = " );
Serial.println( add_stack );
*/
      if ( index_input == 0 && add_stack == 1 ) {   // スタックの追加
        struct stack *p;                            
 
        if ( ( p = (struct stack *)malloc( sizeof( struct stack ))) == NULL ) {
          Serial.println( "メモリ不足のためスタックの挿入ができません。" );
          error = 5;
        }
        // スタックの連結ポインタをセット
        *p = { bcd, 0, 0, 0, 0, 0, true, false };   // スタックを初期化
        if ( index_input == 1 && add_stack == 2 ) (*p).decimal_point = true;
        bcd = p;
        add_stack = 0;
        dsp_start = 1;

      }

      unsigned int number = in_key - 48;    // キャラクターコードを数値に変換

      if ( ( (*bcd).numbers[3] & 0b1111000000000000 ) == 0 ) {
        (*bcd).numbers[3] = (*bcd).numbers[3] << 4;
        (*bcd).numbers[3] += ( (*bcd).numbers[2] & 0b1111000000000000 ) >> 12;
        (*bcd).numbers[2] = (*bcd).numbers[2] << 4;
        (*bcd).numbers[2] += ( (*bcd).numbers[1] & 0b1111000000000000 ) >> 12;
        (*bcd).numbers[1] = (*bcd).numbers[1] << 4;
        (*bcd).numbers[1] += ( (*bcd).numbers[0] & 0b1111000000000000 ) >> 12;
        (*bcd).numbers[0] = (*bcd).numbers[0] << 4;
        (*bcd).numbers[0] += number;
      }

      index_input++;    // 入力文字数のカウント

      if ( cal_mode == 1 ){
        if ( (*bcd).decimal_point == false ) {
          if ( index_input == 1 && in_key == '0' )  index_input = 0;
          if ( index_input > 1 ) (*bcd).digit++;    // 1桁目以上の入力
        } else {
          if ( index_input == 1 ) (*bcd).digit--;   // 小数点以下の入力
          else if ( index_input >= 2 ) {
            int dat[16];
            get_num( *bcd, dat );
            if ( chkdigit( dat ) == 1 ) (*bcd).digit--;
            else if ( chkzero( dat ) == 0 ) (*bcd).digit--;
          }

        }
      }
      print_key = String( in_key );     // 入力キーの表示
      in_key = (char)NULL;
      dsp_start = 1;
      
    } else if ( in_key == 'E' ){                      // *******  ENTERキー *******************************

      if ( base != 10 ) {
        base = 10;
        print_key = "Dec";     // 入力キーの表示
      } else {
        
        int dat[16];
        get_num( *bcd, dat );
        if ( chkzero( dat ) != 0 ) {

      // スタックの追加
          struct stack *p;

          if ( ( p = (struct stack *)malloc( sizeof( struct stack ))) == NULL ) {
            Serial.println( "メモリ不足のためスタックの挿入ができません。" );
            error = 5;
          }

      // スタックの連結ポインタをセット
          *p = { bcd, 0, 0, 0, 0, 0, true, false };   // スタックを初期化
          bcd = p;
          print_key = "ENTER";     // 入力キーの表示
        }
        
      }

//      Serial.println( print_key );
      if ( script == 0 ) printStack();

      index_input = 0;  // 入力桁数のリセット
      dsp_start = 1;

      in_key = (char)NULL;

    } else if ( in_key == '+' || in_key == '-' ) {    // ******** + キー or - キー ****************************

      if ( (*bcd).next != 0 ) {
        struct stack *p = (*bcd).next;
        int add1[16], add2[16];

if ( debug == 1 )
  printStack();

        if ( in_key == '-' ) (*bcd).code ^= true;     // '-' のとき、符号を反転させる
//      printStack();
        
        get_num(   *p, add1 );    // BCDコードをint配列に取り出す
        get_num( *bcd, add2 );

        if ( (*p).code == (*bcd).code ) {                                   // (+) + (+)  or  (-) + (-)
          if ( cal_mode == 0 )
            intadd( add1, add2 );                                               // 整数加算
          else
            (*p).digit = realadd( add1, (*p).digit, add2, (*bcd).digit );       // 実数加算
        } else {                                                            // (+) + (-)  or  (-) + (+)
          if ( cal_mode == 0 ) {
            
            if ( ls_comparison( add1, add2 ) == 2 ) {     // add2 - add1 :　add1 < add2 のとき add1とadd2を入れ替える
              
              boolean code_tmp;   // 符号 正 = true or 負 = false
              code_tmp = (*p).code;
              (*p).code = (*bcd).code;
              (*bcd).code = code_tmp;

              int numbers_tmp;   // BCDコードで数字を保持　2バイト * 4  = 16桁
              for ( int i = 0 ; i < 16 ; i++ ){
                numbers_tmp = add1[i];
                add1[i] = add2[i];
                add2[i] = numbers_tmp;
              }
            }
            
            (*p).code = intdel( (*p).code, add1, (*bcd).code,add2 );            // 整数減算
            
          } else {
            realdel( p, add2 );                                                 // 実数減算
            get_num( *p, add1 );
          }
        }
//      printStack();

        if ( cal_mode == 1 ) (*p).decimal_point = true;

        set_num( p, add1 );      // int配列のデータをBCDコードに変換する
        struct stack *temp_bcd = bcd;
        bcd = p;
        free( temp_bcd );              // スタックの開放
        add_stack = 1;
      }

//      printStack();
      print_key = String( in_key );     // 入力キーの表示
      index_input = 0;                  // 入力桁数のリセット
      dsp_start = 1;
      in_key = (char)NULL;

    } else if ( in_key == '*' ) {                       // ******* * ( 乗算 ) キー ****************************
      
      if ( (*bcd).next != 0 ) {
        struct stack *p = (*bcd).next;

//      Serial.println( in_key );
//      printStack();

        if ( cal_mode == 0 ) {    // 整数積
          int add1[16], add2[16];

          get_num( *p, add1 );
          get_num( *bcd, add2 );

          multiplication( add1, add2 );   // 整数積関数
          set_num( p, add1 );             // int配列のデータをBCDコードに変換する
        } else {                  // 実数積
          real_multiplication( p );  // 実数積関数
          (*p).decimal_point = true;
        }
        
        if      ( (*p).code == false && (*bcd).code == false ) (*p).code = true;        // 符号の乗算
        else if ( (*p).code == false || (*bcd).code == false ) (*p).code = false;

        struct stack *temp_bcd = bcd;
        bcd = p;
        free( temp_bcd );              // スタックの開放
        add_stack = 1;
      }

      //printStack();

      print_key = String( in_key );     // 入力キーの表示
      index_input = 0;                  // 入力桁数のリセット
      dsp_start = 1;
      in_key = (char)NULL;
      
    } else if ( in_key == '/' ) {                         // ****** / (除算）キー ******************************

      if ( (*bcd).next != 0 ) {
        struct stack *p = (*bcd).next;
        int add1[16], add2[16], result[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

//      Serial.println( in_key );
//      printStack();

        if      ( (*p).code == false && (*bcd).code == false ) (*p).code = true;        // 符号の乗算
        else if ( (*p).code == false || (*bcd).code == false ) (*p).code = false;

        if ( cal_mode == 0 ) {
          get_num( *p, add1 );    // 割られる数
          get_num( *bcd, add2 );  // 割る数

          (*p).digit = division( add1, add2, result );

        } else {
          realdivision( p, result );
        }
        
        set_num( p, result );      // int配列のデータをBCDコードに変換する
        struct stack *temp_bcd = bcd;
        bcd = p;
        free( temp_bcd );              // スタックの開放
        add_stack = 1;
      }
      print_key = String( in_key );     // 入力キーの表示
      index_input = 0;                  // 入力桁数のリセット
      dsp_start = 1;
      in_key = (char)NULL;
      
//      printStack();
      do{ delay(10); }while( dsp_start == 1 );    // 画面書き換え待ち
      if ( cal_mode == 0 ) (*bcd).digit = 0;      // 整数演算モードの場合は指数部をクリア（余りを初期化）

    } else if ( in_key == 'C' ) {                           // ******** C (クリア) キー ***********************

        int bcd_str[16];

        get_num( *bcd, bcd_str );
        int n = chkdigit( bcd_str );
        
        if ( n == 0 && (*bcd).next != (struct stack *)NULL ) {
          // カレントのスタックを開放
          struct stack *temp_bcd = bcd;
          bcd = (*bcd).next;
          free( temp_bcd );              // スタックの開放

        } else  if ( n == 0 && (*bcd).next == (struct stack *)NULL ) {
          
          (*bcd).digit = 0;                 // 指数　10^n or 10^-n
          (*bcd).code  = true;              // 符号 正 = true or 負 = false
          (*bcd).decimal_point = false;     // 小数点の有無 有 = true  無 = false
          index_input = 0;

        } else {  // 1文字削除
          
          if ( cal_mode == 0 ) {
            // 整数演算モード
            del_char( bcd_str, n );
          } else {
            // 実数演算モード
            if ( (*bcd).digit >= 16 ) 
              (*bcd).digit--;
            else
              del_char( bcd_str, n );
          }
          
        }

//      printStack();

      add_stack = 0;
      print_key = "CLR";     // 入力キーの表示
      dsp_start = 1;
      in_key = (char)NULL;
      
    } else if ( in_key == 'H' ) {                          // ****** H (16進数表示) キー ***********************

      print_key = "Hex";     // 入力キーの表示
      base = 16;
      dsp_start = 1;
      in_key = (char)NULL;


    } else if ( in_key == 'O' ) {                          // ****** O (8進数表示) キー ***********************

      print_key = "Oct";     // 入力キーの表示
      base = 8;
      dsp_start = 1;
      in_key = (char)NULL;


    } else if ( in_key == 'B' ) {                          // ****** B (2進数表示) キー ***********************

      print_key = "Bin";     // 入力キーの表示
      base = 2;
      dsp_start = 1;
      in_key = (char)NULL;

    } else if ( in_key == '.' ) {                          // ******* . (小数点) キー **************************

      if ( cal_mode >= 1) {        // 実数演算モード
        if ( add_stack == 1 ) {
      // スタックの追加
          struct stack *p;

          if ( ( p = (struct stack *)malloc( sizeof( struct stack ))) == NULL ) {
            Serial.println( "メモリ不足のためスタックの挿入ができません。" );
            error = 5;
          }

      // スタックの連結ポインタをセット
          *p = { bcd, 0, 0, 0, 0, 0, true, false };   // スタックを初期化
          bcd = p;
          index_input = 0;                  // 入力桁数のリセット
          add_stack = 0;
        } else 
        (*bcd).decimal_point = true;
        int num[16];
        get_num( *bcd, num );
        if ( chkzero( num ) == 0 ) (*bcd).digit = index_input;
        else                       (*bcd).digit = index_input - 1;
      }

      print_key = ".";
      dsp_start = 1;
      in_key = (char)NULL;
      
    } else if ( in_key == 'i' ) {                           // ******* Shift '.' （指数加算） ***********************

      if ( (*bcd).digit < 15 ) {   // 全てを右シフトし、1桁目に0を挿入
        int dat[16];
        get_num( *bcd, dat );
        l_shift( dat, 1 );
        set_num( bcd, dat );
      }
      
      (*bcd).digit++;   // 指数部のインクリメント　→　eが'-'の場合は0方向へ、 '+'の場合は桁上げ

      print_key = ".";
      dsp_start = 1;
      in_key = (char)NULL;
      
    } else if ( in_key == 's' ) {                         // ******** Sin ****************************

//      Serial.println( in_key );
//      printStack();

      
      CORDIC( 1 );
      
      index_input = 0;                  // 入力桁数のリセット
      print_key = "Sin";
      dsp_start = 1;
      in_key = (char)NULL;
      
//      printStack();

    } else if ( in_key == 'c' ) {                         // ******** Cos ****************************

//      Serial.println( in_key );
//      printStack();

      CORDIC( 2 );
      
      index_input = 0;                  // 入力桁数のリセット
      print_key = "Cos";
      dsp_start = 1;
      in_key = (char)NULL;
      
//      printStack();

    } else if ( in_key == 't' ) {                         // ******** Tan ****************************

//      Serial.println( in_key );
//      printStack();

      CORDIC( 3 );
      
      index_input = 0;                  // 入力桁数のリセット
      print_key = "Tan";
      dsp_start = 1;
      in_key = (char)NULL;
      
//      printStack();

    } else if ( in_key == 'r' ) {                         // ******** + / - ****************************

      (*bcd).code ^= true;
      
      print_key = "+/-";     // 入力キーの表示
      dsp_start = 1;
      in_key = (char)NULL;
      
//      printStack();

    } else if ( in_key == 'R' ) {                         // ******** Reset *****************************      
      ESP.restart();

    } else if ( in_key == 'S' || in_key == 'A' || in_key == 'T' ) { // ******** 逆三角関数 ***************************

printStack();

      if ( in_key == 'S' )      script = 1;     // aSin
      else if ( in_key == 'A' ) script = 2;     // aCos
      else                      script = 3;     // aTan

      // arc 計算タスク *******************
      xTaskCreatePinnedToCore(
        Task_arc
        ,  "Task_arc"   // A name just for humans
        ,  3072   // This stack size can be checked & adjusted by reading the Stack Highwater
        ,  NULL
        ,  9      // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest. configMAX_PRIORITIES = 25
        ,  &thp[0]  // タスクハンドル 
        ,  tskNO_AFFINITY      // Core 0 : Process CPU   Core 1 : Application CPU． tskNO_AFFINITY : 実行コアはOS任せ
      );
      
//      index_input = 0;                          // 入力桁数のリセット
//      in_key = (char)NULL;

    } else if ( in_key == 'm' ) {                       // ********** x <-> y スタックの入れ替え ************
//      printStack();

      if ( (*bcd).next != (struct stack *)NULL ) {      // stackが１つしかない場合、入れ替えしない
        
        struct stack *tmp0 = bcd;         // stack0 のアドレスを保存
        bcd = (*tmp0).next;               // stack0 の連結アドレスstack1 を bcdのポインタ にセット
        struct stack *tmp2 = (*bcd).next; // stack1 の指し示す stack2 の連結アドレスを保存
        (*bcd).next  = tmp0;              // stack1 の連結アドレスに stack0 のアドレス（ポインタ）をセット
        (*tmp0).next = tmp2;              // stack0 の連結アドレスに stack2 のアドレス（ポインタ）をセット
      
      } else {
        error = 7;    // Stack underflow 
      }

      print_key = "x<->y";     // 入力キーの表示
      dsp_start = 1;
      in_key = (char)NULL;
        
//      printStack();
    }
      
  }   // キー入力処理　終了

  delay(1);


}
