module one {

interface Sinker {
  void sink();
}

interface Callback {
  void listen(Sinker* o);
  void unlisten(Sinker* o);

  void sinkLater(int seconds);

  void sinkNow();
};
  
};
