#pragma once

#include "soc/soc_caps.h"
#if SOC_USB_OTG_SUPPORTED

#include "MyUSBHID.h"
#if CONFIG_TINYUSB_HID_ENABLED

class USBHIDWheel : public MyUSBHIDDevice {
private:
    MyUSBHID hid;
    // Variables pour les axes, boutons, etc.
    uint16_t wheelPosition;       // Position du volant (axe X)
    uint16_t acceleratorPosition; // Position de l'accélérateur
    uint16_t brakePosition;       // Position du frein
    uint16_t clutchPosition;      // Position de l'embrayage

    uint32_t buttons;             // État des boutons (32 bits pour 32 boutons)

    // Méthode interne pour envoyer le rapport
    bool sendReport();

public:
    USBHIDWheel();
    void begin();
    void end();

    // Méthodes pour mettre à jour les états
    void setWheelPosition(uint16_t position);
    void setAcceleratorPosition(uint16_t position);
    void setBrakePosition(uint16_t position);
    void setClutchPosition(uint16_t position);

    void pressButton(uint8_t button);
    void releaseButton(uint8_t button);

    // Méthode pour envoyer manuellement le rapport si nécessaire
    bool sendState();

    // Méthodes pour gérer le force feedback (à implémenter)
    // ...

    // Méthodes internes
    uint16_t _onGetDescriptor(uint8_t *buffer);
    void _onOutput(uint8_t report_id, const uint8_t *buffer, uint16_t len);
};

#endif /* CONFIG_TINYUSB_HID_ENABLED */
#endif /* SOC_USB_OTG_SUPPORTED */
