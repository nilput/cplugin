#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <dirent.h>
#include "plugin_interface.h"
#include "gstate.h"
//called by plugins
void register_plugin(struct gstate *state, struct plugin_interface *iface) {
    assert(state->nplugins < MAX_PLUGIN);
    assert(!state->plugin_vtables[state->nplugins]);
    state->plugin_vtables[state->nplugins] = iface;
}
