//plugins are expected to declare a single function: <plugin_file_name>_init
//example function:
//void <plugin_file_name>_init(struct gstate *state) {
//  //should call register_plugin(state, ptr_to_iface);
//}
struct gstate;
struct plugin_interface {
    const char *plugin_name;
    const char *plugin_description;
    void (*process_message)(const char *message);
};

typedef void (*plugin_init_fptr) (struct gstate *state);

void register_plugin(struct gstate *state, struct plugin_interface *iface);



//called by main application
void load_plugins(struct gstate *state);
void info_plugins(struct gstate *state);
