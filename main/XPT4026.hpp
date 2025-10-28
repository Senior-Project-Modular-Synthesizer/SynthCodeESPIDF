/*
    Driver for XPT2046 to ESP-32 interface
 */

class XPT2046 {

    public:
        XPT2046();
        XPT2046(uint8_t cs, uint8_t irq);
        XPT2046(uint8_t cs);

        void begin();
        void setCallback(void (*cb)());

        uint8_t getInput();
        uint8_t _cs;
        uint8_t irq;

        uint16_t x, y, z;

        void (*irq_callback)();

        //SPIClass *vspi = NULL

        ~XPT2046();





    private:

};