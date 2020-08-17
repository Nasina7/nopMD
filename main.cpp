#include "cpu.hpp"
char option;
int main()
{
    //FILE* log2 = fopen("log.txt", "w+");
    //fclose(log2);
    FILE* log = fopen("log.txt", "r+");
    nopMDwindow = SDL_CreateWindow("nopMD Alpha", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 384, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(nopMDwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    texScreen = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,256, 192);
    SMSVRAM = SDL_CreateWindow("VRAM TILE VIEWER", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 256, SDL_WINDOW_RESIZABLE);
    rendererVRAM = SDL_CreateRenderer(SMSVRAM, -1, SDL_RENDERER_ACCELERATED);
    texScreenVRAM = SDL_CreateTexture(rendererVRAM,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,256, 128);
    SDL_RenderSetScale(renderer, 2, 2);
    SDL_RenderSetScale(rendererVRAM, 2, 2);
    printf("Welcome to nopMD!\n");
    SMSromload();
    bool testBool = false;
    while(closenopMD == false)
    {
        //breakpoint = true;
        //printf("PC:0x%X\n",SMS.pc);
        //printf("PC: 0x%X  Opcode: 0x%X  SP: 0x%X\n",SMS.pc, SMS.z80Memory[SMS.pc],SMS.sp);
        //breakpoint = true;
        prevCycles = SMS.cycles;
        prevScanline = SMS.scanline;
        //fprintf(log, "PC: 0x%X  SP: 0x%X  A:0x%X  F:0x%X  BC:0x%X%X  DE:0x%X%X  HL: 0x%X%X  IX:0x%X  IY:0x%X\n",SMS.pc, SMS.sp, SMS.af[0],SMS.af[1],SMS.bc[0],SMS.bc[1],SMS.de[0],SMS.de[1],SMS.hl[0],SMS.hl[1],SMS.ix, SMS.iy);
        doz80opcode();
        if(SMS.iy == 0xD200)
        {
            //testBool = true;
        }
        if(testBool == true && SMS.iy == 0)
        {
            //breakpoint = true;
            //printf("Scanline: %i\n",SMS.scanline);
        }
        if(SMS.pc == 0x38B0)
        {
            //breakpoint = true;
        }
        if(prevCycles % 3420 > SMS.cycles % 3420)
        {
            if(prevCycles != SMS.cycles)
            {
                SMS.scanline++;
                //if(SMS.scanline == 193)
                //{
                //    SMS.scanline = 0;
                //}
            }
        }
        handleInterrupts();
        //printf("BC: 0x%X%X\n",SMS.bc[0],SMS.bc[1]);
        if(SMS.scanline == 255 && prevScanline != SMS.scanline)
        {
            tempBitBuffer = 0xFF;
            DCCONTROL = tempBitBuffer.to_ulong();
            handleSDLcontrol();
            handleTileViewer();
            handleBASICFRAMERENDER();
        }
        if(breakpoint == true)
        {
            printf("OP: 0x%X\n",SMS.z80Memory[SMS.pc]);
            printf("PC: 0x%X\n",SMS.pc);
            printf("SP: 0x%X\n",SMS.sp);
            printf("A: 0x%X\n",SMS.af[0]);
            tempBitBuffer2 = SMS.af[1];
            cout<<"F: "<<tempBitBuffer2<<endl;
            printf("BC: 0x%X%X\n",SMS.bc[0],SMS.bc[1]);
            printf("DE: 0x%X%X\n",SMS.de[0],SMS.de[1]);
            printf("HL: 0x%X%X\n",SMS.hl[0],SMS.hl[1]);
            printf("IX: 0x%X\n",SMS.ix);
            printf("IY: 0x%X\n",SMS.iy);
            printf("Cycles: %i\n",SMS.cycles);
            cin>>option;
            if(option == 'y')
            {
                breakpoint = false;
            }
            if(option == 'm')
            {
                memDump();
            }
        }
    }
    fclose(log);
    cin>>option;
    return 0;
}
