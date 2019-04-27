LDLIBS := -ldl
all: main relmove
CFLAGS := -fPIC -g3 -O0 -Wall -Wextra
main: LDLIBS := -ldl -lmain
main: LDFLAGS := -L. -Wl,-rpath=\$$ORIGIN/
main: libmain.so
%.so : plugin_src/%.c
	$(CC) -shared $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

libmain.so: LDLIBS := -ldl
libmain.so:
	$(CC)  -shared $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ main_lib.c

reverse_plugin.so : 
comma_plugin.so :
relmove: reverse_plugin.so comma_plugin.so
	if ! [ -d plugins ]; then\
		mkdir plugins;\
	fi; \
	mv reverse_plugin.so comma_plugin.so plugins
clean:
	rm -r plugins main libmain.so
