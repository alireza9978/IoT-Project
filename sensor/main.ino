#include <EEPROM.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "Arduino.h"
#include "Printable.h"
#include "Preferences.h"

#define COMPLEX_LIB_VERSION (F("0.3.1"))
struct sensor_data {
  int time;
};

//Function Declartion
String getMacAddress();
String getFullMacAddress();
void start_wifi();
void ARDUINO_ISR_ATTR onTimer();
void check_http_request();
void send_home_http(WiFiClient client);
void send_data_http(WiFiClient client);
void send_mac_http(WiFiClient client);
void send_time_http(WiFiClient client);
void send_old_data_http(WiFiClient client);
void setup_eeprom();
struct sensor_data read_data_eeprom();
void save_data_eeprom(int address, int data);
void setupCT();
void CalculateVI();
void resetParameters(void);
void CalculateEnergy(uint32_t end_time, uint32_t start_time);
void calculateANDwritenergy(uint32_t end_time, uint32_t start_time);
void save_time(int time);
void save_old_time(int time);
void save_old_addr(int last_addr);
void save_valid_old(int valid_old);
void save_energy();
int load_time();
int load_valid_old();
int load_old_time();
int load_old_addr();
void load_energy();

// Variables Declartion
String board_mac;

// wifi vars
const char* password = "passpass";

WiFiServer server(80);

// timer variables
hw_timer_t* timer = NULL;
hw_timer_t* read_power_timer = NULL;


// leds
int PIN_LED = 14;
int is_on = 0;

//old data
bool old_data = false;

// EEPROM
int time_addr = 0;
int old_time_addr = 8;
int old_addr_addr = 16;
int valid_old_addr = 24;
int addr = 32;
int old_address = 0;
int start_addr = 32;
int address_shift = 8;
#define EEPROM_SIZE 4096
Preferences NVS;

//
int period_calculate = 50;
int periodWrite = 1000 * 10;
uint32_t last_time = 0;
uint32_t last_time_write = 0;

// Complex
class Complex : public Printable {
  public:
    Complex(const float r = 0, const float i = 0)
      : re(r), im(i) {};
    Complex(const Complex& c)
      : re(c.re), im(c.im) {};

    void set(const float r, const float i) {
      re = r;
      im = i;
    };
    float real() {
      return re;
    };
    float imag() {
      return im;
    };

    size_t printTo(Print& p) const;

    void polar(const float, const float);
    float phase() {
      return atan2(im, re);
    };
    float modulus() {
      return hypot(re, im);
    };
    // conjugate is the number mirrored in x-axis
    Complex conjugate() {
      return Complex(re, -im);
    };
    Complex reciprocal();

    bool operator==(const Complex&);
    bool operator!=(const Complex&);

    Complex operator-();  // negation

    Complex operator+(const Complex&);
    Complex operator-(const Complex&);
    Complex operator*(const Complex&);
    Complex operator/(const Complex&);

    Complex& operator+=(const Complex&);
    Complex& operator-=(const Complex&);
    Complex& operator*=(const Complex&);
    Complex& operator/=(const Complex&);

    Complex c_sqrt();
    Complex c_sqr();
    Complex c_exp();
    Complex c_log();
    Complex c_log10();
    Complex c_pow(const Complex&);
    Complex c_logn(const Complex&);

    Complex c_sin();
    Complex c_cos();
    Complex c_tan();
    Complex c_asin();
    Complex c_acos();
    Complex c_atan();

    Complex c_csc();
    Complex c_sec();
    Complex c_cot();
    Complex c_acsc();
    Complex c_asec();
    Complex c_acot();

    Complex c_sinh();
    Complex c_cosh();
    Complex c_tanh();
    Complex c_asinh();
    Complex c_acosh();
    Complex c_atanh();

    Complex c_csch();
    Complex c_sech();
    Complex c_coth();
    Complex c_acsch();
    Complex c_asech();
    Complex c_acoth();

  protected:
    float re;
    float im;

    Complex gonioHelper1(const byte);
    Complex gonioHelper2(const byte);
};

static Complex one(1, 0);

// PRINTING
size_t Complex::printTo(Print& p) const {
  size_t n = 0;
  n += p.print(re, 3);
  n += p.print(' ');
  n += p.print(im, 3);
  n += p.print('i');
  return n;
};

void Complex::polar(const float modulus, const float phase) {
  re = modulus * cos(phase);
  im = modulus * sin(phase);
}

