// balls_and_their_admirers.c
// Requirements per assignment: positions, velocities, radius, color, and a pointer
// to a leader ball; random initialization; each ball follows a random other ball.

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 400
#define HEIGHT 400
#define TITLE "Balls and their admirers"
#define BALL_COUNT 100
#define FPS 60
#define VEL_MAX 5
#define RADIUS_MAX 20
#define RADIUS_MIN 5

// Palette from the assignment page
static Color COLORS[] = {
    LIGHTGRAY, GRAY,   DARKGRAY, YELLOW,     GOLD,      ORANGE,  PINK,
    RED,       MAROON, GREEN,    LIME,       DARKGREEN, SKYBLUE, BLUE,
    DARKBLUE,  PURPLE, VIOLET,   DARKPURPLE, BEIGE,     BROWN,   DARKBROWN,
};
#define COLORS_COUNT (int)(sizeof(COLORS)/sizeof(COLORS[0]))

// ---------------------------- Data model ----------------------------
typedef struct Ball {
    int posx, posy;        // position
    int velx, vely;        // velocity
    int radius;            // radius
    Color color;           // raylib color
    struct Ball *follows;  // leader this ball follows
} Ball;

static Ball balls[BALL_COUNT];

// ---------------------------- Helpers ----------------------------
static int rand_range(int lo, int hi_inclusive) {
    // returns lo..hi_inclusive
    return lo + (rand() % (hi_inclusive - lo + 1));
}

static int nonzero_rand_step(int maxAbs) {
    // returns a step in [-maxAbs, -1] ∪ [1, maxAbs]
    int step = 0;
    while (step == 0) step = rand_range(-maxAbs, maxAbs);
    return step;
}

// Initializes a single ball with random state and leader (not itself)
static Ball* init_ball_random(Ball *p) {
    p->posx  = rand_range(0, WIDTH  - 1);
    p->posy  = rand_range(0, HEIGHT - 1);
    p->velx  = nonzero_rand_step(VEL_MAX);
    p->vely  = nonzero_rand_step(VEL_MAX);
    p->radius = rand_range(RADIUS_MIN, RADIUS_MAX);
    p->color  = COLORS[rand_range(0, COLORS_COUNT - 1)];

    // pick a random leader that is not this ball
    Ball *leader = p;
    while (leader == p) {
        int idx = rand_range(0, BALL_COUNT - 1);
        leader = &balls[idx];
    }
    p->follows = leader;

    return p;
}

// Initialize all balls
static void init_balls_random(void) {
    for (int i = 0; i < BALL_COUNT; ++i) init_ball_random(&balls[i]);
}

// Draw one ball
static Ball* draw_ball(Ball *p) {
    DrawCircle(p->posx, p->posy, (float)p->radius, p->color);
    return p;
}

// Update velocity so this ball points toward its leader with unit steps
static Ball* update_vel_for_following(Ball *p) {
    int errx = p->follows->posx - p->posx;
    int erry = p->follows->posy - p->posy;
    // Move one pixel per axis toward leader (simple pursuit)
    p->velx = (errx == 0) ? 0 : (errx > 0 ? 1 : -1);
    p->vely = (erry == 0) ? 0 : (erry > 0 ? 1 : -1);
    return p;
}

// Update position with toroidal wrap-around
static Ball* update_pos(Ball *p) {
    p->posx = (WIDTH  + p->posx + p->velx) % WIDTH;
    p->posy = (HEIGHT + p->posy + p->vely) % HEIGHT;
    return p;
}

// Update & render all balls (follow -> move -> draw)
static void update_elements(void) {
    for (int i = 0; i < BALL_COUNT; ++i) {
        draw_ball(update_pos(update_vel_for_following(&balls[i])));
    }
}

int main(void) {
    srand((unsigned)time(NULL));

    InitWindow(WIDTH, HEIGHT, TITLE);
    SetTargetFPS(FPS);

    init_balls_random();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);          // clear before drawing
        update_elements();                  // update + draw
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
