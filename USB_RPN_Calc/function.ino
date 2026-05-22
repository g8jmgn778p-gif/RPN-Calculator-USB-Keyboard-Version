
// ****************** 文字列読み込み *******************
String str_read(File fp, char seper ) {
  
  char temp = (char)NULL;
  String data_string = "";

  while ( temp != seper ) {
    data_string += (String)temp;
    temp = fp.read();
//    Serial.printf("%02x %c ", temp, temp );
  }

  int indNo = data_string.lastIndexOf( (String)'\r' );    // 改行コードが CR LF の場合の CR(\r)を取り除く
//  Serial.print(indNo);
  if ( indNo != -1 ) {
    data_string.remove(indNo, 1 ); 
  }
//  Serial.print(data_string);
//  Serial.println(data_string.length());
  return( data_string );
}

// ****************** double 列読み込み *******************
double double_read(File fp, char seper ) {
  
  char temp;
  String data_string = "";

  temp = fp.read();
  while ( temp != seper ) {
    data_string += (String)temp;
    temp = fp.read();
//    Serial.print( temp );
  }
//  Serial.println(data_string);
  return( data_string.toDouble() );
}

// ****************** int 列読み込み *******************
int int_read(File fp, char seper ) {
  
  char temp;
  String data_string = "";

  temp = fp.read();
  while ( temp != seper ) {
    data_string += (String)temp;
    temp = fp.read();
//    Serial.print( temp );
  }
//  Serial.println(data_string);
  return( data_string.toInt() );
  
}