Complex Complex::reciprocal() {
  float f = 1.0 / (re * re + im * im);
  float r = re * f;
  float i = -im * f;
  return Complex(r, i);
}


// EQUALITIES
bool Complex::operator==(const Complex& c) {
  return (re == c.re) && (im == c.im);
}

bool Complex::operator!=(const Complex& c) {
  return (re != c.re) || (im != c.im);
}

// NEGATE
Complex Complex::operator-() {
  return Complex(-re, -im);
}

// BASIC MATH
Complex Complex::operator+(const Complex& c) {
  return Complex(re + c.re, im + c.im);
}

Complex Complex::operator-(const Complex& c) {
  return Complex(re - c.re, im - c.im);
}

Complex Complex::operator*(const Complex& c) {
  float r = re * c.re - im * c.im;
  float i = re * c.im + im * c.re;
  return Complex(r, i);
}

Complex Complex::operator/(const Complex& c) {
  float f = 1.0 / (c.re * c.re + c.im * c.im);
  float r = (re * c.re + im * c.im) * f;
  float i = (im * c.re - re * c.im) * f;
  return Complex(r, i);
}

Complex& Complex::operator+=(const Complex& c) {
  re += c.re;
  im += c.im;
  return *this;
}

Complex& Complex::operator-=(const Complex& c) {
  re -= c.re;
  im -= c.im;
  return *this;
}

Complex& Complex::operator*=(const Complex& c) {
  float r = re * c.re - im * c.im;
  float i = re * c.im + im * c.re;
  re = r;
  im = i;
  return *this;
}

Complex& Complex::operator/=(const Complex& c) {
  float f = 1.0 / (c.re * c.re + c.im * c.im);
  float r = (re * c.re + im * c.im) * f;
  float i = (im * c.re - re * c.im) * f;
  re = r;
  im = i;
  return *this;
}

//
// POWER FUNCTIONS
//
Complex Complex::c_sqr() {
  float r = re * re - im * im;
  float i = 2 * re * im;
  return Complex(r, i);
}

Complex Complex::c_sqrt() {
  float m = modulus();
  float r = sqrt(0.5 * (m + re));
  float i = sqrt(0.5 * (m - re));
  if (im < 0) i = -i;
  return Complex(r, i);
}

Complex Complex::c_exp() {
  float e = exp(re);
  return Complex(e * cos(im), e * sin(im));
}

Complex Complex::c_log() {
  float m = modulus();
  float p = phase();
  if (p > PI) p -= 2 * PI;
  return Complex(log(m), p);
}

Complex Complex::c_pow(const Complex& c) {
  Complex t = c_log();
  t = t * c;
  return t.c_exp();
}

Complex Complex::c_logn(const Complex& c) {
  Complex t = c;
  return c_log() / t.c_log();
}

Complex Complex::c_log10() {
  return c_logn(10);
}

//
// GONIO I - SIN COS TAN
//
Complex Complex::c_sin() {
  return Complex(sin(re) * cosh(im), cos(re) * sinh(im));
}

Complex Complex::c_cos() {
  return Complex(cos(re) * cosh(im), -sin(re) * sinh(im));
}

Complex Complex::c_tan() {

  return c_sin() / c_cos();
}

Complex Complex::gonioHelper1(const byte mode) {
  Complex c = (one - this->c_sqr()).c_sqrt();
  if (mode == 0) {
    c = c + *this * Complex(0, -1);
  } else {
    c = *this + c * Complex(0, -1);
  }
  c = c.c_log() * Complex(0, 1);
  return c;
}

Complex Complex::c_asin() {
  return gonioHelper1(0);
}

Complex Complex::c_acos() {
  return gonioHelper1(1);
}

Complex Complex::c_atan() {
  return (Complex(0, -1) * (Complex(re, im - 1) / Complex(-re, -im - 1)).c_log()) * 0.5;
}

//
// GONIO II - CSC SEC COT
//
Complex Complex::c_csc() {
  return one / c_sin();
}

Complex Complex::c_sec() {
  return one / c_cos();
}

Complex Complex::c_cot() {
  return one / c_tan();
}

Complex Complex::c_acsc() {
  return (one / *this).c_asin();
}

Complex Complex::c_asec() {
  return (one / *this).c_acos();
}

Complex Complex::c_acot() {
  return (one / *this).c_atan();
}

