#define MSP_ARM                  151
#define MSP_DISARM               152
#define MSP_TRIM_UP              153
#define MSP_TRIM_DOWN            154
#define MSP_TRIM_LEFT            155
#define MSP_TRIM_RIGHT           156
#define MSP_SET_RAW_RC           200

enum
{
    leftX = 0, leftY, rightX, rightY,

    leftBotton = 0, rightBotton,

    rightToggle2, rightToggle1, armToggle, leftToggle2, leftToggle1,

    rcRoll = 0, rcPitch, rcYaw, rcThrottle, rcAux1, rcAux2, rcAux3, rcAux4,
};

enum
{
    pressed = 0, release,

    toggleUp = 0, toggleDown,
};

enum
{
    RC_MAX = 1900, RC_MIN = 1100,
};

uint16_t joysticRead[4];
uint8_t bottonRead[7];
uint8_t checksum;

void armState()
{
    static uint8_t preState = toggleDown;
    static uint8_t currentState = toggleDown;

    currentState = bottonRead[armToggle];

    if (currentState != preState)
    {
        if (currentState == toggleUp)
        {
            Serial.write('$');
            Serial.write('M');
            Serial.write('<');
            Serial.write(0);
            Serial.write(MSP_ARM);
            Serial.write(MSP_ARM);
        }
        else
        {
            Serial.write('$');
            Serial.write('M');
            Serial.write('<');
            Serial.write(0);
            Serial.write(MSP_DISARM);
            Serial.write(MSP_DISARM);
        }

    }

    preState = currentState;
}

void leftBottonState()
{
    static uint8_t preState = release;
    static uint8_t currentState = release;

    currentState = bottonRead[leftBotton];

    if (currentState != preState)
    {
        if (currentState == pressed)
        {
            Serial.write('$');
            Serial.write('M');
            Serial.write('<');
            Serial.write(0);
            Serial.write(149);
            Serial.write(149);
        }
    }

    preState = currentState;
}

void rightBottonState()
{
    static uint8_t preState = release;
    static uint8_t currentState = release;

    currentState = bottonRead[rightBotton];

    if (currentState != preState)
    {
        if (currentState == pressed)
        {
            Serial.write('$');
            Serial.write('M');
            Serial.write('<');
            Serial.write(0);
            Serial.write(148);
            Serial.write(148);
        }
    }

    preState = currentState;
}

void rcState()
{
    uint32_t current = millis();
    static uint32_t preTime = 0;
    uint16_t rc16[8];
    uint8_t rc8[16];

    if((current - preTime) > 80)
    {
        rc16[rcRoll] = map(joysticRead[leftX], 0, 1023, RC_MIN + 200, RC_MAX - 200);
        rc16[rcPitch] = map(joysticRead[leftY], 0, 1023, RC_MIN + 200, RC_MAX - 200);
        rc16[rcYaw] = map(joysticRead[rightX], 0, 1023, RC_MIN + 200, RC_MAX - 200);
        rc16[rcThrottle] = map(joysticRead[rightY], 0, 1023, RC_MIN, RC_MAX);
        rc16[rcAux1] = map(bottonRead[leftToggle1], 0, 1, RC_MAX, RC_MIN);
        rc16[rcAux2] = RC_MIN;
        rc16[rcAux3] = RC_MIN;
        rc16[rcAux4] = RC_MIN;

        checksum = 16^200;
        for (uint8_t i = 0; i < 8; i++)
        {
            rc8[2 * i] = rc16[i] & 0xff;
            rc8[2 * i + 1] = (rc16[i] >> 8) & 0xff;

            checksum = checksum^rc8[2 * i]^rc8[2 * i + 1];
        }

        Serial.write('$');
        Serial.write('M');
        Serial.write('<');
        Serial.write(16);
        Serial.write(MSP_SET_RAW_RC);

        for(uint8_t i = 0; i < 16; i++)
            Serial.write(rc8[i]);

        Serial.write(checksum);

        preTime = current;
    }

}

void trimState()
{
    static uint16_t pre_X = 512;
    static uint16_t pre_Y = 512;

    uint16_t current_X = joysticRead[leftX];
    uint16_t current_Y = joysticRead[leftY];

    if(pre_X <= 900)
    {
        if(current_X > 900)
        {
            Serial.write('$');
            Serial.write('M');
            Serial.write('<');
            Serial.write(0);
            Serial.write(MSP_TRIM_RIGHT);
            Serial.write(MSP_TRIM_RIGHT);
        }
    }

    if(pre_X >= 100)
    {
        if(current_X < 100)
        {
            Serial.write('$');
            Serial.write('M');
            Serial.write('<');
            Serial.write(0);
            Serial.write(MSP_TRIM_LEFT);
            Serial.write(MSP_TRIM_LEFT);
        }
    }
    pre_X = current_X;

    if(pre_Y <= 900)
    {
        if(current_Y > 900)
        {
            Serial.write('$');
            Serial.write('M');
            Serial.write('<');
            Serial.write(0);
            Serial.write(MSP_TRIM_UP);
            Serial.write(MSP_TRIM_UP);
        }
    }

    if(pre_Y >= 100)
    {
        if(current_Y < 100)
        {
            Serial.write('$');
            Serial.write('M');
            Serial.write('<');
            Serial.write(0);
            Serial.write(MSP_TRIM_DOWN);
            Serial.write(MSP_TRIM_DOWN);
        }
    }
    pre_Y = current_Y;
}

//The setup function is called once at startup of the sketch
void setup()
{
    Serial.begin(115200);
    for (int i = 2; i < 9; i++)
    {
        pinMode(i, INPUT);
        digitalWrite(i, HIGH);
    }
}

// The loop function is called in an endless loop
void loop()
{
    for (int i = 0; i < 4; i++)
    {
        joysticRead[i] = analogRead(i);
    }
    for (int i = 0; i < 7; i++)

    {
        bottonRead[i] = digitalRead(i + 2);
    }

    armState();

    //normal RC controll mode
    if(bottonRead[rightToggle2] == toggleDown)
    {
        rcState();
    }
    //set trim if right toggle switch 2 is up state
    else
    {
        trimState();
    }
}
