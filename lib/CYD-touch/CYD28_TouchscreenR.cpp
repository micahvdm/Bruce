/* Touchscreen library for XPT2046 Touch Controller Chip
 * Copyright (c) 2015, Paul Stoffregen, paul@pjrc.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ----------------------- CYD28 (2.8" cheap yellow display) version --------------------------
 *   using software bit banged SPI and fixed GPIOs  + manual calibration
 * 08.2023 - Piotr Zapart www.hexefx.com
 *
 *
 */

#include "CYD28_TouchscreenR.h"
#include <SPI.h>

#define ISR_PREFIX IRAM_ATTR
#define MSEC_THRESHOLD 3
#define SPI_SETTING SPISettings(2000000, MSBFIRST, SPI_MODE0)

static CYD28_TouchR *isrPinptr;
void isrPin(void);
// ------------------------------------------------------------
bool CYD28_TouchR::begin()
{
    pinMode(CYD28_TouchR_MOSI, OUTPUT);
    pinMode(CYD28_TouchR_MISO, INPUT);
    pinMode(CYD28_TouchR_CLK, OUTPUT);
    pinMode(CYD28_TouchR_CS, OUTPUT);
    digitalWrite(CYD28_TouchR_CLK, LOW);
    digitalWrite(CYD28_TouchR_CS, HIGH);
    if (CYD28_TouchR_IRQ != -1)
    {
        pinMode(CYD28_TouchR_IRQ, INPUT);
        attachInterrupt(digitalPinToInterrupt(CYD28_TouchR_IRQ), isrPin, FALLING);
        isrPinptr = this;
    }
    else
        isrWake = true;

    return true;
}

