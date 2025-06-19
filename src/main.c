
#include <unistd.h>
int main(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "hv")) != -1) {
        switch (opt) {
        case 'h': return 0;
        case 'v': return 0;
        }
    }
    return 0;
}
