#ifndef TURTLE_H
#define TURTLE_H

#include <SDL2/SDL.h>

#include <stdbool.h>
#include <math.h>
#include <time.h>

static SDL_Renderer *renderer;
static SDL_Window *window;
static SDL_Texture *layer_top;
static SDL_Texture *layer_bottom;

static SDL_Point wires[] = {
    {0, 0},
    {-1, +1},
    {+2, 0},
    {-1, -1},
    {0, 0},
};

static size_t nwires = 5;

typedef struct Turtle Turtle;

struct Turtle
{
    double x, y;
    double heading;
    bool isPenDown;
    int r, g, b, a;
    SDL_Point *wires;
    size_t nwires;
};

static Turtle **turtles = NULL;
static size_t nturtles = 0;

void CreateTurtleWorld(int *w, int *h);
void DestroyTurtleWorld();
void UpdateTurtleWorld();
void PauseTurtleWorld(SDL_Event *event);

void drawTurtles(size_t nturtles, Turtle **turtles);

double drand();
double rnd(double n);
double sgn(double x);

static void plot(Turtle *turtle, int x, int y, int r, int g, int b, int a);
static void line(Turtle *turtle, int x1, int y1, int x2, int y2, int r, int g, int b, int a);

void move(Turtle *turtle, double d);
void turn(Turtle *turtle, double a);
void jump(Turtle *turtle, double nx, double ny);
void penUp(Turtle *turtle);
void penDown(Turtle *turtle);

void CreateTurtleWorld(int *w, int *h)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(*w, *h, 0, &window, &renderer);
    SDL_GetWindowSize(window, w, h);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    layer_top = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, *w, *h);

    layer_bottom = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, *w, *h);
}

void DestroyTurtleWorld()
{
    SDL_DestroyTexture(layer_top);
    SDL_DestroyTexture(layer_bottom);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void UpdateTurtleWorld()
{
    SDL_RenderPresent(renderer);
}

void AddTurtle(size_t *nturtles, Turtle ***turtles, Turtle *turtle)
{
    (*turtles) = realloc(*turtles, sizeof(*turtles) * (*nturtles + 1));
    if (*turtles)
    {
        (*turtles)[(*nturtles)++] = turtle;
    }
    else
    {
        printf("Error: AddTurtle: realloc\n");
        exit(1);
    }
}

Turtle *CreateTurtle(double x, double y, double h, int r, int g, int b, int a)
{
    Turtle *turtle = malloc(sizeof(*turtle));
    if (turtle)
    {
        turtle->x = x;
        turtle->y = y;
        turtle->heading = h;
        turtle->isPenDown = true;
        turtle->r = r;
        turtle->g = g;
        turtle->b = b;
        turtle->a = a;
        turtle->wires = malloc(sizeof(*turtle->wires) * nwires);
        if (turtle->wires)
        {
            for (int i = 0; i < nwires; i++)
            {
                turtle->wires[i] = wires[i];
            }
            turtle->nwires = nwires;
        }
        else
        {
            printf("Error: CreateTurtle: malloc: turtle->wires\n");
            exit(1);
        }

        AddTurtle(&nturtles, &turtles, turtle);
    }
    else
    {
        printf("Error: CreateTurtle: malloc: turtle\n");
        exit(1);
    }
    return turtle;
}

void rotatePoints(size_t n, SDL_Point *p, SDL_Point c, double a)
{
    double cs = cos(a);
    double sn = sin(a);
    double temp;
    for (int i = 0; i < n; i++)
    {
        temp = ((p[i].x - c.x) * cs - (p[i].y - c.y) * sn) + c.x;
        p[i].y = ((p[i].x - c.x) * sn + (p[i].y - c.y) * cs) + c.y;
        p[i].x = temp;
    }
}

void drawTurtles(size_t nturtles, Turtle **turtles)
{
    SDL_SetRenderTarget(renderer, layer_top);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetTextureBlendMode(layer_top, SDL_BLENDMODE_ADD);

    for (int i = 0; i < nturtles; i++)
    {
        int size = 10;
        SDL_Point *w = malloc(sizeof(*w) * nwires);
        SDL_Point c;
        for (int j = 0; j < nwires; j++)
        {
            w[j].x = turtles[i]->wires[j].x * size + turtles[i]->x;
            w[j].y = turtles[i]->wires[j].y * size + turtles[i]->y;
        }

        c.x = w[0].x;
        c.y = w[0].y;

        rotatePoints(nwires, w, c, turtles[i]->heading * M_PI / 180);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLines(renderer, w, nwires);
    }
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, layer_bottom, NULL, NULL);
    SDL_RenderCopy(renderer, layer_top, NULL, NULL);
    SDL_RenderPresent(renderer);
}

