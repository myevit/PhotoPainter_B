#include "EPD_Test.h"   // Examples
#include "run_File.h"

#include "led.h"
#include "waveshare_PCF85063.h" // RTC
#include "DEV_Config.h"

#include <time.h>

extern const char *fileList;
extern char pathName[];

// Make sure RTC alarm is enabled
#define enChargingRtc 1

/*
Mode 0: Automatically get pic folder names and sort them
Mode 1: Automatically get pic folder names but not sorted
Mode 2: pic folder name is not automatically obtained, users need to create fileList.txt file and write the picture name in TF card by themselves, no file limit
Mode 3: Same as mode 2 but next image will be selected randomly from fileList.txt
*/

// We'll use settings.txt instead of hard-coded values
int Mode = 1; // Default that can be overridden by settings.txt

float measureVBAT(void)
{
    float Voltage=0.0;
    const float conversion_factor = 3.3f / (1 << 12);
    uint16_t result = adc_read();
    Voltage = result * conversion_factor * 3;
    printf("Raw value: 0x%03x, voltage: %f V\n", result, Voltage);
    return Voltage;
}

void chargeState_callback() 
{
    if(DEV_Digital_Read(VBUS)) {
        if(!DEV_Digital_Read(CHARGE_STATE)) {  // is charging
            ledCharging();
        }
        else {  // charge complete
            ledCharged();
        }
    }
}

void run_display(Time_data Time, Time_data alarmTime, char hasCard)
{
    if(hasCard) {
        setFilePath();
        EPD_7in3e_display_BMP(pathName, measureVBAT());   // display bmp
    }
    else {
        printf("led_ON_PWR...\r\n");
        EPD_7in3e_display(measureVBAT());
    }
    DEV_Delay_ms(100);
    
    // Clear alarm flag and reset the alarm
    PCF85063_clear_alarm_flag();
    printf("Cleared RTC alarm flag\r\n");
    
    #if enChargingRtc
        printf("Setting new RTC alarm: %02d:%02d:%02d\r\n", 
               alarmTime.hours, alarmTime.minutes, alarmTime.seconds);
        rtcRunAlarm(Time, alarmTime);  // RTC run alarm
    #else
        printf("WARNING: RTC alarm disabled by enChargingRtc=0\r\n");
    #endif
}

