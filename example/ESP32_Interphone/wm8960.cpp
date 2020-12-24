#include "wm8960.h"

static uint16_t WM8960_REG_VAL[56] =
    {
        0x0097, 0x0097, 0x0000, 0x0000, 0x0000, 0x0008, 0x0000, 0x000A,
        0x01C0, 0x0000, 0x00FF, 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x007B, 0x0100, 0x0032, 0x0000, 0x00C3, 0x00C3, 0x01C0,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0100, 0x0100, 0x0050, 0x0050, 0x0050, 0x0050, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0040, 0x0000, 0x0000, 0x0050, 0x0050, 0x0000,
        0x0000, 0x0037, 0x004D, 0x0080, 0x0008, 0x0031, 0x0026, 0x00ED};

uint8_t WM8960_Write_Reg(uint8_t reg, uint16_t dat)
{
    uint8_t res, I2C_Data[2];

    I2C_Data[0] = (reg << 1) | ((uint8_t)((dat >> 8) & 0x0001)); //RegAddr
    I2C_Data[1] = (uint8_t)(dat & 0x00FF);                       //RegValue

    Wire.beginTransmission(WM8960_ADDRESS); // transmit to device lsb=0 => write
    Wire.write(I2C_Data, 2);                // buffer 1 byte lsb of val1
    res = Wire.endTransmission();           // transmit buffer and then stop

    if (res == 0)
    {
        WM8960_REG_VAL[reg] = dat;
    }

    return res;
}

uint8_t WM8960_Init(int DIN, int DOUT, int LRC, int BCLK)
{
    I2S_Init(DIN, DOUT, LRC, BCLK);

    uint8_t res;
    res = WM8960_Write_Reg(0x0f, 0x0000);
    if (res == 0)
        Serial.println("WM8960 reset completed");
    else
        return res;

    //POWER
    res = WM8960_Write_Reg(POWER_MANAGEMENT_1, 1 << 8 | 1 << 7 | 1 << 6 | 1 << 5 | 1 << 4 | 1 << 3 | 1 << 2 | 1 << 1);
    res += WM8960_Write_Reg(POWER_MANAGEMENT_2, 1 << 8 | 1 << 7 | 1 << 6 | 1 << 5 | 1 << 4 | 1 << 3 | 1 << 2 | 1);
    res += WM8960_Write_Reg(POWER_MANAGEMENT_3, 1 << 5 | 1 << 4 | 1 << 3 | 1 << 2);
    if (res == 0)
        Serial.println("WM8960 power 1,2,3 completed");
    else
        return res;

    res = WM8960_Write_Reg(CLOCKING_1, 0x00DD); // Select 011011101
    res = WM8960_Write_Reg(CLOCKING_2, 0x0080); // Select 011011101
    if (res == 0)
        Serial.println("WM8960 Configure clock");
    else
        return res;

    //PLL
    res = WM8960_Write_Reg(PLL_N, 0x0038);
    res = WM8960_Write_Reg(PLL_K_1, 0x0031);
    res = WM8960_Write_Reg(PLL_K_2, 0x0026);
    res = WM8960_Write_Reg(PLL_K_3, 0x00E8);

    // Configure ADC/DAC
    // bit0 = 1 ADC High Pass Filter Disable
    // bit1,2 De-emphasis 00 = No de-emphasis
    res = WM8960_Write_Reg(ADC_AND_DAC_CONTROL_1, 0x0000);
    if (res == 0)
        Serial.println("WM8960 Configure ADC/DAC");
    else
        return res;

    // Configure audio interface
    // I2S format 16 bits word length
    res = WM8960_Write_Reg(DIGITAL_AUDIO_INTERFACE, 0x0002);
    if (res == 0)
        Serial.println("WM8960 Configure audio interface");
    else
        return res;

    // Configure HP_L and HP_R OUTPUTS
    res = WM8960_Write_Reg(LOUT1_VOLUME, 0x0079 | 0x0100);  //LOUT1 Volume Set
    res += WM8960_Write_Reg(ROUT1_VOLUME, 0x0079 | 0x0100); //ROUT1 Volume Set
    if (res == 0)
        Serial.println("WM8960 Configure HP_L and HP_R OUTPUTS");
    else
        return res;

    // Configure SPK_RP and SPK_RN
    res = WM8960_Write_Reg(LOUT2_VOLUME, 0x0079 | 0x0100);  //Left Speaker Volume
    res += WM8960_Write_Reg(ROUT2_VOLUME, 0x0079 | 0x0100); //Right Speaker Volume
    if (res == 0)
        Serial.println("WM8960 Configure SPK_RP and SPK_RN");
    else
        return res;

    // Enable the OUTPUTS
    res = WM8960_Write_Reg(CLASS_D_CONTROL_1, 0x00F7); //Enable Class D Speaker Outputs
    if (res == 0)
        Serial.println("WM8960 Enable Class D Speaker Outputs");
    else
        return res;

    // Configure DAC volume
    res = WM8960_Write_Reg(LEFT_DAC_VOLUME, 0x00FF | 0x0100);
    res += WM8960_Write_Reg(RIGHT_DAC_VOLUME, 0x00FF | 0x0100);
    if (res == 0)
        Serial.println("WM8960 Configure DAC volume");
    else
        return res;

    // Configure MIXER
    res = WM8960_Write_Reg(LEFT_OUT_MIX, 1 << 8 | 1 << 7);
    res += WM8960_Write_Reg(RIGHT_OUT_MIX, 1 << 8 | 1 << 7);
    if (res == 0)
        Serial.println("WM8960 Configure MIXER");
    else
        return res;

    // Jack Detect
    res = WM8960_Write_Reg(ADDITIONAL_CONTROL_2, 0 << 6 | 0 << 5);
    res += WM8960_Write_Reg(ADDITIONAL_CONTROL_1, 0x01C3);
    res += WM8960_Write_Reg(ADDITIONAL_CONTROL_4, 0x0009); //0x000D,0x0005
    if (res == 0)
        Serial.println("WM8960 Jack Detect");
    else
        return res;

    //Myset

    // set Input PGA Volume
    WM8960_Write_Reg(LEFT_INPUT_VOLUME, 0X0027 | 0X0100);
    WM8960_Write_Reg(RIGHT_INPUT_VOLUME, 0X0027 | 0X0100);

    // set ADC Volume
    WM8960_Write_Reg(LEFT_ADC_VOLUME, 0X00c3 | 0X0100);
    WM8960_Write_Reg(RIGHT_ADC_VOLUME, 0X00c3 | 0X0100);

    // disable bypass switch
    WM8960_Write_Reg(BYPASS_1, 0x0000);
    WM8960_Write_Reg(BYPASS_2, 0x0000);

    // connect LINPUT1 to PGA and set PGA Boost Gain.
    WM8960_Write_Reg(ADCL_SIGNAL_PATH, 0X0020 | 1 << 8 | 1 << 3);
    WM8960_Write_Reg(ADCR_SIGNAL_PATH, 0X0020 | 1 << 8 | 1 << 3);

    return 0;
}

