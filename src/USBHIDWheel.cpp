#include "MyUSBHID.h"
#if SOC_USB_OTG_SUPPORTED

#if CONFIG_TINYUSB_HID_ENABLED

#include "USBHIDWheel.h"

#define ARDUHAL_LOG_LEVEL ARDUHAL_LOG_LEVEL_DEBUG


// Votre descripteur HID personnalisé
static const uint8_t report_descriptor[] = {
    // Première Collection (Application)
    0x05, 0x01,        // Usage Page (Generic Desktop Controls)
    0x09, 0x04,        // Usage (Joystick)
    0xA1, 0x01,        // Collection (Application)

    // Report ID pour la première collection
    0x85, 0x01,        // Report ID (1)

    // Axe X (Wheel)
    0x09, 0x30,        // Usage (X)
    0x15, 0x00,        // Logical Minimum (0)
    0x26, 0xFF, 0x03,  // Logical Maximum (1023)
    0x75, 0x10,        // Report Size (16 bits)
    0x95, 0x01,        // Report Count (1)
    0x81, 0x02,        // Input (Data, Variable, Absolute)

    // Buttons (14 boutons + 2 paddles)
    0x05, 0x09,        // Usage Page (Button)
    0x19, 0x01,        // Usage Minimum (Button 1)
    0x29, 0x10,        // Usage Maximum (Button 14 + 2 paddles)
    0x15, 0x00,        // Logical Minimum (0)
    0x25, 0x01,        // Logical Maximum (1)
    0x75, 0x01,        // Report Size (1)
    0x95, 0x10,        // Report Count (16)
    0x81, 0x02,        // Input (Data, Variable, Absolute)

    // Gear shift 8 boutons
    0x05, 0x09,        // Usage Page (Button)
    0x19, 0x11,        // Usage Minimum (Button 13)
    0x29, 0x19,        // Usage Maximum (Button 21)
    0x15, 0x00,        // Logical Minimum (0)
    0x25, 0x01,        // Logical Maximum (1)
    0x75, 0x01,        // Report Size (1)
    0x95, 0x08,        // Report Count (8)
    0x81, 0x02,        // Input (Data, Variable, Absolute)

    // 3 pédales (accelerator, brake, clutch)
    0x05, 0x01,        // Usage Page (Generic Desktop)
    0x09, 0x33,        // Usage (Rx - Accélérateur)
    0x15, 0x00,        // Logical Minimum (0)
    0x26, 0xFF, 0x03,  // Logical Maximum (1023)
    0x75, 0x10,        // Report Size (16 bits)
    0x95, 0x01,        // Report Count (1)
    0x81, 0x02,        // Input (Data, Variable, Absolute)

    0x09, 0x34,        // Usage (Ry - Frein)
    0x15, 0x00,        // Logical Minimum (0)
    0x26, 0xFF, 0x03,  // Logical Maximum (1023)
    0x75, 0x10,        // Report Size (16 bits)
    0x95, 0x01,        // Report Count (1)
    0x81, 0x02,        // Input (Data, Variable, Absolute)

    0x09, 0x35,        // Usage (Rz - Embrayage)
    0x15, 0x00,        // Logical Minimum (0)
    0x26, 0xFF, 0x03,  // Logical Maximum (1023)
    0x75, 0x10,        // Report Size (16 bits)
    0x95, 0x01,        // Report Count (1)
    0x81, 0x02,        // Input (Data, Variable, Absolute)


    // **********************************************************************
    // Force Feedback (PID)
    0x05, 0x0F,        // Usage Page (Physical Interface)
    0x09, 0x92,        // Usage (PID State Report)
    0xA1, 0x02,        // Collection (Application)

    // Report ID 2 - PID State Report
    0x85, 0x02,        //     REPORT_ID (2)
    0x09, 0x22,        //     USAGE (Effect Block Index)
    0x15, 0x01,        //     LOGICAL_MINIMUM (1)
    0x25, 0x28,        //     LOGICAL_MAXIMUM (40)
    0x35, 0x01,        //     PHYSICAL_MINIMUM (1)
    0x45, 0x28,        //     PHYSICAL_MAXIMUM (40)
    0x75, 0x08,        //     REPORT_SIZE (8)
    0x95, 0x01,        //     REPORT_COUNT (1)
    0x81, 0x02,        //     INPUT (Data,Var,Abs)

    // Effect Playing, Actuators Enabled, Safety Switch, Actuator Power
    0x09, 0x94,        //     USAGE (Effect Playing)
    0x09, 0xA0,        //     USAGE (Actuators Enabled)
    0x09, 0xA4,        //     USAGE (Safety Switch)
    0x09, 0xA6,        //     USAGE (Actuator Power)
    0x75, 0x01,        //     REPORT_SIZE (1)
    0x95, 0x04,        //     REPORT_COUNT (4)
    0x81, 0x02,        //     INPUT (Data,Var,Abs)
    0x75, 0x04,        //     REPORT_SIZE (4)
    0x95, 0x01,        //     REPORT_COUNT (1)
    0x81, 0x03,        //     INPUT (Cnst,Var,Abs)

    0xC0,              //   END_COLLECTION (Logical)

    // PID Pool Report (Feature)
    0x09, 0x7F,        //   USAGE (PID Pool Report)
    0xA1, 0x02,        //   COLLECTION (Logical)
    0x85, 0x03,        //     REPORT_ID (3)
    0x09, 0x80,        //     USAGE (RAM Pool Size)
    0x75, 0x10,        //     REPORT_SIZE (16)
    0x95, 0x01,        //     REPORT_COUNT (1)
    0x15, 0x00,        //     LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x7F,  //     LOGICAL_MAXIMUM (32767)
    0xB1, 0x02,        //     FEATURE (Data,Var,Abs)
    0xC0,              //   END_COLLECTION (Logical)

    // Set Effect Report (Output)
    0x09, 0x21,        //   USAGE (Set Effect Report)
    0xA1, 0x02,        //   COLLECTION (Logical)
    0x85, 0x01,        //     REPORT_ID (1)
    0x09, 0x22,        //     USAGE (Effect Block Index)
    0x15, 0x01,        //     LOGICAL_MINIMUM (1)
    0x25, 0x28,        //     LOGICAL_MAXIMUM (40)
    0x75, 0x08,        //     REPORT_SIZE (8)
    0x95, 0x01,        //     REPORT_COUNT (1)
    0x91, 0x02,        //     OUTPUT (Data,Var,Abs)

    // ... Ajoutez les autres champs nécessaires pour le rapport "Set Effect"

    0xC0,              //   END_COLLECTION (Logical)

    // Device Gain Report (Feature)
    0x09, 0x7D,        //   USAGE (Device Gain Report)
    0xA1, 0x02,        //   COLLECTION (Logical)
    0x85, 0x04,        //     REPORT_ID (4)
    0x09, 0x7E,        //     USAGE (Device Gain)
    0x15, 0x00,        //     LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00,  //     LOGICAL_MAXIMUM (255)
    0x75, 0x08,        //     REPORT_SIZE (8)
    0x95, 0x01,        //     REPORT_COUNT (1)
    0xB1, 0x02,        //     FEATURE (Data,Var,Abs)
    0xC0,              //   END_COLLECTION (Logical)

    0xC0               // Fin de la collection PID
};