//
// GONIO HYPERBOLICUS I
//
Complex Complex::c_sinh() {
  return Complex(cos(im) * sinh(re), sin(im) * cosh(re));
}

Complex Complex::c_cosh() {
  return Complex(cos(im) * cosh(re), sin(im) * sinh(re));
}

Complex Complex::c_tanh() {
  return c_sinh() / c_cosh();
}

Complex Complex::gonioHelper2(const byte mode) {
  Complex c = c_sqr();
  if (mode == 0) {
    c += 1;
  } else {
    c -= 1;
  }
  c = (*this + c.c_sqrt()).c_log();
  return c;
}

Complex Complex::c_asinh() {
  return gonioHelper2(0);
}

Complex Complex::c_acosh() {
  return gonioHelper2(1);
}

Complex Complex::c_atanh() {
  Complex c = (*this + one).c_log();
  c = c - (-(*this - one)).c_log();
  return c * 0.5;
}

//
// GONIO HYPERBOLICUS II
//
Complex Complex::c_csch() {
  return one / c_sinh();
}

Complex Complex::c_sech() {
  return one / c_cosh();
}

Complex Complex::c_coth() {
  return one / c_tanh();
}

Complex Complex::c_acsch() {
  return (one / *this).c_asinh();
}

Complex Complex::c_asech() {
  return (one / *this).c_acosh();
}

Complex Complex::c_acoth() {
  return (one / *this).c_atanh();
}

// -------------------------end of complex

//global variables:
const int N = 1000;
const int K = 30;
double xp = 0;
double xq = 0;
double XV = 0;
double XI = 0;
double hV = 0;
double hI = 0;
Complex xv[K + 1];
Complex xi[K + 1];
double abs_xv[K + 1];
double abs_xi[K + 1];
double tv[K + 1];
double ti[K + 1];
double p[K + 1];
double q[K + 1];
double Vrms_fft;
double Irms_fft;
double S;
double P;
double Q;
double D;
double PF_fft;
double THDv;
double THDi;
double v[1000];
double i[1000];

float Current_Scale_FFT;  //used for assigning the value of current_scale_fft and writing it to flash
float Voltage_Scale_FFT;  //used for assigning the value of voltage_scale_fft and writing it to flash
//PARAMETER     SETTING
int FAZ3_1Pin = 0;
int FAZ = 1;  //ATTENTION!!!!!!!!!!!! If FAZ is set to 3 changes must happen n setup function for writing the scaling values to the EEPROM
int SAMPLE = 2;
int inPinCurrent_3FAZ[3] = { 32, 35, 34 };  //GPIO
int inPinVoltage_3FAZ[3] = { 39, 36, 33 };  //GPIO            // برد های قدیمی
double current_scale_fft[3] = { 102.0 };    //111.1;
double voltage_scale_fft[3] = { 232.5 };
int dc_voltage[3] = { 1892, 1892, 1892 };
int dc_current[3] = { 1635, 1635, 1635 };
size_t dlen = 409;       //  301;  //409;
unsigned char dst[409];  //dst[dlen]
size_t slen = 304;       //304;//strlen(src);

int inPinCurrent_1FAZ = 35;
int inPinVoltage_1FAZ = 34;

#define ANALOG_INPUT A0
double power[3];
double voltage[3];
double current[3];
double energy[3];
double wifioff;

double PF[3];
double lastenergy;
bool lock = false;
double lastwifioff = 0;
double THD_voltage[3];
double THD_current[3];

const char* addrScale_I = "AdScale_I";
const char* addrScale_V = "AdScale_V";
const char* addrkey = "Adkey";
const char* addwdtrst = "AdWDTRST";
const char* addrenergy = "AdEnergy";

void setup_eeprom() {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("failed to initialise EEPROM");
    delay(1000000);
  }
  Serial.println("EEPROM legth");
  Serial.println(EEPROM.length());
}

void save_time(int time) {
  EEPROM.put(time_addr, time);
  EEPROM.commit();
  Serial.print("time saved => ");
  Serial.println(time);
}

void save_old_time(int time) {
  EEPROM.put(old_time_addr, time);
  EEPROM.commit();
  Serial.print("old time saved => ");
  Serial.println(time);
}

void save_old_addr(int addr) {
  EEPROM.put(old_addr_addr, addr);
  Serial.print("old address saved => ");
  Serial.println(addr);
  EEPROM.commit();
}

