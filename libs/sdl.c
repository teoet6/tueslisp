#include <tueslisp.h>
#include <SDL2/SDL.h>

SDL_Window *win = NULL;
SDL_Renderer *rend = NULL; 
SDL_Surface *surf = NULL;
int win_x = 0;
int win_y = 0;

void redraw() {
    SDL_Texture *text = SDL_CreateTextureFromSurface(rend, surf);
    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, text, NULL, NULL);
    SDL_RenderPresent(rend);
    SDL_DestroyTexture(text);
}

Any *open_window(Any *stack, Any *env, Any *body) {
    stack = make_pair(make_nil(), stack);
    if (list_len(body) != 3)
        ERROR("Error: `init-window` expects 3 arguments!.\n");

    Any *name = eval_any(stack, env, CAR(body));
    RETNULL(name);
    append(name, CAR(stack));
    if (name->type != SYMBOL)
        ERROR("Error: first argument to `init-window` doesn't evaluate to a symbol!\n");
    body = CDR(body);
    
    Any *x = eval_any(stack, env, CAR(body));
    RETNULL(x);
    append(x, CAR(stack));
    if (x->type != NUMBER)
        ERROR("Error: second argument to `init-window` doesn't evaluate to a number!\n");
    body = CDR(body);

    Any *y = eval_any(stack, env, CAR(body));
    RETNULL(y);
    append(y, CAR(stack));
    if (y->type != NUMBER)
        ERROR("Error: third argument to `init-window` doesn't evaluate to a number!\n");
    body = CDR(body);

    SDL_Init(SDL_INIT_VIDEO);
    if (win) SDL_DestroyWindow(win);
    if (surf) SDL_FreeSurface(surf);
    if (rend) SDL_DestroyRenderer(rend);
    win_x = x->num->top / x->num->bot;
    win_y = y->num->top / y->num->bot;
    win = SDL_CreateWindow(name->sym->val, 
                           0,
                           0,
                           win_x,
                           win_y,
                           0);
    rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    surf = SDL_CreateRGBSurface(0, win_x, win_y, 32, 0, 0, 0, 0);
    redraw();
    return make_nil();
}

Any *close_window(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 0)
        ERROR("Error: `close-window` expects no arguments");
    if (win) SDL_DestroyWindow(win), win = NULL;
    if (surf) SDL_FreeSurface(surf), surf = NULL;
    if (rend) SDL_DestroyRenderer(rend), rend = NULL;
    win_x = 0;
    win_y = 0;
    SDL_Quit();
    return make_nil();
}

#define GET_NUM(ID) { \
    Any *num = eval_any(stack, env, CAR(body)); \
    if (num->type != NUMBER) \
        ERROR("Error: all arguments to `fill-rect` should be numbers!\n"); \
    ID = num->num->top / num->num->bot; \
    body = CDR(body); \
}
Any *fill_rect(Any *stack, Any *env, Any *body) {
    if (list_len(body) != 7)
        ERROR("Error: `fill-rect` expects 7 arguments!\n");
    int x, y, w, h, r, g, b, a;
    GET_NUM(x);
    GET_NUM(y);
    GET_NUM(w);
    GET_NUM(h);
    GET_NUM(r);
    GET_NUM(g);
    GET_NUM(b);
    // GET_NUM(a);
    SDL_Rect rect = {x, y, w, h};
    SDL_FillRect(surf, &rect, SDL_MapRGB(surf->format, r, g, b));
    redraw();
    return make_nil();
}
#undef GET_NUM
