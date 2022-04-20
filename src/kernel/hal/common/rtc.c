#include <kernel/hal/common/rtc.h>
#include <kernel/core/kernel.h>

const double RTC_MILLIS_TIME = 0.9765625;

// rtc ports
#define RTC_PORT_CMD  0x70
#define RTC_PORT_DATA 0x71

time_t   rtc_time;
uint32_t rtc_ticks;
uint32_t rtc_timer;
uint32_t rtc_cur_sec, rtc_last_sec;
double   rtc_millis;

void rtc_callback(irq_regs_t* regs)
{
    irq_ack(regs);

    rtc_ticks++;
    rtc_timer++;
    rtc_cur_sec = rtc_time.second;
    if (rtc_cur_sec != rtc_last_sec)
    {
        //rtc_millis   = 0;
        rtc_last_sec = rtc_cur_sec;
    }

    if (rtc_timer >= 500) { rtc_read(); rtc_timer = 0; }

    port_outb(RTC_PORT_CMD, 0x0C);
    (void)port_inb(RTC_PORT_DATA);
}

void rtc_init()
{
    _cli();	
    port_outb(RTC_PORT_CMD, 0x8B);	
    char prev = port_inb(RTC_PORT_DATA);
    port_outb(RTC_PORT_CMD, 0x8B);
    port_outb(RTC_PORT_DATA, prev | 0x40);
    port_outb(RTC_PORT_CMD, 0x0C);
    (void)port_inb(RTC_PORT_DATA);

    irq_register(IRQ8, rtc_callback);
    _sti();

    debug_ok("Initialized RTC controller");
}

uint8_t rtc_get_reg(uint16_t reg)
{
    port_outb(RTC_PORT_CMD, reg);
    return port_inb(RTC_PORT_DATA);
}

void rtc_read()
{
    // read data from registers
    rtc_time.second = rtc_get_reg(0x00);
    rtc_time.minute = rtc_get_reg(0x02);
    rtc_time.hour   = rtc_get_reg(0x04);
    rtc_time.day    = rtc_get_reg(0x07);
    rtc_time.month  = rtc_get_reg(0x08);
    rtc_time.year   = rtc_get_reg(0x09);

    // check if data is bcd
    uint8_t bcd = rtc_get_reg(0x0B);

    // decode bcd formatted data
    if (!(bcd & 0x04))
    {
        rtc_time.second  = (rtc_time.second & 0x0F) + (rtc_time.second / 16) * 10;
        rtc_time.minute  = (rtc_time.minute & 0x0F) + (rtc_time.minute / 16) * 10;
        rtc_time.hour    = ((rtc_time.hour & 0x0F)  + (rtc_time.hour / 16) * 10) | (rtc_time.hour & 0x80);
        rtc_time.day     = (rtc_time.day & 0x0F)    + (rtc_time.day / 16) * 10;
        rtc_time.month   = (rtc_time.month & 0x0F)  + (rtc_time.month / 16) * 10;
        rtc_time.year    = (rtc_time.year & 0x0F)   + (rtc_time.year / 16) * 10;
        rtc_time.year    += 2000;
    }
}

time_t rtc_get_time() { return rtc_time; }