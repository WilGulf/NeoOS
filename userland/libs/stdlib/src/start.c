#include "stdlib.h"

extern void declarations(void);
extern int main(int argc, char **argv);

void wait_for_promise();

void c_start() {
    struct process_arguments arguments;
    process_get_arguments(&arguments);

    declarations();

    wait_for_promise();

    main(arguments.argc, arguments.argv);
}