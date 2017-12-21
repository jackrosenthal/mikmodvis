#include <ncurses.h>
#include <mikmod.h>

int main(int argc, char **argv) {
    initscr();
    curs_set(FALSE);
    timeout(10);

    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();

    md_mode |= DMODE_SOFT_MUSIC;
    if (MikMod_Init("")) {
        fprintf(stderr, "Could not initialize sound, reason: %s\n",
                MikMod_strerror(MikMod_errno));
        return 1;
    }

    MODULE *module = Player_Load(argv[1], 128, 1);

    if (module) {
        Player_Start(module);
        while (Player_Active()) {
            MikMod_Update();
            werase(stdscr);

            for (int chan = 0; chan < module->realchn; chan++) {
                int v = Player_GetChannelVoice(chan);
                int realv = Voice_RealVolume(v);
                mvprintw(v, 0, "%d", Voice_GetFrequency(v));
                int maxx, maxy;
                getmaxyx(stdscr, maxy, maxx);
                int stopcol = 6 + (int)(realv/65535.0 * (maxx - 6));
                for (int col = 6; col < stopcol; col++) {
                    mvprintw(v, col, "*");
                }
            }

            refresh();
            char c = getch();
            if (c == 'q') {
                endwin();
                MikMod_Exit();
                return 0;
            }
            else if (c == 'p') {
                Player_TogglePause();
            }
        }

        Player_Stop();
        Player_Free(module);
    }
    else {
        fprintf(stderr, "Could not load module, reason: %s\n",
                MikMod_strerror(MikMod_errno));
    }

    endwin();
    MikMod_Exit();
    return 0;
}
