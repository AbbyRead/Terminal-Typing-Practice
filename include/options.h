#ifndef OPTIONS_H
#define OPTIONS_H

enum Ingest_Mode {FILE_INGEST, STDIN_INGEST, CLIPBOARD_INGEST};
extern enum Ingest_Mode ingest_mode;

int parse_options(int argc, char **argv);

#endif
