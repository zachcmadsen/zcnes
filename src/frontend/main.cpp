#include <SDL.h>
#include <math.h>

int main(int argc, char *argv[])
{
    int width = 1280;
    int height = 720;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("SDL pixels", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                                          SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture *pixels =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING, width, height);

    unsigned int t1 = SDL_GetTicks();

    float pos = 0;

    for (;;)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT)
            {
                return 0;
            }
        }

        unsigned int t2 = SDL_GetTicks();
        float delta = (t2 - t1) / 1000.0f;
        t1 = t2;

        void *data;
        int pitch;
        SDL_LockTexture(pixels, NULL, &data, &pitch);
        {
            // clear to black background
            SDL_memset(data, 0, pitch * height);

            // move 100 pixels/second
            pos += delta * 100.0f;
            pos = fmodf(pos, width);

            // draw red diagonal line
            for (int i = 0; i < height; i++)
            {
                int y = i;
                int x = ((int)pos + i) % width;

                unsigned int *row = (unsigned int *)((char *)data + pitch * y);
                row[x] = 0xff0000ff; // 0xAABBGGRR
            }
        }
        SDL_UnlockTexture(pixels);

        // copy to window
        SDL_RenderCopy(renderer, pixels, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
}
