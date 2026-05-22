
// 整数　積

void multiplication( int add1[16], int add2[16] ) {

  int add1_digit =  chkdigit( add1 );
  int add2_digit =  chkdigit( add2 );

  if ( add1_digit + add2_digit - 1 > 16 ) error = 1;    // オーバーフロー発生　2024.04.16

  if ( add1_digit == 0 || add2_digit == 0 )
    for ( int i = 0 ; i < 16 ; i++ ) add1[i] = 0;
  else {
    int add1N = add1_digit + add2_digit - 1;
    int add_temp[ add2_digit ][ add1N ];    // かけ算の途中
    int add_sum[ add1N ];                   // 途中の値の合計

    for ( int i = 0 ; i < add2_digit ; i++ )
      for ( int j = 0 ; j < add1N ; j++ )
        add_temp[i][j] = 0;                     // 初期化

    for ( int i = 0 ; i < add2_digit ; i++ )
      for ( int j = 0 ; j < add1_digit ; j++ )
        add_temp[i][i+j] = add1[j] * add2[i];   // 積

/*    for ( int i = 0 ; i < add2_digit ; i++ ) {            // 積の結果を表示
      for ( int j = add1_digit - 1 ; j >= 0 ; j-- ) {
        Serial.print( add_temp[i][i+j] );
        Serial.print( " " );
      }
      Serial.println();
    }
*/
    for ( int i = 0 ; i < add1N ; i++ ) {       // 積の結果の縦の和
      add_sum[i] = 0;
      for ( int j = 0 ; j < add2_digit; j++ ) {
        add_sum[i] += add_temp[j][i];
      }

    }

    int carry = 0;
    for ( int i = 0 ; i < add1N ; i++ ) {       // 縦の和を合計する
      add1[i]  = add_sum[i] + carry;
      carry    = add1[i] / 10;
      add1[i] -= carry * 10;
//      if ( i == 15 && carry > 0 ) error = 1;    // オーバーフロー発生
      if ( carry > 0 ) {
        if ( i == 15 ) error = 1;    // オーバーフロー発生
        else add1[ i + 1 ] = carry;
      }
    }
  }
}

// 実数　積　関数

