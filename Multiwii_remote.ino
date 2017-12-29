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
            Serial.write(151);
            Serial.write(151);
        }
        else
        {
            Serial.write('$');
            Serial.write('M');
            Serial.write('<');
            Serial.write(0);
            Serial.write(152);
            Serial.write(152);
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
        rc16[rcRoll] = map(joysticRead[leftX], 0, 1023, RC_MIN, RC_MAX);
        rc16[rcPitch] = map(joysticRead[leftY], 0, 1023, RC_MIN, RC_MAX);
        rc16[rcYaw] = map(joysticRead[rightX], 0, 1023, RC_MIN, RC_MAX);
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
        Serial.write(200);

        for(uint8_t i = 0; i < 16; i++)
            Serial.write(rc8[i]);

        Serial.write(checksum);

        preTime = current;
    }

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
    rcState();
}
