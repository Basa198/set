CFLAGS = -Wall

LIBS = -lcunit

TARGET = tests

SRCS = set_test.c set.c rb_tree.c
OBJS = set_test set rb_tree

$(TARGET): $(OBJS)
	gcc $(CFLAGS) -o $@ $^ $(LIBS)

set_test: set_test.c
	gcc $(CFLAGS) -c $< -o $@

set: set.c
	gcc $(CFLAGS) -c $< -o $@

rb_tree: ./red_black_tree/rb_tree.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)	