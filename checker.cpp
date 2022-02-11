#include <assert.h>
#include <iostream>
#define TEMPSCALARTEST 1.0
#define SOCSCALARTEST 1.0
#define CHRASCALARTEST 0.01
class Battery{
private:
  enum ErrorCode{
    NoError = 0,
    TempError = 0x1,
    SoCError = 0x2,
    ChargeRateError = 0x4
  };
  const float _MinTemp, _MaxTemp;
  const float _MinSoC, _MaxSoC;
  const float _MaxChargeRate;
public:
  bool IsTemperatureDanger(float temp){
    bool ret = (temp < _MinTemp || temp > _MaxTemp);
    if(ret) std::cerr << "Temperature out of range!\n";
    return ret;
  }
  bool IsStateOfChargeDanger(float soc){
    bool ret = (soc < _MinSoC || soc > _MaxSoC);
    if(ret) std::cerr << "State of Charge out of range!\n";
    return ret;
  }
  bool IsChargeRateDanger(float chargeRate){
    bool ret = (chargeRate > _MaxChargeRate);
    if(ret) std::cerr << "Charge Rate out of range!\n";
    return ret;
  }
  Battery(float MinTemp, float MaxTemp, float MinSoC, float MaxSoC, float MaxChargeRate):
          _MinTemp(MinTemp), _MaxTemp(MaxTemp), _MinSoC(MinSoC), _MaxSoC(MaxSoC), _MaxChargeRate(MaxChargeRate){};
  bool batteryIsOk(float temperature, float soc, float chargeRate);
};

bool Battery::batteryIsOk(float temperature, float soc, float chargeRate) {
  unsigned char Error = (IsTemperatureDanger(temperature) | (IsStateOfChargeDanger(soc) << 1) | (IsChargeRateDanger(chargeRate) << 2));
  return !Error;
}

int main() {
  //As each measurement is independent of each other a linear vector can
  //be used to test instead of a 3D space
  Battery MyBattery(0.0, 45.0, 20.0, 80.0, 0.8);
  for(float BaseIndex = 0; BaseIndex < 100; BaseIndex+=10){
    std::cout << "\tTest index : " << BaseIndex << std::endl;
     assert(MyBattery.batteryIsOk(BaseIndex*TEMPSCALARTEST, 70, 0.7) == !MyBattery.IsTemperatureDanger(BaseIndex*TEMPSCALARTEST));
     assert(MyBattery.batteryIsOk(40.0, BaseIndex*SOCSCALARTEST, 0.7) == !MyBattery.IsStateOfChargeDanger(BaseIndex*SOCSCALARTEST));
     assert(MyBattery.batteryIsOk(40.0, 70, BaseIndex*CHRASCALARTEST) == !MyBattery.IsChargeRateDanger(BaseIndex*CHRASCALARTEST));
  }
}
