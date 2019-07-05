model one {

struct Topic {};

interface Page {
  sequence<Topic> topics(int offset, int limit);
};
  
};
