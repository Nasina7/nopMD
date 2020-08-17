#include "input.hpp"
uint8_t lineCountINT = 0xFF;
uint8_t lineCountINTBACK;
// VDP IO STUFF
bool firstStartLN = true;
uint16_t VDPADDREG;
uint8_t VDPCODREG;
bool VDPCTRLBYTE;
uint8_t VDPADDRTEMP;
uint8_t VDPREGNUM;
bool lineINTPENDING;
bool frameINTPENDING;
bool lineThrottle;
uint8_t VDPREG0;
std::bitset<8> flagBitBuffer;
std::bitset<8> flagBitBuffer2;
std::bitset<16> flagBitBuffer16;
uint16_t reg16;
void handleNMI()
{
    if(SMS.scanline > 192)
    {
        frameINTPENDING = true;
    }
    if(SMS.scanline <= 192)
    {
        frameINTPENDING = false;
    }
    if(SMS.scanline == 192 && SMS.scanline != prevScanline && frameINTPENDING == true)
    {

    }
}
bool LineINTSERVE;
void handleLine()
{
    tempBitBuffer = VDPREG0;
    if(prevScanline != SMS.scanline)
    {
        if(lineCountINT != 0xFF)
        {
            lineCountINT--;
        }
        if(SMS.scanline > 192)
        {
            lineCountINT = lineCountINTBACK;
            firstStartLN = false;
        }
        if(lineCountINT == 0xFF)
        {
            //lineCountINT = lineCountINTBACK;
            if(firstStartLN == false)
            {
                lineINTPENDING = true;
            }
        }
        //cout<<"EI "<<z80int1<<endl;
        //cout<<"LINE INT "<<lineINTPENDING<<endl;
        //cout<<"LINE ENABLE "<<tempBitBuffer[5]<<endl;
    }
    if(SMS.scanline == 255)
    {
        lineThrottle = false;
    }
    if(lineINTPENDING == true && tempBitBuffer[5] == 1 && z80int1 == true && lineThrottle == false)
    {
        //printf("Line INTERRUPT!\n");
        lineINTPENDING = false;
        lineThrottle = true;
        //z80int1 = false;
        SMS.sp -= 1;
        SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
        SMS.sp -= 1;
        SMS.z80Memory[SMS.sp] = SMS.pc;
        SMS.pc = 0x38;
    }
}
std::bitset<4> bit4buffer;
std::bitset<4> bit4buffer2;
std::bitset<4> bit4buffer3;
bool flagBool;
uint8_t flag8;
bool helpFlag2;
void handleInterrupts()
{
    handleNMI();
    handleLine();
}
uint8_t DDDCONTROL = 0xFF;
uint8_t IOportREAD(uint8_t port)
{
    switch(port)
    {
        case 0x7E:
            return SMS.scanline;
        break;

        case 0xBF:
            if(frameINTPENDING == true)
            {
                return 0x9F;
            }
            return 0x1F;
        break;

        case 0xDC:
            return DCCONTROL;
        break;

        case 0xDD:
            if(DDDCONTROL == 0xFF)
            {
                DDDCONTROL = 0x3F;
                return 0xFF;
            }
            return 0x3F;
        break;

        default:
            //printf("READ FROM INVALID IO PORT 0x%X\n!\n",port);
            return 0xFF;
        break;
    }
}
std::bitset<16> IOPORT16;
void IOportWRITE(uint8_t port, uint8_t value)
{
    switch(port)
    {

        case 0xBE:
            if(VDPCODREG == 0 || VDPCODREG == 1)
            {
                //printf("VRAM WRITE ADDR: 0x%X\n!\n",VDPADDREG);
                if(value != 0xE0)
                {
                    //breakpoint = true;
                }

                //printf("Value 0x%X\n",value);
                SMS.VRAM[VDPADDREG] = value;
                VDPADDREG++;
                if(VDPADDREG == 0x4000)
                {
                    VDPADDREG = 0x00;
                }
            }
        break;

        case 0xBF:
            if(VDPCTRLBYTE == false)
            {
                VDPADDRTEMP = value;
                //printf("Value 0x%X\n",VDPADDRTEMP);
                //breakpoint = true;
                VDPCTRLBYTE = true;
                break;
            }
            if(VDPCTRLBYTE == true)
            {
                tempBitBuffer = value;
                tempBitBuffer16 = VDPADDREG;
                tempBitBuffer2 = VDPCODREG;
                tempBitBuffer2[0] = tempBitBuffer[6];
                tempBitBuffer2[1] = tempBitBuffer[7];
                VDPCODREG = tempBitBuffer2.to_ulong(); // VDPCODREG is finalized
                if(VDPCODREG == 2) // HANDLE VDP REGS
                {
                    temp8 = VDPADDRTEMP;
                    temp82 = value << 4;
                    temp82 = temp82 >> 4;
                    VDPREGNUM = temp82;
                    if(VDPREGNUM == 0x00)
                    {
                        VDPREG0 = temp8;
                    }
                    if(VDPREGNUM == 0x05)
                    {
                        IOPORT16 = temp8 << 7;
                        IOPORT16[7] = 0;
                        IOPORT16[14] = 0;
                        spriteTableLocate = IOPORT16.to_ulong();
                        //printf("SPRITE TABLE LOCATE 0x%X\n",spriteTableLocate);
                    }
                    if(VDPREGNUM == 0x08)
                    {
                        xScroll = temp8;
                    }
                    if(VDPREGNUM == 0x09)
                    {
                        yScroll = temp8;
                    }
                    if(VDPREGNUM == 0x0A)
                    {
                        //printf("VALUE %i\n",temp8);
                        //printf("Scanline %i\n",SMS.scanline);
                        lineCountINTBACK = temp8;
                    }
                    // TODO IMPLEMENT REGISTER WRITES
                }
                if(VDPCODREG != 2)
                {
                    tempBitBuffer3 = VDPADDRTEMP;
                    tempBitBuffer16 = VDPADDREG;
                    tempBitBuffer = value;
                    tempBitBuffer16[0] = tempBitBuffer3[0];
                    tempBitBuffer16[1] = tempBitBuffer3[1];
                    tempBitBuffer16[2] = tempBitBuffer3[2];
                    tempBitBuffer16[3] = tempBitBuffer3[3];
                    tempBitBuffer16[4] = tempBitBuffer3[4];
                    tempBitBuffer16[5] = tempBitBuffer3[5];
                    tempBitBuffer16[6] = tempBitBuffer3[6];
                    tempBitBuffer16[7] = tempBitBuffer3[7];
                    tempBitBuffer16[8] = tempBitBuffer[0];
                    tempBitBuffer16[9] = tempBitBuffer[1];
                    tempBitBuffer16[10] = tempBitBuffer[2];
                    tempBitBuffer16[11] = tempBitBuffer[3];
                    tempBitBuffer16[12] = tempBitBuffer[4];
                    tempBitBuffer16[13] = tempBitBuffer[5];
                    tempBitBuffer16[14] = 0;
                    tempBitBuffer16[15] = 0;
                    VDPADDREG = tempBitBuffer16.to_ulong(); // VDPADDREG is finalized

                    //printf("VDPADDREG: 0x%X\n",VDPADDREG);
                }

                VDPCTRLBYTE = false;
                //printf("VDPCOD: %i\n",VDPCODREG);
            }
        break;

        case 0xFD:
            cout<<value;
        break;

        default:
            //printf("WRITE TO INVALID IO PORT 0x%X!\n",port);
        break;
    }
}
uint16_t getREGpair16(uint8_t val1, uint8_t val2)
{
    reg16 = val1 << 8 | val2;
    return reg16;
}
void handleFlag7(uint8_t result)
{
    flagBitBuffer = result;
    flagBitBuffer2 = SMS.af[1];
    flagBitBuffer2[7] = flagBitBuffer[7];
    SMS.af[1] = flagBitBuffer2.to_ulong();
}
void handleFlag716(uint16_t result)
{
    flagBitBuffer16 = result;
    flagBitBuffer2 = SMS.af[1];
    flagBitBuffer2[7] = flagBitBuffer16[15];
    SMS.af[1] = flagBitBuffer2.to_ulong();
}
void handleFlag6(uint8_t before, uint8_t result)
{
    flagBitBuffer2 = SMS.af[1];
    flagBool = (result == 0);
    flagBitBuffer2[6] = flagBool;
    SMS.af[1] = flagBitBuffer2.to_ulong();
}
void handleFlag616(uint16_t result)
{
    flagBitBuffer2 = SMS.af[1];
    flagBitBuffer2[6] = (result == 0);
    SMS.af[1] = flagBitBuffer2.to_ulong();
}
void handleFlag5(uint8_t result)
{
    flagBitBuffer = result;
    flagBitBuffer2 = SMS.af[1];
    flagBitBuffer2[5] = flagBitBuffer[5];
    SMS.af[1] = flagBitBuffer2.to_ulong();
}
void handleFlag4(uint8_t before, uint8_t result)
{
    //flagBitBuffer = result;
    //flagBitBuffer2 = SMS.af[1];
    //flagBitBuffer2[4] = flagBitBuffer[4];
    //SMS.af[1] = flagBitBuffer2.to_ulong();
    flag8 = ((before&0xf) + (result&0xf))&0x10;
    flag8 >> 5;
    flagBitBuffer = SMS.af[1];
    flagBitBuffer[4] = flag8;
    SMS.af[1] = flagBitBuffer.to_ulong();
    //printf("Half Carry Stub!\n");
}
void handleFlag3(uint8_t result)
{
    flagBitBuffer = result;
    flagBitBuffer2 = SMS.af[1];
    flagBitBuffer2[3] = flagBitBuffer[3];
    SMS.af[1] = flagBitBuffer2.to_ulong();
}
void handleFlag2(uint8_t before, uint8_t result, bool overflow)
{
    //printf("Flag 2 Stub!\n");
    flagBitBuffer = SMS.af[1];
    if(overflow == true)
    {
        flagBitBuffer[2] = 0;
        if(before <= 0x7F && result > 0x7F && flagBitBuffer[1] == 1)
        {
            flagBitBuffer[2] = 1;
        }
        if(before > 0x7F && result <= 0x7F && flagBitBuffer[1] == 0)
        {
            flagBitBuffer[2] = 1;
        }
        SMS.af[1] = flagBitBuffer.to_ulong();
    }
    if(overflow == false)
    {
        flag8 = 0;
        flagBitBuffer2 = result;
        flag8 = flag8 + flagBitBuffer2[0];
        flag8 = flag8 + flagBitBuffer2[1];
        flag8 = flag8 + flagBitBuffer2[2];
        flag8 = flag8 + flagBitBuffer2[3];
        flag8 = flag8 + flagBitBuffer2[4];
        flag8 = flag8 + flagBitBuffer2[5];
        flag8 = flag8 + flagBitBuffer2[6];
        flag8 = flag8 + flagBitBuffer2[7];
        flag8 = flag8 % 2;
        flagBitBuffer = SMS.af[1];
        flagBitBuffer[2] = (flag8 == 0);
        SMS.af[1] = flagBitBuffer.to_ulong();
        //printf("FLAG 0x%X\n",flag8);
    }
}
void handleFlag1(uint8_t result)
{
    flagBitBuffer2 = SMS.af[1];
    flagBitBuffer2[1] = result;
    SMS.af[1] = flagBitBuffer2.to_ulong();
}
void handleFlag0sub(uint8_t before, uint8_t result)
{
    flagBitBuffer2 = SMS.af[1];
    flagBitBuffer2[0] = (result > before);
    SMS.af[1] = flagBitBuffer2.to_ulong();
}
void handleFlag0add(uint8_t before, uint8_t result)
{
    flagBitBuffer2 = SMS.af[1];
    flagBitBuffer2[0] = (result < before);
    SMS.af[1] = flagBitBuffer2.to_ulong();
}
void handleFlag016sub(uint16_t before, uint16_t result)
{
    flagBitBuffer2 = SMS.af[1];
    flagBitBuffer2[0] = (result > before);
    SMS.af[1] = flagBitBuffer2.to_ulong();
}
void handleFlag016add(uint16_t before, uint16_t result)
{
    flagBitBuffer2 = SMS.af[1];
    flagBitBuffer2[0] = (result < before);
    SMS.af[1] = flagBitBuffer2.to_ulong();
}
void handleAllFlag16(uint16_t before, uint16_t result)
{
    helpFlag2 = true;
    handleFlag716(result);
    handleFlag616(result);
    handleFlag5(result >> 8);
    handleFlag4(before,result);
    handleFlag3(result >> 8);
    handleFlag1(0);
    handleFlag2(before, result, 1);
    handleFlag016sub(before, result);
}
void handleAllFlag16sub(uint16_t before, uint16_t result)
{
    helpFlag2 = true;
    handleFlag716(result);
    handleFlag616(result);
    handleFlag5(result >> 8);
    handleFlag4(before,result);
    handleFlag3(result >> 8);
    handleFlag1(1);
    handleFlag2(before, result, 1);
    handleFlag016sub(before, result);
}
void handleAllFlag8add(uint8_t before, uint8_t result)
{
    helpFlag2 = false;
    handleFlag7(result);
    handleFlag6(before, result);
    handleFlag5(result);
    handleFlag4(before,result);
    handleFlag3(result);
    handleFlag1(0);
    handleFlag2(before, result,1);
    handleFlag0add(before, result);
}
void handleAllFlag8sub(uint8_t before, uint8_t result)
{
    helpFlag2 = true;
    handleFlag7(result);
    handleFlag6(before, result);
    handleFlag5(result);
    handleFlag4(before,result);
    handleFlag3(result);
    handleFlag1(1);
    handleFlag2(before,result,1);
    handleFlag0sub(before, result);
}
void doBankSwitch(uint8_t value, uint8_t slot)
{
        value = value % 0x20;
        //printf("Bank Switched to 0x%X on slot %i!\n",value, slot);
        FILE* rom = fopen(romname, "rb");
        fseek(rom,value * 0x4000, SEEK_SET);
        fread(SMS.z80Memory + (slot * 0x4000),0x4000,1,rom);
        fclose(rom);
}
int doSMSmemWrite(uint16_t location, uint8_t value)
{
    switch(location)
    {
        case 0x0000 ... 0xBFFF:
            //printf("PC:0x%X WRITE TO ROM!\n",SMS.pc);
            return 0;
        break;

        case 0xFFFD:
            printf("BANK SWITCH 0 PREVENTED!\n");
        break;

        case 0xFFFE:
            doBankSwitch(value, 1);
        break;

        case 0xFFFF:
            doBankSwitch(value, 2);
        break;

        default:
            SMS.z80Memory[location] = value;
        break;
    }
    SMS.z80Memory[location] = value;
}
uint8_t doSMSmemRead(uint16_t location)
{
    return SMS.z80Memory[location];
}
void doz80FDCBopcode(uint8_t opcode)
{
    switch(opcode)
    {
        case 0x06:
            tempBitBuffer = SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer[0] = tempBitBuffer2[7];
            SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x0E:
            tempBitBuffer = SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[0] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer[7] = tempBitBuffer2[0];
            SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[0];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x16:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer[0] = tempBitBuffer3[0];
            SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x1E:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer[7] = tempBitBuffer3[0];
            SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x26:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x2E:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer2[0] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer[7] = tempBitBuffer2[0];
            SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x36:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer[0] = 1;
            SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x3E:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x46:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(0);
            tempBitBuffer2[6] = tempBitBuffer[0];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x4E:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(1);
            tempBitBuffer2[6] = tempBitBuffer[1];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x56:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(2);
            tempBitBuffer2[6] = tempBitBuffer[2];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x5E:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(3);
            tempBitBuffer2[6] = tempBitBuffer[3];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x66:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(4);
            tempBitBuffer2[6] = tempBitBuffer[4];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x6E:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(5);
            tempBitBuffer2[6] = tempBitBuffer[5];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x76:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(6);
            tempBitBuffer2[6] = tempBitBuffer[6];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x7E:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(7);
            tempBitBuffer2[6] = tempBitBuffer[7];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x86:
            //printf("IY: 0x%X\n",SMS.iy);
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[0] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x8E:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[1] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x96:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[2] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x9E:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[3] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xA6:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[4] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xAE:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[5] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xB6:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[6] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xBE:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[7] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xC6:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[0] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xCE:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[1] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xD6:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[2] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xDE:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[3] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xE6:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[4] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xEE:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[5] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xF6:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[6] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xFE:
            temp8 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[7] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.iy + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        default:
            printf("CPU ERROR: UNKNOWN FDCB OPCODE 0x%X!\n",SMS.z80Memory[SMS.pc + 3]);
            printf("PC: 0x%X\n",SMS.pc);
            printf("SP: 0x%X\n",SMS.sp);
            printf("AF: 0x%X%X\n",SMS.af[0],SMS.af[1]);
            printf("BC: 0x%X%X\n",SMS.bc[0],SMS.bc[1]);
            printf("DE: 0x%X%X\n",SMS.de[0],SMS.de[1]);
            printf("HL: 0x%X%X\n",SMS.hl[0],SMS.hl[1]);
            printf("IX 0x%X\n",SMS.ix);
            printf("IY 0x%X\n",SMS.iy);
            printf("Cycles: %i\n",SMS.cycles);
            closenopMD = true;
        break;
    }
}
void doz80FDopcode(uint8_t opcode)
{
    switch(opcode)
    {
        case 0x09:
            temp16 = SMS.iy;
            SMS.iy += getREGpair16(SMS.bc[0], SMS.bc[1]);
            handleFlag016add(temp16, SMS.iy);
            handleFlag1(0);
            handleFlag4(temp16,SMS.iy);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        case 0x19:
            temp16 = SMS.iy;
            SMS.iy += getREGpair16(SMS.de[0], SMS.de[1]);
            handleFlag016add(temp16, SMS.iy);
            handleFlag1(0);
            handleFlag4(temp16,SMS.iy);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        case 0x21:
            SMS.iy = SMS.z80Memory[SMS.pc + 3] << 8 | SMS.z80Memory[SMS.pc + 2];
            SMS.pc += 4;
            SMS.cycles += 14;
        break;

        case 0x22:
            temp16 = SMS.z80Memory[SMS.pc + 3] << 8 | SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16 + 1, SMS.iy >> 8);
            doSMSmemWrite(temp16, SMS.iy);
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x23:
            SMS.iy++;
            SMS.pc += 2;
            SMS.cycles += 10;
        break;

        case 0x24:
            temp8 = readIY(1);
            temp8++;
            writeIY(1, temp8);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x25:
            temp8 = readIY(1);
            temp8--;
            writeIY(1, temp8);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x26:
            writeIY(true, SMS.z80Memory[SMS.pc + 2]);
            SMS.pc += 3;
            SMS.cycles += 11;
        break;

        case 0x29:
            temp16 = SMS.iy;
            SMS.iy += SMS.iy;
            handleFlag016add(temp16, SMS.iy);
            handleFlag1(0);
            handleFlag4(temp16,SMS.iy);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        case 0x2E:
            writeIY(false, SMS.z80Memory[SMS.pc + 2]);
            SMS.pc += 3;
            SMS.cycles += 11;
        break;

        case 0x2A:
            temp16 = SMS.z80Memory[SMS.pc + 3] << 8 | SMS.z80Memory[SMS.pc + 2];
            SMS.iy = doSMSmemRead(temp16 + 1) << 8 | doSMSmemRead(temp16);
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x2B:
            SMS.iy--;
            SMS.pc += 2;
            SMS.cycles += 10;
        break;

        case 0x2C:
            temp8 = readIY(0);
            temp8++;
            writeIY(0, temp8);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x2D:
            temp8 = readIY(0);
            temp8--;
            writeIY(0, temp8);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x34:
            //breakpoint = true;
            temp8 = SMS.z80Memory[SMS.iy + SMS.z80Memory[SMS.pc + 2]];
            SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]++;
            temp82 = SMS.z80Memory[SMS.iy + SMS.z80Memory[SMS.pc + 2]];
            handleFlag7(temp82);
            handleFlag6(temp8,temp82);
            handleFlag5(temp82);
            handleFlag4(temp8,temp82);
            handleFlag3(temp82);
            handleFlag2(temp8,temp82,1);
            handleFlag1(0);
            SMS.pc += 3;
            SMS.cycles += 23;
        break;

        case 0x35:
            //breakpoint = true;
            temp8 = SMS.z80Memory[SMS.iy + SMS.z80Memory[SMS.pc + 2]];
            SMS.z80Memory[SMS.iy + doSMSmemRead(SMS.pc + 2)]--;
            temp82 = SMS.z80Memory[SMS.iy + SMS.z80Memory[SMS.pc + 2]];
            handleFlag7(temp82);
            handleFlag6(temp8,temp82);
            handleFlag5(temp82);
            handleFlag4(temp8,temp82);
            handleFlag3(temp82);
            handleFlag2(temp8,temp82,1);
            handleFlag1(1);
            SMS.pc += 3;
            SMS.cycles += 23;
        break;

        case 0xB6:
            temp8 = SMS.af[0];
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            SMS.af[0] = SMS.af[0] | doSMSmemRead(temp16);
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8, SMS.af[0]);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x36:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.z80Memory[SMS.pc + 3]);
            SMS.pc += 4;
            SMS.cycles += 19;
        break;

        case 0x39:
            temp16 = SMS.iy;
            SMS.iy += SMS.sp;
            handleFlag016add(temp16, SMS.iy);
            handleFlag1(0);
            handleFlag4(temp16,SMS.iy);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        case 0x46:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            SMS.bc[0] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x4E:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            SMS.bc[1] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x56:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            SMS.de[0] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x5E:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            SMS.de[1] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x66:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            SMS.hl[0] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x6E:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            SMS.hl[1] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x7E:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            SMS.af[0] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x70:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.bc[0]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x71:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.bc[1]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x72:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.de[0]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x73:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.de[1]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x74:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.hl[0]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x75:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.hl[1]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x77:
            temp16 = SMS.iy + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.af[0]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0xBE:
            //breakpoint = true;
            temp82 = doSMSmemRead(SMS.iy + SMS.z80Memory[SMS.pc + 2]);
            temp8 = SMS.af[0] - temp82;
            handleAllFlag8sub(SMS.af[0],temp8);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0xCB:
            doz80FDCBopcode(SMS.z80Memory[SMS.pc + 3]);
        break;

        case 0xE1:
            SMS.iy = SMS.z80Memory[SMS.sp + 1] << 8 | SMS.z80Memory[SMS.sp];
            SMS.pc += 2;
            SMS.sp += 2;
            SMS.cycles += 14;
        break;

        case 0xE5:
            SMS.sp--;
            doSMSmemWrite(SMS.sp, SMS.iy >> 8);
            SMS.sp--;
            doSMSmemWrite(SMS.sp, SMS.iy);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        default:
            printf("CPU ERROR: UNKNOWN FD OPCODE 0x%X!\n",SMS.z80Memory[SMS.pc + 1]);
            printf("PC: 0x%X\n",SMS.pc);
            printf("SP: 0x%X\n",SMS.sp);
            printf("AF: 0x%X%X\n",SMS.af[0],SMS.af[1]);
            printf("BC: 0x%X%X\n",SMS.bc[0],SMS.bc[1]);
            printf("DE: 0x%X%X\n",SMS.de[0],SMS.de[1]);
            printf("HL: 0x%X%X\n",SMS.hl[0],SMS.hl[1]);
            printf("IX 0x%X\n",SMS.ix);
            printf("IY 0x%X\n",SMS.iy);
            printf("Cycles: %i\n",SMS.cycles);
            memDump();
            closenopMD = true;
        break;
    }
}