void real_multiplication( struct stack *p ) {

//  int carry = 0;
  int digit;
  int add1[16], add2[16];

   get_num( *p, add1 );
   get_num( *bcd, add2 );

   int digit1 = (*p).digit;
   int digit2 = (*bcd).digit;

  // 一番上の表示桁の位を求める
  int add1_digit = chkdigit( add1 );
  int add2_digit = chkdigit( add2 );

  if ( add1_digit != 16 ) l_shift( add1, 16 - add1_digit );   // 左詰め
  if ( add2_digit != 16 ) l_shift( add2, 16 - add2_digit );
/*
Serial.print( digit );
Serial.print( " " );
printBCD( add1 );
Serial.print( digit );
Serial.print( " " );
printBCD( add2 );
*/
  if ( digit1 > digit2 ) {                      // 桁そろえ
    r_shift( add2, digit1 - digit2 );
//    digit = digit1;
  } else {
    r_shift( add1, digit2 - digit1 );
//    digit = digit2;
  }
  digit = digit1 + digit2;
/*
Serial.println();
Serial.print( digit );
Serial.print( " " );
printBCD( add1 );
Serial.print( digit );
Serial.print( " " );
printBCD( add2 );
*/
  // 桁そろえ後の一番上の表示桁の位を求める
  add1_digit = chkdigit( add1 );
  add2_digit = chkdigit( add2 );

  if ( add1_digit == 0 || add2_digit == 0 )
    for ( int i = 0 ; i < 16 ; i++ ) add1[i] = 0;
  else {
    int add1N = add1_digit + add2_digit - 1;
    int add_temp[ add2_digit ][ add1N ];    // かけ算の途中
    int add_sum[ add1N ];                   // 途中の値の合計
/*
    Serial.print( add1N );
    Serial.print(" ");
    Serial.println( add2_digit );
*/
    for ( int i = 0 ; i < add2_digit ; i++ )
      for ( int j = 0 ; j < add1N ; j++ )
        add_temp[i][j] = 0;                     // 初期化

    for ( int i = 0 ; i < add2_digit ; i++ )
      for ( int j = 0 ; j < add1_digit ; j++ )
        add_temp[i][i+j] = add1[j] * add2[i];   // 積
/******
    for ( int i = 0 ; i < add2_digit ; i++ ) {            // 積の結果を表示
      for ( int j = add1_digit - 1 ; j >= 0 ; j-- ) {
        Serial.print( add_temp[i][i+j] );
        Serial.print( " " );
      }
      Serial.println();
    }
******/

    for ( int i = 0 ; i < add1N ; i++ ) {       // 積の結果の縦の和
      add_sum[i] = 0;
      for ( int j = 0 ; j < add2_digit; j++ ) {
        add_sum[i] += add_temp[j][i];
      }

    }

//Serial.println();
//for ( int i = add1N - 1 ; i > 0 ; i-- )
//  Serial.print( add_sum[i] );
//Serial.println();

    int carry = 0;
    for ( int i = 0 ; i < add1N ; i++ ) {       // 縦の和を合計する
      add_sum[i]  = add_sum[i] + carry;
//      Serial.print( add1[i] );
      carry    = add_sum[i] / 10;
      add_sum[i] -= carry * 10;
      if ( i == add1N - 1 && carry > 0 ) {   // オーバーフロー発生
        for ( int i = 1 ; i <= add1N - 1 ; i++ )
          add_sum[ i-1 ] = add_sum[i];
        add_sum[ add1N - 1 ] = carry;
        digit++;
      }
    } 
//    Serial.println();

    for ( int i = 0 ; i < 16 ; i++ )    // 縦の和の合計の上位16桁
      add1[i] = add_sum[ add1N - 16 + i ];
  }


/*
Serial.println();
Serial.print( digit );
Serial.print( " " );
printBCD( add1 );
Serial.print( digit );
Serial.print( " " );
printBCD( add2 );
*/
  if ( chkzero( add1 ) != 0 ) {
    while( add1[ 0 ] == 0 ) {                   // 右詰め
      if ( chkdigit( add1 ) - 1 == digit ) break;       // ０の桁を残す

      for ( int i = 0 ; i < 15 ; i++ )
        add1[ i ] = add1[ i + 1 ];
      add1[ 15 ] = 0;
    };
  }

/*  
Serial.println();
Serial.print( digit );
Serial.print( " " );
printBCD( add1 );
Serial.print( digit );
Serial.print( " " );
printBCD( add2 );
*/
  set_num( p, add1 );             // int配列のデータをBCDコードに変換する
  (*p).digit = digit;

}




// 桁数チェック関数

int chkdigit( int *dat ) {
  int i;
  for ( i = 15 ; i >= 0 ; i-- ){
//    Serial.print( dat[i] );
    if ( dat[i] != 0 ) break;
  }
//  Serial.print(" ");
//  Serial.println( i + 1 );
  return( i + 1 );
}

// BCDデータが０か確認する関数
// 0の場合:0  0でない場合:1

int chkzero( int *dat ) {
  for ( int i = 0 ; i < 16 ; i++ ) {
    if ( dat[i] != 0 ) return(1);
  }
  return(0);
}


// 整数加算

void intadd( int *add1, int *add2 ) {

  int carry = 0;
  
  for ( int i = 0 ; i < 16 ; i++ ) {
    add1[i] += add2[i] + carry;
    carry    = add1[i] / 10;
    add1[i] -= carry * 10;
    if ( i == 15 && carry > 0 )   // オーバーフロー発生
      error = 1;    // オーバーフローメッセージ
  }

}

// 実数加算