void save_valid_old(int valid_old) {
  EEPROM.put(valid_old_addr, valid_old);
  EEPROM.commit();
}

void save_energy() {
  EEPROM.put(addr, lastenergy);
  addr += address_shift;
  save_old_addr(addr);  
  Serial.print("energy saved => ");
  Serial.println(lastenergy);
  lastenergy = 0;
  save_valid_old(1512844);
}

int load_valid_old() {
  int valid_old;
  EEPROM.get(valid_old_addr, valid_old);
  return valid_old;
}

int load_time() {
  int temp_time;
  EEPROM.get(time_addr, temp_time);
  return temp_time;
}

int load_old_time() {
  int temp_time;
  EEPROM.get(old_time_addr, temp_time);
  return temp_time;
}

int load_old_addr() {
  int temp_addr;
  EEPROM.get(old_addr_addr, temp_addr);
  return temp_addr;
}

void send_time_http(WiFiClient client) {
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // the content of the HTTP response follows the header:
  client.print("current time saved");
  client.print("<br>");

  // The HTTP response ends with another blank line:
  client.println();
}


void send_home_http(WiFiClient client) {
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // the content of the HTTP response follows the header:
  client.print("you are connected to smart meter sensor.<br>");
  client.print("sensor mac = ");
  client.print(board_mac);
  client.print("<br>");

  // The HTTP response ends with another blank line:
  client.println();
}

void send_old_data_http(WiFiClient client) {
  Serial.println("send_old data_http");
  if (old_data) {
    int time = load_old_time();
    String buffer = "{\"time\": ";
    buffer += time;
    buffer += ", \"data\": [ ";
    int array_size = (old_address - start_addr) / address_shift;
    Serial.print("array size => ");
    Serial.println(array_size);
    if (array_size > 0) {
      double temp_energy;
      for (int i = start_addr; i < old_address; i += address_shift) {
        EEPROM.get(i, temp_energy);
        buffer += temp_energy;
        if (i + address_shift < old_address) {
          buffer += ", ";
        }
      }
    }
    buffer += " ] }";
    Serial.println(buffer);
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:application/json");
    client.println();
    // the content of the HTTP response follows the header:
    client.print(buffer);
    // The HTTP response ends with another blank line:
    client.println();
  } else {
    String buffer = "{ }";
    Serial.println(buffer);
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:application/json");
    client.println();
    // the content of the HTTP response follows the header:
    client.print(buffer);
    // The HTTP response ends with another blank line:
    client.println();
  }



}

void send_data_http(WiFiClient client) {
  Serial.println("send_data_http");
  if (old_data) {
    int time = 0;
    String buffer = "{\"time\": ";
    buffer += time;
    buffer += ", \"data\": [ ";
    int temp_addr = load_old_addr();
    int array_size = (addr - temp_addr) / address_shift;
    Serial.print("array size => ");
    Serial.println(array_size);
    if (array_size > 0) {
      double temp_energy;
      for (int i = temp_addr; i < addr; i += address_shift) {
        EEPROM.get(i, temp_energy);
        buffer += temp_energy;
        if (i + address_shift < addr) {
          buffer += ", ";
        }
      }
    }
    buffer += " ] }";
    Serial.println(buffer);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:application/json");
    client.println();
    // the content of the HTTP response follows the header:
    client.print(buffer);
    // The HTTP response ends with another blank line:
    client.println();
    addr = start_addr;
    old_data = false;
    save_valid_old(0);
  } else {
    int time = load_time();
    String buffer = "{\"time\": ";
    buffer += time;
    buffer += ", \"data\": [ ";

    int array_size = (addr - start_addr) / address_shift;
    Serial.print("array size => ");
    Serial.println(array_size);
    if (array_size > 0) {
      double temp_energy;
      for (int i = start_addr; i < addr; i += address_shift) {
        EEPROM.get(i, temp_energy);
        buffer += temp_energy;
        if (i + address_shift < addr) {
          buffer += ", ";
        }
      }
    }

    buffer += " ] }";
    Serial.println(buffer);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:application/json");
    client.println();
    // the content of the HTTP response follows the header:
    client.print(buffer);
    // The HTTP response ends with another blank line:
    client.println();
    addr = start_addr;
  }

}

void send_mac_http(WiFiClient client) {
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.println();

  String mac_addr = getFullMacAddress();
  // Serial.println(mac_addr);

  String buffer = "{\"mac address\": \" " + mac_addr + "\"}";

  // the content of the HTTP response follows the header:
  client.print(buffer);

  // The HTTP response ends with another blank line:
  client.println();
}

