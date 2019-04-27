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
"\"I'm selfish, impatient and a little insecure. I make mistakes, I am out of\n"
 "  control and at times hard to handle. But if you can't handle me at my\n"
 "  worst, then you sure as hell don't deserve me at my best.\" \n"
 "  ― Marilyn Monroe\n",

"\"Be yourself; everyone else is already taken.\" \n"
 "  ― Oscar Wilde\n",

"\"Two things are infinite: the universe and human stupidity; and I'm not sure\n"
 "   about the universe.\"\n"
 "  ― Albert Einstein\n",

"\"So many books, so little time.\" \n"
 "  ― Frank Zappa",

"\"Be who you are and say what you feel, because those who mind don't matter,\n"
 "  and those who matter don't mind.\"\n"
 "  ― Bernard M. Baruch",
};
const int nmessages = sizeof messages / sizeof messages[0];

void process_message(const char *message) {
    printf( "processing:\n"
            "'''%s'''\n\n", message);
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
