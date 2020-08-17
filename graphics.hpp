#include "include.hpp"
int pixels[192][256];
int pixelsVRAM[128][256];
uint8_t currentPixValV;
uint8_t round8XV;
uint8_t round8YV;
uint16_t pixXV;
uint16_t pixYV;
uint8_t curPixInLineV;
std::bitset<8> byte1V;
std::bitset<8> byte2V;
std::bitset<8> byte3V;
std::bitset<8> byte4V;
std::bitset<4> finalPixColorV;
uint16_t curVRAMTILEV;
uint8_t renderTileBackV;
uint8_t finalColorV;
void setRenderBlack()
{
    SDL_SetRenderDrawColor(rendererVRAM, 0x00, 0x00, 0x00, 0x00);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
}
void setRenderDarkG()
{
    SDL_SetRenderDrawColor(rendererVRAM, 0x60, 0x60, 0x60, 0x00);
    SDL_SetRenderDrawColor(renderer, 0x60, 0x60, 0x60, 0x00);
}
void setRenderLightG()
{
    SDL_SetRenderDrawColor(rendererVRAM, 0xA0, 0xA0, 0xA0, 0x00);
    SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0x00);
}
void setRenderWhite()
{
    SDL_SetRenderDrawColor(rendererVRAM, 0xFF, 0xFF, 0xFF, 0x00);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
}
void handleTileViewer()
{
    curPixInLineV = 7;
    pixXV = 0;
    pixYV = 0;
    round8XV = 0;
    round8YV = 0;
    curVRAMTILEV = 0x0;
    screenVRAM.h = 128;
    screenVRAM.w = 256;
    screenVRAM.x = 0;
    screenVRAM.y = 0;
    while(pixYV != 128)
    {
        while(pixXV != 256)
        {
            while(round8YV != 8)
            {
                byte1V = SMS.VRAM[curVRAMTILEV];
                byte2V = SMS.VRAM[curVRAMTILEV + 1];
                byte3V = SMS.VRAM[curVRAMTILEV + 2];
                byte4V = SMS.VRAM[curVRAMTILEV + 3];
                while(round8XV != 8)
                {
                    finalPixColorV[3] = byte1V[curPixInLineV];
                    finalPixColorV[2] = byte2V[curPixInLineV];
                    finalPixColorV[1] = byte3V[curPixInLineV];
                    finalPixColorV[0] = byte4V[curPixInLineV];
                    finalColorV = finalPixColorV.to_ulong();
                    //SDL_SetRenderDrawColor(rendererVRAM, finalColorV * 0x10, finalColorV * 0x10, finalColorV * 0x10, 0x00);
                    pixelsVRAM[pixYV][pixXV] = (finalColorV * 0x10) << 24 | (finalColorV) * 0x10 << 16 | (finalColorV * 0x10) << 8 | 0x00;
                    //SDL_RenderDrawPoint(rendererVRAM, pixXV, pixYV);
                    pixXV++;
                    renderTileBackV--;
                    round8XV++;
                    curPixInLineV--;
                }
                pixXV -= 8;
                round8XV = 0;
                round8YV++;
                pixYV++;
                curPixInLineV = 7;
                curVRAMTILEV += 4;
            }
            pixXV += 8;
            pixYV -= 8;
            round8YV = 0;
            round8XV = 0;

        }
        pixYV += 8;
        pixXV = 0;
    }
    SDL_UpdateTexture(texScreenVRAM, NULL, pixelsVRAM, 1024);
    SDL_RenderCopy(rendererVRAM, texScreenVRAM, NULL, &screenVRAM);
    SDL_RenderPresent(rendererVRAM);
}
std::bitset<9> getPatternID;
uint16_t baseNametable;
uint8_t offsetTile;
uint16_t spriteTableLocate;
uint8_t xScroll;
uint8_t yScroll;
uint8_t yposS;
uint8_t xposS;
uint8_t patIndexS;
uint8_t base8x;
uint8_t base8y;
uint8_t currentSprite;
void handleBWsprites()
{
    currentSprite = 0;
    while(currentSprite != 64)
    {
        curPixInLineV = 7;
    yposS = 0;
    xposS = 0;
    offsetTile = 0;
    base8x = 0;
    base8y = 0;
    yposS = SMS.VRAM[spriteTableLocate + currentSprite];
    xposS = SMS.VRAM[spriteTableLocate + (0x80 + currentSprite * 2)];
    patIndexS = SMS.VRAM[spriteTableLocate + (0x81 + currentSprite * 2)];
    //printf("SPRITE TABLE LOCATE 0x%X\n",spriteTableLocate);
    while(base8y != 8)
    {
        byte1V = SMS.VRAM[(patIndexS * 0x20) + 0 + (offsetTile * 4)];
        byte2V = SMS.VRAM[(patIndexS * 0x20) + 1 + (offsetTile * 4)];
        byte3V = SMS.VRAM[(patIndexS * 0x20) + 2 + (offsetTile * 4)];
        byte4V = SMS.VRAM[(patIndexS * 0x20) + 3 + (offsetTile * 4)];
        while(base8x != 8)
        {
            finalPixColorV[3] = byte1V[curPixInLineV];
            finalPixColorV[2] = byte2V[curPixInLineV];
            finalPixColorV[1] = byte3V[curPixInLineV];
            finalPixColorV[0] = byte4V[curPixInLineV];
            finalColorV = finalPixColorV.to_ulong();
            //pixels[yposS][xposS] = 0xFF << 24 | 0x00 << 16 | 0x00 << 8 | 0x00;
            pixels[yposS][xposS] = (finalColorV * 0x10) << 24 | (finalColorV) * 0x10 << 16 | (finalColorV * 0x10) << 8 | 0x00;
            //SDL_SetRenderDrawColor(renderer, finalColorV * 0x10, finalColorV * 0x10, finalColorV * 0x10, 0x00);
            //SDL_RenderDrawPoint(renderer, pixXV, pixYV);
            xposS++;
            base8x++;
            curPixInLineV--;
        }
        base8x = 0;
        base8y++;
        offsetTile++;
        xposS -= 8;
        yposS++;
        curPixInLineV = 7;
    }
    currentSprite++;
    }
}
void handleBASICFRAMERENDER()
{
    baseNametable = 0x3800;
    curPixInLineV = 7;
    pixXV = 0;
    pixYV = 0;
    round8XV = 0;
    round8YV = 0;
    curVRAMTILEV = 0x0;
    offsetTile = 0;
    //getPatternID = SMS.VRAM[baseNametable + 1] << 8 | SMS.VRAM[baseNametable];
    //cout<<getPatternID<<endl;
    //printf("0x%X\n",getPatternID.to_ulong());
    screen.h = 192;
    screen.w = 256;
    screen.x = 0;
    screen.y = 0;
    while(pixYV != 192)
    {
        while(pixXV != 256)
        {
            getPatternID = SMS.VRAM[baseNametable + 1] << 8 | SMS.VRAM[baseNametable];
            curVRAMTILEV = getPatternID.to_ulong();
            while(round8YV != 8)
            {
                byte1V = SMS.VRAM[(curVRAMTILEV * 0x20) + 0 + (offsetTile * 4)];
                byte2V = SMS.VRAM[(curVRAMTILEV * 0x20) + 1 + (offsetTile * 4)];
                byte3V = SMS.VRAM[(curVRAMTILEV * 0x20) + 2 + (offsetTile * 4)];
                byte4V = SMS.VRAM[(curVRAMTILEV * 0x20) + 3 + (offsetTile * 4)];
                while(round8XV != 8)
                {
                    finalPixColorV[3] = byte1V[curPixInLineV];
                    finalPixColorV[2] = byte2V[curPixInLineV];
                    finalPixColorV[1] = byte3V[curPixInLineV];
                    finalPixColorV[0] = byte4V[curPixInLineV];
                    finalColorV = finalPixColorV.to_ulong();
                    //SDL_SetRenderDrawColor(renderer, finalColorV * 0x10, finalColorV * 0x10, finalColorV * 0x10, 0x00);
                    pixels[pixYV][pixXV] = (finalColorV * 0x10) << 24 | (finalColorV) * 0x10 << 16 | (finalColorV * 0x10) << 8 | 0x00;
                    //SDL_RenderDrawPoint(renderer, pixXV, pixYV);
                    pixXV++;
                    renderTileBackV--;
                    round8XV++;
                    curPixInLineV--;
                }
                offsetTile++;
                pixXV -= 8;
                round8XV = 0;
                round8YV++;
                pixYV++;
                curPixInLineV = 7;
            }
            offsetTile = 0;
            baseNametable += 2;
            pixXV += 8;
            pixYV -= 8;
            round8YV = 0;
            round8XV = 0;

        }
        pixYV += 8;
        pixXV = 0;
    }
   // screen.x = 0 - xScroll;
    handleBWsprites();
    SDL_UpdateTexture(texScreen, NULL, pixels, 1024);
    SDL_RenderCopy(renderer, texScreen, NULL, &screen);
    //screen.x = 256 - xScroll;
    //SDL_RenderCopy(renderer, texScreen, NULL, &screen);
    SDL_RenderPresent(renderer);
}
