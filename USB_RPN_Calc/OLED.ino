// ***************  表示タスク  *****************

void Task_OLED(void *pvParameters)
{
  (void) pvParameters;
  
  display.init();
  display2.init();

  display.setI2cAutoInit(true);
  display2.setI2cAutoInit(true);

  display.flipScreenVertically();
  display2.flipScreenVertically();

  const int oled_width  = display.getWidth();
  const int oled_height = display.getHeight() - 1;
  const int print_bcd   = oled_height - 24;

  String errMsg[] = { "Overflow",
                      "Lack of memory",
                      "Divide by 0",
                      "Remaining overflow",
                      "Out of memory",
                      "tan(90) = Infinity",
                      "Stack underflow"     };

  Serial.println( "表示 タスク起動" );
  for (;;) // A Task shall never return or exit.
  {

    String dsp2_num;

    if ( dsp_start == 1 ) {

      display.clear();
      display2.clear();
      
      display.drawLine(0, 15, oled_width, 15);
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(oled_width, 0, print_key);
/*      if ( shift != 0 ) {
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(0, 0, " [ Shift ]" );
      }
*/
      display2.drawLine(0, 15, oled_width, 15);
      String print_mode = "Integer mode";
      if ( cal_mode == 1 ) print_mode = "Real number mode";
      display2.setFont(ArialMT_Plain_10);
      display2.setTextAlignment(TEXT_ALIGN_LEFT);
      display2.drawString(0, 0, print_mode);
      
      if ( error != 0 ) {
        String msg = "ERR:" + errMsg[ --error ];
        display2.setTextAlignment(TEXT_ALIGN_LEFT);
        display2.drawString(0, 16, msg );
        error = 0;
      }
      
      String number_str;
     
      number_str = printBCDstr( *bcd );
      int n_str = number_str.length();

      if ( 9 < n_str ) {    // BCDデータを2画面に分割
        dsp2_num   = number_str.substring( 0,n_str - 10 );
        number_str = number_str.substring( n_str - 10 );
      }

      display.setFont(ArialMT_Plain_24);
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(oled_width, print_bcd, number_str );
      
      display2.setFont(ArialMT_Plain_24);
      display2.setTextAlignment(TEXT_ALIGN_RIGHT);
      display2.drawString(oled_width, print_bcd, dsp2_num);

      String e_num = "";  
      if ( cal_mode == 0 ) {
        if ( (*bcd).digit != 0 && in_key == '/' ){
          e_num = "( " + String( (*bcd).digit ) + " )"; // 余り表示
          (*bcd).digit = 0;
        }
      } else {
        int num[16];
        get_num( *bcd, num );
        if ( (*bcd).digit != 0 ){
          if ( chkdigit( num ) - (*bcd).digit <= 17 && (*bcd).digit < 16 )
            e_num = "[E " + String( (*bcd).digit ) + "]";     // 指数 e^n　の参考表示
          else
            e_num = "E " + String( (*bcd).digit );            // 指数 e^n
        }
      }

      display.setFont(ArialMT_Plain_16);
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(oled_width, oled_height - 24 - 19, e_num);

      display.display();
      display2.display();

      dsp_start = 0;      // 画面書き換え完了

    } else delay( 100 );

  }
}




// ********* BCDデータの表示関数 ***************

String printBCDstr( struct stack code ) {
  unsigned int num[16];
  String bcd_num;

  if ( base != 10 ) {
       
    int bcd_str[16];
    get_num( *bcd, bcd_str );
    unsigned long num =  BCDtoUlong( bcd_str );

    char num_char[17];
    if ( base == 16 )                   // 16進数表示
      sprintf( num_char, "%lx", num );
    else if ( base == 8 )               // 8進数表示
      sprintf( num_char, "%lo", num );
    else {                              // 2進数表示
      num_char[16] = (char)NULL;
      unsigned int mask = 0b0000000000000001;
      for ( int i = 15 ; i >= 0 ; i-- ) {
        num_char[i] = putchar(mask & num ? '1' : '0');
        mask = mask << 1;
      }
    }
    
    bcd_num = String( num_char );
    
  } else {    // 10進数表示

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

    // 表示する数列を作成
    for ( int i = 15 ; i >= 0 ; i-- )
      bcd_num += String( num[i] );

    // 先頭の０をスペースに
    for ( int i = 0 ; i <= bcd_num.length() - 2 ; i++ )
      if ( bcd_num.charAt( i ) == '0' ) 
        bcd_num.setCharAt( i, ' ' );
      else 
        break;

    // 右の０をスペースに
    for ( int i = 0 ; i < bcd_num.length() - ( code.digit + 1 ) ; i++ )
      if ( bcd_num.lastIndexOf( '0' ) == bcd_num.length() - i )
        bcd_num.setCharAt( bcd_num.length() + i, ' ' );
      else 
        break;

    bcd_num.trim();   // 前後のスペースを取り除く

    if ( cal_mode == 1 ) {   // 実数演算モードの小数点表示
      if ( code.digit < 0 ) {  // 10^-n の表示

        if ( bcd_num.length() - code.digit <= 17 ) {
          String bcd_num_temp = "0.";
          for ( int i = -1 ; i > code.digit ; i-- )
            bcd_num_temp += "0";
          bcd_num = bcd_num_temp + bcd_num;
        } else
          bcd_num = bcd_num.substring(0,1) + "." + bcd_num.substring(1);
        
      } else if ( code.decimal_point == true && code.digit == 0 ) {
        bcd_num = bcd_num.substring(0,1) + "." + bcd_num.substring(1);
      } else if ( code.decimal_point == true && code.digit != 0 ) {
        int len = bcd_num.length();
        if ( len == code.digit + 1 )        bcd_num += ".";
        else                                bcd_num = bcd_num.substring( 0,code.digit + 1 ) + "." + bcd_num.substring(code.digit + 1);
      } else if ( abs(code.digit) >= 16 )   bcd_num = bcd_num.substring(0,1) + "." + bcd_num.substring(1);  // 実数モードで.を入力せずに16桁以上になるときの表示

    } else if ( bcd_num.length() > 3 ){      // 整数演算モードの３桁区切り表示
      String bcd_num_tmp;
      int len = bcd_num.length();
      
      for ( int i = 0 ; i < len - len%3 ; i = i + 3 ){
        bcd_num_tmp = bcd_num.substring( len-3-i, len-i ) + bcd_num_tmp;
        if ( len != i+3 ) bcd_num_tmp = "," + bcd_num_tmp;
      }
      bcd_num = bcd_num.substring( 0, len%3 ) + bcd_num_tmp;
    }

    if ( code.code == false ) bcd_num = "-" + bcd_num;    // 符号の付与

  }

//  Serial.print(" ");
//  Serial.println( bcd_num );
  return( bcd_num );

}