int realadd( int *add1, int digit1, int *add2, int digit2 ) {
  int carry = 0;
  int digit;

  // 一番上の表示桁の位を求める
  int n1 = chkdigit( add1 );
  int n2 = chkdigit( add2 );

  if ( n1 != 16 ) l_shift( add1, 16 - n1 );   // 左詰め
  if ( n2 != 16 ) l_shift( add2, 16 - n2 );


  if ( digit1 > digit2 ) {                      // 桁そろえ
    r_shift( add2, digit1 - digit2 );
    digit = digit1;
  } else {
    r_shift( add1, digit2 - digit1 );
    digit = digit2;
  }

  // 加算
  for ( int i = 0 ; i < 16 ; i++ ) {
    add1[i] += add2[i] + carry;
    carry    = add1[i] / 10;
    add1[i] -= carry * 10;
    if ( i == 15 && carry > 0 ) {   // オーバーフロー発生
      carry_down( add1, carry );
      digit++;
    }
  }

  if ( chkzero( add1 ) != 0 ) {
    while( add1[ 0 ] == 0 ) {                   // 右詰め
      if ( chkdigit( add1 ) - 1 == digit ) break;       // ０の桁を残す

      for ( int i = 0 ; i < 15 ; i++ )
        add1[ i ] = add1[ i + 1 ];
      add1[ 15 ] = 0;
    };
  }
  return( digit );
}





// 右シフト

void r_shift( int *add2, int n ) {

    for ( int i = 0 ; i < n ; i++ ) {
      for ( int j = 0 ; j < 15 ; j++ )
        add2[j] = add2[ j + 1 ];
      add2[15] = 0;
//      digit++;
    }

//  return( digit );

}


// 左シフト

void l_shift( int *add2, int n ) {

    for ( int i = 0 ; i < n ; i++ ) {
      for ( int j = 15 ; j > 0 ; j-- )
        add2[j] = add2[ j - 1 ];
      add2[0] = 0;
    }

}

// 桁の繰り下げ

void carry_down( int *add, int carry ) {

  for ( int i = 1 ; i <= 15 ; i++ )
    add[ i-1 ] = add[i];

  add[ 15 ] = carry;
  
}


// 整数減算2

boolean intdel2( boolean code1, int *add1, boolean code2, int *add2 ) {

  int calcStart;
  boolean code = code1;

    // 引く数の符号を反転
  if ( code2 == true ) code2 = false;
  else code2 = true;

  if ( code1 == code2 ) {   // 符号が同じなのでたし算
    intadd( add1, add2 );
    code = code1;
  } else {                  // 符号が異なるので引き算
    int n = ls_comparison( add1, add2 );
    if ( n == 0 ) {   // 同じ数での引き算なのでゼロ
      code = true;
      for ( int i = 0 ; i < 16 ; i++ )
        add1[i] = 0;
    } else {
      if ( n == 2 ) {  // 引く数の方が、引かれる数より大きい
        code = code2;
        int tem[16];
        for ( int t = 0 ; t < 16 ; t++ ) {    // 引く数と引かれる数を入れ替え
          tem[t]  = add1[t];
          add1[t] = add2[t];
          add2[t] = tem[t];
        }
      }

      for ( calcStart = 15 ; calcStart >= 0 ; calcStart-- )
        if ( add1[ calcStart ] != 0 || add2[ calcStart ] != 0 ) break;

      if ( add1[calcStart] >= add2[calcStart] ) {
        for ( int i = calcStart ; i >= 0 ; i-- ) {
          if ( add1[i] < add2[i] ) {    // 上の桁から借りてくる
            int n = 0;  // 戻る桁数
            do {
              n++;
            }while( add1[ i + n ] <= 0 );
          
            for ( int j = n ; j > 0 ; j-- ) {
              add1[ i + j ]--;
              add1[ i + j - 1 ] += 10;
            }
          }
          add1[i] -= add2[i];
        }
        
      } else {
        for ( int i = calcStart ; i >= 0 ; i-- ) {
          if ( add1[i] > add2[i] ) {
            add2[i+1]--;
            add2[i] += 10;
          }
          add2[i] -= add1[i];
        }
        for ( int i = 0 ; i < 16 ; i++ ) add1[i] = add2[i];
      }

      if ( chkzero( add1 ) == 0 ) {
        clrBCD();
      }
    }

  }     // 引き算終了
  
  return( code );

}

// 整数減算

