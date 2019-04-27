#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <dirent.h>
#include "plugin_interface.h"
#include "gstate.h"


//loading the plugins from the "./plugins/" directory
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
        //loads files that only end with _plugin.so
        const char *needle = strstr(entry->d_name, "_plugin.so");
        if (needle && strlen(needle) == strlen("_plugin.so")) {
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
    return newstr;
}
static char *str_append(const char *prefix, const char *suffix) {
    char *str = xmalloc(strlen(prefix) + strlen(suffix) + 1);
    strcpy(str, prefix);
    strcat(str, suffix);
    return str;
}

static void load_plugins(struct gstate *state) {
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

static void info_plugins(struct gstate *state) {
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

//examples strings
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

void process_message(struct gstate *g, const char *message) {
    printf( "processing:\n"
            "'''%s'''\n\n", message);
    for (int i=0; i<g->nplugins; i++) {
        assert(g->plugin_vtables[i]);
        if (g->plugin_vtables[i]->process_message) {
            g->plugin_vtables[i]->process_message(message);
        }
    }
}

int main(void) {
    struct gstate g = {0};
    load_plugins(&g);
    info_plugins(&g);
    for (int i=0; i<nmessages; i++) {
        process_message(&g, messages[i]);
    }
}
