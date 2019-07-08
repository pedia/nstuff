module one {

// custom struct for input/output
struct Request {
  int val;
};
struct Response {
  int val;
};

exception Error {};

interface Echo {
  //
  void echoVoid();

  //
  idempotent void echoVoid2();

  //
  int echoInt(int i);

  int echoInt2(int i, out int o);

  // 
  // operation `echo' differs only in capitalization from enclosing interface name `Echo'
  // string echo(string i);

  string echoString(string i);

  // 
  Response echoStruct(Request i);

  // raise Error
  void raise();
};
  
};