int main(void)
{
    // Start with current time
    Time_data Time = {2024-2000, 3, 31, 0, 0, 0};
    Time_data alarmTime = Time;
    Settings_t settings;
    char isCard = 0;
  
    printf("Init...\r\n");
    if(DEV_Module_Init() != 0) {  // DEV init
        return -1;
    }
    
    watchdog_enable(8*1000, 1);   // 8s
    DEV_Delay_ms(1000);
    
    // Load settings
    run_mount();
    if(loadSettings(&settings) == 0) {
        Mode = settings.mode; // Use mode from settings
        printf("Using mode %d from settings\r\n", Mode);
    } else {
        printf("Failed to load settings, using default mode %d\r\n", Mode);
    }
    run_unmount();
    
    // Initialize RTC
    PCF85063_init();
    
    // Get current time from RTC
    Time = PCF85063_GetTime();
    printf("Current RTC time: %02d-%02d-%02d %02d:%02d:%02d\r\n", 
           Time.years, Time.months, Time.days, 
           Time.hours, Time.minutes, Time.seconds);
           
    // Make a copy for alarm time
    alarmTime = Time;
    
    // Set alarm time based on settings (in minutes)
    if(settings.timeInterval > 0) {
        if(settings.timeInterval < 60) {
            // For less than an hour, set minutes directly
            alarmTime.minutes += settings.timeInterval;
            
            // Handle minute overflow
            if(alarmTime.minutes >= 60) {
                alarmTime.hours += alarmTime.minutes / 60;
                alarmTime.minutes = alarmTime.minutes % 60;
            }
            
            printf("Setting alarm for %d minutes from now (%02d:%02d:%02d)\r\n", 
                  settings.timeInterval, alarmTime.hours, alarmTime.minutes, alarmTime.seconds);
        } else {
            // For an hour or more, calculate hours and minutes
            int hours = settings.timeInterval / 60;
            int minutes = settings.timeInterval % 60;
            
            alarmTime.hours += hours;
            alarmTime.minutes += minutes;
            
            // Handle minute overflow
            if(alarmTime.minutes >= 60) {
                alarmTime.hours += 1;
                alarmTime.minutes -= 60;
            }
            
            printf("Setting alarm for %d hours, %d minutes from now (%02d:%02d:%02d)\r\n", 
                  hours, minutes, alarmTime.hours, alarmTime.minutes, alarmTime.seconds);
        }
    } else {
        // Default 12-hour interval if setting is invalid
        alarmTime.hours += 12;
        printf("Using default 12-hour alarm interval (%02d:%02d:%02d)\r\n", 
              alarmTime.hours, alarmTime.minutes, alarmTime.seconds);
    }
    
    // Set up RTC alarm
    printf("Setting RTC alarm\r\n");
    rtcRunAlarm(Time, alarmTime);
    
    gpio_set_irq_enabled_with_callback(CHARGE_STATE, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, chargeState_callback);

    if(measureVBAT() < 3.1) {   // battery power is low
        printf("low power ...\r\n");
        PCF85063_alarm_Time_Disable();
        EPD_7in3e_display_low_battery();
        ledLowPower();  // LED flash for Low power
        powerOff(); // BAT off
        return 0;
    }
    else {
        printf("work ...\r\n");
        ledPowerOn();
    }

    if(!sdTest()) 
    {
        isCard = 1;
        if(Mode == 0)
        {
            printf("Mode 0\r\n");
            sdScanDir();
            file_sort();
        }
        if(Mode == 1)
        {
            printf("Mode 1\r\n");
            sdScanDir();
        }
        if(Mode == 2)
        {
            printf("Mode 2\r\n");
            file_cat();
        }
        if(Mode == 3)
        {
            printf("Mode 3\r\n");
            file_cat();
        }
        
    }
    else 
    {
        isCard = 0;
    }

    if(!DEV_Digital_Read(VBUS)) {    // no charge state
        printf("Running display and setting alarm for next wake-up\r\n");
        run_display(Time, alarmTime, isCard);
    }
    else {  // charge state
        chargeState_callback();
        while(DEV_Digital_Read(VBUS)) {
            measureVBAT();
            #if enChargingRtc
                if(!DEV_Digital_Read(RTC_INT)) {    // RTC interrupt trigger
                    printf("RTC interrupt detected - changing image\r\n");
                    // Read current time for debugging
                    Time_data current_time = PCF85063_GetTime();
                    printf("Current time at RTC trigger: %02d:%02d:%02d\r\n", 
                           current_time.hours, current_time.minutes, current_time.seconds);
                    
                    // Check alarm flag
                    if(PCF85063_get_alarm_flag()) {
                        printf("Alarm flag is set - RTC alarm triggered\r\n");
                    } else {
                        printf("NOTE: Alarm flag not set - might be external trigger\r\n");
                    }
                    
                    run_display(Time, alarmTime, isCard);
                } else {
                    // Periodically check RTC status
                    static uint32_t last_check = 0;
                    uint32_t now = time_us_32() / 1000000; // Current time in seconds
                    if (now - last_check >= 10) { // Check every 10 seconds
                        last_check = now;
                        Time_data current_time = PCF85063_GetTime();
                        printf("RTC check - Current time: %02d:%02d:%02d, Alarm set for: %02d:%02d:%02d\r\n", 
                               current_time.hours, current_time.minutes, current_time.seconds,
                               alarmTime.hours, alarmTime.minutes, alarmTime.seconds);
                        
                        // Check alarm flag for debugging
                        if(PCF85063_get_alarm_flag()) {
                            printf("ALERT: Alarm flag is set but interrupt not detected!\r\n");
                        }
                    }
                }
            #endif

            if(!DEV_Digital_Read(BAT_STATE)) {  // KEY pressed
                printf("Key press detected - changing image\r\n");
                run_display(Time, alarmTime, isCard);
            }
            DEV_Delay_ms(200);
        }
    }
    
    printf("power off ... (should wake up at %02d:%02d:%02d)\r\n", 
           alarmTime.hours, alarmTime.minutes, alarmTime.seconds);
    powerOff(); // BAT off

    return 0;
}
