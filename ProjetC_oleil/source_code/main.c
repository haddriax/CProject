#include "vue_controller/vue_controller.h"

int main(int argc, char **argv) {
    run_game(argc, argv);
    Uint64 t =0;
    FPS_counter_begin(&t);
    quit(Exit, NULL);
    return 0;
}