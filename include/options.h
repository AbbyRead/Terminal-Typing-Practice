#ifndef OPTIONS_H
#define OPTIONS_H

enum IngestMode {
    UNKNOWN,
    CLIPBOARD,
    STDIN,
    FILE
};

int parse_options(int argc, char **argv);

#endif
