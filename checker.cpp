#include <assert.h>
#include <iostream>

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
  bool IsTemperatureDanger(float temp){
    return (temp < _MinTemp || temp > _MaxTemp);
  }
  bool IsStateOfChargeDanger(float soc){
    return (soc < _MinSoC || soc > _MaxSoC);
  }
  bool IsChargeRateDanger(float chargeRate){
    return (chargeRate > _MaxChargeRate);
  }
public:
  Battery(float MinTemp, float MaxTemp, float MinSoC, float MaxSoC, float MaxChargeRate):
          _MinTemp(MinTemp), _MaxTemp(MaxTemp), _MinSoC(MinSoC), _MaxSoC(MaxSoC), _MaxChargeRate(MaxChargeRate){};
  bool batteryIsOk(float temperature, float soc, float chargeRate);
};

bool Battery::batteryIsOk(float temperature, float soc, float chargeRate) {
  unsigned char Error = (IsTemperatureDanger(temperature) | (IsStateOfChargeDanger(soc) << 1) | (IsChargeRateDanger(chargeRate) << 2));
  if(Error){
    if(Error & Battery::TempError){std::cerr << "Temperature out of range!\n";}
    if(Error & Battery::SoCError){std::cerr << "State of Charge out of range!\n";}
    if(Error & Battery::ChargeRateError){std::cerr << "Charge Rate out of range!\n";}
    if(Error & Battery::NoError){std::cout << "Everything OK\n";}
  }
  return !Error;
}

int main() {
  //As each measurement is independent of each other a linear vector can
  //be used to test instead of a 3D space
  Battery MyBattery(0.0, 45.0, 20.0, 80.0, 0.8);
  assert(MyBattery.batteryIsOk(25, 70, 0.7) == true);
  assert(MyBattery.batteryIsOk(50, 85, 0.0) == false);
}