bool CYD28_TouchR::begin(SPIClass *wspi)
{
    _pspi = wspi;
    //_pspi->begin();
    pinMode(CYD28_TouchR_CS, OUTPUT);
    digitalWrite(CYD28_TouchR_CS, HIGH);
    if (CYD28_TouchR_IRQ != -1)
    {
        pinMode(CYD28_TouchR_IRQ, INPUT);
        attachInterrupt(digitalPinToInterrupt(CYD28_TouchR_IRQ), isrPin, FALLING);
        isrPinptr = this;
    }
    else
        isrWake = true;

    return true;
}
// ------------------------------------------------------------
ISR_PREFIX
void isrPin(void)
{
    CYD28_TouchR *o = isrPinptr;
    o->isrWake = true;
}
// ------------------------------------------------------------
uint8_t CYD28_TouchR::transfer(uint8_t val)
{
    if (_pspi == nullptr)
    {
        uint8_t out = 0;
        uint8_t del = _delay >> 1;
        uint8_t bval = 0;
        int sck = LOW;

        int8_t bit = 8;
        while (bit)
        {
            bit--;
            digitalWrite(CYD28_TouchR_MOSI, ((val & (1 << bit)) ? HIGH : LOW)); // Write bit
            wait(del);
            sck ^= 1u;
            digitalWrite(CYD28_TouchR_CLK, sck);
            /* ... Read bit */
            bval = digitalRead(CYD28_TouchR_MISO);
            out <<= 1;
            out |= bval;
            wait(del);
            sck ^= 1u;
            digitalWrite(CYD28_TouchR_CLK, sck);
        }
        return out;
    }
    else
    {
        uint8_t out = _pspi->transfer(val);
        return out;
    }
}
// ------------------------------------------------------------
uint16_t CYD28_TouchR::transfer16(uint16_t data)
{
    union
    {
        uint16_t val;
        struct
        {
            uint8_t lsb;
            uint8_t msb;
        };
    } in, out;
    in.val = data;

    if (_pspi == nullptr)
    {
        out.msb = transfer(in.msb);
        out.lsb = transfer(in.lsb);
        return out.val;
    }
    else
    {
        out.msb = _pspi->transfer(in.msb);
        out.lsb = _pspi->transfer(in.lsb);
        return out.val;
    }
}
// ------------------------------------------------------------
void CYD28_TouchR::wait(uint_fast8_t del)
{
    for (uint_fast8_t i = 0; i < del; i++)
    {
        asm volatile("nop");
    }
}
// ------------------------------------------------------------
CYD28_TS_Point CYD28_TouchR::getPointScaled()
{
    update();
    int16_t x = xraw, y = yraw;
    convertRawXY(&x, &y);
    return CYD28_TS_Point(x, y, zraw);
}
// ------------------------------------------------------------
CYD28_TS_Point CYD28_TouchR::getPointRaw()
{
    update();
    return CYD28_TS_Point(xraw, yraw, zraw);
}
// ------------------------------------------------------------
bool CYD28_TouchR::touched()
{
    update();
    return ((zraw >= threshold) && isrWake);
}
// ------------------------------------------------------------
void CYD28_TouchR::readData(uint16_t *x, uint16_t *y, uint8_t *z)
{
    update();
    *x = xraw;
    *y = yraw;
    *z = zraw;
}
// ------------------------------------------------------------
static int16_t besttwoavg(int16_t x, int16_t y, int16_t z)
{
    int16_t da, db, dc;
    int16_t reta = 0;
    if (x > y)
        da = x - y;
    else
        da = y - x;
    if (x > z)
        db = x - z;
    else
        db = z - x;
    if (z > y)
        dc = z - y;
    else
        dc = y - z;

    if (da <= db && da <= dc)
        reta = (x + y) >> 1;
    else if (db <= da && db <= dc)
        reta = (x + z) >> 1;
    else
        reta = (y + z) >> 1;

    return (reta);
}
// ------------------------------------------------------------
void CYD28_TouchR::update()
{
    int16_t data[6];
    int z;
    if (!isrWake)
        return;
    uint32_t now = millis();
    if (now - msraw < MSEC_THRESHOLD)
        return;

    digitalWrite(CYD28_TouchR_CS, LOW);

    if (_pspi != nullptr)
        _pspi->beginTransaction(SPI_SETTING);

    transfer(0xB1 /* Z1 */);
    int16_t z1 = transfer16(0xC1 /* Z2 */) >> 3;
    z = z1 + 4095;
    int16_t z2 = transfer16(0x91 /* X */) >> 3;
    z -= z2;
    if (z >= threshold)
    {
        transfer16(0x91 /* X */); // dummy X measure, 1st is always noisy
        data[0] = transfer16(0xD1 /* Y */) >> 3;
        data[1] = transfer16(0x91 /* X */) >> 3; // make 3 x-y measurements
        data[2] = transfer16(0xD1 /* Y */) >> 3;
        data[3] = transfer16(0x91 /* X */) >> 3;
    }
    else
        data[0] = data[1] = data[2] = data[3] = 0;
    data[4] = transfer16(0xD0 /* Y */) >> 3;
    data[5] = transfer16(0) >> 3;

    if (_pspi != nullptr)
        _pspi->endTransaction();

    digitalWrite(CYD28_TouchR_CS, HIGH);

    if (z < 0)
        z = 0;
    if (z < threshold)
    {
        zraw = 0;
        if (z < CYD28_TouchR_Z_THRES_INT && CYD28_TouchR_IRQ != -1)
        {
            isrWake = false;
        }
        return;
    }
    zraw = z;

    // The readings are interleaved; data[0,2,4] are Y, data[1,3,5] are X
    int16_t x_avg = besttwoavg(data[1], data[3], data[5]);
    int16_t y_avg = besttwoavg(data[0], data[2], data[4]);

    if (z >= threshold)
    {
        msraw = now; // good read completed, set wait
        xraw = x_avg;
        yraw = y_avg;
        // log_i("xraw= %d, yraw= %d", xraw, yraw);
    }
}
// ------------------------------------------------------------
void CYD28_TouchR::convertRawXY(int16_t *x, int16_t *y)
{
    int16_t x_tmp = *x, y_tmp = *y;
    int16_t xx, yy;

    if (!cal_rot)
    {
        xx = map(x_tmp, cal_xmin, cal_xmax, 0, sizeX_px);
        yy = map(y_tmp, cal_ymin, cal_ymax, 0, sizeY_px);
    }
    else
    {
        xx = map(y_tmp, cal_ymin, cal_ymax, 0, sizeX_px);
        yy = map(x_tmp, cal_xmin, cal_xmax, 0, sizeY_px);
    }

    if (rotation == 0)
    { // Portrait
        int16_t temp = xx;
        xx = yy;
        yy = sizeY_px - temp;
    }
    else if (rotation == 2)
    { // Portrait inverted
        int16_t temp = xx;
        xx = sizeX_px - yy;
        yy = temp;
    }
    else if (rotation == 3)
    { // Landscape inverted
        xx = sizeX_px - xx;
        yy = sizeY_px - yy;
    }
    // rotation 1 (Landscape) is default mapping

    *x = xx;
    *y = yy;
}

