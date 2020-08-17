#include "graphics.hpp"
std::bitset<8> controlbit;
void handleControls()
{
    switch( SDL_EVENT_HANDLING.key.keysym.sym )
    {
        case SDLK_UP:
            controlbit[0] = 0;
        break;

        case SDLK_DOWN:
            controlbit[1] = 0;
        break;

        case SDLK_LEFT:
            controlbit[2] = 0;
        break;

        case SDLK_RIGHT:
            controlbit[3] = 0;
        break;

        case SDLK_z:
            controlbit[4] = 0;
        break;

        case SDLK_x:
            controlbit[5] = 0;
        break;

    }
}
void handleControlsr()
{
    switch( SDL_EVENT_HANDLING.key.keysym.sym )
    {

        case SDLK_UP:
            controlbit[0] = 1;
        break;

        case SDLK_DOWN:
            controlbit[1] = 1;
        break;

        case SDLK_LEFT:
            controlbit[2] = 1;
        break;

        case SDLK_RIGHT:
            controlbit[3] = 1;
        break;

        case SDLK_z:
            controlbit[4] = 1;
        break;

        case SDLK_x:
            controlbit[5] = 1;
        break;

        case SDLK_m:
            memDump();
        break;

        case SDLK_0:
            currentSprite++;
        break;

        case SDLK_p:
            printf("PC: 0x%X  Opcode: 0x%X\n",SMS.pc, SMS.z80Memory[SMS.pc]);
        break;

    }
}
int handleSDLcontrol()
{
    controlbit = 0xFF;
    while( SDL_PollEvent( &SDL_EVENT_HANDLING)) // While Event to handle Random Stuff
        {
            if (SDL_EVENT_HANDLING.type == SDL_QUIT) // If the SDL Window is Closed, close the program.
            {
                closenopMD = true;
            }
            if (SDL_EVENT_HANDLING.type == SDL_KEYDOWN) // If a key is being pressed, handle controls.
            {   // Handle Controls
                handleControls();
            }
            if (SDL_EVENT_HANDLING.type == SDL_KEYUP)
            {
                handleControlsr();
            }
        }
        DCCONTROL = controlbit.to_ulong();
    return 0;
}
