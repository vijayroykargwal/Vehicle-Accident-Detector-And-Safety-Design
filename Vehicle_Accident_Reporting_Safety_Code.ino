#include <dht.h>
#define dataPin 13 // Defines pin number to which the sensor is connected
dht DHT;
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
#include <SoftwareSerial.h>
#include <TinyGPS.h>
TinyGPS gps;
SoftwareSerial ss(10, 11);
static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);
int fan = 8;
int ir = 9;
int irs = 0;
int mq = 12;
int mqs = 0;
int buzz = A0;
void setup()
{
  pinMode(fan, OUTPUT);
  pinMode(buzz, OUTPUT);
  pinMode(ir, INPUT);
  pinMode(mq, INPUT);
  Serial.begin(9600);///gsm
  ss.begin(9600);//gps
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print("NIELIT");
  delay(2000);
  lcd.clear();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCIDENT DETE.");
  lcd.setCursor(0, 1);
  lcd.print("& SAFTY SYSTEM");
  delay(2000);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GUIDED BY:-");
  lcd.setCursor(0, 1);
  lcd.print("PROF.A.K. JOSHI");
  delay(4000);
  lcd.clear();
}

void loop()
{
  int readData = DHT.read22(dataPin); // Reads the data from the sensor
  float t = DHT.temperature; // Gets the values of the temperature
  float h = DHT.humidity; // Gets the values of the humidity


  int frc = digitalRead(ir);
  int mqs = digitalRead(mq);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TEMP   HUMI   O2L");
  lcd.setCursor(0, 1);
  lcd.print(t);
  lcd.setCursor(7, 1);
  lcd.print(h);
  lcd.setCursor(15, 1);
  lcd.print(mqs);
  delay(400);

  if (t >= 50.0)
  {
    digitalWrite(fan, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TEMP CROSSED");
    lcd.setCursor(0, 1);
    lcd.print("THE LIMIT");
    delay(400);

  }
  else
  {
    digitalWrite(fan, LOW);
  }
  if (mqs == LOW)
  {
    digitalWrite(fan, HIGH);
    digitalWrite(buzz, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("O2 CROSSED");
    lcd.setCursor(0, 1);
    lcd.print("THE LIMIT");
    delay(400);
  }
  else
  {
    digitalWrite(fan, LOW);
    digitalWrite(buzz, LOW);
  }

  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
  gps.satellites();
  gps.hdop();
  gps.f_get_position(&flat, &flon, &age);

  if (frc == LOW)
  {
    digitalWrite(buzz, HIGH);
    /////////////////////////////////////////////////////////////
    Serial.println();
    delay(1000);
    Serial.println("AT+CMGF=1");
    delay(200);        // delay in between reads for stability
    Serial.println("AT+CMGF=1");
    delay(200);
    Serial.println("AT+CMGF=1");
    delay(200);
    Serial.println("AT+CMGS=\"+917972288506\""); //set the phone number
    delay(200);
    Serial.println("Attention!!!!!! Accident OF MH 20 BS 7777 ");
    print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
    print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
    delay(500);
    Serial.print("Location");
    //Serial.print("Guded By:S.M.Badave Mam");
    delay(50);
    delay(500);
    Serial.write(26);
    ////////////////////////////////////////////////////////
    digitalWrite(buzz, LOW);
  }


  smartdelay(1000);

}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  }
  while (millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
  smartdelay(0);
}

static void print_date(TinyGPS &gps)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
            month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  smartdelay(0);
}

static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i = 0; i < len; ++i)
    Serial.print(i < slen ? str[i] : ' ');
  smartdelay(0);
}
