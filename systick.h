#ifndef _SYSTICK_H
#define _SYSTICK_H

#define HZ 1000

#define time_after(a,b) ((long)((b) - (a)) < 0)
#define time_before(a,b) time_after(b,a)
#define time_after_eq(a,b) ((long)((b) - (a)) <= 0)
#define time_before_eq(a,b) time_after_eq(b,a)

extern volatile unsigned long jiffies;

void setup_systick(void);

void mdelay(unsigned int msecs);

#endif
