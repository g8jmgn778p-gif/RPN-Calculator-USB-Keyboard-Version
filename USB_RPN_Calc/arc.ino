/*
  (*bcd).next = ;                   // 次のスタックへの連結用ポインタ
  (*n).digit = (*bcd).digit;                  // 指数　10^n or 10^-n
  (*n).numbers[0] = (*bcd).numbers[0];        // BCDコードで数字を保持　2バイト * 4  = 16桁
  (*n).numbers[1] = (*bcd).numbers[1];
  (*n).numbers[2] = (*bcd).numbers[2];
  (*n).numbers[3] = (*bcd).numbers[3];
  (*n).code = (*bcd).code;                    // 符号 正 = true or 負 = false
  (*n).decimal_point = (*bcd).decimal_point;  // 小数点の有無 有 = true  無 = false

  テーラー展開にて逆三角関数を算出

script = 1;     // aSin
script = 2;     // aCos
script = 3;     // aTan
 
*/

// ***************  逆三角関数計算タスク  *****************

void Task_arc(void *pvParameters)
{
  (void) pvParameters;

  long long int arc[][20] = { {  166666666666667,               75,  446428571428571,  303819444444444,  223721590909091, 
                                 173527644230769,       1396484375,  115518008961397,  976160952919408,  839033580961682, 
                                 731252587359885,  644721031188965,  574003767084192,   51533096823199,   46601434869151, 
                                 424090709367936,  388096455883767,  356920539382593,  329705950347348,  305782164925803 },
                              { -166666666666667,              -75, -446428571428571, -303819444444444, -223721590909091,
                                -173527644230769,      -1396484375, -115518008961397, -976160952919408, -839033580961682,
                                -731252587359885, -644721031188965, -574003767084192,  -51533096823199,  -46601434869151,
                                -424090709367936, -388096455883767, -356920539382593, -329705950347348, -305782164925803 },
                               { -333333333333333,               2, -142857142857143,  111111111111111, -909090909090909,
                                 769230769230769, -666666666666667,  588235294117647, -526315789473684,  476190476190476,
                                -434782608695652,                4,  -37037037037037,  344827586206897,  -32258064516129,
                                 303030303030303, -285714285714286,   27027027027027, -256410256410256,   24390243902439 } };
                             
  int arc_e[][20] = { { -1, -2, -2, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3 },
                      { -1, -2, -2, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3 },
                      { -1, -1, -1, -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2 } };

  String funcStr[] = { "aSin", "aCos", "aTan" };
//  Serial.println( funcStr[ script - 1 ] );
//  printStack();

  in_key = (char)NULL;                    
  struct stack *n  = new stack { (struct stack *)NULL, (*bcd).digit, (*bcd).numbers[0], (*bcd).numbers[1], (*bcd).numbers[2], (*bcd).numbers[3], (*bcd).code, true };
  struct stack *nn = new stack { (struct stack *)NULL, 0, 0, 0, 0, 0, true, true };
  struct stack *r  = new stack { (struct stack *)NULL, 0, 0, 0, 0, 0, true, true };

  int lb = 0;     // -16桁より小さい計算になった場合ループを抜ける。その時のループ回数
  
//  next_key();
  if ( script == 2 ){       // aCos
    (*bcd).code = false;
    in_key = 'E';
    next_key();
    setStack( (long long int)15707963267949, 0, bcd );
  }

  in_key = 'E';
  next_key();

  bcd_cp( n );
  
  in_key = 'E';
  next_key();

  for ( int i = 0 ; i < 20 ; i++ ){
    bcd_cp( n );
    
    in_key = 'E';
    next_key();

    bcd_cp( n );

    in_key = '*';   // 2乗
    next_key();

    in_key = '*';   // 3乗
    next_key();

    tmp_cp( nn );

    in_key = 'E';
    next_key();

    setStack( arc[script-1][i], arc_e[script-1][i], r );

    in_key = 'E';
    next_key();

    bcd_cp( r );
    in_key = '*';
    next_key();

    lb = i;   // ループ回数
    if ( i < 19 && (*bcd).digit > -16 ){
      in_key = 'E';
      next_key();

      bcd_cp( nn );

      in_key = 'E';
      next_key();

    } else {
      break;
    }

  }

//printStack();
debug = 0;
  if ( script == 2 ) {    // aCos用
    for ( int i = 0 ; i <= lb + 1 ; i++ ){
      in_key = '+';
      next_key();
    }  
  } else {
    for ( int i = 0 ; i <= lb ; i++ ){
      in_key = '+';
      next_key();
//printStack();
    }
  }
  debug = 0;


//--------------------  
  in_key = 'E';
  next_key();
  setStack( 572957795130823, 1, bcd );    // Rad → 度
  in_key = '*';
  next_key();

//  printStack();
  
  free( n );   // スタックの開放
  free( nn );  // スタックの開放
  free( r );   // スタックの開放

//  printStack();

  if ( script == 1 )      print_key = "arcSin";     // 入力キーの表示
  else if ( script == 2 ) print_key = "arcCos";     // 入力キーの表示
  else                    print_key = "arcTan";     // 入力キーの表示

  while( dsp_start != 0 ) {   // 画面書き換え終了待ち
    delay(1);
  }
  
  dsp_start = 1;
  while( dsp_start != 0 ) {   // 画面書き換え終了待ち
    delay(1);
  }

  script = 0;
  index_input = 0;                          // 入力桁数のリセット
  in_key = (char)NULL;
  
  vTaskDelete(NULL);      // タスク終了
}




// ********** long long int を BCDデータにセットする ************

void setStack( long long int nn, int e, struct stack *n ) {
  (*n).digit = e;                   // 指数　10^n or 10^-n
//  Serial.print( "digit = " );
//  Serial.println( (*n).digit );
  if ( nn < 0 ) {
    (*n).code = false;   // 符号 正 = true or 負 = false
    nn *= -1;
  } else {
    (*n).code = true;   // 符号 正 = true or 負 = false
  }

  int num[16];

  for ( int i = 0 ; i < 16 ; i++ ){
    num[i] = nn % 10;
    nn = nn / 10;
//    Serial.print( num[i] );
  }
//  Serial.println();

  set_num( n, num );  // BCDデータをセット
  
}

// ******* コマンド処理待ち後 ***********

void next_key( void ){

//Serial.println( in_key );
  delay(20);

  while( in_key != (char)NULL ){
    delay(20);
  }

//  Serial.print( in_key );
//  Serial.println( "!!!" );

}


// ******* BCD にセット **************

void bcd_cp( struct stack *code ){
  
  (*bcd).digit = (*code).digit;
  (*bcd).numbers[0] = (*code).numbers[0];
  (*bcd).numbers[1] = (*code).numbers[1];
  (*bcd).numbers[2] = (*code).numbers[2];
  (*bcd).numbers[3] = (*code).numbers[3];
  (*bcd).code = (*code).code;
  (*bcd).decimal_point = (*code).decimal_point;

//  printStack();

}


// ******* 一時メモリに保存 **************

void tmp_cp( struct stack *code ){
  
  (*code).digit = (*bcd).digit;
  (*code).numbers[0] = (*bcd).numbers[0];
  (*code).numbers[1] = (*bcd).numbers[1];
  (*code).numbers[2] = (*bcd).numbers[2];
  (*code).numbers[3] = (*bcd).numbers[3];
  (*code).code = (*bcd).code;
  (*code).decimal_point = (*bcd).decimal_point;

}
