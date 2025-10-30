/*
    Driver for XPT2046 to ESP-32 interface
 */

#include <cstdint>

class XPT2046 {

    public:
        XPT2046();
        XPT2046(uint8_t cs, uint8_t irq);
        XPT2046(uint8_t cs);

        spi_device_handle_t spi;
        spi_device_interface_config_t devcfg;

        void begin();
        void setCallback(void (*cb)());

        uint8_t getInput();
        uint8_t _cs;
        uint8_t _irq;

        uint16_t x, y, z;

        void (*irq_callback)();

        //SPIClass *vspi = NULL

        ~XPT2046();

    private:

};