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

static void die(const char *msg) {
    fprintf(stderr, "error: %s\n", msg);
    exit(1);
}

static void *xmalloc(size_t sz) {
    void *m = malloc(sz);
    if (!m)
        die("xmalloc(): no mem");
    return m;
}

static int filter_relevent_files(const struct dirent *entry) {
    if (entry->d_name && strlen(entry->d_name)) {
        //loads plugins that only end with _plugin
        const char *needle = strstr(entry->d_name, "_plugin.so");
        if (needle && strlen(needle) == strlen("_plugin.so")) {
            //"_plugin" and not "_plugin_foo"
            return 1;
        }
    }
    return 0;
}
static char *str_del_suffix(const char *str, const char *suffix) {
    size_t len = strlen(str);
    char *newstr = xmalloc(len+1);
    if (len > strlen(suffix) && strcmp(str + (len - strlen(suffix)), suffix) == 0) {
        strncpy(newstr, str, len - strlen(suffix));
        newstr[len - strlen(suffix)] = 0;
    }
    else {
        strcpy(newstr, str);
    }
    printf("'%s'\n", newstr);
    return newstr;
}
static char *str_append(const char *prefix, const char *suffix) {
    char *str = xmalloc(strlen(prefix) + strlen(suffix) + 1);
    strcpy(str, prefix);
    strcat(str, suffix);
    return str;
}

void load_plugins(struct gstate *state) {
    struct dirent **entries = NULL;
    int nentries = scandir("./plugins/", &entries, filter_relevent_files, alphasort);
    if (!nentries) {
        fprintf(stderr, "found no plugins!\n");
        goto end;
    }
    for (int i=0; i<nentries; i++) {
        char *relpath = str_append("plugins/", entries[i]->d_name);
        void *handle = dlopen(relpath, RTLD_LAZY);
        if (!handle) {
            free(relpath);
            fprintf(stderr, "failed to load the plugin %s!\n", entries[i]->d_name);
            continue;
        }
        char *init_func_name = NULL; 
        {
            char *tmp = str_del_suffix(entries[i]->d_name, ".so");
            init_func_name = str_append(tmp, "_init");
            free(tmp);
        }
        plugin_init_fptr init_func = dlsym(handle, init_func_name);
        if (init_func) {
            init_func(state);
            if (state->plugin_vtables[state->nplugins]) {
                state->plugin_handles[state->nplugins] = handle;
                state->nplugins++;
            }
            else {
                fprintf(stderr, "%s didn't register itself\n", init_func_name);
                dlclose(handle);
            }
        }
        else {
            fprintf(stderr, "failed to load the function %s in plugin %s!\n", init_func_name, entries[i]->d_name);
            dlclose(handle);
        }
        free(relpath);
        free(init_func_name);
    }

end: 
    for (int i=0; i<nentries; i++) {
        free(entries[i]);
    }
    free(entries);
}

void info_plugins(struct gstate *state) {
    for (int i=0; i<state->nplugins; i++) {
        const char *name = state->plugin_vtables[i]->plugin_name;
        const char *desc = state->plugin_vtables[i]->plugin_description;
        if (!name)
            name = "<unavailable>";
        if (!desc)
            desc = "<unavailable>";
        printf( "plugin[%d]\n"
                "   name: %s\n" 
                "   description: %s\n", i+1, name, desc);
    }
}
