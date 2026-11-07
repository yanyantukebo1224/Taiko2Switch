#include <switch.h>
#include <cstdio>

namespace {
void draw_screen() {
    consoleClear();
    std::printf("\x1b[1;1HTaiko2Switch");
    std::printf("\x1b[3;1H Nintendo Switch TJA Player");
    std::printf("\x1b[5;1H----------------------------------------");
    std::printf("\x1b[7;1HProject bootstrap build");
    std::printf("\x1b[9;1HNext: TJA parser, audio clock, chart renderer");
    std::printf("\x1b[11;1H");
    std::printf("Press PLUS to exit.");
}
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    consoleInit(nullptr);
    draw_screen();

    while (appletMainLoop()) {
        hidScanInput();
        const u64 down = hidKeysDown(CONTROLLER_P1_AUTO);

        if (down & KEY_PLUS) {
            break;
        }

        consoleUpdate(nullptr);
    }

    consoleExit();
    return 0;
}
