#ifndef GSTATE_H
#define GSTATE_H
#define MAX_PLUGIN 10
struct gstate {
    struct plugin_interface *plugin_vtables[MAX_PLUGIN];
    void                    *plugin_handles[MAX_PLUGIN];
    int nplugins;
};
#endif
