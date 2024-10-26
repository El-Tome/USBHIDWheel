#include "MyUSBHID.h"
#if SOC_USB_OTG_SUPPORTED

#if CONFIG_TINYUSB_HID_ENABLED

#include "USBHIDWheel.h"

#define ARDUHAL_LOG_LEVEL ARDUHAL_LOG_LEVEL_DEBUG


// Votre descripteur HID personnalisé
static const uint8_t report_descriptor[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Controls)
    0x09, 0x04,        // Usage (Joystick)
    0xA1, 0x01,        // Collection (Application)

    // *** Rapport d'entrée ***
    // Volant - Axe X
    0x09, 0x30,        // Usage (X)
    0x15, 0x00,        // Logical Minimum (0)
    0x26, 0xFF, 0x03,  // Logical Maximum (1023)
    0x75, 0x10,        // Report Size (16 bits)
    0x95, 0x01,        // Report Count (1)
    0x81, 0x02,        // Input (Data, Variable, Absolute)

    // Boutons (14 boutons + 2 paddles)
    0x05, 0x09,        // Usage Page (Button)
    0x19, 0x01,        // Usage Minimum (Button 1)
    0x29, 0x10,        // Usage Maximum (Button 14 + 2 paddles)
    0x15, 0x00,        // Logical Minimum (0)
    0x25, 0x01,        // Logical Maximum (1)
    0x75, 0x01,        // Report Size (1)
    0x95, 0x10,        // Report Count (16)
    0x81, 0x02,        // Input (Data, Variable, Absolute)


    // gear shift 8 boutons
    0x05, 0x09,        // Usage Page (Button)
    0x19, 0x11,        // Usage Minimum (Button 17)
    0x29, 0x18,        // Usage Maximum (Button 24)
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


    // *** Rapport de sortie ***
    // Usage Page (Physical Interface Device)
    0x05, 0x0F,             // PID Page

    // --- Set Effect Report ---
    0x09, 0x21,             // Usage (Set Effect Report)
    0xA1, 0x02,             // Collection (Logical)
        0x85, 0x01,             // Report ID (1)
        // Effect Block Index
        0x09, 0x22,             // Usage (Effect Block Index)
        0x15, 0x01,             // Logical Minimum (1)
        0x25, 0x28,             // Logical Maximum (40)
        0x35, 0x01,             // Physical Minimum (1)
        0x45, 0x28,             // Physical Maximum (40)
        0x75, 0x08,             // Report Size (8 bits)
        0x95, 0x01,             // Report Count (1)
        0x91, 0x02,             // Output (Data, Variable, Absolute)

        // Effect Type
        0x09, 0x25,             // Usage (Effect Type)
        0xA1, 0x02,             // Collection (Logical)
            // Liste des types d'effets
            0x09, 0x26,         // Usage (ET Constant Force)
            0x09, 0x27,         // Usage (ET Ramp)
            0x09, 0x30,         // Usage (ET Square)
            0x09, 0x31,         // Usage (ET Sine)
            0x09, 0x32,         // Usage (ET Triangle)
            0x09, 0x33,         // Usage (ET Sawtooth Up)
            0x09, 0x34,         // Usage (ET Sawtooth Down)
            0x09, 0x28,         // Usage (ET Spring)
            0x09, 0x29,         // Usage (ET Damper)
            0x09, 0x2A,         // Usage (ET Inertia)
            0x09, 0x2B,         // Usage (ET Friction)
            0x25, 0x0B,         // Logical Maximum (11)
            0x15, 0x01,         // Logical Minimum (1)
            0x75, 0x08,         // Report Size (8 bits)
            0x95, 0x01,         // Report Count (1)
            0x91, 0x00,         // Output (Data, Array)
        0xC0,                   // Fin de la Collection (Effect Type)

        // Duration
        0x09, 0x52,             // Usage (Duration)
        0x66, 0x03, 0x10,       // Unit (Seconds)
        0x55, 0xFD,             // Unit Exponent (-3)
        0x15, 0x00,             // Logical Minimum (0)
        0x26, 0xFF, 0x7F,       // Logical Maximum (32767)
        0x35, 0x00,             // Physical Minimum (0)
        0x46, 0xFF, 0x7F,       // Physical Maximum (32767)
        0x75, 0x10,             // Report Size (16 bits)
        0x95, 0x01,             // Report Count (1)
        0x91, 0x02,             // Output (Data, Variable, Absolute)

        // Sample Period
        0x09, 0x50,             // Usage (Sample Period)
        0x66, 0x03, 0x10,       // Unit (Seconds)
        0x55, 0xFD,             // Unit Exponent (-3)
        0x15, 0x00,
        0x26, 0xFF, 0x7F,
        0x35, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

        // Gain
        0x09, 0x54,             // Usage (Gain)
        0x15, 0x00,
        0x26, 0xFF, 0x00,       // Logical Maximum (255)
        0x35, 0x00,
        0x46, 0x10, 0x27,       // Physical Maximum (10000)
        0x75, 0x08,
        0x95, 0x01,
        0x91, 0x02,

        // Trigger Button
        0x09, 0x53,             // Usage (Trigger Button)
        0x15, 0x01,
        0x25, 0x10,             // Logical Maximum (16)
        0x35, 0x01,
        0x45, 0x10,             // Physical Maximum (16)
        0x75, 0x08,
        0x95, 0x01,
        0x91, 0x02,

        // Trigger Repeat Interval
        0x09, 0x55,             // Usage (Trigger Repeat Interval)
        0x66, 0x03, 0x10,       // Unit (Seconds)
        0x55, 0xFD,             // Unit Exponent (-3)
        0x15, 0x00,
        0x26, 0xFF, 0x7F,
        0x35, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

        // Axes Enable
        0x09, 0x5A,             // Usage (Axes Enable)
        0xA1, 0x02,             // Collection (Logical)
            0x09, 0x5B,         // Usage (Direction Enable)
            0x15, 0x00,
            0x25, 0x01,
            0x75, 0x01,
            0x95, 0x02,         // Report Count (2)
            0x91, 0x02,         // Output (Data, Variable, Absolute)
            // Padding
            0x75, 0x06,
            0x95, 0x01,
            0x91, 0x03,         // Output (Constant, Variable, Absolute)
        0xC0,                   // Fin de la Collection (Axes Enable)

        // Direction
        0x09, 0x5C,             // Usage (Direction)
        0x15, 0x00,
        0x26, 0xFF, 0x00,
        0x35, 0x00,
        0x46, 0xFF, 0x00,
        0x75, 0x08,
        0x95, 0x02,             // Report Count (2 directions)
        0x91, 0x02,

    0xC0,                   // Fin de la Collection (Set Effect Report)

    // --- Set Envelope Report ---
    0x09, 0x67,             // Usage (Set Envelope Report)
    0xA1, 0x02,             // Collection (Logical)
        0x85, 0x02,             // Report ID (2)
        // Effect Block Index
        0x09, 0x22,             // Usage (Effect Block Index)
        0x15, 0x01,
        0x25, 0x28,
        0x35, 0x01,
        0x45, 0x28,
        0x75, 0x08,
        0x95, 0x01,
        0x91, 0x02,

        // Attack Level
        0x09, 0x65,             // Usage (Attack Level)
        0x16, 0x00, 0x00,
        0x26, 0xFF, 0x7F,
        0x35, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

        // Fade Level
        0x09, 0x66,             // Usage (Fade Level)
        0x16, 0x00, 0x00,
        0x26, 0xFF, 0x7F,
        0x35, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

        // Attack Time
        0x09, 0x63,             // Usage (Attack Time)
        0x66, 0x03, 0x10,
        0x55, 0xFD,
        0x15, 0x00,
        0x26, 0xFF, 0x7F,
        0x35, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

        // Fade Time
        0x09, 0x64,             // Usage (Fade Time)
        0x66, 0x03, 0x10,
        0x55, 0xFD,
        0x15, 0x00,
        0x26, 0xFF, 0x7F,
        0x35, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

    0xC0,                   // Fin de la Collection (Set Envelope Report)

    // --- Set Condition Report ---
    0x09, 0x7E,             // Usage (Set Condition Report)
    0xA1, 0x02,             // Collection (Logical)
        0x85, 0x03,             // Report ID (3)
        // Effect Block Index
        0x09, 0x22,             // Usage (Effect Block Index)
        0x15, 0x01,
        0x25, 0x28,
        0x35, 0x01,
        0x45, 0x28,
        0x75, 0x08,
        0x95, 0x01,
        0x91, 0x02,

        // Parameter Block Offset
        0x09, 0x23,             // Usage (Parameter Block Offset)
        0x15, 0x00,
        0x25, 0x01,
        0x35, 0x00,
        0x45, 0x01,
        0x75, 0x04,
        0x95, 0x01,
        0x91, 0x02,

        // Reserved
        0x75, 0x04,
        0x95, 0x01,
        0x91, 0x03,

        // Positive Coefficient
        0x09, 0x7F,             // Usage (Positive Coefficient)
        0x16, 0x00, 0x00,
        0x26, 0xFF, 0x7F,
        0x36, 0x00, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

        // Negative Coefficient
        0x09, 0x80,             // Usage (Negative Coefficient)
        0x16, 0x00, 0x00,
        0x26, 0xFF, 0x7F,
        0x36, 0x00, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

        // Positive Saturation
        0x09, 0x81,             // Usage (Positive Saturation)
        0x16, 0x00, 0x00,
        0x26, 0xFF, 0x7F,
        0x36, 0x00, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

        // Negative Saturation
        0x09, 0x82,             // Usage (Negative Saturation)
        0x16, 0x00, 0x00,
        0x26, 0xFF, 0x7F,
        0x36, 0x00, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

        // Dead Band
        0x09, 0x83,             // Usage (Dead Band)
        0x16, 0x00, 0x00,
        0x26, 0xFF, 0x7F,
        0x36, 0x00, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

    0xC0,                   // Fin de la Collection (Set Condition Report)

    // --- Set Periodic Report ---
    0x09, 0x79,             // Usage (Set Periodic Report)
    0xA1, 0x02,             // Collection (Logical)
        0x85, 0x04,             // Report ID (4)
        // Effect Block Index
        0x09, 0x22,             // Usage (Effect Block Index)
        0x15, 0x01,
        0x25, 0x28,
        0x35, 0x01,
        0x45, 0x28,
        0x75, 0x08,
        0x95, 0x01,
        0x91, 0x02,

        // Magnitude
        0x09, 0x70,             // Usage (Magnitude)
        0x16, 0x00, 0x00,
        0x26, 0xFF, 0x7F,
        0x35, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

        // Offset
        0x09, 0x71,             // Usage (Offset)
        0x16, 0x00, 0x80,
        0x26, 0xFF, 0x7F,
        0x36, 0x00, 0x80,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0x91, 0x02,

        // Phase
        0x09, 0x72,             // Usage (Phase)
        0x15, 0x00,
        0x26, 0xFF, 0x00,
        0x35, 0x00,
        0x46, 0xFF, 0x00,
        0x75, 0x08,
        0x95, 0x01,
        0x91, 0x02,

        // Period
        0x09, 0x73,             // Usage (Period)
        0x66, 0x03, 0x10,
        0x55, 0xFD,
        0x15, 0x00,
        0x26, 0xFF, 0x7F,
        0x35, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x20,
        0x95, 0x01,
        0x91, 0x02,

    0xC0,                   // Fin de la Collection (Set Periodic Report)

    // --- Set Constant Force Report ---
    0x09, 0x73,             // Usage (Set Constant Force Report)
    0xA1, 0x02,             // Collection (Logical)
        0x85, 0x05,             // Report ID (5)
        // Effect Block Index
        0x09, 0x22,             // Usage (Effect Block Index)
        0x15, 0x01,
        0x25, 0x28,
        0x35, 0x01,
        0x45, 0x28,
        0x75, 0x08,
        0x95, 0x01,
        0x91, 0x02,

        // Magnitude
        0x09, 0x70,             // Usage (Magnitude)
        0x16, 0x00, 0x80,       // Logical Minimum (-32768)
        0x26, 0xFF, 0x7F,       // Logical Maximum (32767)
        0x36, 0x00, 0x80,       // Physical Minimum (-32768)
        0x46, 0xFF, 0x7F,       // Physical Maximum (32767)
        0x75, 0x10,             // Report Size (16 bits)
        0x95, 0x01,             // Report Count (1)
        0x91, 0x02,

    0xC0,                   // Fin de la Collection (Set Constant Force Report)

    // --- Effect Operation Report ---
    0x09, 0x78,             // Usage (Effect Operation Report)
    0xA1, 0x02,             // Collection (Logical)
        0x85, 0x06,             // Report ID (6)
        // Effect Block Index
        0x09, 0x22,             // Usage (Effect Block Index)
        0x15, 0x01,
        0x25, 0x28,
        0x35, 0x01,
        0x45, 0x28,
        0x75, 0x08,
        0x95, 0x01,
        0x91, 0x02,

        // Operation
        0x09, 0xA9,             // Usage (Effect Operation)
        0xA1, 0x02,             // Collection (Logical)
            0x09, 0x7C,         // Usage (Op Effect Start)
            0x09, 0x7D,         // Usage (Op Effect Start Solo)
            0x09, 0x7E,         // Usage (Op Effect Stop)
            0x15, 0x01,
            0x25, 0x03,
            0x75, 0x08,
            0x95, 0x01,
            0x91, 0x00,         // Output (Data, Array)
        0xC0,                   // Fin de la Collection (Effect Operation)

        // Loop Count
        0x09, 0xAC,             // Usage (Loop Count)
        0x15, 0x00,
        0x26, 0xFF, 0x00,
        0x35, 0x00,
        0x46, 0xFF, 0x00,
        0x75, 0x08,
        0x95, 0x01,
        0x91, 0x02,

    0xC0,                   // Fin de la Collection (Effect Operation Report)

    // --- PID Block Free Report ---
    0x09, 0x77,             // Usage (PID Block Free Report)
    0xA1, 0x02,             // Collection (Logical)
        0x85, 0x07,             // Report ID (7)
        // Effect Block Index
        0x09, 0x22,             // Usage (Effect Block Index)
        0x15, 0x01,
        0x25, 0x28,
        0x35, 0x01,
        0x45, 0x28,
        0x75, 0x08,
        0x95, 0x01,
        0x91, 0x02,

    0xC0,                   // Fin de la Collection (PID Block Free Report)

    // --- Device Gain Report ---
    0x09, 0x92,             // Usage (Device Gain Report)
    0xA1, 0x02,             // Collection (Logical)
        0x85, 0x08,             // Report ID (8)
        // Gain
        0x09, 0x7F,             // Usage (Device Gain)
        0x15, 0x00,
        0x26, 0xFF, 0x00,
        0x35, 0x00,
        0x46, 0x10, 0x27,
        0x75, 0x08,
        0x95, 0x01,
        0x91, 0x02,

    0xC0,                   // Fin de la Collection (Device Gain Report)

    // --- PID Pool Report ---
    0x09, 0x80,             // Usage (PID Pool Report)
    0xA1, 0x02,             // Collection (Logical)
        0x85, 0x09,             // Report ID (9)
        // RAM Pool Size
        0x09, 0x83,             // Usage (RAM Pool Size)
        0x15, 0x00,
        0x26, 0xFF, 0x7F,
        0x35, 0x00,
        0x46, 0xFF, 0x7F,
        0x75, 0x10,
        0x95, 0x01,
        0xB1, 0x02,             // Feature (Data, Variable, Absolute)

        // Simultaneous Effects Max
        0x09, 0xA9,             // Usage (Simultaneous Effects Max)
        0x15, 0x00,
        0x25, 0x28,
        0x35, 0x00,
        0x45, 0x28,
        0x75, 0x08,
        0x95, 0x01,
        0xB1, 0x02,

        // Device Managed Pool
        0x09, 0xAA,             // Usage (Device Managed Pool)
        0x15, 0x00,
        0x25, 0x01,
        0x35, 0x00,
        0x45, 0x01,
        0x75, 0x08,
        0x95, 0x01,
        0xB1, 0x02,

    0xC0,                   // Fin de la Collection (PID Pool Report)

    0xC0                    // Fin de la Collection (Application)
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

    const int reportSizeBits = 16 + 14 + 2 + 8 + 16 + 16 + 16;   // wheel + buttons + gear shift + brake + clutch + accelerator
    const int reportSize = reportSizeBits / 8;
    uint8_t reportData[reportSize];
    int index = 0;

    // Préparer le rapport
    reportData[index++] = wheelPosition & 0xFF;                         // octet 0
    reportData[index++] = (wheelPosition >> 8) & 0xFF;                  // octet 1
    reportData[index++] = (buttons & 0xFF);                             // octet 2
    reportData[index++] = (buttons >> 8) & 0xFF;                        // octet 3
    reportData[index++] = (buttons >> 16) & 0xFF;                       // octet 4
    reportData[index++] = acceleratorPosition & 0xFF;                   // octet 5
    reportData[index++] = (acceleratorPosition >> 8) & 0xFF;            // octet 6
    reportData[index++] = brakePosition & 0xFF;                         // octet 7
    reportData[index++] = (brakePosition >> 8) & 0xFF;                  // octet 8
    reportData[index++] = clutchPosition & 0xFF;                        // octet 9
    reportData[index++] = (clutchPosition >> 8) & 0xFF;                 // octet 10


    // Envoyer le rapport HID
    bool result = hid.SendReport(0, reportData, index);
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
    switch (report_id) {
        case 1: // Set Effect Report
            parseSetEffectReport(buffer, len);
        break;
        case 2: // Set Envelope Report
            parseSetEnvelopeReport(buffer, len);
        break;
        case 3: // Set Condition Report
            parseSetConditionReport(buffer, len);
        break;
        case 4: // Set Periodic Report
            parseSetPeriodicReport(buffer, len);
        break;
        case 5: // Set Constant Force Report
            parseSetConstantForceReport(buffer, len);
        break;
        case 6: // Effect Operation Report
            parseEffectOperationReport(buffer, len);
        break;
        // Ajouter d'autres cas pour les autres Report IDs
        default:
            // Rapport non reconnu
                break;
    }
}


uint16_t USBHIDWheel::_onGetFeature(uint8_t report_id, uint8_t* buffer, uint16_t len) {
    switch (report_id) {
        case 9: // PID Pool Report
            if (len < 7) {
                return 0; // Taille du buffer insuffisante
            }

        // RAM Pool Size (16 bits)
        buffer[0] = 0x00; // LSB
        buffer[1] = 0x20; // MSB (par exemple, 8192 octets de RAM)

        // Simultaneous Effects Max (8 bits)
        buffer[2] = 0x10; // Par exemple, 16 effets simultanés

        // Device Managed Pool (8 bits)
        buffer[3] = 0x01; // 1 si le périphérique gère la mémoire des effets

        // RAM Pool Available (16 bits)
        buffer[4] = 0x00; // LSB
        buffer[5] = 0x20; // MSB (par exemple, 8192 octets disponibles)

        // Remplir le reste si nécessaire
        return 6; // Taille du rapport
        default:
            // Rapport non reconnu
                return 0;
    }
}




#endif /* CONFIG_TINYUSB_HID_ENABLED */
#endif /* SOC_USB_OTG_SUPPORTED */
