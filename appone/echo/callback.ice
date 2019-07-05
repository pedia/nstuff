model one {

interface Sinker {
  void sink();
}

interface Callback {
  void listen(Sinker* o);

  void sinkLater(int seconds);

  void sinkNow();
};
  
};
