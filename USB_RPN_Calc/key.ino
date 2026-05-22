/* ***************  キースキャンタスク  *****************

21           : キーの数
62 0 0 0 i 0 : 0
59 0 1 1 m m : 1 m:x<->y メモリの入れ替え
5A 0 2 2 2 2 : 2
5B 0 3 3 3 3 : 3
5C 0 4 4 4 4 : 4
5D 0 5 5 5 5 : 5
5E 0 6 6 6 6 : 6
5F 0 7 7 S 7 : 7    S:asin
60 0 8 8 A 8 : 8    A:acos
61 0 9 9 T 9 : 9    T:atan
57 0 + + + + : +
56 0 - - - - : -
55 0 * * * * : *
54 0 / / / / : /
58 0 E E E E : ENTER
59 4 C C C C : F1  C:1文字消去  '='
2B 0 H s S H : F2  H:16進数表示 s:sin       'Tab'
29 0 O c A O : F3  O:8進数表示  c:cos         'Esc'
2A 0 B t T B : F4  B:2進数表示  t:tan         'BackSpace'
63 0 . . r . : F5  r:+/-      '.'
5E 4         : "="キーは 5E 4  59 4 が連続するため 5E 4 を無視する


53 0 F F F : shift      Num Lock + どれかのキー
2A   R R R : Reset

           ↑ 整数演算 + shift モード
         ↑ 実数演算 + shift モード
       ↑ 実数演算モード
     ↑ 整数演算モード
   ↑ mod
 ↑ keyコード



   46 47 48
53 54 55 56
5F 60 61 
5C 5D 5E 57
59 5A 5B 
62    63 58

4B

*/


int  key_num = 0;               // キーの数
int  **code = (int**)NULL;      // キーコード
char **key_ch = (char**)NULL;   // キーマップ


// キーマトリクス　読み込み *******************

