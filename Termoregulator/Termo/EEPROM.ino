
///***********************///
///EEPROM
///***********************///

#define STRING_BYTES  32

void SaveString(int addr, String buffer) {
  for (int L = 0; L < STRING_BYTES; L++) {
    EEPROM.write(addr + L, buffer[L]);
  }
  EEPROM.commit();
}
String LoadString(int addr) {
  String buffer;
  for (int L = addr; L < addr+STRING_BYTES; L++)
    if (isAscii(EEPROM.read(L)))
      buffer += char(EEPROM.read(L));
  return buffer.c_str();
}
void SaveFloat(int addr, float val)
{
  byte raw[4];
  (float&)raw = val;
  for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
  EEPROM.commit();
}
float LoadFloat(int addr)
{
  byte raw[4];
  for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr+i);
  float &val = (float&)raw;
  return val;
}
void SaveLong(int addr, unsigned long val)
{
  byte raw[4];
  (unsigned long&)raw = val;
  for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
  EEPROM.commit();
}
unsigned long LoadLong(int addr)
{
  byte raw[4];
  for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr+i);
  unsigned long &val = (unsigned long&)raw;
  return val;
}
void SaveInt(int addr, int val)
{
  EEPROM.write(addr, highByte(val));
  EEPROM.write(addr+1, lowByte(val));
  EEPROM.commit();
}
int LoadInt(int addr)
{
  byte high = EEPROM.read(addr);
  byte low = EEPROM.read(addr+1);

  int val = (high << 8)+ low;
  return val;
}

void SaveDateTime()
{
  EEPROM.write(A_DATETIME  , byte(hour()));
  EEPROM.write(A_DATETIME+1, byte(minute()));
  EEPROM.write(A_DATETIME+2, byte(second()));
  EEPROM.write(A_DATETIME+3, byte(day()));
  EEPROM.write(A_DATETIME+4, byte(month()));
  EEPROM.write(A_DATETIME+5, byte(year()-1970));
  EEPROM.commit();
}

void LoadDateTime()
{
  byte hours = EEPROM.read(A_DATETIME);
  byte minutes = EEPROM.read(A_DATETIME+1);
  byte seconds = EEPROM.read(A_DATETIME+2);
  byte days = EEPROM.read(A_DATETIME+3);
  byte months = EEPROM.read(A_DATETIME+4);
  int years = EEPROM.read(A_DATETIME+5)+1970;
  setTime(hours, minutes, seconds, days, months, years);
}
void SaveStatistic(int addr)
{
  for(int i=0; i<7; i++)
    SaveFloat(addr+i*4, statistic[i]);
}
void LoadStatistic(int addr)
{
  for(int i=0; i<7; i++)
    statistic[i] = LoadFloat(addr + i*4);
}
