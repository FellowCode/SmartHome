String getDate()
{
  return getDay() + "/" + getMonth() + "/" + getYear();
}
String getDateFromVal(time_t t)
{
  return getDigits(day(t)) + "/" + getDigits(month(t)) + "/" + year(t);
}
String getTime()
{
  return getHour() + ":" + getMinute();
}
String getHour()
{
  return getDigits(hour());
}
String getMinute()
{
  return getDigits(minute());
}
String getDay()
{
  return getDigits(day());
}
String getMonth()
{
  return getDigits(month());
}
String getYear()
{
  return String(year());
}
String getDigits(int val)
{
  String param = "";
  if(val < 10)
    param += "0";
  param += String(val);
  return param;
}
void setDate(String date)
{
  setDay(date.substring(0,2).toInt());
  setMonth(date.substring(3,5).toInt());
  setYear(date.substring(6).toInt());
}
void SetTime(String time)
{
  setHour(time.substring(0,2).toInt());
  setMinute(time.substring(3,5).toInt());
  setSecond(time.substring(6).toInt());
}
void setHour(int hour)
{
  setTime(hour, minute(), second(), day(), month(), year());
}
void setMinute(int minute)
{
  setTime(hour(), minute, 0, day(), month(), year());
}
void setSecond(int second)
{
  setTime(hour(), minute(), second, day(), month(), year());
}
void setDay(int day)
{
  setTime(hour(), minute(), second(), day, month(), year());
}
time_t setDayFromVal(time_t t, int day)
{
  return makeTime(hour(t), minute(t), second(t), day, month(t), year(t));
}
void setMonth(int month)
{
  setTime(hour(), minute(), second(), day(), month, year());
}
void setYear(int year)
{
  setTime(hour(), minute(), second(), day(), month(), year);
}
