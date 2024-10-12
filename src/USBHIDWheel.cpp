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

    // Volant - Axe X
    0x09, 0x30,        // Usage (X)
    0x15, 0x00,        // Logical Minimum (0)
    0x26, 0xFF, 0x03,  // Logical Maximum (1023)
    0x75, 0x10,        // Report Size (16 bits)
    0x95, 0x01,        // Report Count (1)
    0x81, 0x02,        // Input (Data, Variable, Absolute)

    // Boutons (4 boutons)
    0x05, 0x09,        // Usage Page (Button)
    0x19, 0x01,        // Usage Minimum (Button 1)
    0x29, 0x04,        // Usage Maximum (Button 4)
    0x15, 0x00,        // Logical Minimum (0)
    0x25, 0x01,        // Logical Maximum (1)
    0x75, 0x01,        // Report Size (1)
    0x95, 0x04,        // Report Count (4)
    0x81, 0x02,        // Input (Data, Variable, Absolute)

    // Padding pour aligner sur un octet
    0x75, 0x04,        // Report Size (4 bits)
    0x95, 0x01,        // Report Count (1)
    0x81, 0x03,        // Input (Constant, Variable, Absolute)

    0xC0               // End Collection
};


USBHIDWheel::USBHIDWheel() : hid() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    hid.addDevice(this, sizeof(report_descriptor));
  }

  // Initialiser les variables
  wheelPosition = 512;        // Milieu
  acceleratorPosition = 0;
  brakePosition = 0;
  clutchPosition = 0;
  buttons = 0;
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
    //    log_w("HID not ready");
        return false;
    }

    const int reportSize = 3;
    uint8_t reportData[reportSize];
    int index = 0;

    // Préparer le rapport
    reportData[index++] = wheelPosition & 0xFF;
    reportData[index++] = (wheelPosition >> 8) & 0xFF;
    reportData[index++] = (buttons & 0x0F); // Boutons + padding

    // Afficher les données du rapport pour vérification
//    log_d("Sending report: %02X %02X %02X", reportData[0], reportData[1], reportData[2]);

    // Envoyer le rapport HID
    bool result = hid.SendReport(0, reportData, index);
    if (!result) {
  //      log_e("Failed to send report");
    }
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
  // Vérifier la longueur du rapport
  if (len < 4) {
    return; // Rapport incomplet
  }

  // Extraire les valeurs de force feedback
  uint8_t motorForce = buffer[0];
  uint8_t acceleratorForce = buffer[1];
  uint8_t brakeForce = buffer[2];
  uint8_t clutchForce = buffer[3];

  // Utiliser ces valeurs pour contrôler vos moteurs ou actionneurs
  // Par exemple :
  // setMotorForce(motorForce);
  // setAcceleratorForce(acceleratorForce);
  // etc.
}


#endif /* CONFIG_TINYUSB_HID_ENABLED */
#endif /* SOC_USB_OTG_SUPPORTED */
