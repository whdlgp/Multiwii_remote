# Multiwii_remote
simple Multiwii remote controller with MSP(Multiwii Serial Protocol)

# In multiwii

Add some definitions in Protocol.cpp(multiwii 2.4). 
```cpp
//--------------add for Serial remote control start---------//
#define MSP_ARM                  151
#define MSP_DISARM               152
#define MSP_TRIM_UP              153
#define MSP_TRIM_DOWN            154
#define MSP_TRIM_LEFT            155
#define MSP_TRIM_RIGHT           156
//--------------add for Serial remote control end------------//
```

and Add some cases for evaluate MSP commands in void evaluateCommand(uint8_t c) function
```cpp
    //--------------add for Serial remote control start---------//
    case MSP_ARM:
      go_arm();
      break;
    case MSP_DISARM:
      go_disarm();
      break;
    case MSP_TRIM_UP:
      conf.angleTrim[PITCH]+=1;
      writeParams(1);
      break;
    case MSP_TRIM_DOWN:
      conf.angleTrim[PITCH]-=1;
      writeParams(1);
      break;
    case MSP_TRIM_LEFT:
      conf.angleTrim[ROLL]-=1;
      writeParams(1);
      break;
  	case MSP_TRIM_RIGHT:
      conf.angleTrim[ROLL]+=1;
      writeParams(1);
      break;
    //--------------add for Serial remote control end------------//
```

This remote controller source is for Arduino. simply send ARM, DISARM and RC chennel command('MSP_SET_RAW_RC')
