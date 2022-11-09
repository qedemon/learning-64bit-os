#include "RTC.h"
#include "AssemblyUtility.h"

void kReadRTCTime(BYTE* pbHour, BYTE* pbMinute, BYTE* pbSecond){
    BYTE bData;
    kOutPortByte(CMOS_MEMORYADDRESS_PORT, CMOS_MEMORYADDRESS_RTC_SECONDS);
    bData=kInPortByte(CMOS_MEMORYDATA_PORT);
    *pbSecond=RTC_BCDTOBINARY(bData);

    kOutPortByte(CMOS_MEMORYADDRESS_PORT, CMOS_MEMORYADDRESS_RTC_MINUTE);
    bData=kInPortByte(CMOS_MEMORYDATA_PORT);
    *pbMinute=RTC_BCDTOBINARY(bData);
    
    kOutPortByte(CMOS_MEMORYADDRESS_PORT, CMOS_MEMORYADDRESS_RTC_HOUR);
    bData=kInPortByte(CMOS_MEMORYDATA_PORT);
    *pbHour=RTC_BCDTOBINARY(bData);
    
}

void kReadRTCDate(WORD* pwYear, BYTE* pbMonth, BYTE* pbDayOfMonth, BYTE* pbDayOfWeek){
    BYTE bData;

    kOutPortByte(CMOS_MEMORYADDRESS_PORT, CMOS_MEMORYADDRESS_RTC_DAYOFWEEK);
    bData=kInPortByte(CMOS_MEMORYDATA_PORT);
    *pbDayOfWeek=RTC_BCDTOBINARY(bData);

    kOutPortByte(CMOS_MEMORYADDRESS_PORT, CMOS_MEMORYADDRESS_RTC_DAYOFMONTH);
    bData=kInPortByte(CMOS_MEMORYDATA_PORT);
    *pbDayOfMonth=RTC_BCDTOBINARY(bData);

    kOutPortByte(CMOS_MEMORYADDRESS_PORT, CMOS_MEMORYADDRESS_RTC_MONTH);
    bData=kInPortByte(CMOS_MEMORYDATA_PORT);
    *pbMonth=RTC_BCDTOBINARY(bData);

    kOutPortByte(CMOS_MEMORYADDRESS_PORT, CMOS_MEMORYADDRESS_RTC_YEAR);
    bData=kInPortByte(CMOS_MEMORYDATA_PORT);
    *pwYear=RTC_BCDTOBINARY(bData)+2000;

}