String getFullMacAddress() {
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  String buffer = "";
  for (int i = 0; i < 6; i++) {
    buffer += String(baseMac[i], HEX);
    if (i < 5) {
      buffer += ":";
    }
  }
  return buffer;
}

String getMacAddress() {
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char baseMacChr[6] = { 0 };
  sprintf(baseMacChr, "%02X_%02X_%02X", baseMac[3], baseMac[4], baseMac[5]);
  return String(baseMacChr);
}

void start_wifi() {
  String temp = "iot_s_";
  char ssid[10];
  char mac[10];
  char final_ssid[50];
  board_mac = getMacAddress();
  Serial.println(board_mac);
  board_mac.toCharArray(mac, 10);
  temp.toCharArray(ssid, 10);

  strcat(final_ssid, ssid);
  strcat(final_ssid, mac);
  Serial.println(final_ssid);
  WiFi.softAP(final_ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println(myIP);
  server.begin();
}

void ARDUINO_ISR_ATTR onTimer() {
  if (is_on == 0) {
    digitalWrite(PIN_LED, HIGH);
    is_on = 1;
  } else {
    digitalWrite(PIN_LED, LOW);
    is_on = 0;
  }
}

void setupTimer() {
  //  timer = timerBegin(0, 80, true);
  //  // Attach onTimer function to our timer.
  //  timerAttachInterrupt(timer, &onTimer, true);
  //
  //  // Set alarm to call onTimer function every second (value in microseconds).
  //  // Repeat the alarm (third parameter)
  //  timerAlarmWrite(timer, 1000000, true);
  //
  //  // Start an alarm
  //  timerAlarmEnable(timer);
}

void calculate_IRMS(int Time, int phase, int inPinCurrent) {

  Complex j(0.0, 1.0);
  uint32_t startTime = millis();
  int No = 0;
  int volt, current_fft, voltage_fft;
  while ((millis() - startTime) < Time) {

    current_fft = analogRead(inPinCurrent);
    current_fft = current_fft - dc_current[phase];
    i[No] = current_scale_fft[phase] * ((current_fft * (3.3)) / 4095);
    No++;
  }

  No--;
  for (int h = 1; h <= K; h++) {
    for (int k = 0; k < No; k++) {
      //    for (int k = 1; k < 850; k++) {
      xi[h] += Complex(i[k], 0.0) * (Complex(cos(2 * PI * h * k / No), 0.0) + j * Complex(sin(2 * PI * h * k / No), 0.0));
    }
  }


  for (int i = 1; i <= K; i++) {
    ti[i] = atan(xi[i].imag() / xi[i].real());
  }

  for (int i = 1; i <= K; i++) {
    abs_xi[i] = (2 * sqrt(xi[i].real() * xi[i].real() + xi[i].imag() * xi[i].imag())) / No;
  }

  for (int h = 1; h <= K; h++) {
    XI = (abs_xi[h] / sqrt(2)) * (abs_xi[h] / sqrt(2)) + XI;
  }
  for (int h = 2; h <= K; h++) {
    hI = abs_xi[h] * abs_xi[h] + hI;
  }

  Irms_fft = sqrt(XI);
  THDv = 0.06;
  THDi = sqrt(hI) / (abs_xi[1]);
  THD_voltage[phase] = THDv;
  THD_current[phase] = THDi;
  voltage[phase] = Vrms_fft;
  current[phase] = Irms_fft;
  PF[phase] = 1;
  power[phase] = Irms_fft * Vrms_fft;
  if (power[phase] < 0) power[phase] = -power[phase];
}

void calculate(int Time, int phase, int inPinVoltage, int inPinCurrent) {

  //Serial.println("***calculate****");
  Complex j(0.0, 1.0);
  uint32_t startTime = millis();
  int No = 0;
  int volt, current_fft, voltage_fft;
  while ((millis() - startTime) < Time) {
    volt = analogRead(inPinVoltage);
    voltage_fft = volt - dc_voltage[phase];
    v[No] = voltage_scale_fft[phase] * ((voltage_fft * (3.3)) / 4095);
    current_fft = analogRead(inPinCurrent);
    current_fft = current_fft - dc_current[phase];
    i[No] = current_scale_fft[phase] * ((current_fft * (3.3)) / 4095);
    No++;
  }

  No--;
  for (int h = 1; h <= K; h++) {
    for (int k = 0; k < No; k++) {
      //    for (int k = 1; k < 850; k++) {
      xv[h] += Complex(v[k], 0.0) * (Complex(cos(2 * PI * h * k / No), 0.0) + j * Complex(sin(2 * PI * h * k / No), 0.0));
      xi[h] += Complex(i[k], 0.0) * (Complex(cos(2 * PI * h * k / No), 0.0) + j * Complex(sin(2 * PI * h * k / No), 0.0));
    }
  }


  for (int i = 1; i <= K; i++) {
    tv[i] = atan(xv[i].imag() / xv[i].real());
    ti[i] = atan(xi[i].imag() / xi[i].real());
  }

  for (int i = 1; i <= K; i++) {
    abs_xv[i] = (2 * sqrt(xv[i].real() * xv[i].real() + xv[i].imag() * xv[i].imag())) / No;
    abs_xi[i] = (2 * sqrt(xi[i].real() * xi[i].real() + xi[i].imag() * xi[i].imag())) / No;
  }

  for (int h = 1; h <= K; h++) {
    p[h] = 0.5 * abs_xv[h] * abs_xi[h] * cos(tv[h] - ti[h]);
    xp += p[h];
    q[h] = 0.5 * abs_xv[h] * abs_xi[h] * sin(tv[h] - ti[h]);
    xq += q[h];
    XV = (abs_xv[h] / sqrt(2)) * (abs_xv[h] / sqrt(2)) + XV;
    XI = (abs_xi[h] / sqrt(2)) * (abs_xi[h] / sqrt(2)) + XI;
  }
  for (int h = 2; h <= K; h++) {
    hV = abs_xv[h] * abs_xv[h] + hV;
    hI = abs_xi[h] * abs_xi[h] + hI;
  }

  Vrms_fft = sqrt(XV);
  Irms_fft = sqrt(XI);
  S = Vrms_fft * Irms_fft;

  P = xp;

  Q = xq;
  D = sqrt(S * S - P * P - Q * Q);
  PF_fft = P / S;
  THDv = sqrt(hV) / (abs_xv[1]);
  THDi = sqrt(hI) / (abs_xi[1]);

  /////////////////////////////////CONFIGUING WITH FLUK/////////////////////////////////
  THDv = THDv / 2;
  THDi = THDi * 3 / 4;
  Q = xq / 10;
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  THD_voltage[phase] = THDv;
  THD_current[phase] = THDi;
  voltage[phase] = Vrms_fft;
  current[phase] = Irms_fft;
  PF[phase] = PF_fft;  // Must be corrected
  if (PF[phase] < 0) PF[phase] = -PF[phase];
  if (P < 0) P = -P;
  power[phase] = P;
  power[phase] = voltage[phase] * current[phase];
}

void resetParameters(void) {
  xp = 0;
  xq = 0;
  XV = 0;
  XI = 0;
  hV = 0;
  hI = 0;
  for (int i = 1; i <= K; i++) {
    xv[i] = Complex(0.0, 0.0);
    xi[i] = Complex(0.0, 0.0);
    abs_xv[i] = 0;
    abs_xi[i] = 0;
    tv[i] = 0;
    ti[i] = 0;
    p[i] = 0;
    q[i] = 0;
  }
  Irms_fft = 0;
  S = 0;
  P = 0;
  Q = 0;
  D = 0;
  PF_fft = 0;
  THDv = 0;
  THDi = 0;
}

void CalculateVI() {

  int phase;
  // Serial.println("***CalculateVI****");

  if (FAZ == 1) {
    resetParameters();
    calculate(20, 0, inPinVoltage_1FAZ, inPinCurrent_1FAZ);
  } else {
    if (FAZ3_1Pin == 1) {
      resetParameters();
      calculate(20, 0, inPinVoltage_3FAZ[0], inPinCurrent_3FAZ[0]);
      resetParameters();
      calculate_IRMS(20, 1, inPinCurrent_3FAZ[1]);
      resetParameters();
      calculate_IRMS(20, 2, inPinCurrent_3FAZ[2]);
    } else {
      resetParameters();
      calculate(20, 0, inPinVoltage_3FAZ[0], inPinCurrent_3FAZ[0]);
      resetParameters();
      calculate(20, 1, inPinVoltage_3FAZ[1], inPinCurrent_3FAZ[1]);
      resetParameters();
      calculate(20, 2, inPinVoltage_3FAZ[2], inPinCurrent_3FAZ[2]);
    }
  }
}

void setupCT() {
  Serial.println("start ct");

  int i;
  int phase;
  NVS.begin("SM1001", false);

  if (FAZ == 3) {
    // for (i = 0; i < FAZ; i++) {
    //   pinMode(inPinCurrent_3FAZ[i], INPUT);
    //   pinMode(inPinVoltage_3FAZ[i], INPUT);
    // }
  } else {
    pinMode(inPinCurrent_1FAZ, INPUT);
    pinMode(inPinVoltage_1FAZ, INPUT);
    Current_Scale_FFT = current_scale_fft[0];
    Voltage_Scale_FFT = voltage_scale_fft[0];
    NVS.putFloat(addrScale_I, Current_Scale_FFT);
    NVS.putFloat(addrScale_V, Voltage_Scale_FFT);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  setup_eeprom();
  int temp_time = load_time();
  int valid_old = load_valid_old();
  Serial.println(valid_old);
  if (temp_time > 1656755767 & temp_time < 1751490167 & valid_old == 1512844) {
    save_old_time(temp_time);
    addr = load_old_addr();
    old_address = addr;
    Serial.print("old address => ");
    Serial.println(old_address );
    int array_size = (old_address - start_addr) / address_shift;
    Serial.print("old data size => ");
    Serial.println(array_size);
    if (array_size > 0) {
      double temp_energy;
      for (int i = start_addr; i < old_address; i += address_shift) {
        EEPROM.get(i, temp_energy);
        Serial.println(temp_energy);
      }
    }
    addr += address_shift;
    old_data = true;
    Serial.println("we have old data");
  } else {
    old_data = false;
    Serial.println("no old data");
  }
  save_time(0);
  start_wifi();
  setupTimer();
  setupCT();
  last_time = millis();
  last_time_write = millis();
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
}

void check_http_request() {
  WiFiClient client = server.available();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    int response_type = 0;
    int time = 0;
    while (client.connected()) {  // loop while the client's connected
      if (client.available()) {   // if there's bytes to read from the client,
        char c = client.read();   // read a byte, then
        Serial.write(c);          // print it out the serial monitor
        if (c == '\n') {          // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            if (response_type == 0) {
              send_home_http(client);
            } else if (response_type == 1) {
              send_data_http(client);
              save_time(time);
            } else if (response_type == 2) {
              send_mac_http(client);
            } else if (response_type == 3) {
              send_time_http(client);
              save_time(time);
            } else if (response_type == 4) {
              send_old_data_http(client);
            }

            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            // Check to see if the client request was get data:
            if (currentLine.startsWith("GET /sync_old")) {
              response_type = 4;
            } else if (currentLine.startsWith("GET /sync")) {
              response_type = 1;
              String s = currentLine.substring(15, currentLine.length());
              int space_index = s.indexOf(' ');
              s = s.substring(0, space_index);
              time = s.toInt();
            } else if (currentLine.startsWith("GET /mac HTTP/1.1")) {
              response_type = 2;
            } else if (currentLine.startsWith("POST /time")) {
              response_type = 3;
              String s = currentLine.substring(16, currentLine.length());
              int space_index = s.indexOf(' ');
              s = s.substring(0, space_index);
              time = s.toInt();
            }
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void CalculateEnergy(uint32_t end_time, uint32_t start_time) {
  int phase = 0;
  double sum = 0;
  for (phase = 0; phase < FAZ; phase++) {
    energy[phase] = (((end_time - start_time) * power[phase] * 0.001) / 3600);  //Wh{
    sum = sum + energy[phase];
  }
  if (sum < 0) sum = 0;
  lastenergy = lastenergy + sum;
  if (lastenergy < 0) lastenergy = 0;
  // Serial.println(lastenergy / 1000);
}

void calculateANDwritenergy(uint32_t end_time, uint32_t start_time) {
  CalculateVI();
  CalculateEnergy(end_time, start_time);
}

void loop() {
  check_http_request();
  int now = millis();
  if (now > last_time + period_calculate) {
    calculateANDwritenergy(now, last_time);
    last_time = now;
  }
  if (now > last_time_write + periodWrite) {
    // Serial.println(lastenergy / 1000);
    save_energy();
    // todo save lastenergy / 1000
    last_time_write = now;
  }
  // Serial.println(now);
  // delay(3000);
}
