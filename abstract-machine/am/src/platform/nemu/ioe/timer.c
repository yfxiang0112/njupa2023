#include <am.h>
#include <nemu.h>

#include <stdio.h>

void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint32_t rtc_hi = RTC_ADDR + sizeof(uint32_t);
  uint64_t us_h = (uint64_t)inl(rtc_hi) << 32;
  uint64_t us_l = inl(RTC_ADDR);
  uptime->us = us_h | us_l;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