boolean intdel( boolean code1, int *add1, boolean code2, int *add2 ) {

  int calcStart;
  boolean code;

  for ( calcStart = 15 ; calcStart >= 0 ; calcStart-- ) {
    if ( add1[ calcStart ] != 0 || add2[ calcStart ] != 0 ) break;
  }

  if ( add1[calcStart] >= add2[calcStart] ) {
    for ( int i = calcStart ; i >= 0 ; i-- ) {
      if ( add1[i] < add2[i] ) {    // 上の桁から借りてくる
        int n = 0;  // 戻る桁数
        do {
          n++;
        }while( add1[ i + n ] <= 0 );
        
        for ( int j = n ; j > 0 ; j-- ) {
          add1[ i + j ]--;
          add1[ i + j - 1 ] += 10;
        }
      }
      add1[i] -= add2[i];
    }
    code = code1;
  } else {
    for ( int i = calcStart ; i >= 0 ; i-- ) {
      if ( add1[i] > add2[i] ) {
        add2[i+1]--;
        add2[i] += 10;
      }
      add2[i] -= add1[i];
    }
    code = code2;
    for ( int i = 0 ; i < 16 ; i++ ) add1[i] = add2[i];
  }

  if ( chkzero( add1 ) == 0 ) {
    clrBCD();
    code = true;
  }
  
  return( code );

}



// 実数減算

void realdel( struct stack *p, int *add2 ) {
  boolean code = true;
  int digit;
  int add1[16];
  
  int digit1 = (*p).digit;
  boolean code1 = (*p).code;

  int digit2 = (*bcd).digit;
  boolean code2 = (*bcd).code;

  get_num( *p, add1 );
  
  // 一番上の表示桁の位を求める
  int n1 = chkdigit( add1 );
  int n2 = chkdigit( add2 );

  int bors = rels_comparison( p );  // 0:同じ　1:*pが大　2:*bcdが大 

  if      ( bors == 1 ) code = code1;
  else if ( bors == 2 ) code = code2;

  // 小数点をそろえる
  l_shift( add1, 16 - n1 );
  l_shift( add2, 16 - n2 );
  if ( digit1 >= digit2 ) {
    r_shift( add2, digit1 - digit2 );
    digit = digit1;
  } else {
    r_shift( add1, digit2 - digit1 );
    digit = digit2;
  }
/*
Serial.print("  ");
printBCD( add1 );
Serial.print("  ");
printBCD( add2 );
Serial.print("  ");
Serial.print( digit );
Serial.print("  ");
*/
  int ls = ls_comparison( add1, add2 );
  if ( ls == 2 ) {

    for ( int i = 0 ; i < 16 ; i++ ) {    // 引き算
      if ( add2[i] < add1[i] ) {    // 上の桁から借りてくる
        int n = 0;  // 戻る桁数
        do {
          n++;
        }while( add2[ i + n ] == 0 );
        for ( int j = n ; j > 0 ; j-- ) {
          add2[ i + j ]--;
          add2[ i + j - 1 ] += 10;
        }
      }
      add2[i] -= add1[i];
    }
    for ( int i = 0 ; i < 16 ; i++ )
      add1[i] = add2[i];
    
  } else {

    for ( int i = 0 ; i < 16 ; i++ ) {    // 引き算
      if ( add1[i] < add2[i] ) {    // 上の桁から借りてくる
        int n = 0;  // 戻る桁数
        do {
          n++;
        }while( add1[ i + n ] == 0 );
        for ( int j = n ; j > 0 ; j-- ) {
          add1[ i + j ]--;
          add1[ i + j - 1 ] += 10;
        }
      }/*
      Serial.print( i );
      Serial.print(" ");
      Serial.print( add1[i] );
      Serial.print(" ");
      Serial.print( add2[i] );  */
      add1[i] -= add2[i];
/*      Serial.print(" ");
      Serial.print( add1[i] );
      Serial.print(" ");
      Serial.println( add2[i] );
*/    }
    
  }
  
//printBCD( add1 );
//Serial.print("  ");

  if ( chkzero( add1 ) != 0 ) {

  // 桁チェック
    for ( int i = 15 ; i >= 0 ; i-- ) {
      if ( add1[i] == 0 ) digit--;
      else break;
    }
  // 右の０を削除する
    int n = chkdigit( add1 ) - ( digit + 1 );   // 右の０を消す文字数
    while( add1[0] == 0 && n != 0) {
      r_shift( add1, 1 );
      n--;
    }
    
  } else {
    code = true;
    digit = 0;
  }
  
  set_num( p, add1 );
  (*p).code = code;
  (*p).digit = digit;

}




