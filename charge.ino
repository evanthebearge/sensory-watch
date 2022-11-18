/*
FROM: https://forum.seeedstudio.com/t/xiao-ble-sense-battery-level-and-charging-status/263248/36
*/
#include <nrf52840.h>
#include <nrfx_saadc.h>
#include <AnalogIn.h>
#include <pinDefinitions.h>

class HackAnalogIn: public mbed::AnalogIn {
  using mbed::AnalogIn::AnalogIn;
  public:
    analogin_t getAnalogIn_t();
};

analogin_t HackAnalogIn::getAnalogIn_t() {
  return this->_adc;
}

void startReadingBatteryLevel (nrf_saadc_value_t* buffer) {
  auto pin = PIN_VBAT;
  PinName name = analogPinToPinName(pin);
  if (name == NC) { return; }
  HackAnalogIn* adc = static_cast<HackAnalogIn*>(analogPinToAdcObj(pin));
  if (adc == NULL) {
    adc = new HackAnalogIn(name);
    analogPinToAdcObj(pin) = static_cast<mbed::AnalogIn*>(adc);
#ifdef ANALOG_CONFIG
    if (isAdcConfigChanged) {
      adc->configure(adcCurrentConfig);
    }
#endif
  }

  nrfx_saadc_buffer_convert(buffer, 1);
  nrfx_err_t ret = nrfx_saadc_sample();
  if (ret == NRFX_ERROR_BUSY) {
    // failed to start sampling
    return;
  }
}

void setup() {
  pinMode(P0_14, OUTPUT);
  digitalWrite(P0_14,LOW);
  // init BLE, IMU and other peripherals here
  ...
}

nrf_saadc_value_t BatteryLevel = { 0 };

void loop() {
  // BLE.poll()
  static unsigned long _lastT = 0;
  unsigned long _t = millis();

  if (_t - _lastT > 1000) {
    // read battery level every 1 second
    startReadingBatteryLevel(&BatteryLevel);
    _lastT = _t;
  }
  
  // check if ADC conversion has completed
  if (nrf_saadc_event_check(NRF_SAADC_EVENT_DONE)) {
    // ADC conversion completed. Reading is stored in BatteryLevel
    nrf_saadc_event_clear(NRF_SAADC_EVENT_DONE);
    float vBat = (float)BatteryLevel / 4096 * 3.3 / 510 * (1000 + 510);
    // write value to characteristic or things you want to do
    ...
  }
  ...
}
