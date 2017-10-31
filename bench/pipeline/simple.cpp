// The simple producer-consumer pipeline described in "Pipelining with Futures"

#define DEFAULT_SIZE 1024
typedef int data_t;

struct node {
  node* next;
  node* prev;
  future<data_t>* data;

  node(future<data_t>* f) : data(f) {}
  future<data_t>* head() const { return data; }
  node* rest() const { return next; }
  
  static node* cons(node* head, future<data_t>& f) {
    node* n = new node(f);
    head->prev->next = n;
    
    n->next = head;
    n->prev = head->prev;

    head->prev = n;

    return head;
  }
};

node* produce(data_t n, future<data_t>* self) {
  if (n < 0) return nullptr;

  // This isn't great for eager (work-first) futures, since there's
  // not much work after the async. 
  future<data_t> f = cilk::async produce(n-1);
  return cons(n, &f);

  // Rather than implement lazy (help-first) futures, here it's okay
  // to not parallelize the continuation, and put it before the future
  // is "launched".
  // future<data_t> f = future<data_t>();
  // cons(n, &f);
  // self->put(n);
  // produce(n-1, &f);
  // Use "cilk::async f;" if we want the continuation to execute in parallel.
  // Basically, "cilk::async" should allow 'spawning' already
  // constructed futures.
}

data_t consume(data_t sum, future<data_t>* f) {
  node* n = new node(f);
  return consume(sum, n);
}

data_t consume(data_t sum, node* n) {
  if (n == nullptr) return sum;
  return consume(sum + n->head()->get(), n->rest());
}

int main(int argc, char* argv[]) {
  data_t n = (argc == 2) ? atoi(argv[1]) : DEFAULT_SIZE;
  future<data_t> f = cilk::async produce(n);
  data_t sum = consume(0, &f);
  f.get();
  printf("Sum is %i\n", sum);
  return 0;
}
