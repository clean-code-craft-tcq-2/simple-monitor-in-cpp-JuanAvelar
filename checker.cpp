#include <assert.h>
#include <iostream>
#include <vector>
#include <array>
#define TEMPSCALARTEST 1.0
#define SOCSCALARTEST 1.0
#define CHRASCALARTEST 0.01
#define EARLYWARNINGSON true
enum Language{
  English,
  German,
  Spanish,
  NumberOfLaguagesIncluded
};
std::vector<std::array<std::string,2>> WarnTextLimitPassed =
                                      {{"WARNING: ", " out of range!\n"},
                                       {"ACHTUNG: ", " außer Reichweite!\n"},
                                       {"ADVERTENCIA: ", " fuera de rango!\n"}};
std::vector<std::array<std::string,2>> WarnTextLimitApproached =
                                      {{"IMPORTANT INFO: ", " approaching Limit\n"},
                                       {"WICHTIGE INFO: ", " Grenze nähern\n"},
                                       {"INFORMACION IMPORTANTE: ", " cerca del limite\n"}};

class Data_Limit{
private:
  const float _Minimum_limit, _Maximum_limit;
  std::string _WarningMessage;
  bool _EarlyWarningON;
  void PrintLimitWarning(Language lang = English){
    std::cerr << WarnTextLimitPassed.at(lang).at(0) << _WarningMessage << WarnTextLimitPassed.at(lang).at(1);
  }
  void PrintApproachingLimit(Language lang = English){
    std::cout << WarnTextLimitApproached.at(lang).at(0) << _WarningMessage << WarnTextLimitApproached.at(lang).at(1);
  }
  bool EarlyWarning(float reading, float percentTolerance){
    return (  ((reading < (_Minimum_limit + _Maximum_limit*percentTolerance)) &(reading >=_Minimum_limit))
            | ((reading <= _Maximum_limit)                                    &(reading > _Maximum_limit*(1.0-percentTolerance))));
  }
public:
  Data_Limit(float min, float max, std::string warning, bool early_warning = false): _Minimum_limit(min), _Maximum_limit(max), _WarningMessage(warning), _EarlyWarningON(early_warning){};
  bool IsLimitDanger(float reading, Language lang = English, bool mute = false){
    bool ret = ((reading < _Minimum_limit) | (reading > _Maximum_limit));
    if(ret & !mute) PrintLimitWarning(lang);
    if(!ret & EarlyWarning(reading, 0.05) & _EarlyWarningON & !mute) PrintApproachingLimit(lang);
    return ret;
  }
};
class Battery{
private:
  std::vector<Data_Limit> _Data_Ranges;
  Language _language;
  void checkDataIntegrity(std::vector<float> realTimeSensorData){
    if(realTimeSensorData.size()!=_Data_Ranges.size())std::cerr << "Error: Test Data is not correctly mapped\n";
  }
public:
  Battery(std::vector<Data_Limit> limits, Language lang = English): _Data_Ranges(limits), _language(lang){};
  bool batteryIsOk(std::vector<float> realTimeSensorData);
};
bool Battery::batteryIsOk(std::vector<float> realTimeSensorData) {
  unsigned int left_shift = 0, Error = 0, next_data = 0;
  checkDataIntegrity(realTimeSensorData);
  for(auto&& DATA : _Data_Ranges){
    //By performing a left shift a unique code of the error/s is generated
    Error |= (DATA.IsLimitDanger(realTimeSensorData.at(next_data++), _language) << left_shift++);
  }
  return !Error;//returning true means OK
}
int main(void) {
  //As each measurement is independent of each other a linear vector can
  //be used to test this instead of a 3D space
  std::vector<Data_Limit> Specification;
  Specification.push_back(Data_Limit(00.0,45.0,"Temperature"));
  Specification.push_back(Data_Limit(20.0,80.0,"State of Charge", EARLYWARNINGSON));//by default early warnings are turned OFF unless specified by parameter
  Specification.push_back(Data_Limit(0.00,0.80,"Charge Rate"));
  Battery MyBattery(Specification, Spanish);//change here Language or nothing to leave english
  for(float BaseIndex = -0.99; BaseIndex < 100; BaseIndex+=1){
    std::vector<float> SensorMapTest;
    std::cout << "\tTest index : " << BaseIndex << std::endl;
    SensorMapTest = {BaseIndex*(float)TEMPSCALARTEST, 70, 0.7};
    assert(MyBattery.batteryIsOk(SensorMapTest) == !Specification.at(0).IsLimitDanger(BaseIndex*TEMPSCALARTEST, English, true));
    SensorMapTest = {40.0, BaseIndex*(float)SOCSCALARTEST, 0.7};
    assert(MyBattery.batteryIsOk(SensorMapTest) == !Specification.at(1).IsLimitDanger(BaseIndex*SOCSCALARTEST , English, true));
    SensorMapTest = {40.0, 70, BaseIndex*(float)CHRASCALARTEST};
    assert(MyBattery.batteryIsOk(SensorMapTest) == !Specification.at(2).IsLimitDanger(BaseIndex*CHRASCALARTEST, English, true));
  }
  return 0;
}
