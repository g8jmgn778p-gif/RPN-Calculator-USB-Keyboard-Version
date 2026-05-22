// 積
void multiplication( int add1[16], int add2[16] );

// 実数　積　関数
void real_multiplication( struct stack *p );

// 桁数チェック関数
int chkdigit( int *dat );

// BCDデータが０か確認する関数
// 0の場合:0  0でない場合:1
int chkzero( int *dat );

// 整数加算
void intadd( int *add1, int *add2 );

// 実数加算
int realadd( int *add1, int digit1, int *add2, int digit2 );

// 右シフト
void r_shift( int *add2, int n );

// 左シフト
void l_shift( int *add2, int n );

// 整数減算2
boolean intdel2( boolean code1, int *add1, boolean code2, int *add2 );
// 整数減算
boolean intdel( boolean code1, int *add1, boolean code2, int *add2 );

// 実数減算
void realdel( struct stack *p, int *add2 );

// int配列をBCDコードに入れる関数
void set_num( struct stack *code, int *num );

// BCDコードをint配列に入れる関数
void get_num( struct stack code, int *num );

// 整数　割り算
int division( int *add1, int *add2, int *result );

// 実数　割り算
void realdivision( struct stack *p, int *result );

// 整数　大小比較
// 0:同じ　1:1項目が大　2:2項目が大 
int ls_comparison( int *add1, int *add2 );

// 実数　絶対値での大小比較  ※si配列専用
// 0:同じ　1:*pが大　2:*bcdが大 
int rels_comparison( struct stack *p );

// BCDデータをint変数に変換する
int BCDtoint( int *bcd );

// BCDデータを unsigned long変数に変換する
unsigned long BCDtoUlong( int *bcd );

// 一文字削除
void del_char( int *bcd_str, int n );

// 構造体の初期化
void clrBCD( void );

//---------------------------------
// デバッグ用　BCDデータ表示
void printBCD( int *dat );

// デバッグ用　スタック表示
void printStack( void );
