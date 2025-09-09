#include "core/powerSave.h"
#include "core/utils.h"
#include <Arduino.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <interface.h>
#include <soc/adc_channel.h>
#include <soc/soc_caps.h>
#include <interface.h>
#include <LittleFS.h>

#include "CYD28_TouchscreenR.h"
#define CYD28_DISPLAY_HOR_RES_MAX 320
#define CYD28_DISPLAY_VER_RES_MAX 240
CYD28_TouchR touch(CYD28_DISPLAY_HOR_RES_MAX, CYD28_DISPLAY_VER_RES_MAX);
#if defined(TOUCH_XPT2046_SPI)
#define XPT2046_CS XPT2046_SPI_CONFIG_CS_GPIO_NUM
#else
#define XPT2046_CS 9
#endif

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/

SPIClass touchSPI;
void _setup_gpio()
{
    bruceConfig.colorInverted = 0;
    pinMode(CC1101_SS_PIN, OUTPUT);
    pinMode(NRF24_SS_PIN, OUTPUT);
    digitalWrite(CC1101_SS_PIN, HIGH);
    digitalWrite(NRF24_SS_PIN, HIGH);
    pinMode(TFT_BL, OUTPUT);
    tft.setRotation(bruceConfig.rotation);
    bruceConfig.rfModule = CC1101_SPI_MODULE;
    bruceConfig.irRx = RXLED;
    bruceConfig.irTx = LED;
    Wire.setPins(SDA, SCL);
    Wire.begin();
}

/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   second stage gpio setup to make a few functions work
***************************************************************************************/
void _post_setup_gpio()
{
    if (!touch.begin(&tft.getSPIinstance()))
    {
        Serial.println("Touch IC not Started");
        log_i("Touch IC not Started");
        return;
    }
    Serial.println("Touch IC Started");

    uint16_t calData[5];
    File calfile = LittleFS.open("/calData", "r");

    if (!calfile)
    {
        Serial.println("Calibration data not found, starting calibration...");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor(20, 0);
        tft.setTextDatum(MC_DATUM);
        tft.setTextSize(1);
        tft.drawString("Touch Calibration", tft.width() / 2, tft.height() / 2 - 20);
        tft.drawString("Touch corners as they appear", tft.width() / 2, tft.height() / 2 + 20);

        uint8_t size = 10;
        uint32_t color_fg = TFT_WHITE;
        uint32_t color_bg = TFT_BLACK;

        touch.calibrate(calData, &tft, size, color_fg, color_bg);

        Serial.println("Calibration complete.");

        calfile = LittleFS.open("/calData", "w");
        if (calfile)
        {
            calfile.printf(
                "%d\n%d\n%d\n%d\n%d\n", calData[0], calData[1], calData[2], calData[3], calData[4]);
            calfile.close();
            Serial.println("Calibration data saved.");
        }
    }
    else
    {
        Serial.print("\nFound tft Calibration data: ");
        for (int i = 0; i < 5; i++)
        {
            String line = calfile.readStringUntil('\n');
            calData[i] = line.toInt();
            Serial.printf("%d, ", calData[i]);
        }
        Serial.println();
        calfile.close();
        touch.setCalibration(calData);
    }
}

/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() { return 0; }

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval)
{
    if (brightval == 0)
    {
        analogWrite(TFT_BL, brightval);
    }
    else
    {
        int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval / 100));
        analogWrite(TFT_BL, bl);
    }
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/

void InputHandler(void)
{
    static long tm = millis();

    // debounce touch events
    if (millis() - tm > 300 || LongPress)
    {
        if (touch.touched())
        {
            auto raw = touch.getPointScaled();
            tm = millis();

            // --- Rotation compensation ---
            int16_t tx = raw.x;
            int16_t ty = raw.y;

            switch (bruceConfig.rotation)
            {
            case 2: // portrait
            {
                int tmp = tx;
                tx = tftWidth - ty;
                ty = tmp;
            }
            break;
            case 3: // landscape
                // no swap needed
                break;
            case 0: // portrait inverted
            {
                int tmp = tx;
                tx = ty;
                ty = (tftHeight + 0) - tmp; // calibrate in real time
            }
            break;
            case 1:                        // landscape inverted
                ty = (tftHeight + 0) - ty; // calibrate in real time
                tx = tftWidth - tx;
                break;
            }

            // Serial.printf( "Touch: raw=(%d,%d) mapped=(%d,%d) rot=%d\n", raw.x, raw.y, tx, ty,
            // bruceConfig.rotation" );

            // wake screen if off
            if (!wakeUpScreen())
            {
                AnyKeyPress = true;
            }
            else
            {
                return;
            }

            // store in global touch point
            touchPoint.x = tx;
            touchPoint.y = ty;
            touchPoint.pressed = true;

            // optional: heatmap logging
            touchHeatMap(touchPoint);
        }
        else
        {
            touchPoint.pressed = false;
        }
    }
}

/*********************************************************************
** Function: keyboard
** location: mykeyboard.cpp
** Starts keyboard to type data
**********************************************************************/
// String keyboard(String mytext, int maxSize, String msg) {}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to tornoff the device (name is odd btw)
**********************************************************************/
void checkReboot() {}