// int配列をBCDコードに入れる関数

void set_num( struct stack *code, int *num ) {

  (*code).numbers[3]  = num[15] << 12;
  (*code).numbers[3] += num[14] <<  8;
  (*code).numbers[3] += num[13] <<  4;
  (*code).numbers[3] += num[12];
  (*code).numbers[2]  = num[11] << 12;
  (*code).numbers[2] += num[10] <<  8;
  (*code).numbers[2] += num[ 9] <<  4;
  (*code).numbers[2] += num[ 8];
  (*code).numbers[1]  = num[ 7] << 12;
  (*code).numbers[1] += num[ 6] <<  8;
  (*code).numbers[1] += num[ 5] <<  4;
  (*code).numbers[1] += num[ 4];
  (*code).numbers[0]  = num[ 3] << 12;
  (*code).numbers[0] += num[ 2] <<  8;
  (*code).numbers[0] += num[ 1] <<  4;
  (*code).numbers[0] += num[ 0];

}


// BCDコードをint配列に入れる関数

void get_num( struct stack code, int *num ) {

  num[15] = ( code.numbers[3] & 0b1111000000000000 ) >> 12;
  num[14] = ( code.numbers[3] & 0b0000111100000000 ) >>  8;
  num[13] = ( code.numbers[3] & 0b0000000011110000 ) >>  4;
  num[12] = ( code.numbers[3] & 0b0000000000001111 );
  num[11] = ( code.numbers[2] & 0b1111000000000000 ) >> 12;
  num[10] = ( code.numbers[2] & 0b0000111100000000 ) >>  8;
  num[ 9] = ( code.numbers[2] & 0b0000000011110000 ) >>  4;
  num[ 8] = ( code.numbers[2] & 0b0000000000001111 );
  num[ 7] = ( code.numbers[1] & 0b1111000000000000 ) >> 12;
  num[ 6] = ( code.numbers[1] & 0b0000111100000000 ) >>  8;
  num[ 5] = ( code.numbers[1] & 0b0000000011110000 ) >>  4;
  num[ 4] = ( code.numbers[1] & 0b0000000000001111 );
  num[ 3] = ( code.numbers[0] & 0b1111000000000000 ) >> 12;
  num[ 2] = ( code.numbers[0] & 0b0000111100000000 ) >>  8;
  num[ 1] = ( code.numbers[0] & 0b0000000011110000 ) >>  4;
  num[ 0] = ( code.numbers[0] & 0b0000000000001111 );

}


// 整数　割り算

int division( int *add1, int *add2, int *result ) {

  int amari = 0;  // 余り
  
  if ( chkzero( add2 ) == 0 ) error = 3;    // 割る数が０（＝０で割ろうとした場合）
  else if ( chkzero( add1 ) != 0 ) {        // 割られる数が０でない
    int digit  = chkdigit( add2 );  // 割る数の桁数
    int digit2 = chkdigit( add1 );  // 割られる数の桁数 = 取り出す最初の桁　→　digit2からdigit桁までが割り算の対象とするフレーム
    int subtraction[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };   // 割られる数保存配列
    int n = digit2 - digit;         // 計算対象桁

//  (割られる数)  1 2 3 4 5      (割る数)  6  digit = 1
//              ↑
//    　　　　 digit2 = 5

    if ( n < 0 ) n = 0;
    for ( int j = digit2 - 1 ; j >= n ; j-- )
      subtraction[ j - n ] = add1[ j ];
            

    if ( ls_comparison( add2, subtraction ) == 1 ){     // 割る数が、割られる数より大きい場合
      if ( n > 0 ) {
        n--;
        for ( int j = digit2 - 1 ; j >= n ; j-- )
          subtraction[ j - n ] = add1[ j ];
      } 
    }
    
    do {
         // 商を求めるため差を求めることができる回数をカウント
      int ans = 0;
      while( ls_comparison( subtraction, add2 ) != 2 ) {
        ans++;
              
        for ( int i = chkdigit( subtraction ) - 1 ; i >= 0 ; i-- ) {      // 引き算開始
          if ( subtraction[i] < add2[i] ) {    // 上の桁から借りてくる
            int nn = 0;  // 戻る桁数
            do {
              nn++;
            }while( subtraction[ i + nn ] <= 0 );
            for ( int j = nn ; j > 0 ; j-- ) {
              subtraction[ i + j ]--;
              subtraction[ i + j - 1 ] += 10;
            }
          }
          subtraction[i] -= add2[i];
        }                                                                 // 引き算終了
           
      }
      result[n] = ans;

      if ( n != 0 ) {
        n--;
        for ( int j = digit-1 ; j >= 0 ; j-- )
          subtraction[j+1] = subtraction[ j ];
        subtraction[0] = add1[n];
      }
            
    }while( ls_comparison( subtraction, add2 ) != 2 );
          
    amari = BCDtoint( subtraction );   // 余りを指数表示部に

//    for ( int i = 15 ; i <= 0 ; i-- )
//      Serial.print( subtraction[i] );
//    Serial.println();  
    

  }
        
  return( amari );
}



