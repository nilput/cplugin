#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <dirent.h>
#include "plugin_interface.h"
#include "gstate.h"


struct gstate g = {0};


const char * const messages[] = {
"\"I'm selfish, impatient and a little insecure. I make mistakes, I am out of control and at times hard to handle. But if you can't handle me at my worst, then you sure as hell don't deserve me at my best.\" "
"― Marilyn Monroe",

"\"Be yourself; everyone else is already taken.\" "
"― Oscar Wilde",

"\"Two things are infinite: the universe and human stupidity; and I'm not sure about the universe.\" "
"― Albert Einstein",

"\"So many books, so little time.\" "
"― Frank Zappa",

"\"Be who you are and say what you feel, because those who mind don't matter, and those who matter don't mind.\" "
"― Bernard M. Baruch",
};
const int nmessages = sizeof messages / sizeof messages[0];

void process_message(const char *message) {
    printf( "main program: \n"
            "processing the message:\n"
            "\t'''%s'''\n\n", message);
    for (int i=0; i<g.nplugins; i++) {
        assert(g.plugin_vtables[i]);
        if (g.plugin_vtables[i]->process_message) {
            g.plugin_vtables[i]->process_message(message);
        }
    }
}

int main(void) {
    load_plugins(&g);
    info_plugins(&g);
    for (int i=0; i<nmessages; i++) {
        process_message(messages[i]);
    }
}
