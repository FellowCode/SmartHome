

String SplitLeft(String value, char c){
  int i = value.indexOf(c);
  if (i>0)
    return  value.substring(0, i);
  else
    return value;
}
String SplitRight(String value, char c){
  int i = value.indexOf(c);
  if (i>0)
    return  value.substring(i+1);
  else
    return "";
}

String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}

IPAddress String2IPAddress(String strIP){
  int Parts[4] = {0,0,0,0};
  int Part = 0;
  for ( int i=0; i<strIP.length(); i++ )
  {
    char c = strIP[i];
    if ( c == '.' )
    {
      Part++;
      continue;
    }
    Parts[Part] *= 10;
    Parts[Part] += c - '0';
  }
  IPAddress ip( Parts[0], Parts[1], Parts[2], Parts[3] );
  return ip;
}

float getKWtH(int timeSec)
{
  return timeSec/60.0/60.0*watts/1000.0;
}
int getSecond(long milsec)
{
  return milsec/1000;
}
float getMax(float a[])
{
   float max = -100000;
   for(int i=0; i<sizeof(a); i++)
    if(a[i] > max)
      max = a[i];
   return max;
}
float getMin(float a[])
{
   float min = 100000;
   for(int i=0; i<sizeof(a); i++)
    if(a[i] < min)
      min = a[i];
   return min;
}
