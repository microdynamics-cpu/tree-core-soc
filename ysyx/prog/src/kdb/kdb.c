#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#define PS2_REG_RB *((volatile uint8_t *)0x10003000)

// use function instead of static const char* array limited to flash 4-bytes mem access
char *names(uint8_t val)
{
    // clang-format off
    switch (val)
    {
    case 0x1C: return "A"; break;
    case 0x32: return "B"; break;
    case 0x21: return "C"; break;
    case 0x23: return "D"; break;
    case 0x24: return "E"; break;
    case 0x2B: return "F"; break;
    case 0x34: return "G"; break;
    case 0x33: return "H"; break;
    case 0x43: return "I"; break;
    case 0x3B: return "J"; break;
    case 0x42: return "K"; break;
    case 0x4B: return "L"; break;
    case 0x3A: return "M"; break;
    case 0x31: return "N"; break;
    case 0x44: return "O"; break;
    case 0x4D: return "P"; break;
    case 0x15: return "Q"; break;
    case 0x2D: return "R"; break;
    case 0x1B: return "S"; break;
    case 0x2C: return "T"; break;
    case 0x3C: return "U"; break;
    case 0x2A: return "V"; break;
    case 0x1D: return "W"; break;
    case 0x22: return "X"; break;
    case 0x35: return "Y"; break;
    case 0x1A: return "Z"; break;
    case 0x16: return "1"; break;
    case 0x1E: return "2"; break;
    case 0x26: return "3"; break;
    case 0x25: return "4"; break;
    case 0x2E: return "5"; break;
    case 0x36: return "6"; break;
    case 0x3D: return "7"; break;
    case 0x3E: return "8"; break;
    case 0x46: return "9"; break;
    case 0x45: return "0"; break;
    case 0x5A: return "RETURN"; break;
    case 0x76: return "ESCAPE"; break;
    case 0x66: return "BACKSPACE"; break;
    case 0x0D: return "TAB"; break;
    case 0x29: return "SPACE"; break;
    case 0x4E: return "MINUS"; break;
    case 0x55: return "EQUALS"; break;
    case 0x54: return "LEFTBRACKET"; break;
    case 0x5B: return "RIGHTBRACKET"; break;
    case 0x5D: return "BACKSLASH"; break;
    // case 0x5D: return "NONUSHASH"; break;
    case 0x4C: return "SEMICOLON"; break;
    case 0x52: return "APOSTROPHE"; break;
    case 0x0E: return "GRAVE"; break;
    case 0x41: return "COMMA"; break;
    case 0x49: return "PERIOD"; break;
    case 0x4A: return "SLASH"; break;
    case 0x58: return "CAPSLOCK"; break;
    default:
        printf("%x val not support!!\n", val);
        return " ";
        break;
    }
    // clang-format on
}

// uint8_t bit_reverse(uint8_t val)
// {
    // val = ((val & 0x55) << 1) | ((val >> 1) & 0x55);
    // val = ((val & 0x33) << 2) | ((val >> 2) & 0x33);
    // val = ((val & 0x0F) << 4) | ((val >> 4) & 0x0F);
    // return val;
// }

int main()
{
    putstr("try to press any key (keyboard)...\n");
    uint8_t kdb_code;
    while (1)
    {
        kdb_code = PS2_REG_RB;
        if (kdb_code)
        {
            printf("Got  (kbd): %s (%x)\n", names(kdb_code), kdb_code);
        }
    }
    return 0;
}
