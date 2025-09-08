#ifndef __BADUSB_MENU_H__
#define __BADUSB_MENU_H__

#include <MenuItemInterface.h>

class BadUSBMenu : public MenuItemInterface {
public:
    BadUSBMenu() : MenuItemInterface("USB") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.others; }
};

#endif
