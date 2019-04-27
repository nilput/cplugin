struct gstate;
void register_plugin(struct gstate *state, struct plugin_interface *iface);

//called by main application
void load_plugins(struct gstate *state);
void info_plugins(struct gstate *state);