USBHIDWheel::USBHIDWheel() : hid() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    hid.addDevice(this, sizeof(report_descriptor));
  }

  // Initialiser les variables
  wheelPosition = 512;        // Milieu
  buttons = 0;
  acceleratorPosition = 0;
  brakePosition = 0;
  clutchPosition = 0;
}

uint16_t USBHIDWheel::_onGetDescriptor(uint8_t *dst) {
  memcpy(dst, report_descriptor, sizeof(report_descriptor));
  return sizeof(report_descriptor);
}

void USBHIDWheel::begin() {
  hid.begin();
}

void USBHIDWheel::end() {
  hid.end();
}

bool USBHIDWheel::sendReport() {
    if (!hid.ready()) {
        return false;
    }

    const int reportSizeBits = 8 + 16 + 14 + 2 + 8 + 16 + 16 + 16;   // Report ID + wheel + buttons + paddles + gear shift + brake + clutch + accelerator
    const int reportSize = reportSizeBits / 8;
    uint8_t reportData[reportSize];
    int index = 0;

    // Préparer le rapport
    // Wheel position
    reportData[index++] = wheelPosition & 0xFF;                         // octet 0
    reportData[index++] = (wheelPosition >> 8) & 0xFF;                  // octet 1

    // buttons, paddles, gearshift
    reportData[index++] = (buttons & 0xFF);                             // octet 2
    reportData[index++] = (buttons >> 8) & 0xFF;                        // octet 3
    reportData[index++] = (buttons >> 16) & 0xFF;                       // octet 4

    // accelerator
    reportData[index++] = acceleratorPosition & 0xFF;                   // octet 5
    reportData[index++] = (acceleratorPosition >> 8) & 0xFF;            // octet 6

    // brake
    reportData[index++] = brakePosition & 0xFF;                         // octet 7
    reportData[index++] = (brakePosition >> 8) & 0xFF;                  // octet 8

    // clutch
    reportData[index++] = clutchPosition & 0xFF;                        // octet 9
    reportData[index++] = (clutchPosition >> 8) & 0xFF;                 // octet 10


    // Envoyer le rapport HID
    bool result = hid.SendReport(0x01, reportData, index);
    return result;
}

void USBHIDWheel::setWheelPosition(uint16_t position) {
  wheelPosition = position;
  sendReport();
}

void USBHIDWheel::setAcceleratorPosition(uint16_t position) {
  acceleratorPosition = position;
  sendReport();
}

void USBHIDWheel::setBrakePosition(uint16_t position) {
  brakePosition = position;
  sendReport();
}

void USBHIDWheel::setClutchPosition(uint16_t position) {
  clutchPosition = position;
  sendReport();
}

void USBHIDWheel::pressButton(uint8_t button) {
  if (button < 32) {
    buttons |= (1 << button);
    sendReport();
  }
}

void USBHIDWheel::releaseButton(uint8_t button) {
  if (button < 32) {
    buttons &= ~(1 << button);
    sendReport();
  }
}

bool USBHIDWheel::sendState() {
  return sendReport();
}

// Gestion des rapports de sortie (pour le force feedback)
void USBHIDWheel::_onOutput(uint8_t report_id, const uint8_t *buffer, uint16_t len) {
    // Analyser le rapport de sortie en fonction du report_id
    switch (report_id) {
        case 0x01:
            handlePIDStateReport(buffer, len);
            break;
        default:
            // Rapport inconnu
            break;
    }
}

void USBHIDWheel::handlePIDStateReport(const uint8_t *buffer, uint16_t len) {
    // Analysez le contenu du rapport PID State
    // Implémentez la logique pour mettre à jour l'état des effets
}

#endif /* CONFIG_TINYUSB_HID_ENABLED */
#endif /* SOC_USB_OTG_SUPPORTED */
