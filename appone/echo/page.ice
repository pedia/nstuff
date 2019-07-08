module one {

struct Topic {
  int id;
};

sequence<Topic> Topics;

interface Page {
  Topics fetch(int offset, int limit);
};
  
}
