#include <assert.h>
#include "plugin_interface.h"
#include "gstate.h"
//called by plugins
void register_plugin(struct gstate *state, struct plugin_interface *iface) {
    assert(state->nplugins < MAX_PLUGIN);
    assert(!state->plugin_vtables[state->nplugins]);
    state->plugin_vtables[state->nplugins] = iface;
}