// 実数　割り算

void realdivision( struct stack *p, int *result ) {

  int add1[16], add2[16];

  get_num( *p, add1 );
  get_num( *bcd, add2 );

  if ( chkzero( add2 ) == 0 ) error = 3;    // 割る数が０（＝０で割ろうとした場合）
  else if ( chkzero( add1 ) != 0 ) {        // 割られる数が０でない
    (*p).decimal_point = true;

  // 桁
  int digit  = chkdigit( add2 );  // 割る数の桁数     *bcd
  int digit2 = chkdigit( add1 );  // 割られる数の桁数  *p

  // 右詰め
  while( add1[0] == 0 ){
    r_shift( add1, 1 );
  }
  while( add2[0] == 0 ){
    r_shift( add2, 1 );
  }
/*
Serial.print( "/ (*p).digit = " );
Serial.print( (*p).digit );
Serial.print( "  (*bcd).digit = " );
Serial.println( (*bcd).digit );*/
  // 桁計算
  (*p).digit -= (*bcd).digit;

//Serial.print( "/ (*p).digit = " );
//Serial.print( (*p).digit );

  digit2 = chkdigit( add1 );    // *p    割られる数
  digit  = chkdigit( add2 );    // *bcd　割る数
  int n = digit2 - digit;         // 計算対象桁

  int ls_tmp = ls_comparison( add1, add2 );
  if ( ls_tmp == 1 ){           // 割る数より割られる数の方が大きい場合のケタ処理

    int chk_tmp[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    for ( int i = digit - 1 ; i >= 0 ; i-- )            // 割る桁数分　割られる数　を取出
      chk_tmp[ digit - 1 - i ] = add1[ digit2 - 1 - i];

    if ( ls_comparison( chk_tmp, add2 ) == 2 ) (*p).digit--;    // 取り出した桁と割る数の比較

  } else if ( ls_tmp == 2 ){    // 割られる数(*p) より 割る数(*bcd) の方が大きい場合のケタ処理
    (*p).digit--;
//Serial.printf( "3 %d ", (*p).digit );

  }

  // 割る数と割られる数がmaxの16桁入っている場合は割る数の最小桁を削除する
  if ( digit == 16 && chkdigit( add1 ) == 16 && ls_comparison( add2, add1 ) == 1 ) {
    r_shift( add2, 1 );
    if ( (*p).digit == digit - 1 && (*p).digit > 0 )  (*p).digit++;
  }
/*
Serial.print( "add1 = " );
printBCD( add1 );
Serial.print( "(*p).digit = " );
Serial.print( (*p).digit );
Serial.print( "  digit = " );
Serial.println( digit2 );

Serial.print( "add2 = " );
printBCD( add2 );
Serial.print( "(*bcd).digit = " );
Serial.print( (*bcd).digit );
Serial.print( "  digit = " );
Serial.println( digit );

Serial.print( "p = " );
Serial.print( (*p).digit );
Serial.print( " bcd = " );
Serial.println( (*bcd).digit );
Serial.print( " n = " );
Serial.println( n );
*/

  // 割り算開始
  int subtraction[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };   // 割られる数保存配列
  
  // 割られる数NO桁数で、割られる数とらる数の代償比較
  // 割られる数の方が大きい場合は桁を*10
  if ( n < 0 ){
    n = 0;
    int subtraction_tmp[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    for ( int j = digit - 1 ; j >= digit2 - 1 ; j-- ){
      subtraction_tmp[ j - n ] = add2[ j ];
//      Serial.print( add2[ j ] );
    }
//    Serial.println();
    if ( ls_comparison( subtraction, add1 ) == 2 ) (*p).digit++;
  }


  for ( int j = digit2 - 1 ; j >= n ; j-- )
    subtraction[ j - n ] = add1[ j ];

    do {
         // 商を求めるため差を求めることができる回数をカウント
      int ans = 0;
      while( ls_comparison( subtraction, add2 ) != 2 ) {
        ans++;
//Serial.println( ans );
//Serial.print( " " );
//printBCD( subtraction );
//printBCD( add2 );
        for ( int i = chkdigit( subtraction ) - 1 ; i >= 0 ; i-- ) {      // 引き算開始
          if ( subtraction[i] < add2[i] ) {    // 上の桁から借りてくる
            int nn = 0;  // 戻る桁数
            do {
              nn++;
            }while( subtraction[ i + nn ] <= 0 );
            for ( int j = nn ; j > 0 ; j-- ) {
              subtraction[ i + j ]--;
              subtraction[ i + j - 1 ] += 10;
            }
          }
          subtraction[i] -= add2[i];
        }                                                                 // 引き算終了
           
      }
      result[n] = ans;
//Serial.println( ans );
      
      if ( chkzero( subtraction ) == 0 && n == 0 ) break;   // 割り切れて終了

      if ( n != 0 ) {     // 最後まで割ったか？
        n--;
        for ( int j = digit-1 ; j >= 0 ; j-- )
          subtraction[j+1] = subtraction[ j ];
        subtraction[0] = add1[n];
      } else if ( chkzero( subtraction ) != 0 ){
      
        if ( ( abs( (*p).digit ) >= 16 && result[15] == 0 ) ||      // 指数表示をするため16桁全てを使う
             ( abs( (*p).digit ) <  16 && result[14] == 0 ) ) {     // 小数点用事のため15桁でストップ
              
          l_shift( result, 1 );
          if ( subtraction[15] == 0 ) l_shift( subtraction, 1 );    // 割られる数の桁を上げる
          else                        r_shift( add2, 1 );           // 割る数の桁を下げる

        } else break;
      }

    }while( 1 );

  // 表示を整える
    // 右詰め : 0の消去
//    for ( int dif = chkdigit( result ) ; result[0] == 0 ; dif-- )
//      r_shift( result, 1 );

    // 指数と表示を合わせる
    if ( (*p).digit > 0 && (*p).digit > chkdigit( result ) - 1 ) {
      if ( (*p).digit + chkdigit( result ) - 1 < 16 )
        for ( int i = (*p).digit ; i > 0 ; i-- )
          l_shift( result, 1 );
    }

//Serial.print( (*p).digit );
//Serial.print( "\t" );
//printBCD( result );

  }
       
}


// 整数　大小比較
// 0:同じ　1:1項目が大　2:2項目が大 

int ls_comparison( int *add1, int *add2 ) {
  
  int ret = 0;
  int add1_digit = chkdigit( add1 );
  int add2_digit = chkdigit( add2 );

  if ( add1_digit == add2_digit ) {     // 同じ桁数
    
    for ( int i = add1_digit - 1 ; i >= 0 ; i-- ) {

      if        ( add1[i] > add2[i] ) {
        ret = 1;
        break;
      } else if ( add1[i] < add2[i] ) {
        ret = 2;
        break;
      }
    }

  } else if ( add1_digit > add2_digit ) ret = 1;
  else   if ( add1_digit < add2_digit ) ret = 2;
  return( ret );
}



// 実数　絶対値での大小比較  ※si配列専用
// 0:同じ　1:*pが大　2:*bcdが大 

int rels_comparison( struct stack *p ) {
  
  int ret = 0;

  if      ( (*p).digit > (*bcd).digit ) ret = 1;
  else if ( (*p).digit < (*bcd).digit ) ret = 2;
  else {  // 桁が一緒
    int add1[16], add2[16];

    get_num( *p,   add1 );
    get_num( *bcd, add2 );

    int digit1 = chkdigit( add1 );
    int digit2 = chkdigit( add2 );

    do {
      if ( add1[digit1] == add2[digit2] ) {
        digit1--;
        digit2--;
      } else {
        if ( add1[digit1] > add2[digit2] ) ret = 1;
        else ret = 2;
        break;
      }
      
    }while( digit1 > -1 && digit2 > -1 );
  }
  return( ret );
}




// BCDデータをint変数に変換する

int BCDtoint( int *bcd ) {
  int intbcd = 0;
  
  int digit = chkdigit( bcd );
  if ( digit > 9 ) error = 4;
  else
    for ( int i = 0 ; i < digit ; i++ )
      intbcd += bcd[ i ] * pow( 10, i );
  return( intbcd );
}

  
// BCDデータを unsigned long変数に変換する

unsigned long BCDtoUlong( int *bcd ) {
  unsigned long ulongbcd = 0;
  
  int digit = chkdigit( bcd );
  if ( digit > 9 ) error = 4;
  else
    for ( int i = 0 ; i < digit ; i++ )
      ulongbcd += bcd[ i ] * pow( 10, i );
  return( ulongbcd );
}


// 一文字削除

void del_char( int *bcd_str, int n ) {
  
  for ( int i = 0 ; i < n ; i++ )
    bcd_str[i] = bcd_str[i+1];
    
  n--;
  set_num( bcd, bcd_str );
    
  if ( n == 0 ) {     // 初期化
    
    (*bcd).digit = 0;
    (*bcd).code = true;               // 符号 正 = true or 負 = false
    (*bcd).decimal_point = false;     // 小数点の有無 有 = true  無 = false
    index_input = 0;
    
  } else if ( n ==  (*bcd).digit ) {
    (*bcd).digit--;
    (*bcd).decimal_point = false;
    index_input--;
  } else {
    index_input--;
  }

//Serial.println( (*bcd).digit );
}


// カレントBCDスタックをクリアーする

void clrBCD( void ) {
  (*bcd).digit = 0;                 // 指数　10^n or 10^-n
  (*bcd).numbers[0] = 0;            // BCDコードで数字を保持　2バイト * 4  = 16桁
  (*bcd).numbers[1] = 0;            // BCDコードで数字を保持　2バイト * 4  = 16桁
  (*bcd).numbers[2] = 0;            // BCDコードで数字を保持　2バイト * 4  = 16桁
  (*bcd).numbers[3] = 0;            // BCDコードで数字を保持　2バイト * 4  = 16桁
  (*bcd).code = true;               // 符号 正 = true or 負 = false
  (*bcd).decimal_point = false;     // 小数点の有無 有 = true  無 = false
}

//==============================================
// デバッグ用　BCDデータ表示

void printBCD( int *dat ) {
  for ( int i = 15 ; i >= 0 ; i-- )
    Serial.print( dat[i] );
  Serial.println("  ");
}


// デバッグ用　スタック表示

void printStack( void ) {
  int dat[16];
  struct stack *pp = bcd;
  int n = 0;


  do {

    get_num( *pp, dat );
    Serial.printf( "%d %p %p %d ", n, pp, (*pp).next, (*pp).digit );
    for ( int i = 15 ; i >= 0 ; i-- )
      Serial.print( dat[i] );
    Serial.printf( " %s %s\n", ((*pp).code ? "+" : "-"), ((*pp).decimal_point ? "." : "=") );

    if ( (*pp).next == (struct stack *)NULL ) break;
    
    pp = (*pp).next;
    n++;
    delay(10);
    
  }while( 1 );
  Serial.println();

}
