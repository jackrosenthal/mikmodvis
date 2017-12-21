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

            int maxx, maxy;
            getmaxyx(stdscr, maxy, maxx);

            double fbins[250] = {0};
            int bin_size = 200000 / maxx;
            for (int chan = 0; chan < module->realchn; chan++) {
                int v = Player_GetChannelVoice(chan);
                int bin = Voice_GetFrequency(v) / bin_size;
                bin = bin >= maxx ? maxx - 1 : bin;
                fbins[bin] += Voice_RealVolume(v);
            }

            werase(stdscr);
            for (int b = 0; b < maxx; b++) {
                int stoprow = maxy - (fbins[b] / 65535.0 * maxy);
                for (int row = maxy - 1; row > stoprow; row--) {
                    mvprintw(row, b, "*");
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
