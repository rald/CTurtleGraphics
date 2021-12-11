#include <SDL2/SDL.h>

#include "turtle.h"

int w = 1024, h = 768;

void star(Turtle *turtle, double d)
{
    for (int i = 0; i < 5; i++)
    {
        move(turtle, d);
        turn(turtle, 144);
    }
}

int main(int argc, char *argv[])
{
    SDL_Event event;

    Turtle *turtle = NULL;

    srand(time(NULL));

    CreateTurtleWorld(&w, &h);

    turtle = CreateTurtle(w / 2, h / 2, 0, 255, 255, 255, 255);
    penUp(turtle);

    jump(turtle, (w - 300) / 2, h / 2);

    penDown(turtle);

    star(turtle, 300);

    UpdateTurtleWorld();

    PauseTurtleWorld(&event);

    DestroyTurtleWorld();

    SDL_Quit();

    return EXIT_SUCCESS;
}