double drand()
{
    return rand() / (RAND_MAX + 1.0);
}

double rnd(double n)
{
    return floor(drand() * n);
}

double sgn(double x)
{
    if (x > 0.0)
        return 1.0;
    if (x < 0.0)
        return -1.0;
    return x;
}

static void plot(Turtle *turtle, int x, int y, int r, int g, int b, int a)
{
    turtle->x = x;
    turtle->y = y;

    SDL_SetRenderTarget(renderer, layer_bottom);
    SDL_SetTextureBlendMode(layer_bottom, SDL_BLENDMODE_ADD);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawPoint(renderer, x, y);
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, layer_bottom, NULL, NULL);
    SDL_RenderCopy(renderer, layer_top, NULL, NULL);
    SDL_RenderPresent(renderer);

    drawTurtles(nturtles, turtles);
}

static void line(Turtle *turtle, int x1, int y1, int x2, int y2, int r, int g, int b, int a)
{
    int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

    dx = x2 - x1; /* the horizontal distance of the line */
    dy = y2 - y1; /* the vertical distance of the line */
    dxabs = abs(dx);
    dyabs = abs(dy);
    sdx = sgn(dx);
    sdy = sgn(dy);
    x = dyabs >> 1;
    y = dxabs >> 1;
    px = x1;
    py = y1;

    plot(turtle, px, py, r, g, b, a);

    if (dxabs >= dyabs)
    { /* the line is more horizontal than vertical */
        for (i = 0; i < dxabs; i++)
        {
            y += dyabs;
            if (y >= dxabs)
            {
                y -= dxabs;
                py += sdy;
            }
            px += sdx;
            plot(turtle, px, py, r, g, b, a);
        }
    }
    else
    { /* the line is more vertical than horizontal */
        for (i = 0; i < dyabs; i++)
        {
            x += dxabs;
            if (x >= dyabs)
            {
                x -= dyabs;
                px += sdx;
            }
            py += sdy;
            plot(turtle, px, py, r, g, b, a);
        }
    }
}

void move(Turtle *turtle, double d)
{
    double nx = d * cos(turtle->heading * M_PI / 180) + turtle->x;
    double ny = d * sin(turtle->heading * M_PI / 180) + turtle->y;
    if (turtle->isPenDown)
    {
        line(turtle, turtle->x, turtle->y, nx, ny, turtle->r, turtle->g, turtle->b, turtle->a);
    }
    turtle->x = nx;
    turtle->y = ny;
}

void turn(Turtle *turtle, double a)
{
    for (int i = 0; i < fabs(a); i++)
    {
        turtle->heading += (a >= sgn(a) ? sgn(a) : a);
        drawTurtles(nturtles, turtles);
    }
}

void jump(Turtle *turtle, double nx, double ny)
{
    if (turtle->isPenDown)
    {
        line(turtle, turtle->x, turtle->y, nx, ny, turtle->r, turtle->g, turtle->b, turtle->a);
    }
    turtle->x = nx;
    turtle->y = ny;
}

void penUp(Turtle *turtle)
{
    turtle->isPenDown = false;
}

void penDown(Turtle *turtle)
{
    turtle->isPenDown = true;
}

void setPenColor(Turtle *turtle, int r, int g, int b, int a)
{
    turtle->r = r;
    turtle->g = g;
    turtle->b = b;
    turtle->a = a;
}

void PauseTurtleWorld(SDL_Event *event)
{
    while (1)
    {
        if (SDL_PollEvent(event) && event->type == SDL_QUIT)
        {
            break;
        }
    }
}

#endif