void key_code( File fp ){
  
  Serial.println( "キーマップの読み込み" );
  String code_str = str_read(fp, ' ');
  key_num = code_str.toInt();
  Serial.print( key_num );
  String dumy = str_read( fp, '\n' );
  Serial.println( dumy );

  code = (int**)malloc(sizeof(int*) * key_num);  // キーコードのメモリを確保
  for ( int i = 0 ; i < key_num ; i++ ){
    code[i] = (int*)malloc( sizeof(int) * 2);
  }
  Serial.println( "キーコードのメモリを確保" );

  key_ch = (char**)malloc(sizeof(char*) * key_num );    // キーキャラクタの行分のメモリを確保
  for ( int i = 0 ; i < key_num ; i++ ){
    key_ch[i] = (char*)malloc( sizeof(char) * 4 );
  }
  Serial.println( "キーキャラクタのメモリを確保" );

  for ( int i = 0 ; i < key_num ; i++ ){
    Serial.print( i );
    Serial.print(" ");
    
    char buf[5];
    code_str = str_read(fp, ' ');
//    Serial.print( code_str );
//    Serial.print(" ");
    code_str.toCharArray( buf, 3 );     // キーコードの読み込み
//    Serial.print( buf );
//    Serial.print(" ");
    code[i][0] = strtol(buf,NULL,16);   // 16進数文字列＝コードから数値へ変換
    Serial.print( code[i][0], HEX );
    Serial.print(" ");

    code_str = str_read(fp, ' ');       // キーモードの読み込み
    code[i][1] = code_str.toInt();
    Serial.print( code[i][1] );
    Serial.print(" ");
    
    for ( int j = 0 ; j < 4 ; j++ ){
      key_ch[i][j] = fp.read();         // キーマップの読み込み
      Serial.print( key_ch[i][j] );
      char tmp = fp.read();                  // ' 'の読み込み
      Serial.print( tmp );

    }
    dumy = str_read( fp, '\n' );        // キーマップの読み込み
    Serial.println( dumy );

  }
  Serial.println();

  for ( int i = 0 ; i < key_num ; i++ ){
    for ( int j = 0 ; j < 2 ; j++ ){
      Serial.print( code[i][j], HEX );
      Serial.print(" ");
    }
    for ( int j = 0 ; j < 4 ; j++ ){
      Serial.print( key_ch[i][j] );
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println();
}




// キーコードの変換 **********************
// Num Lock => 0x53 + (各コード) + 0x53

int num_on = 0;     // numロック on 時の 53 のカウンター

char key_cov( uint8_t mod, uint8_t key ){

  if ( mod == 0 && key == 0x53 ){     // num キー対応
    num_on ^= 1;
    return(0);
  }

  
  for ( int i = 0 ; i < key_num ; i++ ){

  // 整数 num off ->  0 0 = 0
  // 整数 num on  ->  0 1 = 1
  // 実数 num off ->  1 0 = 2
  // 実数 num on  ->  1 1 = 3
  
    if ( code[i][0] == key && code[i][1] == mod ){      // 整数:0  整数+num:1  実数:2  実数+num:3
      int cal_mode_tmp = cal_mode << 1;
      char ret_char = key_ch[i][cal_mode_tmp + num_on ];
      if ( ret_char == '.' && cal_mode == 0 ) cal_mode = 1;  // 実数モード
//      Serial.print( " " );
//      Serial.print( cal_mode );
//      Serial.print( " " );
//      Serial.print( cal_mode_tmp );
//      Serial.print( " " );
//      Serial.print( num_on );
//      Serial.print( " " );
//      Serial.println( ret_char );
      return( ret_char );
    }

  }

  Serial.print("key_conv ERROR!! key = ");
  Serial.print( key, HEX );
  Serial.print(" mod = " );
  Serial.println( mod );
  return(1);
}



// キーマップの定義 *******************

void key_def( void ){
  
  Serial.println( "キーマップの定義" );
  key_num = 18;
  Serial.println( key_num );

                                              // 整数
                                              //  ↓ 整数+num
                                              //  ↓ ↓ 実数
                                              //  ↓ ↓ ↓ 実数+num
                                              //  ↓ ↓ ↓ ↓
  int code_tmp[key_num][2] = { { 0x62, 0 },   //  0 i 0 i : 0    i:*10
                               { 0x59, 0 },   //  1 m 1 m : 1    x<->y メモリの入れ替え
                               { 0x5A, 0 },   //  2   2   : 2
                               { 0x5B, 0 },   //  3   3   : 3
                               { 0x5C, 0 },   //  4   4 S : 4    S:aSin
                               { 0x5D, 0 },   //  5   5 A : 5    A:aCos
                               { 0x5E, 0 },   //  6   6 T : 6    T:aTan
                               { 0x5F, 0 },   //  7 H 7 s : 7    H:16進数表示 s:sin
                               { 0x60, 0 },   //  8 O 8 c : 8    O:8進数表示  c:cos
                               { 0x61, 0 },   //  9 B 9 t : 9    B:2進数表示  t:tan
                               { 0x57, 0 },   //  +   +   : +
                               { 0x56, 0 },   //  -   -   : -
                               { 0x55, 0 },   //  *   *   : *
                               { 0x54, 0 },   //  /   /   : /
                               { 0x58, 0 },   //  E   E   : ENTER
                               { 0x2A, 0 },   //  C   C   : C:1文字消去           // Back Space
                               { 0x63, 0 },   //  .   .   : '.':小数点
                               { 0x2B, 0 } }; //  r   r   : +/-                  // Tab

  char key_ch_tmp[key_num][4] = { { '0', 'i', '0', 'i' },
                                  { '1', 'm', '1', 'm' },
                                  { '2', ' ', '2', ' ' },
                                  { '3', ' ', '3', ' ' },
                                  { '4', ' ', '4', 'S' },
                                  { '5', ' ', '5', 'A' },
                                  { '6', ' ', '6', 'T' },
                                  { '7', 'H', '7', 's' },
                                  { '8', 'O', '8', 'c' },
                                  { '9', 'B', '9', 't' },
                                  { '+', ' ', '+', ' ' },
                                  { '-', ' ', '-', ' ' },
                                  { '*', ' ', '*', ' ' },
                                  { '/', ' ', '/', ' ' },
                                  { 'E', ' ', 'E', ' ' },
                                  { 'C', ' ', 'C', ' ' },
                                  { '.', ' ', '.', ' ' },
                                  { 'r', ' ', 'r', ' ' } };

  code = (int**)malloc(sizeof(int*) * key_num);  // キーコードのメモリを確保
  for ( int i = 0 ; i < key_num ; i++ ){
    code[i] = (int*)malloc( sizeof(int) * 2);
  }
  Serial.println( "キーコードのメモリを確保" );

  key_ch = (char**)malloc(sizeof(char*) * key_num );    // キーキャラクタの行分のメモリを確保
  for ( int i = 0 ; i < key_num ; i++ ){
    key_ch[i] = (char*)malloc( sizeof(char) * 4 );
  }
  Serial.println( "キーキャラクタのメモリを確保" );

  for ( int i = 0 ; i < key_num ; i++ ){
    
    code[i][0] = code_tmp[i][0];
    code[i][1] = code_tmp[i][1];
    
    for ( int j = 0 ; j < 4 ; j++ )
      key_ch[i][j] = key_ch_tmp[i][j];

  }
  
  for ( int i = 0 ; i < key_num ; i++ ){
    for ( int j = 0 ; j < 2 ; j++ ){
      Serial.print( code[i][j], HEX );
      Serial.print(" ");
    }
    for ( int j = 0 ; j < 4 ; j++ ){
      Serial.print( key_ch[i][j] );
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println();
}
