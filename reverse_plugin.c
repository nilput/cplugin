#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "plugin_interface.h"

static void pmessage(const char *message) {
    int msglen = strlen(message);
    char *rmsg = malloc(msglen + 1);
    if (!rmsg)
        return;
    int j = 0;
    for (int i=msglen-1; i>=0; i--) {
        rmsg[j++] = message[i];
    }
    rmsg[msglen] = 0;
    printf("reversed: '''%s'''\n", rmsg);
    free(rmsg);
}
static struct plugin_interface iface = {
    .plugin_name = "reverse plugin",
    .plugin_description = "a plugin that reverses a message",
    .process_message = pmessage,
};

void reverse_plugin_init(struct gstate *g) {
    printf("initializing reverse_plugin..\n");
    register_plugin(g, &iface);
}
