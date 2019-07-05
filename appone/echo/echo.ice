model one {

// custom struct for input/output
struct Request {};
struct Response {};

exception Error {};

interface Echo {
  //
  void echoVoid();

  //
  idempotent void echoVoid2();

  //
  int echoInt(int i);

  // 
  string echoString(string i);

  // 
  Response echo(Request i);

  // raise Error
  void raise();
};
  
};
