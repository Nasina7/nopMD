/*
OPCODES FINISHED:
0x00: 15/16
0x10: 14/16
0x20: 15/16
0x30: 15/16
0x40: 16/16 DONE
0x50: 16/16 DONE
0x60: 16/16 DONE
0x70: 15/16
0x80: 16/16 DONE
0x90: 16/16 DONE
0xA0: 16/16 DONE
0xB0: 16/16 DONE
0xC0: 15/15 DONE
0xD0: 14/15
0xE0: 14/15
0xF0: 15/15 DONE

0xCB: 128/255
0xED: ?/?

0xDD: 0/85
0xDDCB: 0/255
0xFD: 0/85
0xFDCB: 0/255
*/

// Memory and Registers
#include <string>
#include <bitset>
#include <iostream>
#include <SDL2/SDL.h>
uint8_t emuMode;
SDL_Window* nopMDwindow = NULL;
SDL_Renderer* renderer;
SDL_Rect screen;
SDL_Texture* texScreen;
SDL_Window* SMSVRAM = NULL;
SDL_Renderer* rendererVRAM;
SDL_Rect screenVRAM;
SDL_Texture* texScreenVRAM;
SDL_Event SDL_EVENT_HANDLING;
using namespace std;
// Other CPU Things
uint8_t DCCONTROL;
uint8_t DDCONTROL;
std::bitset<8> tempBitBuffer;
std::bitset<8> tempBitBuffer2;
std::bitset<8> tempBitBuffer3;
std::bitset<16> tempBitBuffer16;
std::bitset<16> tempBitBuffer216;
bool z80int1;
uint8_t prevScanline;
uint8_t SDSC[80*25];
bool z80int2;
bool breakpoint = false;
uint8_t z80intmode;
char romname[50];
bool closenopMD;
uint64_t prevCycles;
class SMSclass
{
    public:
        uint8_t z80Memory[0x10000];
        uint8_t VRAM[0x4000];
        uint8_t CRAM[32];
        uint8_t scanline;
        uint8_t af[2];
        uint8_t bc[2];
        uint8_t de[2];
        uint8_t hl[2];
        uint8_t afS[2] = {0xFF,0xFF};
        uint8_t bcS[2] = {0xFF,0xFF};
        uint8_t deS[2] = {0xFF,0xFF};
        uint8_t hlS[2] = {0xFF,0xFF};
        uint16_t pc = 0x0000;
        uint16_t sp;
        uint16_t ix;
        uint16_t iy;
        uint8_t I;
        uint8_t R;
        uint64_t cycles;
};
SMSclass SMS;
uint8_t temp8;
uint8_t temp82;
uint8_t temp83;
uint16_t temp16;
uint16_t temp162;
uint16_t temp163;
void writeIX(bool high, uint8_t value)
{
    if(high == true)
    {
        temp8 = SMS.ix >> 8;
        temp82 = SMS.ix;
        temp8 = value;
        SMS.ix = temp8 << 8 | temp82;
    }
    if(high == false)
    {
        temp8 = SMS.ix >> 8;
        temp82 = SMS.ix;
        temp82 = value;
        SMS.ix = temp8 << 8 | temp82;
    }
}
uint8_t readIX(bool high)
{
    if(high == true)
    {
        temp8 = SMS.ix >> 8;
        temp82 = SMS.ix;
        return temp8;
    }
    if(high == false)
    {
        temp8 = SMS.ix >> 8;
        temp82 = SMS.ix;
        return temp82;
    }
}
void writeIY(bool high, uint8_t value)
{
    if(high == true)
    {
        temp8 = SMS.iy >> 8;
        temp82 = SMS.iy;
        temp8 = value;
        SMS.iy = temp8 << 8 | temp82;
    }
    if(high == false)
    {
        temp8 = SMS.iy >> 8;
        temp82 = SMS.iy;
        temp82 = value;
        SMS.iy = temp8 << 8 | temp82;
    }
}
uint8_t readIY(bool high)
{
    if(high == true)
    {
        temp8 = SMS.iy >> 8;
        temp82 = SMS.iy;
        return temp8;
    }
    if(high == false)
    {
        temp8 = SMS.iy >> 8;
        temp82 = SMS.iy;
        return temp82;
    }
}
int memDump()
{
    FILE* z80mem = fopen("z80RAM","w+");
    fwrite (SMS.z80Memory , sizeof(char), sizeof(SMS.z80Memory), z80mem);
    fclose (z80mem);
    FILE* z80VRAM = fopen("z80VRAM","w+");
    fwrite (SMS.VRAM , sizeof(char), sizeof(SMS.VRAM), z80VRAM);
    fclose (z80VRAM);
    return 0;
}
int SMSromload()
{
    //printf("What do you want to emulate?\n1. Sega Master System\n2. Sega Game Gear\n3. Sega Genesis\nOption: ");
    //scanf("%i",emuMode);
    temp16 = 0;
    while(temp16 != 0xC000)
    {
        SMS.z80Memory[temp16] = 0xFF;
        temp16++;
    }
    printf("Enter Rom Name: ");
    std::cin>>romname;
    if(romname[0] == 'a')
    {
        FILE* rom = fopen("rom/test/zexdoc_sdsc.sms", "rb");
        fread(SMS.z80Memory,0xC000,1,rom);
        fclose(rom);
        memDump();
        return 0;
    }
    FILE* rom = fopen(romname, "rb");
    fread(SMS.z80Memory,0xC000,1,rom);
    fclose(rom);
    memDump();
    SMS.af[0] = 0xAB;
    SMS.af[1] = 0x69;
    SMS.de[0] = 0xC7;
    SMS.de[1] = 0x14;
    SMS.hl[0] = 0x02;
    SMS.hl[1] = 0x93;
    SMS.sp = 0xDFF0;
    return 0;
}
