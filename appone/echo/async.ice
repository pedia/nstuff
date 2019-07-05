model one {

interface Sinker {
  void sink();
}

interface Async {
  void listen(Sinker* o);

  void sinkLater(int seconds);

  void sinkNow();
};
  
};
