/*

s = s1 + or - s2 + or - s3 ....

xi
x1 = 1
x2 = x1 + or - y1 / 2^2
x3 = x2 + or - y2 / 2^3
x4 = ...

y1 = 1
y2 = y1 + or - x1 / 2^2
y3 = y2 + or - x2 / 2^3
y4 =  ....

sin s = y / r
cos s = x / r
tan s = y / x

*/

void CORDIC( int func ) {

  int si[][16] = {  { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,4 },
                    { 0,0,8,7,0,7,7,1,1,5,0,5,6,5,6,2 },
                    { 0,5,6,2,9,7,6,4,3,4,2,6,3,0,4,1 },
                    { 0,8,1,0,9,8,4,3,6,1,0,5,2,1,7,0 },
                    { 6,3,7,9,9,4,7,3,4,3,3,6,7,5,3,0 },
                    { 7,0,6,4,2,8,0,6,0,1,9,9,8,7,1,0 },
                    { 8,0,1,1,2,0,1,7,3,7,1,5,9,8,0,0 },
                    { 5,5,0,6,8,0,7,1,4,1,6,7,4,4,0,0 },
                    { 4,5,8,6,3,0,0,5,0,1,8,3,2,2,0,0 },
                    { 1,2,6,6,0,7,7,6,5,0,9,1,1,1,0,0 },
                    { 0,8,3,9,8,1,9,8,2,5,9,5,5,0,0,0 },
                    { 0,0,7,1,6,2,5,4,6,7,9,7,2,0,0,0 },
                    { 7,2,2,4,1,7,2,2,8,8,9,3,1,0,0,0 },
                    { 5,3,5,7,6,3,1,1,4,9,9,6,0,0,0,0 },
                    { 0,7,0,5,8,6,5,0,7,9,4,3,0,0,0,0 },
                    { 8,9,6,2,4,8,2,5,8,4,7,1,0,0,0,0 },
                    { 9,6,3,1,2,4,6,2,4,7,8,0,0,0,0,0 },
                    { 7,8,6,0,1,2,3,1,7,3,4,0,0,0,0,0 },
                    { 4,4,3,5,0,6,6,5,8,1,2,0,0,0,0,0 },
                    { 2,7,6,2,0,3,8,2,9,0,1,0,0,0,0,0 },
                    { 6,3,3,1,5,1,4,6,4,5,0,0,0,0,0,0 },
                    { 8,6,6,5,7,0,2,3,7,2,0,0,0,0,0,0 },
                    { 4,3,8,7,3,0,6,6,3,1,0,0,0,0,0,0 },
                    { 7,1,9,8,1,0,3,8,6,0,0,0,0,0,0,0 },
                    { 9,5,4,9,0,5,1,4,3,0,0,0,0,0,0,0 },
                    { 9,2,7,4,5,7,0,7,1,0,0,0,0,0,0,0 },
                    { 5,6,3,7,7,3,5,8,0,0,0,0,0,0,0,0 },
                    { 2,8,6,8,8,6,2,4,0,0,0,0,0,0,0,0 },
                    { 1,4,3,4,4,3,1,2,0,0,0,0,0,0,0,0 },
                    { 1,7,1,2,7,6,0,1,0,0,0,0,0,0,0,0 },
                    { 5,8,0,6,3,3,5,0,0,0,0,0,0,0,0,0 },
                    { 3,4,0,8,6,6,2,0,0,0,0,0,0,0,0,0 },
                    { 1,2,0,4,3,3,1,0,0,0,0,0,0,0,0,0 },
                    { 1,1,0,7,6,6,0,0,0,0,0,0,0,0,0,0 },
                    { 5,0,5,3,3,3,0,0,0,0,0,0,0,0,0,0 },
                    { 3,5,7,6,6,1,0,0,0,0,0,0,0,0,0,0 },
                    { 6,7,3,3,8,0,0,0,0,0,0,0,0,0,0,0 },
                    { 8,8,6,1,4,0,0,0,0,0,0,0,0,0,0,0 },
                    { 4,4,8,0,2,0,0,0,0,0,0,0,0,0,0,0 },
                    { 2,2,4,0,1,0,0,0,0,0,0,0,0,0,0,0 },
                    { 1,1,2,5,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 6,0,6,2,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 3,0,3,1,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 1,5,6,0,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 6,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 3,6,1,0,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 1,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
                    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } };

  struct stack *r     = new stack { (struct stack *)NULL, 0, 8455, 9601, 26464, 356, true, true };  // スタックを初期化 r = 1.64676025812107
  struct stack *two   = new stack { (struct stack *)NULL, 0, 2, 0, 0, 0, true, true };
  struct stack *njyou = new stack { (struct stack *)NULL, 0, 2, 0, 0, 0, true, true };
  struct stack *xi0   = new stack { (struct stack *)NULL, 0, 1, 0, 0, 0, true, true };
  struct stack *xi1   = new stack { (struct stack *)NULL, 0, 1, 0, 0, 0, true, true };
  struct stack *yi0   = new stack { (struct stack *)NULL, 0, 1, 0, 0, 0, true, true };
  struct stack *yi1   = new stack { (struct stack *)NULL, 0, 1, 0, 0, 0, true, true };
  struct stack *d90   = new stack { (struct stack *)NULL, 1, 0, 0, 0, 0, true, true };

  int st[16];                 // 求めたい角度へ近づける
  boolean st_code = true;     //  true:正  false:負
  int ss[16];                 // 求めたい角度
  boolean bcd_code = (*bcd).code;

  struct stack *temp_bcd = bcd;   // BCDのポインタを保存
  get_num( *bcd, ss );

  // どの象限かを調べる
  int syougen = 1;
  int d_90[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0 };
  set_num( d90, d_90 );
  bcd = d90;
  while ( rels_comparison( temp_bcd ) == 1 ) {
    realdel( temp_bcd, d_90 );
    syougen++;
  };
  bcd = temp_bcd;
  if ( func == 3 && rels_comparison( d90 ) == 0 ) error = 6;   // tan(90度) = 無限大エラー
  free( d90 );   // スタックの開放
  while( syougen > 4 ) { syougen -= 4; }    // 5以上の場合は4以下にする
/*
  Serial.print( "象限 = " );
  Serial.println( syougen );
printStack();
*/
  get_num( *bcd, ss );
  shift_zero( ss, (*bcd).digit );   // 15カラム目に１の桁が来るようシフト

  for ( int i = 0 ; i < 16 ; i++ )
    st[i] = si[0][i];

  int hantei = 0;
  if ( st_code == false ) hantei = 1;  // stがマイナスか？
  else hantei = ls_comparison( ss, st );     // 大小判定

  for ( int i = 1 ; i <= 54 ; i++ ) {
    int result_xi[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    int result_yi[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    int add1[16], add2[16];

    bcd = two;
    if ( i > 1 ) real_multiplication( njyou );   // 2^i

int temp_bcd[16];
    bcd = njyou;
    realdivision( yi0, result_xi ); // 割り算　＊＊＊＊＊＊＊＊ xiの (yi0 / 2^i)部
    realdivision( xi0, result_yi ); // 割り算　＊＊＊＊＊＊＊＊ yiの (xi0 / 2^i)部
    set_num( yi0, result_xi );
    set_num( xi0, result_yi );

//***********************

    if ( hantei <=1 ) {
      st_code = intdel2( st_code, st, false, si[i] );
      if ( (*yi0).code == true ) (*yi0).code = false; else (*yi0).code = true;
    } else {
      st_code = intdel2( st_code, st, true,  si[i] );
      if ( (*xi0).code == true ) (*xi0).code = false; else (*xi0).code = true;
    }
/*
Serial.print( i );
Serial.print( "  ST " );
Serial.printf( "%s", (st_code ? "+" : "-") );
printBCD( st );
*/
//  xi
    get_num( *xi1, add1 );
    get_num( *yi0, add2 );
    bcd = yi0;
/*
Serial.print( (*xi1).digit );
Serial.printf( " %s", ((*xi1).code ? "+" : "-") );
printBCD( add1 );
Serial.print( (*bcd).digit );
Serial.printf( " %s", ((*bcd).code ? "+" : "-") );
printBCD( add2 );
*/
    if ( (*xi1).code == (*bcd).code ) {   // (+) + (+)  or  (-) + (-)
       (*xi1).digit = realadd( add1, (*xi1).digit, add2, (*bcd).digit );   // 実数加算
      set_num( xi1, add1 );
    } else {                              // (+) + (-)  or  (-) + (+)
       realdel( xi1, add2 );                   // 実数減算
       get_num( *xi1, add1 );
    }
/*
Serial.print( "xi " );
Serial.printf( "%s", ((*xi1).code ? "+" : "-") );
printBCD( add1 );
*/
//  yi
    get_num( *yi1, add1 );
    get_num( *xi0, add2 );
    bcd = xi0;

    if ( (*yi1).code == (*bcd).code ) {   // (+) + (+)  or  (-) + (-)
       (*yi1).digit = realadd( add1, (*yi1).digit, add2, (*bcd).digit );   // 実数加算
      set_num( yi1, add1 );
    } else {                              // (+) + (-)  or  (-) + (+)
       realdel( yi1, add2 );                   // 実数減算
       get_num( *yi1, add1 );
    }
/*
Serial.print( "yi " );
Serial.printf( "%s", ((*yi1).code ? "+" : "-") );
printBCD( add1 );
Serial.println();
*/
    // xi0 = xi1, yi0 = yi1
    (*xi0).code       = (*xi1).code;         // 符号 正 = true or 負 = false
    (*xi0).digit      = (*xi1).digit;        // 指数　10^n or 10^-n
    (*xi0).numbers[0] = (*xi1).numbers[0];   // BCDコードで数字を保持　2バイト * 4  = 16桁
    (*xi0).numbers[1] = (*xi1).numbers[1];   // BCDコードで数字を保持　2バイト * 4  = 16桁
    (*xi0).numbers[2] = (*xi1).numbers[2];   // BCDコードで数字を保持　2バイト * 4  = 16桁
    (*xi0).numbers[3] = (*xi1).numbers[3];   // BCDコードで数字を保持　2バイト * 4  = 16桁
    (*yi0).code       = (*yi1).code;         // 符号 正 = true or 負 = false
    (*yi0).digit      = (*yi1).digit;        // 指数　10^n or 10^-n
    (*yi0).numbers[0] = (*yi1).numbers[0];   // BCDコードで数字を保持　2バイト * 4  = 16桁
    (*yi0).numbers[1] = (*yi1).numbers[1];   // BCDコードで数字を保持　2バイト * 4  = 16桁
    (*yi0).numbers[2] = (*yi1).numbers[2];   // BCDコードで数字を保持　2バイト * 4  = 16桁
    (*yi0).numbers[3] = (*yi1).numbers[3];   // BCDコードで数字を保持　2バイト * 4  = 16桁

  if ( st_code == false ) hantei = 1;  // stがマイナスか？
  else hantei = ls_comparison( ss, st );     // 大小判定

  }

  int result_s[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
  int result_c[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
  int result_t[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

//  struct stack *p_cos = new stack { (struct stack *)NULL, 0, 0, 0, 0, 0, true, true };
//  struct stack *p_tan = new stack { (struct stack *)NULL, 0, 0, 0, 0, 0, true, true };
/*
bcd = xi1;
(*bcd).next = yi1;
printStack();
*/

  bcd = r;
  realdivision( yi1, result_s );
  set_num( yi1, result_s );
  realdivision( xi1, result_c );
  set_num( xi1, result_c );
  bcd = xi0;
  realdivision( yi0, result_t );
  set_num( yi0, result_t );
    
  Serial.print( "  sin = " );
  printBCD( result_s );
  Serial.print( "  cos = " );
  printBCD( result_c );
  Serial.print( "  tan = " );
  printBCD( result_t );
  Serial.println();
  Serial.println();

  (*bcd).code = true;
  switch ( func ){
    case 1:   // sin
      bcd = yi1;     // ポインタをもとに戻す。
      (*bcd).next = (*temp_bcd).next;
      if      ( bcd_code == true  && ( syougen == 3 || syougen == 4 ) ) (*bcd).code = false;
      else if ( bcd_code == false && ( syougen == 1 || syougen == 2 ) ) (*bcd).code = false;
      free( xi1 );   // スタックの開放
      free( yi0 );   // スタックの開放
      break;
    case 2:   // cos
      bcd = xi1;     // ポインタをもとに戻す。
      (*bcd).next = (*temp_bcd).next;
      if ( syougen == 2 || syougen == 3 ) (*bcd).code = false;
      free( yi0 );   // スタックの開放
      free( yi1 );   // スタックの開放
      break;
    case 3:   // tan
      bcd = yi0;     // ポインタをもとに戻す。
      (*bcd).next = (*temp_bcd).next;
      if      ( bcd_code == true  && ( syougen == 2 || syougen == 4 ) ) (*bcd).code = false;
      else if ( bcd_code == false && ( syougen == 1 || syougen == 3 ) ) (*bcd).code = false;
      free( xi1 );   // スタックの開放
      free( yi1 );   // スタックの開放
  }

  free( xi0 );   // スタックの開放
  free(  r  );   // スタックの開放
  free( two );   // スタックの開放
  free( njyou ); // スタックの開放
  free( temp_bcd ); // スタックの開放
 
}



// si配列をスタックにセットする

void set_sid( struct stack *sid, int *si ) {
  set_num( sid, si );
  int n = chkdigit( si );
  if ( n == 16 ) (*sid).digit = 1;
//  else if ( n == 15 ) (*sid).digit = 0;
  else if ( n <= 14 ) (*sid).digit = 13 - n;
}




// 15カラム目に１の桁が来るようシフト
void shift_zero( int *add1, int digit ) {

  int add1_digit = chkdigit( add1 );
  
  if ( digit == 1 ) l_shift( add1, 16 - add1_digit );
  else if ( digit == 0 && add1_digit == 16 ) r_shift( add1, 1 );
  else if ( digit == 0 && add1_digit <= 14 ) l_shift( add1, 15 - add1_digit );
  else if ( digit <  0 && add1_digit >= 15 ) r_shift( add1, add1_digit - 15 -digit );
  else l_shift( add1, add1_digit - 15 - digit );
  
}
