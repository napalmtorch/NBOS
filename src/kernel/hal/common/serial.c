#include <kernel/hal/common/serial.h>
#include <kernel/core/kernel.h>

// private methods
bool serial_recieved();
bool serial_send();
bool serial_validate();

// properties
const char* SERIAL_PORTNAMES[6] = { "DISABLED", "COM1", "COM2", "COM3", "COM4", "INVALID" };
SERIALPORT _serial_port;

// set current serial port
void serial_setport(SERIALPORT port)
{
    // set current port value
    _serial_port = port;

    // send port data to serial controller
    port_outb(_serial_port + 1, 0x00);    // disable all interrupts
    port_outb(_serial_port + 3, 0x80);    // set baud rate divisor
    port_outb(_serial_port + 0, 0x03);    // set divisor to 3 (lo uint8_t) 38400 baud
    port_outb(_serial_port + 1, 0x00);    // hi uint8_t
    port_outb(_serial_port + 3, 0x03);    // 8 bits, no parity, one stop bit
    port_outb(_serial_port + 2, 0xC7);    // enable fifo, clear them, with 14-uint8_t threshold
    port_outb(_serial_port + 4, 0x0B);    // irqs enabled, rts/dsr set
    port_outb(_serial_port + 4, 0x1E);    // set in loopback mode, test the serial chip
    port_outb(_serial_port + 0, 0xAE);    // test serial chip (send uint8_t 0xAE and check if serial returns same uint8_t)

    // check if serial is faulty
    if (port_inb(_serial_port) != 0xAE) { _serial_port = SERIALPORT_NONE; return; }
    
    // ff serial is not faulty set it in normal operation mode
    port_outb(_serial_port + 4, 0x0F);
}

// get current serial port
SERIALPORT serial_getport() { return _serial_port; }

// get string representation of specified serial port
const char* serial_getportstr(SERIALPORT port)
{
    switch (port)
    {
        case SERIALPORT_NONE: { return SERIAL_PORTNAMES[0]; }
        case SERIALPORT_COM1: { return SERIAL_PORTNAMES[1]; }
        case SERIALPORT_COM2: { return SERIAL_PORTNAMES[2]; }
        case SERIALPORT_COM3: { return SERIAL_PORTNAMES[3]; }
        case SERIALPORT_COM4: { return SERIAL_PORTNAMES[4]; }
        default:              { return SERIAL_PORTNAMES[5]; }
    }
}

// read character from current serial port
char serial_read()
{
    if (!serial_validate()) { return 0; }
    while (!serial_recieved());
    return port_inb(_serial_port);
}

// write character to current serial port
void serial_printc(char c)
{
    if (!serial_validate()) { return; }
    while (!serial_send());
    port_outb(_serial_port, c);
}

// write colored character to current serial port
void serial_printc_fg(char c, uint8_t color)
{
    serial_setcol(color);
    serial_printc(c);
    serial_print(ANSI_RESET);
}

// write string to current serial port
void serial_print(const char* str)
{
    if (!serial_validate()) { return; }
    int i = 0;
    while (str[i] != 0) { serial_printc(str[i]); i++; }
}

// write colored string to current serial port
void serial_print_fg(const char* str, uint8_t color)
{
    serial_setcol(color);
    serial_print(str);
    serial_print(ANSI_RESET);
}

// write line to current serial port
void serial_println(const char* str)
{
    serial_print(str);
    serial_newline();
}

// write colored line to current serial port
void serial_println_fg(const char* str, uint8_t color)
{
    serial_print_fg(str, color);
    serial_newline();
}

// write a new line to current serial port
void serial_newline() { serial_printc('\n'); }

// print formatted string with va_list of arguments to current serial port
int serial_vprintf(const char* str, va_list args)
{
    while (*str != 0)
    {
        if (*str == '%')
        {
            str++;
            if (*str == '%') { serial_printc('%'); }
            if (*str == 'c') { serial_printc((char)va_arg(args, int)); }
            else if (*str == 'd' || *str == 'u')
            {
                int dec = va_arg(args, int);
                char str[16];
                memset(str, 0, 16);
                serial_print(itoa(dec, str, 10));
            }
            else if (*str == 'u')
            {
                uint32_t dec = va_arg(args, uint32_t);
                char str[16];
                memset(str, 0, 16);
                serial_print(ltoa(dec, str, 10));
            }
            else if (*str == 'x')
            {
                uint32_t dec = va_arg(args, uint32_t);
                char str[16];
                memset(str, 0, 16);
                serial_print(ltoa(dec, str, 16));
            }
            else if (*str == '2')
            {
                str++;
                if (*str != 'x' && *str != 'X') { serial_printc(*str); }
                else
                {
                    uint32_t num = va_arg(args, uint32_t);
                    char str[16];
                    memset(str, 0, 16);
                    serial_print(strhex(num, str, false, 1));
                }
            }
            else if (*str == '4')
            {
                str++;
                if (*str != 'x' && *str != 'X') { serial_printc(*str); }
                else
                {
                    uint32_t num = va_arg(args, uint32_t);
                    char str[16];
                    memset(str, 0, 16);
                    serial_print(strhex(num, str, false, 2));
                }
            }
            else if (*str == '8')
            {
                str++;
                if (*str != 'x' && *str != 'X') { serial_printc(*str); }
                else
                {
                    uint32_t num = va_arg(args, uint32_t);
                    char str[16];
                    memset(str, 0, 16);
                    serial_print(strhex(num, str, false, 4));
                }
            }
            else if (*str == 'f')
            {
                /* TODO : implement float to string */
            }
            else if (*str == 's')
            {
                char* val = va_arg(args, char*);
                serial_print(val);
            }
            else { serial_printc(*str); }
        }
        else { serial_printc(*str); }
        str++;
    }
    return true;
}

// print formatted string to current serial port
int serial_printf(const char* str, ...)
{
    va_list args;
    va_start(args, str);
    int res = serial_vprintf(str, args);
    va_end(args);
    return res;
}

// set color of current serial port
void serial_setcol(uint8_t color)
{
    if (!serial_validate()) { return; }
    serial_print(COL4ToANSI(color, false));
}

// check if serial has recieved data
bool serial_recieved()
{
    if (!serial_validate()) { return false; }
    return port_inb((uint32_t)(_serial_port + 5)) & 1;
}

// check if serial is able to send data
bool serial_send()
{
    if (!serial_validate()) { return false; }
    return port_inb((uint32_t)(_serial_port + 5)) & 0x20;
}

// validate current serial port
bool serial_validate()
{
    if (_serial_port == SERIALPORT_COM1) { return true; }
    if (_serial_port == SERIALPORT_COM2) { return true; }
    if (_serial_port == SERIALPORT_COM3) { return true; }
    if (_serial_port == SERIALPORT_COM4) { return true; }
    return false;
}