bool WM8960_Volume(float L_volume, float R_volume)
{
    L_volume = L_volume * 0xff;
    WM8960_Write_Reg(LEFT_DAC_VOLUME, (uint8_t)L_volume | 0x0100);
    R_volume = R_volume * 0xff;
    WM8960_Write_Reg(RIGHT_DAC_VOLUME, (uint8_t)R_volume | 0x0100);
    return true;
}

void WM8960_Play(String filename, char *buff)
{
    File file = SD.open(filename);
    if (!file)
        return;
    Serial.println("Begin to play:");
    Serial.println(filename);
    file.seek(44);
    while (file.readBytes(buff, sizeof(buff)))
    {
        I2S_Write(buff, sizeof(buff));
    }
    Serial.println("Finish");
    file.close();
}

void WM8960_Record(String filename, char *buff, int record_time)
{
    int headerSize = 44;
    byte header[headerSize];
    int waveDataSize = record_time * 16000 * 16 * 2 / 8;
    int recode_time = millis();
    int part_time = recode_time;

    File file = SD.open(filename, FILE_WRITE);
    if (!file)
        return;

    Serial.println("Begin to record:");
    //CreateWavHeader(header, waveDataSize);
    //file.write(header, headerSize);

    for (int j = 0; j < waveDataSize / sizeof(buff); ++j)
    {
        I2S_Read(buff, sizeof(buff));
        file.write((const byte *)buff, sizeof(buff));
        if ((millis() - part_time) > 1000)
        {
            Serial.print(".");
            part_time = millis();
        }
    }

    file.seek(0);
    CreateWavHeader(header, waveDataSize);
    file.write(header, headerSize);

    Serial.println("");
    Serial.println("Finish");
    Serial.println(millis() - recode_time);
    file.close();
}

void WM8960_Alarm()
{
    int runtime = millis();
    int part_time = runtime;
    while (1)
    {
        int16_t fy[2] = {10000, 30000};
        size_t i2s_bytes_write = 1;
        for (int i = 0; i < 1000; i++)
        {
            i2s_write(I2S_NUM_0, fy, 1, &i2s_bytes_write, 100);
            fy[0]++;
            fy[1]++;
        }
        if ((millis() - part_time) > 500)
        {
            Serial.print(".");
            part_time = millis();
        }
        if ((millis() - runtime) > 3000)
            return;
    }
}


void WM8960_Play_psram(uint8_t *buff, int len)
{

    I2S_Write((char *)buff, len);
}

void WM8960_Record_psram(uint8_t *buff, int len)
{
    I2S_Read((char *)buff, len);
}
