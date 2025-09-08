#include "BadUSBMenu.h"
#include "core/display.h"
#include "core/massStorage.h"
#include "core/sd_functions.h"
#include "core/utils.h"
#include "modules/badusb_ble/ducky_typer.h"
#include "modules/others/clicker.h"

void BadUSBMenu::optionsMenu() {
    options = {
        {"BadUSB",       [=]() { ducky_setup(hid_usb, false); }   },
        {"USB Keyboard", [=]() { ducky_keyboard(hid_usb, false); }},
#if defined(ARDUINO_USB_MODE) && !defined(USE_SD_MMC)
        {"Mass Storage", [=]() { MassStorage(); }                 },
#endif
#ifdef USB_as_HID
        {"Clicker",      clicker_setup                            },
#endif
    };
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "USB");
}
void BadUSBMenu::drawIconImg() {
    drawImg(
        *bruceConfig.themeFS(),
        bruceConfig.getThemeItemImg(bruceConfig.theme.paths.others),
        0,
        imgCenterY,
        true
    );
}
void BadUSBMenu::drawIcon(float scale) {
    clearIconArea();

    // Define sizes based on scale
    int lineWidth = scale * 3;
    if (lineWidth < 2) lineWidth = 2;

    // End shapes sizes
    int topCircleRadius = scale * 6;
    int mainCircleRadius = scale * 6;
    int squareSize = scale * 11;
    int triangleSize = scale * 13;

    // To make it look centered, let's shift everything
    int x_offset = scale * 28;
    int y_offset = scale * 5;

    // Central point of the main circle `( )`
    int cX = iconCenterX - x_offset;
    int cY = iconCenterY - y_offset;
    tft.fillCircle(cX, cY, mainCircleRadius, bruceConfig.priColor);

    // Main line junction point `-'`
    int junctionX = cX + mainCircleRadius;
    int junctionY = cY;
    tft.drawWideLine(
        junctionX,
        junctionY,
        junctionX + scale * 8,
        junctionY,
        lineWidth,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    junctionX += scale * 8; // Move junction to the right

    // --- Middle prong with triangle ---
    int mainLineEndX = junctionX + scale * 30;
    tft.drawWideLine(
        junctionX, junctionY, mainLineEndX, cY, lineWidth, bruceConfig.priColor, bruceConfig.bgColor
    );
    // Triangle at the end `|>`
    tft.fillTriangle(
        mainLineEndX,
        cY - triangleSize / 2, // Top-left vertex
        mainLineEndX,
        cY + triangleSize / 2, // Bottom-left vertex
        mainLineEndX + triangleSize,
        cY, // Right vertex
        bruceConfig.priColor
    );

    // --- Top prong with circle ---
    // `_ ,--()`
    int topProngTurnX = junctionX + scale * 15;
    int topProngEndY = cY - scale * 16;
    int topProngEndX = topProngTurnX + scale * 15;
    // Diagonal part, then straight part, then circle
    tft.drawWideLine(
        junctionX,
        junctionY,
        topProngTurnX,
        topProngEndY,
        lineWidth,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawWideLine(
        topProngTurnX,
        topProngEndY,
        topProngEndX,
        topProngEndY,
        lineWidth,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.fillCircle(topProngEndX, topProngEndY, topCircleRadius, bruceConfig.priColor);

    // --- Bottom prong with square (offset and with straight section) ---
    // `" `--[]`
    int bottomJunctionX = junctionX + scale * 8;
    int bottomProngTurnX = bottomJunctionX + scale * 15;
    int bottomProngEndY = cY + scale * 16;
    int bottomProngEndX = bottomProngTurnX + scale * 15;
    // Diagonal part, then straight part, then square
    tft.drawWideLine(
        bottomJunctionX,
        junctionY,
        bottomProngTurnX,
        bottomProngEndY,
        lineWidth,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawWideLine(
        bottomProngTurnX,
        bottomProngEndY,
        bottomProngEndX,
        bottomProngEndY,
        lineWidth,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.fillRect(
        bottomProngEndX - squareSize / 2,
        bottomProngEndY - squareSize / 2,
        squareSize,
        squareSize,
        bruceConfig.priColor
    );
}
