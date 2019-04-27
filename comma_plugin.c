#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "plugin_interface.h"

static void pmessage(const char *message) {
    const char *comma = strchr(message, ',');
    int ncommas = 0;
    while (comma) {
        ncommas++;
        comma = strchr(comma+1, ',');
    }
    printf("Found %d commas!\n", ncommas);
}
static struct plugin_interface iface = {
    .plugin_name = "comma plugin",
    .plugin_description = "a useless plugin that counts commas",
    .process_message = pmessage,
};

void comma_plugin_init(struct gstate *g) {
    printf("initializing comma_plugin..\n");
    register_plugin(g, &iface);
}
