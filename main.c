#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

#define ROW 1+10+1
#define COL 1+15+1
#define MINE 20

INPUT_RECORD rec;
DWORD dwNOER;

typedef struct {
    char map[COL + 2][ROW + 2];
} Map;

enum Cell {
    Edge = 9,
    Mine = 8,
    Empty = 0
};

enum Gui {
    Close = 0,
    Open,
    Chk,
    Notyet
};

void mapset(Map *map);

void drawedge(Map *map);

void countmine(Map *map);

int minevalid(Map *map, int row, int col);

void mineset(Map *map, int mines);

void makemap(Map *map, int mines);

void makegui(Map *map);

int dead(Map *map, Map *gui);

void emptyopen(Map *map, Map *gui, int x, int y, int *count);

int open(Map *map, Map *gui, int x, int y, int *count);

void chk(Map *gui, int x, int y, int *minecount);

void printm(Map *map, Map *gui, int count);

void clearconsole();

void mapset(Map *map) {
    for (int y = 0; y < COL; y++) {
        for (int x = 0; x < ROW; x++) {
            map->map[y][x] = Empty;
        }
    }
}

void drawedge(Map *map) {
    for (int x = 0; x < ROW; x++) {
        map->map[0][x] = Edge;
        map->map[COL - 1][x] = Edge;
    }
    for (int y = 0; y < COL; y++) {
        map->map[y][0] = Edge;
        map->map[y][ROW - 1] = Edge;
    }
}

void countmine(Map *map) {
    for (int y = 1; y < COL - 1; y++) {
        for (int x = 1; x < ROW - 1; x++) {
            if (map->map[y][x] == Empty) {
                int temp = 0;
                for (int a = -1; a < 2; a++) {
                    for (int b = -1; b < 2; b++) {
                        if (map->map[y + a][x + b] == Mine) {
                            temp++;
                        }
                    }
                }
                map->map[y][x] = (char) temp;
            }
        }
    }
}

int minevalid(Map *map, int row, int col) {
    if (map->map[col + 1][row + 1] == Mine) {
        return 0;
    }
    return 1;
}

void mineset(Map *map, int mines) {
    int count = 0;
    while (count < mines) {
        int row, col;
        row = rand() % (ROW - 2);
        col = rand() % (COL - 2);
        if (minevalid(map, row, col)) {
            map->map[col + 1][row + 1] = Mine;
            count++;
        }
    }
}

void makemap(Map *map, int mines) {
    mapset(map);
    drawedge(map);
    mineset(map, mines);
    countmine(map);
}

void makegui(Map *map) {
    mapset(map);
    drawedge(map);
}

int dead(Map *map, Map *gui) {
    for (int y = 0; y < COL; y++) {
        for (int x = 0; x < ROW; x++) {
            if (map->map[y][x] == Mine) {
                gui->map[y][x] = Open;
            }
        }
    }
    return 1;
}

void emptyopen(Map *map, Map *gui, int x, int y, int *count) {
    if (0 < x && x < (ROW - 1) && 0 < y && y < (COL - 1)) {
        gui->map[y][x] = Open;
        *count += 1;
        for (int a = -1; a < 2; a++) {
            for (int b = -1; b < 2; b++) {
                if ((gui->map[y + a][x + b] != Edge) && (gui->map[y + a][x + b] != Open)) {
                    if (map->map[y + a][x + b] != Empty) {
                        gui->map[y + a][x + b] = Open;
                        *count += 1;
                    } else if (map->map[y + a][x + b] == Empty) {
                        emptyopen(map, gui, x + b, y + a, count);
                    }
                }
            }
        }
    }
}

int open(Map *map, Map *gui, int x, int y, int *count) {
    int death = 0;
    if (0 < x && x < (ROW - 1) && 0 < y && y < (COL - 1) && (gui->map[y][x] == Close)) {
        if (map->map[y][x] == Mine) {
            death = dead(map, gui);
        } else if (map->map[y][x] == Empty) {
            emptyopen(map, gui, x, y, count);
        } else {
            gui->map[y][x] = Open;
            *count += 1;
        }
    }
    return death;
}

void chk(Map *gui, int x, int y, int *minecount) {
    if (0 < x && x < (ROW - 1) && 0 < y && y < (COL - 1) && (gui->map[y][x] == Close)) {
        gui->map[y][x] = Chk;
        *minecount += 1;
    } else if (0 < x && x < (ROW - 1) && 0 < y && y < (COL - 1) && (gui->map[y][x] == Chk)) {
        gui->map[y][x] = Notyet;
        *minecount -= 1;
    } else if (0 < x && x < (ROW - 1) && 0 < y && y < (COL - 1) && (gui->map[y][x] == Notyet)) {
        gui->map[y][x] = Close;
    }
}

void printm(Map *map, Map *gui, int count) {
    for (int x = 0; x < ROW; x++) {
        printf("* ");
    }
    printf("\n*    Mines Left: %02d   *\n", count);
    for (int y = 0; y < COL; y++) {
        for (int x = 0; x < ROW; x++) {
            if (gui->map[y][x] == Edge) {
                printf("* ");
            } else if (gui->map[y][x] == Chk) {
                printf("! ");
            } else if (gui->map[y][x] == Notyet) {
                printf("? ");
            } else if (gui->map[y][x] == Open) {
                if (map->map[y][x] == Empty) {
                    printf("  ");
                } else if (map->map[y][x] == Mine) {
                    printf("* ");
                } else {
                    printf("%d ", map->map[y][x]);
                }
            } else if (gui->map[y][x] == Close) {
                printf("# ");
            }
        }
        printf("\n");
    }
}

void clearconsole() {
    COORD Coor = {0, 0};
    DWORD dw;
    FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 80*25, Coor, &dw); // 콘솔창 화면을 지운다.
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Coor);

}

int main() {
    system("mode con: lines=30 cols=30");
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_MOUSE_INPUT);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
    srand(time(NULL));
    int count = 0, mouse_x, mouse_y;
    int minecount = 0, death = 0;
    char r = 'r';
    Map board;
    Map gui;
    while(r =='r') {
        makemap(&board, MINE);
        makegui(&gui);
        clearconsole();
        printm(&board, &gui, MINE - minecount);
        while (count < ((COL - 2) * (ROW - 2) - MINE) && death == 0) {
            while (1) {
                ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &rec, 1, &dwNOER);
                if (rec.EventType == MOUSE_EVENT) {

                    if (rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                        mouse_x = rec.Event.MouseEvent.dwMousePosition.X;
                        mouse_y = rec.Event.MouseEvent.dwMousePosition.Y;
                        death = open(&board, &gui, mouse_x/2, mouse_y - 2, &count);
                        break;
                    } else if (rec.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
                        mouse_x = rec.Event.MouseEvent.dwMousePosition.X;
                        mouse_y = rec.Event.MouseEvent.dwMousePosition.Y;
                        chk(&gui, mouse_x/2, mouse_y - 2, &minecount);
                        break;
                    }
                }
            }
            clearconsole();
            printm(&board, &gui, MINE - minecount);
            Sleep(100);
        }
        if (death == 0) {
            clearconsole();
            printf("success!\n");
        } else if (death == 1) {
            printf("You selected mine... \nGame Over!\n");
        }
        printf("Press r to restart!\n");
        scanf("%c",&r);
        death =0;
        count =0;
        minecount =0;
    }
    return 0;
}