void CYD28_TouchR::setCalibration(uint16_t *parameters)
{
    cal_xmin = parameters[0];
    cal_xmax = parameters[1];
    cal_ymin = parameters[2];
    cal_ymax = parameters[3];
    cal_rot = parameters[4] & 0x01;
}

void CYD28_TouchR::calibrate(uint16_t *parameters, TFT_eSPI *tft, uint8_t size, uint32_t color_fg, uint32_t color_bg)
{
    int16_t values[] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t x_tmp, y_tmp;

    for (uint8_t i = 0; i < 4; i++)
    {
        tft->fillRect(0, 0, size + 1, size + 1, color_bg);
        tft->fillRect(0, tft->height() - size - 1, size + 1, size + 1, color_bg);
        tft->fillRect(tft->width() - size - 1, 0, size + 1, size + 1, color_bg);
        tft->fillRect(tft->width() - size - 1, tft->height() - size - 1, size + 1, size + 1, color_bg);

        switch (i)
        {
        case 0: // top left
            tft->drawLine(0, 0, 0, size, color_fg);
            tft->drawLine(0, 0, size, 0, color_fg);
            tft->drawLine(0, 0, size, size, color_fg);
            break;
        case 1: // bottom left
            tft->drawLine(0, tft->height() - size - 1, 0, tft->height() - 1, color_fg);
            tft->drawLine(0, tft->height() - 1, size, tft->height() - 1, color_fg);
            tft->drawLine(size, tft->height() - size - 1, 0, tft->height() - 1, color_fg);
            break;
        case 2: // top right
            tft->drawLine(tft->width() - size - 1, 0, tft->width() - 1, 0, color_fg);
            tft->drawLine(tft->width() - size - 1, size, tft->width() - 1, 0, color_fg);
            tft->drawLine(tft->width() - 1, size, tft->width() - 1, 0, color_fg);
            break;
        case 3: // bottom right
            tft->drawLine(tft->width() - size - 1, tft->height() - size - 1, tft->width() - 1, tft->height() - 1, color_fg);
            tft->drawLine(tft->width() - 1, tft->height() - 1 - size, tft->width() - 1, tft->height() - 1, color_fg);
            tft->drawLine(tft->width() - 1 - size, tft->height() - 1, tft->width() - 1, tft->height() - 1, color_fg);
            break;
        }

        delay(1000); // give user time to react

        // Wait for touch release
        while (touched())
        {
            update();
            delay(10);
        }

        for (uint8_t j = 0; j < 8; j++)
        {
            while (true)
            {
                update();
                if (zraw >= threshold)
                {
                    x_tmp = xraw;
                    y_tmp = yraw;
                    break;
                }
                delay(10);
            }
            values[i * 2] += x_tmp;
            values[i * 2 + 1] += y_tmp;
        }
        values[i * 2] /= 8;
        values[i * 2 + 1] /= 8;
    }

    bool rot = false;
    if (abs(values[0] - values[2]) > abs(values[1] - values[3]))
    {
        rot = true;
        cal_xmin = (values[1] + values[3]) / 2; // calc min x
        cal_xmax = (values[5] + values[7]) / 2; // calc max x
        cal_ymin = (values[0] + values[4]) / 2; // calc min y
        cal_ymax = (values[2] + values[6]) / 2; // calc max y
    }
    else
    {
        cal_xmin = (values[0] + values[2]) / 2; // calc min x
        cal_xmax = (values[4] + values[6]) / 2; // calc max x
        cal_ymin = (values[1] + values[5]) / 2; // calc min y
        cal_ymax = (values[3] + values[7]) / 2; // calc max y
    }

    bool inv_x = false;
    if (cal_xmin > cal_xmax)
    {
        int16_t temp = cal_xmin;
        cal_xmin = cal_xmax;
        cal_xmax = temp;
        inv_x = true;
    }

    bool inv_y = false;
    if (cal_ymin > cal_ymax)
    {
        int16_t temp = cal_ymin;
        cal_ymin = cal_ymax;
        cal_ymax = temp;
        inv_y = true;
    }

    cal_rot = rot;

    if (parameters != NULL)
    {
        parameters[0] = cal_xmin;
        parameters[1] = cal_xmax;
        parameters[2] = cal_ymin;
        parameters[3] = cal_ymax;
        // This parameter is for compatibility with TFT_eSPI's format,
        // but inv_x and inv_y are not used in this library's convertRawXY.
        // The mapping handles inversion implicitly.
        parameters[4] = rot | (inv_x << 1) | (inv_y << 2);
    }
}