void doz80DDCBopcode(uint8_t opcode)
{
    switch(opcode)
    {
        case 0x06:
            tempBitBuffer = SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer[0] = tempBitBuffer2[7];
            SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x0E:
            tempBitBuffer = SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[0] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer[7] = tempBitBuffer2[0];
            SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[0];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x16:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer[0] = tempBitBuffer3[0];
            SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x1E:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer[7] = tempBitBuffer3[0];
            SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x26:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x2E:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer2[0] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer[7] = tempBitBuffer2[0];
            SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x36:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer[0] = 1;
            SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x3E:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            handleFlag5(SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x46:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(0);
            tempBitBuffer2[6] = tempBitBuffer[0];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x4E:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(1);
            tempBitBuffer2[6] = tempBitBuffer[1];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x56:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(2);
            tempBitBuffer2[6] = tempBitBuffer[2];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x5E:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(3);
            tempBitBuffer2[6] = tempBitBuffer[3];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x66:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(4);
            tempBitBuffer2[6] = tempBitBuffer[4];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x6E:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(5);
            tempBitBuffer2[6] = tempBitBuffer[5];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x76:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(6);
            tempBitBuffer2[6] = tempBitBuffer[6];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x7E:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            tempBitBuffer = doSMSmemRead(temp16);
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(7);
            tempBitBuffer2[6] = tempBitBuffer[7];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x86:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[0] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x8E:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[1] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x96:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[2] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x9E:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[3] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xA6:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[4] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xAE:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[5] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xB6:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[6] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xBE:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[7] = 0;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xC6:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[0] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xCE:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[1] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xD6:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[2] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xDE:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[3] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xE6:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[4] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xEE:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[5] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xF6:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[6] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0xFE:
            temp8 = doSMSmemRead(SMS.ix + SMS.z80Memory[SMS.pc + 2]);
            tempBitBuffer = temp8;
            tempBitBuffer[7] = 1;
            temp8 = tempBitBuffer.to_ulong();
            doSMSmemWrite(SMS.ix + SMS.z80Memory[SMS.pc + 2], temp8);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        default:
            printf("CPU ERROR: UNKNOWN DDCB OPCODE 0x%X!\n",SMS.z80Memory[SMS.pc + 3]);
            printf("PC: 0x%X\n",SMS.pc);
            printf("SP: 0x%X\n",SMS.sp);
            printf("AF: 0x%X%X\n",SMS.af[0],SMS.af[1]);
            printf("BC: 0x%X%X\n",SMS.bc[0],SMS.bc[1]);
            printf("DE: 0x%X%X\n",SMS.de[0],SMS.de[1]);
            printf("HL: 0x%X%X\n",SMS.hl[0],SMS.hl[1]);
            printf("IX 0x%X\n",SMS.ix);
            printf("IY 0x%X\n",SMS.iy);
            printf("Cycles: %i\n",SMS.cycles);
            closenopMD = true;
        break;
    }
}
void doz80DDopcode(uint8_t opcode)
{
    switch(opcode)
    {
        case 0x09:
            temp16 = SMS.ix;
            SMS.ix += getREGpair16(SMS.bc[0], SMS.bc[1]);
            handleFlag016add(temp16, SMS.ix);
            handleFlag1(0);
            handleFlag4(temp16,SMS.ix);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        case 0x19:
            temp16 = SMS.ix;
            SMS.ix += getREGpair16(SMS.de[0], SMS.de[1]);
            handleFlag016add(temp16, SMS.ix);
            handleFlag1(0);
            handleFlag4(temp16,SMS.ix);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        case 0x21:
            SMS.ix = SMS.z80Memory[SMS.pc + 3] << 8 | SMS.z80Memory[SMS.pc + 2];
            SMS.pc += 4;
            SMS.cycles += 14;
        break;

        case 0x22:
            temp16 = SMS.z80Memory[SMS.pc + 3] << 8 | SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16 + 1, SMS.ix >> 8);
            doSMSmemWrite(temp16, SMS.ix);
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x23:
            SMS.ix++;
            SMS.pc += 2;
            SMS.cycles += 10;
        break;

        case 0x24:
            temp8 = readIX(1);
            temp8++;
            writeIX(1, temp8);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x25:
            temp8 = readIX(1);
            temp8--;
            writeIX(1, temp8);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x26:
            writeIX(true, SMS.z80Memory[SMS.pc + 2]);
            SMS.pc += 3;
            SMS.cycles += 11;
        break;

        case 0x2E:
            writeIX(false, SMS.z80Memory[SMS.pc + 2]);
            SMS.pc += 3;
            SMS.cycles += 11;
        break;

        case 0x2A:
            temp16 = SMS.z80Memory[SMS.pc + 3] << 8 | SMS.z80Memory[SMS.pc + 2];
            SMS.ix = doSMSmemRead(temp16 + 1) << 8 | doSMSmemRead(temp16);
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x2B:
            SMS.ix--;
            SMS.pc += 2;
            SMS.cycles += 10;
        break;

        case 0x2C:
            temp8 = readIX(0);
            temp8++;
            writeIX(0, temp8);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x2D:
            temp8 = readIX(0);
            temp8--;
            writeIX(0, temp8);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x34:
            temp8 = SMS.z80Memory[SMS.ix + SMS.z80Memory[SMS.pc + 2]];
            SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]++;
            temp82 = SMS.z80Memory[SMS.ix + SMS.z80Memory[SMS.pc + 2]];
            handleFlag7(temp82);
            handleFlag6(temp8,temp82);
            handleFlag5(temp82);
            handleFlag4(temp8,temp82);
            handleFlag3(temp82);
            handleFlag2(temp8,temp82,1);
            handleFlag1(0);
            SMS.pc += 3;
            SMS.cycles += 23;
        break;

        case 0x35:
            temp8 = SMS.z80Memory[SMS.ix + SMS.z80Memory[SMS.pc + 2]];
            SMS.z80Memory[SMS.ix + doSMSmemRead(SMS.pc + 2)]--;
            temp82 = SMS.z80Memory[SMS.ix + SMS.z80Memory[SMS.pc + 2]];
            handleFlag7(temp82);
            handleFlag6(temp8,temp82);
            handleFlag5(temp82);
            handleFlag4(temp8,temp82);
            handleFlag3(temp82);
            handleFlag2(temp8,temp82,1);
            handleFlag1(1);
            SMS.pc += 3;
            SMS.cycles += 23;
        break;

        case 0x36:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.z80Memory[SMS.pc + 3]);
            SMS.pc += 4;
            SMS.cycles += 19;
        break;

        case 0x46:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.bc[0] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x4E:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.bc[1] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x56:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.de[0] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x5E:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.de[1] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x66:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.hl[0] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x6E:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.hl[1] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x7E:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.af[0] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x70:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.bc[0]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x71:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.bc[1]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x72:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.de[0]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x73:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.de[1]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x74:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.hl[0]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x75:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.hl[1]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x77:
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16, SMS.af[0]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x86:
            temp8 = SMS.af[0];
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.af[0] += doSMSmemRead(temp16);
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x8E:
            temp8 = SMS.af[0];
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.af[0] += doSMSmemRead(temp16);
            tempBitBuffer = SMS.af[1];
            SMS.af[0] += tempBitBuffer[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0x96:
            temp8 = SMS.af[0];
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.af[0] = SMS.af[0] - doSMSmemRead(temp16);
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0xA6:
            temp8 = SMS.af[0];
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.af[0] = SMS.af[0] & doSMSmemRead(temp16);
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0xAE:
            temp8 = SMS.af[0];
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.af[0] = SMS.af[0] ^ doSMSmemRead(temp16);
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0xB6:
            temp8 = SMS.af[0];
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            SMS.af[0] = SMS.af[0] | doSMSmemRead(temp16);
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0xBE:
            temp8 = SMS.af[0];
            temp16 = SMS.ix + SMS.z80Memory[SMS.pc + 2];
            temp82 = SMS.af[0] - doSMSmemRead(temp16);
            handleAllFlag8sub(temp8, temp82);
            SMS.pc += 3;
            SMS.cycles += 19;
        break;

        case 0xCB:
            doz80DDCBopcode(SMS.z80Memory[SMS.pc + 3]);
        break;

        case 0xE1:
            SMS.ix = SMS.z80Memory[SMS.sp + 1] << 8 | SMS.z80Memory[SMS.sp];
            SMS.pc += 2;
            SMS.sp += 2;
            SMS.cycles += 14;
        break;

        case 0xE5:
            SMS.sp--;
            doSMSmemWrite(SMS.sp, SMS.ix >> 8);
            SMS.sp--;
            doSMSmemWrite(SMS.sp, SMS.ix);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        default:
            printf("CPU ERROR: UNKNOWN DD OPCODE 0x%X!\n",SMS.z80Memory[SMS.pc + 1]);
            printf("PC: 0x%X\n",SMS.pc);
            printf("SP: 0x%X\n",SMS.sp);
            printf("AF: 0x%X%X\n",SMS.af[0],SMS.af[1]);
            printf("BC: 0x%X%X\n",SMS.bc[0],SMS.bc[1]);
            printf("DE: 0x%X%X\n",SMS.de[0],SMS.de[1]);
            printf("HL: 0x%X%X\n",SMS.hl[0],SMS.hl[1]);
            printf("IX 0x%X\n",SMS.ix);
            printf("IY 0x%X\n",SMS.iy);
            printf("Cycles: %i\n",SMS.cycles);
            memDump();
            closenopMD = true;
        break;
    }
}
void doz80EDopcode(uint8_t opcode)
{
    switch(opcode)
    {
        case 0x40:
            SMS.bc[0] = IOportREAD(SMS.bc[1]);
            SMS.pc += 2;
            SMS.cycles += 11;
        break;

        case 0x41:
            IOportWRITE(SMS.bc[1],SMS.bc[0]);
            SMS.pc += 2;
            SMS.cycles += 12;
        break;

        case 0x42:
            tempBitBuffer = SMS.af[1];
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp163 = temp16;
            temp162 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            temp16 = temp16 - (temp162 + tempBitBuffer[0]);
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            handleAllFlag16sub(temp163, temp16);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        case 0x43:
            temp16 = SMS.z80Memory[SMS.pc + 3] << 8 | SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16 + 1, SMS.bc[0]);
            doSMSmemWrite(temp16, SMS.bc[1]);
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x44:
            temp8 = SMS.af[0];
            SMS.af[0] = 0 - SMS.af[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x45:
            SMS.pc = SMS.z80Memory[SMS.sp + 1] << 8 | SMS.z80Memory[SMS.sp];
            z80int1 = z80int2;
            SMS.sp += 2;
            SMS.cycles += 14;
        break;

        case 0x47:
            temp8 = SMS.I;
            SMS.I = SMS.af[0];
            handleFlag7(SMS.I);
            handleFlag6(temp8,SMS.I);
            SMS.pc += 2;
            SMS.cycles += 9;
        break;

        case 0x48:
            SMS.bc[1] = IOportREAD(SMS.bc[1]);
            SMS.pc += 2;
            SMS.cycles += 11;
        break;

        case 0x49:
            IOportWRITE(SMS.bc[1],SMS.bc[1]);
            SMS.pc += 2;
            SMS.cycles += 12;
        break;

        case 0x4B:
            temp16 = SMS.z80Memory[SMS.pc + 3] << 8 | SMS.z80Memory[SMS.pc + 2];
            SMS.bc[0] = doSMSmemRead(temp16 + 1);
            SMS.bc[1] = doSMSmemRead(temp16);
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x4F:
            temp8 = SMS.R;
            SMS.R = SMS.af[0];
            handleFlag7(SMS.R);
            handleFlag6(temp8,SMS.R);
            SMS.pc += 2;
            SMS.cycles += 9;
        break;

        case 0x50:
            SMS.de[0] = IOportREAD(SMS.bc[1]);
            SMS.pc += 2;
            SMS.cycles += 11;
        break;

        case 0x51:
            IOportWRITE(SMS.bc[1],SMS.de[0]);
            SMS.pc += 2;
            SMS.cycles += 12;
        break;

        case 0x52:
            tempBitBuffer = SMS.af[1];
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp163 = temp16;
            temp162 = getREGpair16(SMS.de[0],SMS.de[1]);
            temp16 = temp16 - (temp162 + tempBitBuffer[0]);
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            handleAllFlag16sub(temp163, temp16);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        case 0x53:
            temp16 = SMS.z80Memory[SMS.pc + 3] << 8 | SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16 + 1, SMS.de[0]);
            doSMSmemWrite(temp16, SMS.de[1]);
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x56:
            z80intmode = 1;
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x57:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.I;
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 9;
        break;

        case 0x58:
            SMS.de[1] = IOportREAD(SMS.bc[1]);
            SMS.pc += 2;
            SMS.cycles += 11;
        break;

        case 0x59:
            IOportWRITE(SMS.bc[1],SMS.de[1]);
            SMS.pc += 2;
            SMS.cycles += 12;
        break;

        case 0x5B:
            temp16 = SMS.z80Memory[SMS.pc + 3] << 8 | SMS.z80Memory[SMS.pc + 2];
            SMS.de[0] = doSMSmemRead(temp16 + 1);
            SMS.de[1] = doSMSmemRead(temp16);
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x5F:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.R;
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 9;
        break;

        case 0x60:
            SMS.hl[0] = IOportREAD(SMS.bc[1]);
            SMS.pc += 2;
            SMS.cycles += 11;
        break;

        case 0x61:
            IOportWRITE(SMS.bc[1],SMS.hl[0]);
            SMS.pc += 2;
            SMS.cycles += 12;
        break;

        case 0x62:
            tempBitBuffer = SMS.af[1];
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp163 = temp16;
            temp162 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp16 = temp16 - (temp162 + tempBitBuffer[0]);
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            handleAllFlag16sub(temp163, temp16);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        case 0x67:
            temp8 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            temp83 = SMS.af[0];
            bit4buffer = temp8 >> 4;
            bit4buffer2 = temp8;
            bit4buffer3 = SMS.af[0];
            temp8 = bit4buffer3.to_ulong() << 4 | bit4buffer.to_ulong();
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[3] = bit4buffer2[3];
            tempBitBuffer[2] = bit4buffer2[2];
            tempBitBuffer[1] = bit4buffer2[1];
            tempBitBuffer[0] = bit4buffer2[0];
            SMS.af[0] = tempBitBuffer.to_ulong();
            doSMSmemWrite(getREGpair16(SMS.hl[0],SMS.hl[1]),temp8);
            handleFlag7(SMS.af[0]);
            handleFlag6(temp83,SMS.af[0]);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 18;
        break;

        case 0x68:
            SMS.hl[1] = IOportREAD(SMS.bc[1]);
            SMS.pc += 2;
            SMS.cycles += 11;
        break;

        case 0x69:
            IOportWRITE(SMS.bc[1],SMS.hl[1]);
            SMS.pc += 2;
            SMS.cycles += 12;
        break;

        case 0x71:
            IOportWRITE(SMS.bc[1],0);
            SMS.pc += 2;
            SMS.cycles += 12;
        break;

        case 0x72:
            tempBitBuffer = SMS.af[1];
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp163 = temp16;
            temp162 = SMS.sp;
            temp16 = temp16 - (temp162 + tempBitBuffer[0]);
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            handleAllFlag16sub(temp163, temp16);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        case 0x73:
            temp16 = SMS.z80Memory[SMS.pc + 3] << 8 | SMS.z80Memory[SMS.pc + 2];
            doSMSmemWrite(temp16 + 1, SMS.sp >> 8);
            doSMSmemWrite(temp16, SMS.sp);
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0x78:
            SMS.af[0] = IOportREAD(SMS.bc[1]);
            SMS.pc += 2;
            SMS.cycles += 11;
        break;

        case 0x79:
            IOportWRITE(SMS.bc[1],SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 12;
        break;

        case 0x7B:
            temp16 = SMS.z80Memory[SMS.pc + 3] << 8 | SMS.z80Memory[SMS.pc + 2];
            SMS.sp = doSMSmemRead(temp16 + 1) << 8 | doSMSmemRead(temp16);
            SMS.pc += 4;
            SMS.cycles += 20;
        break;

        case 0xA0:
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp162 = getREGpair16(SMS.de[0], SMS.de[1]);
            SMS.z80Memory[temp162] = SMS.z80Memory[temp16];
            temp16++;
            temp162++;
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            SMS.de[0] = temp162 >> 8;
            SMS.de[1] = temp162;
            temp163 = getREGpair16(SMS.bc[0], SMS.bc[1]);
            temp163--;
            SMS.bc[0] = temp163 >> 8;
            SMS.bc[1] = temp163;
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 16;
        break;

        case 0xA1:
            //breakpoint = true;
            temp82 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            temp8 = SMS.af[0] - temp82;
            handleAllFlag8sub(SMS.af[0],temp8);
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp162 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            temp16++;
            temp162--;
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            SMS.bc[0] = temp162 >> 8;
            SMS.bc[1] = temp162;
            SMS.pc += 2;
            SMS.cycles += 16;
        break;

        case 0xA3:
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            IOportWRITE(SMS.bc[1],SMS.z80Memory[temp16]);
            SMS.bc[0]--;
            temp16++;
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[1] = 1;
            tempBitBuffer[6] = 0;
            if(SMS.bc[0] == 0)
            {
                tempBitBuffer[6] = 1;
            }
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 16;
        break;

        case 0xA8:
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp162 = getREGpair16(SMS.de[0], SMS.de[1]);
            SMS.z80Memory[temp162] = SMS.z80Memory[temp16];
            temp16--;
            temp162--;
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            SMS.de[0] = temp162 >> 8;
            SMS.de[1] = temp162;
            temp163 = getREGpair16(SMS.bc[0], SMS.bc[1]);
            temp163--;
            SMS.bc[0] = temp163 >> 8;
            SMS.bc[1] = temp163;
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 16;
        break;

        case 0xA9:
            //breakpoint = true;
            temp82 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            temp8 = SMS.af[0] - temp82;
            handleAllFlag8sub(SMS.af[0],temp8);
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp162 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            temp16--;
            temp162--;
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            SMS.bc[0] = temp162 >> 8;
            SMS.bc[1] = temp162;
            SMS.pc += 2;
            SMS.cycles += 16;
        break;

        case 0xAB:
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            IOportWRITE(SMS.bc[1],SMS.z80Memory[temp16]);
            SMS.bc[0]--;
            temp16--;
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[1] = 1;
            tempBitBuffer[6] = 0;
            if(SMS.bc[0] == 0)
            {
                tempBitBuffer[6] = 1;
            }
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 16;
        break;

        case 0xB0:
            temp163 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            while(temp163 != 0)
            {
            SMS.z80Memory[getREGpair16(SMS.de[0],SMS.de[1])] = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            temp16 = getREGpair16(SMS.de[0],SMS.de[1]);
            temp162 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp16++;
            temp162++;
            SMS.de[0] = temp16 >> 8;
            SMS.de[1] = temp16;
            SMS.hl[0] = temp162 >> 8;
            SMS.hl[1] = temp162;
            temp163 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            temp163--;
            SMS.bc[0] = temp163 >> 8;
            SMS.bc[1] = temp163;
            SMS.cycles += 21;
            }
            SMS.pc += 2;
        break;

        case 0xB1:
            //breakpoint = true;
            temp163 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            flagBitBuffer = SMS.af[1];
            while(temp163 != 0 && flagBitBuffer[6] == 0)
            {
                temp82 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
                temp8 = SMS.af[0] - temp82;
                handleAllFlag8sub(SMS.af[0],temp8);
                temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
                temp162 = getREGpair16(SMS.bc[0],SMS.bc[1]);
                temp16++;
                temp162--;
                SMS.hl[0] = temp16 >> 8;
                SMS.hl[1] = temp16;
                SMS.bc[0] = temp162 >> 8;
                SMS.bc[1] = temp162;
                flagBitBuffer = SMS.af[1];
                temp163 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            }
            SMS.pc += 2;
            SMS.cycles += 21;
        break;

        case 0xB8:
            temp163 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            while(temp163 != 0)
            {
            SMS.z80Memory[getREGpair16(SMS.de[0],SMS.de[1])] = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            temp16 = getREGpair16(SMS.de[0],SMS.de[1]);
            temp162 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp16--;
            temp162--;
            SMS.de[0] = temp16 >> 8;
            SMS.de[1] = temp16;
            SMS.hl[0] = temp162 >> 8;
            SMS.hl[1] = temp162;
            temp163 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            temp163--;
            SMS.bc[0] = temp163 >> 8;
            SMS.bc[1] = temp163;
            SMS.cycles += 21;
            }
            SMS.pc += 2;
        break;

        case 0xB3:
            while(SMS.bc[0] != 0)
            {
                temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
                IOportWRITE(SMS.bc[1],SMS.z80Memory[temp16]);
                temp16++;
                SMS.hl[0] = temp16 >> 8;
                SMS.hl[1] = temp16;
                SMS.bc[0]--;
                SMS.cycles += 21;
            }
            SMS.pc += 2;
        break;

        case 0xB9:
            //breakpoint = true;
            temp163 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            flagBitBuffer = SMS.af[1];
            while(temp163 != 0 && flagBitBuffer[6] == 0)
            {
                temp82 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
                temp8 = SMS.af[0] - temp82;
                handleAllFlag8sub(SMS.af[0],temp8);
                temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
                temp162 = getREGpair16(SMS.bc[0],SMS.bc[1]);
                temp16--;
                temp162--;
                SMS.hl[0] = temp16 >> 8;
                SMS.hl[1] = temp16;
                SMS.bc[0] = temp162 >> 8;
                SMS.bc[1] = temp162;
                flagBitBuffer = SMS.af[1];
                temp163 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            }
            SMS.pc += 2;
            SMS.cycles += 21;
        break;

        default:
            printf("CPU ERROR: UNKNOWN ED OPCODE 0x%X!\n",SMS.z80Memory[SMS.pc + 1]);
            printf("PC: 0x%X\n",SMS.pc);
            printf("SP: 0x%X\n",SMS.sp);
            printf("AF: 0x%X%X\n",SMS.af[0],SMS.af[1]);
            printf("BC: 0x%X%X\n",SMS.bc[0],SMS.bc[1]);
            printf("DE: 0x%X%X\n",SMS.de[0],SMS.de[1]);
            printf("HL: 0x%X%X\n",SMS.hl[0],SMS.hl[1]);
            printf("IX 0x%X\n",SMS.ix);
            printf("IY 0x%X\n",SMS.iy);
            printf("Cycles: %i\n",SMS.cycles);
            memDump();
            closenopMD = true;
        break;
    }
}
void doz80CBopcode(uint8_t opcode)
{
    switch(opcode)
    {
        case 0x10:
            temp8 = SMS.bc[0];
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[0] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.bc[0]);
            handleFlag6(temp8,SMS.bc[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x11:
            temp8 = SMS.bc[1];
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[0] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.bc[1]);
            handleFlag6(temp8,SMS.bc[1]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x12:
            temp8 = SMS.de[0];
            tempBitBuffer = SMS.de[0];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[0] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.de[0]);
            handleFlag6(temp8,SMS.de[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x13:
            temp8 = SMS.de[1];
            tempBitBuffer = SMS.de[1];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[0] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.de[1]);
            handleFlag6(temp8,SMS.de[1]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x14:
            temp8 = SMS.hl[0];
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[0] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.hl[0]);
            handleFlag6(temp8,SMS.hl[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x15:
            temp8 = SMS.hl[1];
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[0] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.hl[1]);
            handleFlag6(temp8,SMS.hl[1]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x16:
            temp83 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            temp8 = temp83;
            tempBitBuffer = temp83;
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[0] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            temp83 = tempBitBuffer.to_ulong();
            doSMSmemWrite(getREGpair16(SMS.hl[0],SMS.hl[1]),temp83);
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(temp83);
            handleFlag6(temp8,temp83);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x17:
            temp8 = SMS.af[0];
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[0] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x18:
            temp8 = SMS.bc[0];
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[7] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.bc[0]);
            handleFlag6(temp8,SMS.bc[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x19:
            temp8 = SMS.bc[1];
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[7] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.bc[1]);
            handleFlag6(temp8,SMS.bc[1]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x1A:
            temp8 = SMS.de[0];
            tempBitBuffer = SMS.de[0];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[7] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.de[0]);
            handleFlag6(temp8,SMS.de[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x1B:
            temp8 = SMS.de[1];
            tempBitBuffer = SMS.de[1];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[7] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.de[1]);
            handleFlag6(temp8,SMS.de[1]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x1C:
            temp8 = SMS.hl[0];
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[7] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.hl[0]);
            handleFlag6(temp8,SMS.hl[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x1D:
            temp8 = SMS.hl[1];
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[7] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.hl[1]);
            handleFlag6(temp8,SMS.hl[1]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x1E:
            temp83 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            temp8 = temp83;
            tempBitBuffer = temp83;
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[7] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            temp83 = tempBitBuffer.to_ulong();
            doSMSmemWrite(getREGpair16(SMS.hl[0],SMS.hl[1]),temp83);
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(temp83);
            handleFlag6(temp8,temp83);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x1F:
            temp8 = SMS.af[0];
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer2[0] = tempBitBuffer3[0];
            tempBitBuffer[7] = tempBitBuffer2[0];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x20:
            temp8 = SMS.bc[0];
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.bc[0]);
            handleFlag6(temp8,SMS.bc[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x21:
            temp8 = SMS.bc[1];
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.bc[1]);
            handleFlag6(temp8,SMS.bc[1]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x22:
            temp8 = SMS.de[0];
            tempBitBuffer = SMS.de[0];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.de[0]);
            handleFlag6(temp8,SMS.de[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x23:
            temp8 = SMS.de[1];
            tempBitBuffer = SMS.de[1];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.de[1]);
            handleFlag6(temp8,SMS.de[1]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x24:
            temp8 = SMS.hl[0];
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.hl[0]);
            handleFlag6(temp8,SMS.hl[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x25:
            temp8 = SMS.hl[1];
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.hl[1]);
            handleFlag6(temp8,SMS.hl[1]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x27:
            temp8 = SMS.af[0];
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x2F:
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer2[0] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer[7] = tempBitBuffer2[0];
            SMS.af[0] = tempBitBuffer.to_ulong();
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.af[0]);
            handleFlag5(SMS.af[0]);
            SMS.pc += 4;
            SMS.cycles += 23;
        break;

        case 0x38:
            temp8 = SMS.bc[0];
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.bc[0]);
            handleFlag6(temp8,SMS.bc[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x39:
            temp8 = SMS.bc[1];
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.bc[1]);
            handleFlag6(temp8,SMS.bc[1]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x3A:
            temp8 = SMS.de[0];
            tempBitBuffer = SMS.de[0];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            SMS.de[0] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.de[0]);
            handleFlag6(temp8,SMS.de[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x3B:
            temp8 = SMS.de[1];
            tempBitBuffer = SMS.de[1];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            SMS.de[1] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.de[1]);
            handleFlag6(temp8,SMS.de[1]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x3C:
            temp8 = SMS.hl[0];
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.hl[0]);
            handleFlag6(temp8,SMS.hl[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x3D:
            temp8 = SMS.hl[1];
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.hl[1]);
            handleFlag6(temp8,SMS.hl[1]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x3E:
            temp83 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            temp8 = temp83;
            tempBitBuffer = temp83;
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            temp83 = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            doSMSmemWrite(getREGpair16(SMS.hl[0],SMS.hl[1]),temp83);
            handleFlag7(temp83);
            handleFlag6(temp8,temp83);
            SMS.pc += 2;
            SMS.cycles += 15;
        break;

        case 0x3F:
            temp8 = SMS.af[0];
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2[7] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            SMS.af[0] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x40:
            temp8 = SMS.bc[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(0);
            tempBitBuffer2[6] = tempBitBuffer[0];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x41:
            temp8 = SMS.bc[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(0);
            tempBitBuffer2[6] = tempBitBuffer[0];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x42:
            temp8 = SMS.de[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(0);
            tempBitBuffer2[6] = tempBitBuffer[0];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x43:
            temp8 = SMS.de[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(0);
            tempBitBuffer2[6] = tempBitBuffer[0];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x44:
            temp8 = SMS.hl[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(0);
            tempBitBuffer2[6] = tempBitBuffer[0];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x45:
            temp8 = SMS.hl[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(0);
            tempBitBuffer2[6] = tempBitBuffer[0];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x46:
            temp8 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(0);
            tempBitBuffer2[6] = tempBitBuffer[0];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x47:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(0);
            tempBitBuffer2[6] = tempBitBuffer[0];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x48:
            temp8 = SMS.bc[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(1);
            tempBitBuffer2[6] = tempBitBuffer[1];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x49:
            temp8 = SMS.bc[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(1);
            tempBitBuffer2[6] = tempBitBuffer[1];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x4A:
            temp8 = SMS.de[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(1);
            tempBitBuffer2[6] = tempBitBuffer[1];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x4B:
            temp8 = SMS.de[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(1);
            tempBitBuffer2[6] = tempBitBuffer[1];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x4C:
            temp8 = SMS.hl[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(1);
            tempBitBuffer2[6] = tempBitBuffer[1];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x4D:
            temp8 = SMS.hl[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(1);
            tempBitBuffer2[6] = tempBitBuffer[1];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x4E:
            temp8 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(1);
            tempBitBuffer2[6] = tempBitBuffer[1];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x4F:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(1);
            tempBitBuffer2[6] = tempBitBuffer[1];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x50:
            temp8 = SMS.bc[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(2);
            tempBitBuffer2[6] = tempBitBuffer[2];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x51:
            temp8 = SMS.bc[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(2);
            tempBitBuffer2[6] = tempBitBuffer[2];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x52:
            temp8 = SMS.de[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(2);
            tempBitBuffer2[6] = tempBitBuffer[2];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x53:
            temp8 = SMS.de[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(2);
            tempBitBuffer2[6] = tempBitBuffer[2];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x54:
            temp8 = SMS.hl[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(2);
            tempBitBuffer2[6] = tempBitBuffer[2];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x55:
            temp8 = SMS.hl[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(2);
            tempBitBuffer2[6] = tempBitBuffer[2];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x56:
            temp8 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(2);
            tempBitBuffer2[6] = tempBitBuffer[2];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x57:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(2);
            tempBitBuffer2[6] = tempBitBuffer[2];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x58:
            temp8 = SMS.bc[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(3);
            tempBitBuffer2[6] = tempBitBuffer[3];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x59:
            temp8 = SMS.bc[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(3);
            tempBitBuffer2[6] = tempBitBuffer[3];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x5A:
            temp8 = SMS.de[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(3);
            tempBitBuffer2[6] = tempBitBuffer[3];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x5B:
            temp8 = SMS.de[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(3);
            tempBitBuffer2[6] = tempBitBuffer[3];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x5C:
            temp8 = SMS.hl[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(3);
            tempBitBuffer2[6] = tempBitBuffer[3];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x5D:
            temp8 = SMS.hl[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(3);
            tempBitBuffer2[6] = tempBitBuffer[3];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x5E:
            temp8 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(3);
            tempBitBuffer2[6] = tempBitBuffer[3];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x5F:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(3);
            tempBitBuffer2[6] = tempBitBuffer[3];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x60:
            temp8 = SMS.bc[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(4);
            tempBitBuffer2[6] = tempBitBuffer[4];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x61:
            temp8 = SMS.bc[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(4);
            tempBitBuffer2[6] = tempBitBuffer[4];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x62:
            temp8 = SMS.de[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(4);
            tempBitBuffer2[6] = tempBitBuffer[4];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x63:
            temp8 = SMS.de[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(4);
            tempBitBuffer2[6] = tempBitBuffer[4];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x64:
            temp8 = SMS.hl[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(4);
            tempBitBuffer2[6] = tempBitBuffer[4];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x65:
            temp8 = SMS.hl[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(4);
            tempBitBuffer2[6] = tempBitBuffer[4];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x66:
            temp8 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(4);
            tempBitBuffer2[6] = tempBitBuffer[4];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x67:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(4);
            tempBitBuffer2[6] = tempBitBuffer[4];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x68:
            temp8 = SMS.bc[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(5);
            tempBitBuffer2[6] = tempBitBuffer[5];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x69:
            temp8 = SMS.bc[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(5);
            tempBitBuffer2[6] = tempBitBuffer[5];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x6A:
            temp8 = SMS.de[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(5);
            tempBitBuffer2[6] = tempBitBuffer[5];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x6B:
            temp8 = SMS.de[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(5);
            tempBitBuffer2[6] = tempBitBuffer[5];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x6C:
            temp8 = SMS.hl[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(5);
            tempBitBuffer2[6] = tempBitBuffer[5];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x6D:
            temp8 = SMS.hl[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(5);
            tempBitBuffer2[6] = tempBitBuffer[5];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x6E:
            temp8 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(5);
            tempBitBuffer2[6] = tempBitBuffer[5];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x6F:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(5);
            tempBitBuffer2[6] = tempBitBuffer[5];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x70:
            temp8 = SMS.bc[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(6);
            tempBitBuffer2[6] = tempBitBuffer[6];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x71:
            temp8 = SMS.bc[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(6);
            tempBitBuffer2[6] = tempBitBuffer[6];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x72:
            temp8 = SMS.de[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(6);
            tempBitBuffer2[6] = tempBitBuffer[6];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x73:
            temp8 = SMS.de[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(6);
            tempBitBuffer2[6] = tempBitBuffer[6];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x74:
            temp8 = SMS.hl[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(6);
            tempBitBuffer2[6] = tempBitBuffer[6];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x75:
            temp8 = SMS.hl[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(6);
            tempBitBuffer2[6] = tempBitBuffer[6];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x76:
            temp8 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(6);
            tempBitBuffer2[6] = tempBitBuffer[6];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x77:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(6);
            tempBitBuffer2[6] = tempBitBuffer[6];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x78:
            temp8 = SMS.bc[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(7);
            tempBitBuffer2[6] = tempBitBuffer[7];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x79:
            temp8 = SMS.bc[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(7);
            tempBitBuffer2[6] = tempBitBuffer[7];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x7A:
            temp8 = SMS.de[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(7);
            tempBitBuffer2[6] = tempBitBuffer[7];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x7B:
            temp8 = SMS.de[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(7);
            tempBitBuffer2[6] = tempBitBuffer[7];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x7C:
            temp8 = SMS.hl[0];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(7);
            tempBitBuffer2[6] = tempBitBuffer[7];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x7D:
            temp8 = SMS.hl[1];
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(7);
            tempBitBuffer2[6] = tempBitBuffer[7];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x7E:
            temp8 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            tempBitBuffer = temp8;
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(7);
            tempBitBuffer2[6] = tempBitBuffer[7];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x7F:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2 = SMS.af[1];
            tempBitBuffer.flip(3);
            tempBitBuffer2[6] = tempBitBuffer[7];
            tempBitBuffer2[4] = 1;
            tempBitBuffer2[1] = 0;
            SMS.af[1] = tempBitBuffer2.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x80:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[0] = 0;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x81:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[0] = 0;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x82:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[0] = 0;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x83:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[0] = 0;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x84:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[0] = 0;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x85:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[0] = 0;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x86:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[0] = 0;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x87:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[0] = 0;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x88:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[1] = 0;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x89:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[1] = 0;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x8A:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[1] = 0;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x8B:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[1] = 0;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x8C:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[1] = 0;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x8D:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[1] = 0;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x8E:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[1] = 0;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x8F:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[1] = 0;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x90:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[2] = 0;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x91:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[2] = 0;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x92:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[2] = 0;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x93:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[2] = 0;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x94:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[2] = 0;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x95:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[2] = 0;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x96:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[2] = 0;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x97:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[2] = 0;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x98:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[3] = 0;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x99:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[3] = 0;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x9A:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[3] = 0;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x9B:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[3] = 0;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x9C:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[3] = 0;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x9D:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[3] = 0;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x9E:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[3] = 0;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0x9F:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[3] = 0;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xA0:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[4] = 0;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xA1:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[4] = 0;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xA2:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[4] = 0;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xA3:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[4] = 0;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xA4:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[4] = 0;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xA5:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[4] = 0;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xA6:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[4] = 0;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xA7:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[4] = 0;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xA8:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[5] = 0;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xA9:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[5] = 0;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xAA:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[5] = 0;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xAB:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[5] = 0;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xAC:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[5] = 0;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xAD:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[5] = 0;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xAE:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[5] = 0;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xAF:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[5] = 0;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xB0:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[6] = 0;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xB1:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[6] = 0;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xB2:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[6] = 0;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xB3:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[6] = 0;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xB4:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[6] = 0;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xB5:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[6] = 0;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xB6:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[6] = 0;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xB7:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[6] = 0;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xB8:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[7] = 0;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xB9:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[7] = 0;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xBA:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[7] = 0;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xBB:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[7] = 0;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xBC:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[7] = 0;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xBD:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[7] = 0;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xBE:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[7] = 0;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xBF:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[7] = 0;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xC0:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[0] = 1;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xC1:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[0] = 1;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xC2:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[0] = 1;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xC3:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[0] = 1;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xC4:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[0] = 1;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xC5:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[0] = 1;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xC6:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[0] = 1;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xC7:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[0] = 1;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xC8:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[1] = 1;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xC9:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[1] = 1;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xCA:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[1] = 1;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xCB:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[1] = 1;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xCC:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[1] = 1;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xCD:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[1] = 1;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xCE:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[1] = 1;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xCF:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[1] = 1;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xD0:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[2] = 1;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xD1:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[2] = 1;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xD2:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[2] = 1;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xD3:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[2] = 1;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xD4:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[2] = 1;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xD5:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[2] = 1;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xD6:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[2] = 1;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xD7:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[2] = 1;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xD8:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[3] = 1;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xD9:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[3] = 1;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xDA:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[3] = 1;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xDB:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[3] = 1;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xDC:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[3] = 1;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xDD:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[3] = 1;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xDE:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[3] = 1;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xDF:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[3] = 1;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xE0:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[4] = 1;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xE1:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[4] = 1;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xE2:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[4] = 1;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xE3:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[4] = 1;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xE4:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[4] = 1;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xE5:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[4] = 1;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xE6:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[4] = 1;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xE7:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[4] = 1;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xE8:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[5] = 1;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xE9:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[5] = 1;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xEA:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[5] = 1;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xEB:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[5] = 1;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xEC:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[5] = 1;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xED:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[5] = 1;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xEE:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[5] = 1;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xEF:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[5] = 1;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xF0:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[6] = 1;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xF1:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[6] = 1;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xF2:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[6] = 1;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xF3:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[6] = 1;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xF4:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[6] = 1;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xF5:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[6] = 1;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xF6:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[6] = 1;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xF7:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[6] = 1;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xF8:
            tempBitBuffer = SMS.bc[0];
            tempBitBuffer[7] = 1;
            SMS.bc[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xF9:
            tempBitBuffer = SMS.bc[1];
            tempBitBuffer[7] = 1;
            SMS.bc[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xFA:
            tempBitBuffer = SMS.de[0];
            tempBitBuffer[7] = 1;
            SMS.de[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xFB:
            tempBitBuffer = SMS.de[1];
            tempBitBuffer[7] = 1;
            SMS.de[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xFC:
            tempBitBuffer = SMS.hl[0];
            tempBitBuffer[7] = 1;
            SMS.hl[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xFD:
            tempBitBuffer = SMS.hl[1];
            tempBitBuffer[7] = 1;
            SMS.hl[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xFE:
            tempBitBuffer = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            tempBitBuffer[7] = 1;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        case 0xFF:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer[7] = 1;
            SMS.af[0] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 8;
        break;

        default:
            printf("CPU ERROR: UNKNOWN CB OPCODE 0x%X!\n",SMS.z80Memory[SMS.pc + 1]);
            printf("PC: 0x%X\n",SMS.pc);
            printf("SP: 0x%X\n",SMS.sp);
            printf("AF: 0x%X%X\n",SMS.af[0],SMS.af[1]);
            printf("BC: 0x%X%X\n",SMS.bc[0],SMS.bc[1]);
            printf("DE: 0x%X%X\n",SMS.de[0],SMS.de[1]);
            printf("HL: 0x%X%X\n",SMS.hl[0],SMS.hl[1]);
            printf("IX 0x%X\n",SMS.ix);
            printf("IY 0x%X\n",SMS.iy);
            printf("Cycles: %i\n",SMS.cycles);
            memDump();
            closenopMD = true;
        break;
    }
}
void doz80opcode()
{
    switch(SMS.z80Memory[SMS.pc])
    {
        case 0x00:
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x01:
            SMS.bc[0] = SMS.z80Memory[SMS.pc + 2];
            SMS.bc[1] = SMS.z80Memory[SMS.pc + 1];
            SMS.pc += 3;
            SMS.cycles += 10;
        break;

        case 0x02:
            doSMSmemWrite(getREGpair16(SMS.bc[0],SMS.bc[1]),SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x03:
            temp16 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            temp16++;
            SMS.bc[0] = temp16 >> 8;
            SMS.bc[1] = temp16;
            SMS.pc++;
            SMS.cycles += 6;
        break;

        case 0x04:
            temp8 = SMS.bc[0];
            SMS.bc[0]++;
            temp82 = SMS.bc[0];
            handleFlag1(0);
            handleFlag7(temp82);
            handleFlag6(temp8,temp82);
            handleFlag5(temp82);
            handleFlag4(temp8,temp82);
            handleFlag3(temp82);
            handleFlag2(temp8,temp82,1);
            SMS.pc += 1;
            SMS.cycles += 4;
        break;

        case 0x05:
            temp8 = SMS.bc[0];
            SMS.bc[0]--;
            handleFlag1(1);
            handleFlag7(SMS.bc[0]);
            handleFlag6(temp8,SMS.bc[0]);
            handleFlag5(SMS.bc[0]);
            handleFlag4(temp8,SMS.bc[0]);
            handleFlag3(SMS.bc[0]);
            handleFlag2(temp8,SMS.bc[0],1);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x06:
            SMS.bc[0] = doSMSmemRead(SMS.pc + 1);
            SMS.pc += 2;
            SMS.cycles += 7;
        break;

        case 0x07:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            tempBitBuffer[0] = tempBitBuffer2[7];
            SMS.af[0] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.af[0]);
            handleFlag5(SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x08:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.afS[0];
            SMS.afS[0] = temp8;
            temp8 = SMS.af[1];
            SMS.af[1] = SMS.afS[1];
            SMS.afS[1] = temp8;
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x09:
            temp16 = getREGpair16(SMS.bc[0], SMS.bc[1]);
            temp162 = getREGpair16(SMS.hl[0], SMS.hl[1]);
            temp163 = temp162;
            temp162 += temp16;
            SMS.hl[0] = temp162 >> 8;
            SMS.hl[1] = temp162;
            handleFlag1(0);
            handleFlag5(temp162 >> 8);
            handleFlag4(temp16,temp162);
            handleFlag3(temp162 >> 8);
            handleFlag016add(temp163, temp162);
            SMS.pc++;
            SMS.cycles += 11;
        break;

        case 0x0A:
            SMS.af[0] = doSMSmemRead(getREGpair16(SMS.bc[0],SMS.bc[1]));
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x0B:
            temp16 = getREGpair16(SMS.bc[0],SMS.bc[1]);
            temp16--;
            SMS.bc[0] = temp16 >> 8;
            SMS.bc[1] = temp16;
            SMS.pc++;
            SMS.cycles += 6;
        break;

        case 0x0C:
            temp8 = SMS.bc[1];
            SMS.bc[1]++;
            temp82 = SMS.bc[1];
            handleFlag1(0);
            handleFlag7(temp82);
            handleFlag6(temp8,temp82);
            handleFlag5(temp82);
            handleFlag4(temp8,temp82);
            handleFlag3(temp82);
            handleFlag2(temp8,temp82,1);
            SMS.pc += 1;
            SMS.cycles += 4;
        break;

        case 0x0D:
            temp8 = SMS.bc[1];
            SMS.bc[1]--;
            handleFlag1(1);
            handleFlag7(SMS.bc[1]);
            handleFlag6(temp8,SMS.bc[1]);
            handleFlag5(SMS.bc[1]);
            handleFlag4(temp8,SMS.bc[1]);
            handleFlag3(SMS.bc[1]);
            handleFlag2(temp8,SMS.bc[1],1);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x0E:
            SMS.bc[1] = doSMSmemRead(SMS.pc + 1);
            SMS.pc += 2;
            SMS.cycles += 7;
        break;

        case 0x0F:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2[0] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            tempBitBuffer[7] = tempBitBuffer2[0];
            SMS.af[0] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[0];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.af[0]);
            handleFlag5(SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x10:
            SMS.bc[0]--;
            if(SMS.bc[0] != 0)
            {
                tempBitBuffer2 = SMS.z80Memory[SMS.pc + 1];
                if(tempBitBuffer2[7] == 1)
                {
                    temp8 = SMS.z80Memory[SMS.pc + 1];
                    tempBitBuffer3 = temp8;
                    tempBitBuffer3.flip();
                    temp8 = tempBitBuffer3.to_ulong();
                    SMS.pc -= temp8;
                    SMS.pc += 1;
                    SMS.cycles += 12;
                }
                if(tempBitBuffer2[7] == 0)
                {
                    temp8 = SMS.z80Memory[SMS.pc + 1];
                    temp8 = temp8 << 1;
                    temp8 = temp8 >> 1;
                    SMS.pc += temp8;
                    SMS.pc += 2;
                    SMS.cycles += 12;
                }
            }
            if(SMS.bc[0] == 0)
            {
                SMS.pc += 2;
                SMS.cycles += 7;
            }
        break;

        case 0x11:
            SMS.de[0] = SMS.z80Memory[SMS.pc + 2];
            SMS.de[1] = SMS.z80Memory[SMS.pc + 1];
            SMS.pc += 3;
            SMS.cycles += 10;
        break;

        case 0x12:
            doSMSmemWrite(getREGpair16(SMS.de[0],SMS.de[1]),SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x13:
            temp16 = getREGpair16(SMS.de[0],SMS.de[1]);
            temp16++;
            SMS.de[0] = temp16 >> 8;
            SMS.de[1] = temp16;
            SMS.pc++;
            SMS.cycles += 6;
        break;

        case 0x14:
            temp8 = SMS.de[0];
            SMS.de[0]++;
            temp82 = SMS.de[0];
            handleFlag1(0);
            handleFlag7(temp82);
            handleFlag6(temp8,temp82);
            handleFlag5(temp82);
            handleFlag4(temp8,temp82);
            handleFlag3(temp82);
            handleFlag2(temp8,temp82,1);
            SMS.pc += 1;
            SMS.cycles += 4;
        break;

        case 0x15:
            temp8 = SMS.de[0];
            SMS.de[0]--;
            handleFlag1(1);
            handleFlag7(SMS.de[0]);
            handleFlag6(temp8,SMS.de[0]);
            handleFlag5(SMS.de[0]);
            handleFlag4(temp8,SMS.de[0]);
            handleFlag3(SMS.de[0]);
            handleFlag2(temp8,SMS.de[0],1);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x16:
            SMS.de[0] = doSMSmemRead(SMS.pc + 1);
            SMS.pc += 2;
            SMS.cycles += 7;
        break;

        case 0x17:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2[7] = tempBitBuffer[7];
            tempBitBuffer = tempBitBuffer << 1;
            SMS.af[0] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[7];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.af[0]);
            handleFlag5(SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x18:
            tempBitBuffer2 = SMS.z80Memory[SMS.pc + 1];
            if(tempBitBuffer2[7] == 1)
            {
                temp8 = SMS.z80Memory[SMS.pc + 1];
                tempBitBuffer3 = temp8;
                tempBitBuffer3.flip();
                temp8 = tempBitBuffer3.to_ulong();
                SMS.pc -= temp8;
                SMS.pc += 1;
                SMS.cycles += 12;
            }
            if(tempBitBuffer2[7] == 0)
            {
                temp8 = SMS.z80Memory[SMS.pc + 1];
                temp8 = temp8 << 1;
                temp8 = temp8 >> 1;
                SMS.pc += temp8;
                SMS.pc += 2;
                SMS.cycles += 12;
            }
        break;

        case 0x19:
            temp16 = getREGpair16(SMS.de[0], SMS.de[1]);
            temp162 = getREGpair16(SMS.hl[0], SMS.hl[1]);
            temp163 = temp162;
            temp162 += temp16;
            SMS.hl[0] = temp162 >> 8;
            SMS.hl[1] = temp162;
            handleFlag5(temp162 >> 8);
            handleFlag4(temp8,temp162);
            handleFlag3(temp162 >> 8);
            handleFlag1(0);
            handleFlag016add(temp163, temp162);
            SMS.pc++;
            SMS.cycles += 11;
        break;

        case 0x1A:
            SMS.af[0] = doSMSmemRead(getREGpair16(SMS.de[0],SMS.de[1]));
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x1B:
            temp16 = getREGpair16(SMS.de[0],SMS.de[1]);
            temp16--;
            SMS.de[0] = temp16 >> 8;
            SMS.de[1] = temp16;
            SMS.pc++;
            SMS.cycles += 6;
        break;

        case 0x1C:
            temp8 = SMS.de[1];
            SMS.de[1]++;
            temp82 = SMS.de[1];
            handleFlag1(0);
            handleFlag7(temp82);
            handleFlag6(temp8,temp82);
            handleFlag5(temp82);
            handleFlag4(temp8,temp82);
            handleFlag3(temp82);
            handleFlag2(temp8,temp82,1);
            SMS.pc += 1;
            SMS.cycles += 4;
        break;

        case 0x1D:
            temp8 = SMS.de[1];
            SMS.de[1]--;
            handleFlag1(1);
            handleFlag7(SMS.de[1]);
            handleFlag6(temp8,SMS.de[1]);
            handleFlag5(SMS.de[1]);
            handleFlag4(temp8,SMS.de[1]);
            handleFlag3(SMS.de[1]);
            handleFlag2(temp8,SMS.de[1],1);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x1E:
            SMS.de[1] = doSMSmemRead(SMS.pc + 1);
            SMS.pc += 2;
            SMS.cycles += 7;
        break;

        case 0x1F:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer2[0] = tempBitBuffer[0];
            tempBitBuffer = tempBitBuffer >> 1;
            SMS.af[0] = tempBitBuffer.to_ulong();
            tempBitBuffer3 = SMS.af[1];
            tempBitBuffer3[0] = tempBitBuffer2[0];
            tempBitBuffer3[1] = 0;
            tempBitBuffer3[4] = 0;
            SMS.af[1] = tempBitBuffer3.to_ulong();
            handleFlag3(SMS.af[0]);
            handleFlag5(SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x20:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[6] == 0)
            {
                tempBitBuffer2 = SMS.z80Memory[SMS.pc + 1];
                if(tempBitBuffer2[7] == 1)
                {
                    temp8 = SMS.z80Memory[SMS.pc + 1];
                    tempBitBuffer3 = temp8;
                    tempBitBuffer3.flip();
                    temp8 = tempBitBuffer3.to_ulong();
                    SMS.pc -= temp8;
                    SMS.pc += 1;
                    SMS.cycles += 12;
                }
                if(tempBitBuffer2[7] == 0)
                {
                    temp8 = SMS.z80Memory[SMS.pc + 1];
                    temp8 = temp8 << 1;
                    temp8 = temp8 >> 1;
                    SMS.pc += temp8;
                    SMS.pc += 2;
                    SMS.cycles += 12;
                }
            }
            if(tempBitBuffer[6] == 1)
            {
                SMS.pc += 2;
                SMS.cycles += 7;
            }
        break;

        case 0x21:
            SMS.hl[0] = SMS.z80Memory[SMS.pc + 2];
            SMS.hl[1] = SMS.z80Memory[SMS.pc + 1];
            SMS.pc += 3;
            SMS.cycles += 10;
        break;

        case 0x22:
            temp16 = SMS.z80Memory[SMS.pc + 2] << 8 | SMS.z80Memory[SMS.pc + 1];
            doSMSmemWrite(temp16 + 1, SMS.hl[0]);
            doSMSmemWrite(temp16, SMS.hl[1]);
            SMS.pc += 3;
            SMS.cycles += 16;
        break;

        case 0x23:
            //breakpoint = true;
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp16++;
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            SMS.pc++;
            SMS.cycles += 6;
        break;

        case 0x24:
            temp8 = SMS.hl[0];
            SMS.hl[0]++;
            temp82 = SMS.hl[0];
            handleFlag1(0);
            handleFlag7(temp82);
            handleFlag6(temp8,temp82);
            handleFlag5(temp82);
            handleFlag4(temp8,temp82);
            handleFlag3(temp82);
            handleFlag2(temp8,temp82,1);
            SMS.pc += 1;
            SMS.cycles += 4;
        break;

        case 0x25:
            temp8 = SMS.hl[0];
            SMS.hl[0]--;
            handleFlag1(1);
            handleFlag7(SMS.hl[0]);
            handleFlag6(temp8,SMS.hl[0]);
            handleFlag5(SMS.hl[0]);
            handleFlag4(temp8,SMS.hl[0]);
            handleFlag3(SMS.hl[0]);
            handleFlag2(temp8,SMS.hl[0],1);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x26:
            SMS.hl[0] = doSMSmemRead(SMS.pc + 1);
            SMS.pc += 2;
            SMS.cycles += 7;
        break;

        case 0x27:
            printf("DAA!\n");
            SMS.pc++;
        break;

        case 0x28:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[6] == 1)
            {
                tempBitBuffer2 = SMS.z80Memory[SMS.pc + 1];
                if(tempBitBuffer2[7] == 1)
                {
                    temp8 = SMS.z80Memory[SMS.pc + 1];
                    tempBitBuffer3 = temp8;
                    tempBitBuffer3.flip();
                    temp8 = tempBitBuffer3.to_ulong();
                    SMS.pc -= temp8;
                    SMS.pc += 1;
                    SMS.cycles += 12;
                }
                if(tempBitBuffer2[7] == 0)
                {
                    temp8 = SMS.z80Memory[SMS.pc + 1];
                    temp8 = temp8 << 1;
                    temp8 = temp8 >> 1;
                    SMS.pc += temp8;
                    SMS.pc += 2;
                    SMS.cycles += 12;
                }
            }
            if(tempBitBuffer[6] == 0)
            {
                SMS.pc += 2;
                SMS.cycles += 7;
            }
        break;

        case 0x29:
            temp16 = getREGpair16(SMS.hl[0], SMS.hl[1]);
            temp162 = getREGpair16(SMS.hl[0], SMS.hl[1]);
            temp163 = temp162;
            temp162 += temp16;
            SMS.hl[0] = temp162 >> 8;
            SMS.hl[1] = temp162;
            handleFlag1(0);
            handleFlag5(temp162 >> 8);
            handleFlag4(temp16,temp162);
            handleFlag3(temp162 >> 8);
            handleFlag016add(temp163, temp162);
            SMS.pc++;
            SMS.cycles += 11;
        break;

        case 0x2A:
            temp16 = SMS.z80Memory[SMS.pc + 2] << 8 | SMS.z80Memory[SMS.pc + 1];
            SMS.hl[0] = doSMSmemRead(temp16 + 1);
            SMS.hl[1] = doSMSmemRead(temp16);
            SMS.pc += 3;
            SMS.cycles += 16;
        break;

        case 0x2B:
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            temp16--;
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            SMS.pc++;
            SMS.cycles += 6;
        break;

        case 0x2C:
            temp8 = SMS.hl[1];
            SMS.hl[1]++;
            temp82 = SMS.hl[1];
            handleFlag1(0);
            handleFlag7(temp82);
            handleFlag6(temp8,temp82);
            handleFlag5(temp82);
            handleFlag4(temp8,temp82);
            handleFlag3(temp82);
            handleFlag2(temp8,temp82,1);
            SMS.pc += 1;
            SMS.cycles += 4;
        break;

        case 0x2D:
            temp8 = SMS.hl[1];
            SMS.hl[1]--;
            handleFlag1(1);
            handleFlag7(SMS.hl[1]);
            handleFlag6(temp8,SMS.hl[1]);
            handleFlag5(SMS.hl[1]);
            handleFlag4(temp8,SMS.hl[1]);
            handleFlag3(SMS.hl[1]);
            handleFlag2(temp8,SMS.hl[1],1);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x2E:
            SMS.hl[1] = doSMSmemRead(SMS.pc + 1);
            SMS.pc += 2;
            SMS.cycles += 7;
        break;

        case 0x2F:
            tempBitBuffer = SMS.af[0];
            tempBitBuffer.flip();
            SMS.af[0] = tempBitBuffer.to_ulong();
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[1] = 1;
            tempBitBuffer[4] = 1;
            SMS.af[1] = tempBitBuffer.to_ulong();
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x30:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[0] == 0)
            {
                tempBitBuffer2 = SMS.z80Memory[SMS.pc + 1];
                if(tempBitBuffer2[7] == 1)
                {
                    temp8 = SMS.z80Memory[SMS.pc + 1];
                    tempBitBuffer3 = temp8;
                    tempBitBuffer3.flip();
                    temp8 = tempBitBuffer3.to_ulong();
                    SMS.pc -= temp8;
                    SMS.pc += 1;
                    SMS.cycles += 12;
                }
                if(tempBitBuffer2[7] == 0)
                {
                    temp8 = SMS.z80Memory[SMS.pc + 1];
                    temp8 = temp8 << 1;
                    temp8 = temp8 >> 1;
                    SMS.pc += temp8;
                    SMS.pc += 2;
                    SMS.cycles += 12;
                }
            }
            if(tempBitBuffer[0] == 1)
            {
                SMS.pc += 2;
                SMS.cycles += 7;
            }
        break;

        case 0x31:
            SMS.sp = SMS.z80Memory[SMS.pc + 2] << 8 | SMS.z80Memory[SMS.pc + 1];
            SMS.pc += 3;
            SMS.cycles += 10;
        break;

        case 0x32:
            doSMSmemWrite(getREGpair16(SMS.z80Memory[SMS.pc + 2],SMS.z80Memory[SMS.pc + 1]),SMS.af[0]);
            SMS.pc += 3;
            SMS.cycles += 13;
        break;

        case 0x33:
            SMS.sp++;
            SMS.pc++;
            SMS.cycles += 6;
        break;

        case 0x34:
            temp8 = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            temp83 = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            temp83++;
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])]++;
            temp82 = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            handleFlag1(0);
            handleFlag7(temp82);
            handleFlag6(temp8,temp82);
            handleFlag5(temp82);
            handleFlag4(temp8,temp82);
            handleFlag3(temp82);
            handleFlag2(temp8,temp82,1);
            SMS.pc += 1;
            SMS.cycles += 11;
        break;

        case 0x35:
            temp8 = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])]--;
            handleFlag1(1);
            handleFlag7(SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])]);
            handleFlag6(temp8,SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])]);
            handleFlag5(SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])]);
            handleFlag4(temp8,SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])]);
            handleFlag3(SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])]);
            handleFlag2(temp8,SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])],1);
            SMS.pc++;
            SMS.cycles += 11;
        break;

        case 0x36:
            doSMSmemWrite( getREGpair16(SMS.hl[0], SMS.hl[1]) , doSMSmemRead(SMS.pc + 1) );
            SMS.pc += 2;
            SMS.cycles += 10;
        break;

        case 0x37:
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[0] = 1;
            tempBitBuffer[1] = 0;
            tempBitBuffer[4] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x38:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[0] == 1)
            {
                tempBitBuffer2 = SMS.z80Memory[SMS.pc + 1];
                if(tempBitBuffer2[7] == 1)
                {
                    temp8 = SMS.z80Memory[SMS.pc + 1];
                    tempBitBuffer3 = temp8;
                    tempBitBuffer3.flip();
                    temp8 = tempBitBuffer3.to_ulong();
                    SMS.pc -= temp8;
                    SMS.pc += 1;
                    SMS.cycles += 12;
                }
                if(tempBitBuffer2[7] == 0)
                {
                    temp8 = SMS.z80Memory[SMS.pc + 1];
                    temp8 = temp8 << 1;
                    temp8 = temp8 >> 1;
                    SMS.pc += temp8;
                    SMS.pc += 2;
                    SMS.cycles += 12;
                }
            }
            if(tempBitBuffer[0] == 0)
            {
                SMS.pc += 2;
                SMS.cycles += 7;
            }
        break;

        case 0x39:
            temp162 = getREGpair16(SMS.hl[0], SMS.hl[1]);
            temp163 = temp162;
            temp162 += SMS.sp;
            SMS.hl[0] = temp162 >> 8;
            SMS.hl[1] = temp162;
            handleFlag5(temp162 >> 8);
            handleFlag4(temp163,temp162);
            handleFlag3(temp162 >> 8);
            handleFlag1(0);
            handleFlag016add(temp163, temp162);
            SMS.pc++;
            SMS.cycles += 11;
        break;

        case 0x3A:
            SMS.af[0] = doSMSmemRead(getREGpair16(SMS.z80Memory[SMS.pc + 2],SMS.z80Memory[SMS.pc + 1]));
            SMS.pc += 3;
            SMS.cycles += 13;
        break;

        case 0x3B:
            SMS.sp--;
            SMS.pc++;
            SMS.cycles += 6;
        break;

        case 0x3C:
            temp8 = SMS.af[0];
            SMS.af[0]++;
            handleFlag1(0);
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag4(temp8,SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],1);
            SMS.pc += 1;
            SMS.cycles += 4;
        break;

        case 0x3D:
            temp8 = SMS.af[0];
            SMS.af[0]--;
            handleFlag1(1);
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag4(temp8,SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],1);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x3E:
            SMS.af[0] = doSMSmemRead(SMS.pc + 1);
            SMS.pc += 2;
            SMS.cycles += 7;
        break;

        case 0x3F:
            tempBitBuffer = SMS.af[1];
            tempBitBuffer.flip(0);
            tempBitBuffer[1] = 0;
            tempBitBuffer.flip(4);
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x40:
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x41:
            SMS.bc[0] = SMS.bc[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x42:
            SMS.bc[0] = SMS.de[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x43:
            SMS.bc[0] = SMS.de[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x44:
            SMS.bc[0] = SMS.hl[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x45:
            SMS.bc[0] = SMS.hl[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x46:
            SMS.bc[0] = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x47:
            SMS.bc[0] = SMS.af[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x48:
            SMS.bc[1] = SMS.bc[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x49:
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x4A:
            SMS.bc[1] = SMS.de[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x4B:
            SMS.bc[1] = SMS.de[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x4C:
            SMS.bc[1] = SMS.hl[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x4D:
            SMS.bc[1] = SMS.hl[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x4E:
            SMS.bc[1] = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x4F:
            SMS.bc[1] = SMS.af[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x50:
            SMS.de[0] = SMS.bc[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x51:
            SMS.de[0] = SMS.bc[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x52:
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x53:
            SMS.de[0] = SMS.de[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x54:
            SMS.de[0] = SMS.hl[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x55:
            SMS.de[0] = SMS.hl[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x56:
            SMS.de[0] = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x57:
            SMS.de[0] = SMS.af[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x58:
            SMS.de[1] = SMS.bc[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x59:
            SMS.de[1] = SMS.bc[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x5A:
            SMS.de[1] = SMS.de[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x5B:
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x5C:
            SMS.de[1] = SMS.hl[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x5D:
            SMS.de[1] = SMS.hl[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x5E:
            SMS.de[1] = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x5F:
            SMS.de[1] = SMS.af[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x60:
            SMS.hl[0] = SMS.bc[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x61:
            SMS.hl[0] = SMS.bc[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x62:
            SMS.hl[0] = SMS.de[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x63:
            SMS.hl[0] = SMS.de[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x64:
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x65:
            SMS.hl[0] = SMS.hl[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x66:
            SMS.hl[0] = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x67:
            SMS.hl[0] = SMS.af[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x68:
            SMS.hl[1] = SMS.bc[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x69:
            SMS.hl[1] = SMS.bc[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x6A:
            SMS.hl[1] = SMS.de[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x6B:
            SMS.hl[1] = SMS.de[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x6C:
            SMS.hl[1] = SMS.hl[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x6D:
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x6E:
            SMS.hl[1] = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x6F:
            SMS.hl[1] = SMS.af[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x70:
            doSMSmemWrite(getREGpair16(SMS.hl[0],SMS.hl[1]),SMS.bc[0]);
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x71:
            doSMSmemWrite(getREGpair16(SMS.hl[0],SMS.hl[1]),SMS.bc[1]);
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x72:
            doSMSmemWrite(getREGpair16(SMS.hl[0],SMS.hl[1]),SMS.de[0]);
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x73:
            doSMSmemWrite(getREGpair16(SMS.hl[0],SMS.hl[1]),SMS.de[1]);
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x74:
            doSMSmemWrite(getREGpair16(SMS.hl[0],SMS.hl[1]),SMS.hl[0]);
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x75:
            doSMSmemWrite(getREGpair16(SMS.hl[0],SMS.hl[1]),SMS.hl[1]);
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x76:
            //printf("StubHALT\n");
            SMS.pc++;
        break;

        case 0x77:
            doSMSmemWrite(getREGpair16(SMS.hl[0],SMS.hl[1]),SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x78:
            SMS.af[0] = SMS.bc[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x79:
            SMS.af[0] = SMS.bc[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x7A:
            SMS.af[0] = SMS.de[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x7B:
            SMS.af[0] = SMS.de[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x7C:
            SMS.af[0] = SMS.hl[0];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x7D:
            SMS.af[0] = SMS.hl[1];
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x7E:
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            SMS.af[0] = doSMSmemRead(temp16);
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x7F:
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x80:
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.bc[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x81:
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.bc[1];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;


        case 0x82:
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.de[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;


        case 0x83:
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.de[1];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;


        case 0x84:
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.hl[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;


        case 0x85:
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.hl[1];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;


        case 0x86:
            temp82 = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            temp8 = SMS.af[0];
            SMS.af[0] += temp82;
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x87:
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.af[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x88:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.bc[0];
            SMS.af[0] += tempBitBuffer[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x89:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.bc[1];
            SMS.af[0] += tempBitBuffer[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x8A:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.de[0];
            SMS.af[0] += tempBitBuffer[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x8B:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.de[1];
            SMS.af[0] += tempBitBuffer[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x8C:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.hl[0];
            SMS.af[0] += tempBitBuffer[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x8D:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.hl[1];
            SMS.af[0] += tempBitBuffer[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x8E:
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.z80Memory[temp16];
            SMS.af[0] += tempBitBuffer[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x8F:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.af[0];
            SMS.af[0] += tempBitBuffer[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x90:
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.bc[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x91:
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.bc[1];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x92:
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.de[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x93:
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.de[1];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x94:
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.hl[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x95:
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.hl[1];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x96:
            temp82 = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            temp8 = SMS.af[0];
            SMS.af[0] -= temp82;
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x97:
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.af[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x98:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.bc[0];
            SMS.af[0] -= tempBitBuffer[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x99:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.bc[1];
            SMS.af[0] -= tempBitBuffer[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x9A:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.de[0];
            SMS.af[0] -= tempBitBuffer[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x9B:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.de[1];
            SMS.af[0] -= tempBitBuffer[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x9C:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.hl[0];
            SMS.af[0] -= tempBitBuffer[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x9D:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.hl[1];
            SMS.af[0] -= tempBitBuffer[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0x9E:
            temp16 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.z80Memory[temp16];
            SMS.af[0] -= tempBitBuffer[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0x9F:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.af[0];
            SMS.af[0] -= tempBitBuffer[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xA0:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] & SMS.bc[0];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 1;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xA1:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] & SMS.bc[1];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 1;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xA2:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] & SMS.de[0];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 1;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xA3:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] & SMS.de[1];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 1;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xA4:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] & SMS.hl[0];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 1;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xA5:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] & SMS.hl[1];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 1;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xA6:
            temp8 = SMS.af[0];
            temp8 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            SMS.af[0] = SMS.af[0] & temp8;
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 1;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xA7:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] & SMS.af[0];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 1;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xA8:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] ^ SMS.bc[0];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xA9:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] ^ SMS.bc[1];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xAA:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] ^ SMS.de[0];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xAB:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] ^ SMS.de[1];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xAC:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] ^ SMS.hl[0];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xAD:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] ^ SMS.hl[1];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xAE:
            temp8 = SMS.af[0];
            temp82 = SMS.z80Memory[getREGpair16(SMS.hl[0],SMS.hl[1])];
            SMS.af[0] = SMS.af[0] ^ temp82;
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xAF:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] ^ SMS.af[0];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xB0:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] | SMS.bc[0];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xB1:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] | SMS.bc[1];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xB2:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] | SMS.de[0];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xB3:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] | SMS.de[1];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xB4:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] | SMS.hl[0];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xB5:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] | SMS.hl[1];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xB6:
            temp8 = SMS.af[0];
            temp8 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            SMS.af[0] = SMS.af[0] | temp8;
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 7;
        break;

        case 0xB7:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] | SMS.af[0];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xB8:
            //breakpoint = true;
            temp8 = SMS.af[0] - SMS.bc[0];
            handleAllFlag8sub(SMS.af[0],temp8);
            SMS.pc += 1;
            SMS.cycles += 7;
        break;

        case 0xB9:
            //breakpoint = true;
            temp8 = SMS.af[0] - SMS.bc[1];
            handleAllFlag8sub(SMS.af[0],temp8);
            SMS.pc += 1;
            SMS.cycles += 7;
        break;

        case 0xBA:
            //breakpoint = true;
            temp8 = SMS.af[0] - SMS.de[0];
            handleAllFlag8sub(SMS.af[0],temp8);
            SMS.pc += 1;
            SMS.cycles += 7;
        break;

        case 0xBB:
            //breakpoint = true;
            temp8 = SMS.af[0] - SMS.de[1];
            handleAllFlag8sub(SMS.af[0],temp8);
            SMS.pc += 1;
            SMS.cycles += 7;
        break;

        case 0xBC:
            //breakpoint = true;
            temp8 = SMS.af[0] - SMS.hl[0];
            handleAllFlag8sub(SMS.af[0],temp8);
            SMS.pc += 1;
            SMS.cycles += 7;
        break;

        case 0xBD:
            //breakpoint = true;
            temp8 = SMS.af[0] - SMS.hl[1];
            handleAllFlag8sub(SMS.af[0],temp8);
            SMS.pc += 1;
            SMS.cycles += 7;
        break;

        case 0xBE:
            //breakpoint = true;
            temp82 = doSMSmemRead(getREGpair16(SMS.hl[0],SMS.hl[1]));
            temp8 = SMS.af[0] - temp82;
            handleAllFlag8sub(SMS.af[0],temp8);
            SMS.pc += 1;
            SMS.cycles += 7;
        break;

        case 0xBF:
            //breakpoint = true;
            temp8 = SMS.af[0] - SMS.af[0];
            handleAllFlag8sub(SMS.af[0],temp8);
            SMS.pc += 1;
            SMS.cycles += 7;
        break;

        case 0xC0:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[6] == 0)
            {
                SMS.pc = SMS.z80Memory[SMS.sp + 1] << 8 | SMS.z80Memory[SMS.sp];
                SMS.sp += 2;
                SMS.cycles += 11;
            }
            if(tempBitBuffer[6] == 1)
            {
                SMS.pc++;
                SMS.cycles += 5;
            }
        break;

        case 0xC1:
            SMS.bc[0] = SMS.z80Memory[SMS.sp + 1];
            SMS.bc[1] = SMS.z80Memory[SMS.sp];
            SMS.pc++;
            SMS.sp += 2;
            SMS.cycles += 10;
        break;

        case 0xC2:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[6] == 0)
            {
                SMS.pc = SMS.z80Memory[SMS.pc + 2] << 8 | SMS.z80Memory[SMS.pc + 1];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[6] == 1)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xC3:
            SMS.pc = SMS.z80Memory[SMS.pc + 2] << 8 | SMS.z80Memory[SMS.pc + 1];
            SMS.cycles += 10;
        break;

        case 0xC4:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[6] == 0)
            {
                SMS.pc += 3;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc;
                SMS.pc = SMS.z80Memory[SMS.pc - 1] << 8 | SMS.z80Memory[SMS.pc - 2];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[6] == 1)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xC5:
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.bc[0];
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.bc[1];
            SMS.pc++;
            SMS.cycles += 11;
        break;

        case 0xC6:
            temp82 = SMS.z80Memory[SMS.pc + 1];
            temp8 = SMS.af[0];
            SMS.af[0] += temp82;
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 7;
        break;

        case 0xC7:
            SMS.pc += 1;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc;
            SMS.pc = 0x00;
            SMS.cycles += 11;
        break;

        case 0xC8:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[6] == 1)
            {
                SMS.pc = SMS.z80Memory[SMS.sp + 1] << 8 | SMS.z80Memory[SMS.sp];
                SMS.sp += 2;
                SMS.cycles += 11;
            }
            if(tempBitBuffer[6] == 0)
            {
                SMS.pc++;
                SMS.cycles += 5;
            }
        break;

        case 0xC9:
            SMS.pc = SMS.z80Memory[SMS.sp + 1] << 8 | SMS.z80Memory[SMS.sp];
            SMS.sp += 2;
            SMS.cycles += 17;
        break;

        case 0xCA:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[6] == 1)
            {
                SMS.pc = SMS.z80Memory[SMS.pc + 2] << 8 | SMS.z80Memory[SMS.pc + 1];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[6] == 0)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xCB:
            doz80CBopcode(SMS.z80Memory[SMS.pc + 1]);
        break;

        case 0xCC:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[6] == 1)
            {
                SMS.pc += 3;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc;
                SMS.pc = SMS.z80Memory[SMS.pc - 1] << 8 | SMS.z80Memory[SMS.pc - 2];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[6] == 0)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xCD:
            SMS.pc += 3;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc;
            SMS.pc = SMS.z80Memory[SMS.pc - 1] << 8 | SMS.z80Memory[SMS.pc - 2];
            SMS.cycles += 17;
        break;

        case 0xCE:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] += SMS.z80Memory[SMS.pc + 1];
            SMS.af[0] += tempBitBuffer[0];
            handleAllFlag8add(temp8, SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 4;
        break;

        case 0xCF:
            SMS.pc += 1;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc;
            SMS.pc = 0x08;
            SMS.cycles += 11;
        break;

        case 0xD0:
            printf("NOT CARRY D0\n");
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[0] == 0)
            {
                SMS.pc = SMS.z80Memory[SMS.sp + 1] << 8 | SMS.z80Memory[SMS.sp];
                SMS.sp += 2;
                SMS.cycles += 11;
            }
            if(tempBitBuffer[0] == 1)
            {
                SMS.pc++;
                SMS.cycles += 5;
            }
        break;

        case 0xD1:
            SMS.de[0] = SMS.z80Memory[SMS.sp + 1];
            SMS.de[1] = SMS.z80Memory[SMS.sp];
            SMS.pc++;
            SMS.sp += 2;
            SMS.cycles += 10;
        break;

        case 0xD2:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[0] == 0)
            {
                SMS.pc = SMS.z80Memory[SMS.pc + 2] << 8 | SMS.z80Memory[SMS.pc + 1];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[0] == 1)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xD3:
            IOportWRITE(SMS.z80Memory[SMS.pc + 1],SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 11;
        break;

        case 0xD4:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[0] == 0)
            {
                SMS.pc += 3;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc;
                SMS.pc = SMS.z80Memory[SMS.pc - 1] << 8 | SMS.z80Memory[SMS.pc - 2];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[0] == 1)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xD5:
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.de[0];
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.de[1];
            SMS.pc++;
            SMS.cycles += 11;
        break;

        case 0xD6:
            temp8 = SMS.af[0];
            SMS.af[0] -= doSMSmemRead(SMS.pc + 1);
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 7;
        break;

        case 0xD7:
            SMS.pc += 1;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc;
            SMS.pc = 0x10;
            SMS.cycles += 11;
        break;

        case 0xD8:
            printf("RET CARRY D8\n");
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[0] == 1)
            {
                SMS.pc = SMS.z80Memory[SMS.sp + 1] << 8 | SMS.z80Memory[SMS.sp];
                SMS.sp += 2;
                SMS.cycles += 11;
            }
            if(tempBitBuffer[0] == 0)
            {
                SMS.pc++;
                SMS.cycles += 5;
            }
        break;

        case 0xD9:
            temp8 = SMS.bc[0];
            SMS.bc[0] = SMS.bcS[0];
            SMS.bcS[0] = temp8;
            temp8 = SMS.bc[1];
            SMS.bc[1] = SMS.bcS[1];
            SMS.bcS[1] = temp8;
            temp8 = SMS.de[0];
            SMS.de[0] = SMS.deS[0];
            SMS.deS[0] = temp8;
            temp8 = SMS.de[1];
            SMS.de[1] = SMS.deS[1];
            SMS.deS[1] = temp8;
            temp8 = SMS.hl[0];
            SMS.hl[0] = SMS.hlS[0];
            SMS.hlS[0] = temp8;
            temp8 = SMS.hl[1];
            SMS.hl[1] = SMS.hlS[1];
            SMS.hlS[1] = temp8;
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xDA:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[0] == 1)
            {
                SMS.pc = SMS.z80Memory[SMS.pc + 2] << 8 | SMS.z80Memory[SMS.pc + 1];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[0] == 0)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xDB:
            SMS.af[0] = IOportREAD(SMS.z80Memory[SMS.pc + 1]);
            SMS.pc += 2;
            SMS.cycles += 11;
        break;

        case 0xDC:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[0] == 1)
            {
                SMS.pc += 3;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc;
                SMS.pc = SMS.z80Memory[SMS.pc - 1] << 8 | SMS.z80Memory[SMS.pc - 2];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[0] == 0)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xDD:
            doz80DDopcode(SMS.z80Memory[SMS.pc + 1]);
        break;

        case 0xDE:
            tempBitBuffer = SMS.af[1];
            temp8 = SMS.af[0];
            SMS.af[0] -= SMS.z80Memory[SMS.pc + 1];
            SMS.af[0] -= tempBitBuffer[0];
            handleAllFlag8sub(temp8, SMS.af[0]);
            SMS.pc += 2;
            SMS.cycles += 4;
        break;

        case 0xDF:
            SMS.pc += 1;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc;
            SMS.pc = 0x18;
            SMS.cycles += 11;
        break;

        case 0xE0:
            printf("PV RET RAN E0\n");
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[2] == 0)
            {
                SMS.pc = SMS.z80Memory[SMS.sp + 1] << 8 | SMS.z80Memory[SMS.sp];
                SMS.sp += 2;
                SMS.cycles += 11;
            }
            if(tempBitBuffer[2] == 1)
            {
                SMS.pc++;
                SMS.cycles += 5;
            }
        break;

        case 0xE1:
            SMS.hl[0] = SMS.z80Memory[SMS.sp + 1];
            SMS.hl[1] = SMS.z80Memory[SMS.sp];
            SMS.pc++;
            SMS.sp += 2;
            SMS.cycles += 10;
        break;

        case 0xE2:
            printf("pv\n");
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[2] == 0)
            {
                SMS.pc = SMS.z80Memory[SMS.pc + 2] << 8 | SMS.z80Memory[SMS.pc + 1];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[2] == 1)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xE3:
            temp8 = SMS.z80Memory[SMS.sp];
            SMS.z80Memory[SMS.sp] = SMS.hl[1];
            SMS.hl[1] = temp8;
            temp8 = SMS.z80Memory[SMS.sp + 1];
            SMS.z80Memory[SMS.sp + 1] = SMS.hl[0];
            SMS.hl[0] = temp8;
            SMS.pc++;
            SMS.cycles += 19;
        break;

        case 0xE4:
            printf("call pv\n");
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[2] == 0)
            {
                SMS.pc += 3;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc;
                SMS.pc = SMS.z80Memory[SMS.pc - 1] << 8 | SMS.z80Memory[SMS.pc - 2];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[2] == 1)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xE5:
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.hl[0];
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.hl[1];
            SMS.pc++;
            SMS.cycles += 11;
        break;

        case 0xE6:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] & SMS.z80Memory[SMS.pc + 1];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 1;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 7;
        break;

        case 0xE7:
            SMS.pc += 1;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc;
            SMS.pc = 0x20;
            SMS.cycles += 11;
        break;

        case 0xE8:
            printf("PV RET RAN E0\n");
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[2] == 1)
            {
                SMS.pc = SMS.z80Memory[SMS.sp + 1] << 8 | SMS.z80Memory[SMS.sp];
                SMS.sp += 2;
                SMS.cycles += 11;
            }
            if(tempBitBuffer[2] == 0)
            {
                SMS.pc++;
                SMS.cycles += 5;
            }
        break;

        case 0xE9:
            SMS.pc = getREGpair16(SMS.hl[0],SMS.hl[1]);
            SMS.cycles += 4;
        break;

        case 0xEA:
            //printf("pv\n");
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[2] == 1)
            {
                SMS.pc = SMS.z80Memory[SMS.pc + 2] << 8 | SMS.z80Memory[SMS.pc + 1];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[2] == 0)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xEB:
            temp16 = getREGpair16(SMS.de[0],SMS.de[1]);
            temp162 = getREGpair16(SMS.hl[0],SMS.hl[1]);
            SMS.de[0] = temp162 >> 8;
            SMS.de[1] = temp162;
            SMS.hl[0] = temp16 >> 8;
            SMS.hl[1] = temp16;
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xEC:
            printf("call pv\n");
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[2] == 1)
            {
                SMS.pc += 3;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc;
                SMS.pc = SMS.z80Memory[SMS.pc - 1] << 8 | SMS.z80Memory[SMS.pc - 2];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[2] == 0)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xED:
            doz80EDopcode(SMS.z80Memory[SMS.pc + 1]);
        break;

        case 0xEE:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] ^ SMS.z80Memory[SMS.pc + 1];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 4;
        break;

        case 0xEF:
            SMS.pc += 1;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc;
            SMS.pc = 0x28;
            SMS.cycles += 11;
        break;

        case 0xF0:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[7] == 0)
            {
                SMS.pc = SMS.z80Memory[SMS.sp + 1] << 8 | SMS.z80Memory[SMS.sp];
                SMS.sp += 2;
                SMS.cycles += 11;
            }
            if(tempBitBuffer[7] == 1)
            {
                SMS.pc++;
                SMS.cycles += 5;
            }
        break;

        case 0xF1:
            SMS.af[0] = SMS.z80Memory[SMS.sp + 1];
            SMS.af[1] = SMS.z80Memory[SMS.sp];
            SMS.pc++;
            SMS.sp += 2;
            SMS.cycles += 10;
        break;

        case 0xF2:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[7] == 0)
            {
                SMS.pc = SMS.z80Memory[SMS.pc + 2] << 8 | SMS.z80Memory[SMS.pc + 1];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[7] == 1)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xF3:
            z80int1 = false;
            z80int2 = false;
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xF4:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[7] == 0)
            {
                SMS.pc += 3;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc;
                SMS.pc = SMS.z80Memory[SMS.pc - 1] << 8 | SMS.z80Memory[SMS.pc - 2];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[7] == 1)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xF5:
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.af[0];
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.af[1];
            SMS.pc++;
            SMS.cycles += 11;
        break;

        case 0xF6:
            temp8 = SMS.af[0];
            SMS.af[0] = SMS.af[0] | SMS.z80Memory[SMS.pc + 1];
            handleFlag7(SMS.af[0]);
            handleFlag6(temp8,SMS.af[0]);
            handleFlag5(SMS.af[0]);
            handleFlag3(SMS.af[0]);
            handleFlag2(temp8,SMS.af[0],0);
            tempBitBuffer = SMS.af[1];
            tempBitBuffer[4] = 0;
            tempBitBuffer[1] = 0;
            tempBitBuffer[0] = 0;
            SMS.af[1] = tempBitBuffer.to_ulong();
            SMS.pc += 2;
            SMS.cycles += 4;
        break;

        case 0xF7:
            SMS.pc += 1;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
            SMS.sp -= 1;
            SMS.z80Memory[SMS.sp] = SMS.pc;
            SMS.pc = 0x30;
            SMS.cycles += 11;
        break;

        case 0xF8:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[7] == 1)
            {
                SMS.pc = SMS.z80Memory[SMS.sp + 1] << 8 | SMS.z80Memory[SMS.sp];
                SMS.sp += 2;
                SMS.cycles += 11;
            }
            if(tempBitBuffer[7] == 0)
            {
                SMS.pc++;
                SMS.cycles += 5;
            }
        break;

        case 0xF9:
            SMS.sp = getREGpair16(SMS.hl[0],SMS.hl[1]);
            SMS.pc++;
            SMS.cycles += 6;
        break;

        case 0xFA:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[7] == 1)
            {
                SMS.pc = SMS.z80Memory[SMS.pc + 2] << 8 | SMS.z80Memory[SMS.pc + 1];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[7] == 0)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xFB:
            z80int1 = true;
            z80int2 = true;
            SMS.pc++;
            SMS.cycles += 4;
        break;

        case 0xFC:
            tempBitBuffer = SMS.af[1];
            if(tempBitBuffer[7] == 1)
            {
                SMS.pc += 3;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
                SMS.sp -= 1;
                SMS.z80Memory[SMS.sp] = SMS.pc;
                SMS.pc = SMS.z80Memory[SMS.pc - 1] << 8 | SMS.z80Memory[SMS.pc - 2];
                SMS.cycles += 10;
            }
            if(tempBitBuffer[7] == 0)
            {
                SMS.pc += 3;
                SMS.cycles += 10;
            }
        break;

        case 0xFD:
            doz80FDopcode(SMS.z80Memory[SMS.pc + 1]);
        break;

        case 0xFE:
            //breakpoint = true;
            temp8 = SMS.af[0] - SMS.z80Memory[SMS.pc + 1];
            handleAllFlag8sub(SMS.af[0],temp8);
            SMS.pc += 2;
            SMS.cycles += 7;
        break;

        case 0xFF:
            printf("FF RAN!\n");
            breakpoint = true;
            SMS.pc += 1;
            //SMS.sp -= 1;
            //SMS.z80Memory[SMS.sp] = SMS.pc >> 8;
            //SMS.sp -= 1;
            //SMS.z80Memory[SMS.sp] = SMS.pc;
            //SMS.pc = 0x38;
            //SMS.cycles += 11;
        break;

        default:
            printf("\nCPU ERROR: UNKNOWN OPCODE 0x%X!\n",SMS.z80Memory[SMS.pc]);
            printf("PC: 0x%X\n",SMS.pc);
            printf("SP: 0x%X\n",SMS.sp);
            printf("A: 0x%X\n",SMS.af[0]);
            tempBitBuffer2 = SMS.af[1];
            cout<<"F: "<<tempBitBuffer2<<endl;
            printf("BC: 0x%X%X\n",SMS.bc[0],SMS.bc[1]);
            printf("DE: 0x%X%X\n",SMS.de[0],SMS.de[1]);
            printf("HL: 0x%X%X\n",SMS.hl[0],SMS.hl[1]);
            printf("IX 0x%X\n",SMS.ix);
            printf("IY 0x%X\n",SMS.iy);
            printf("Cycles: %i\n",SMS.cycles);
            memDump();
            closenopMD = true;
        break;
    }
}
