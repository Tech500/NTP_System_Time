//schufti  --of ESP8266.com provided help with NTP.

#include <WiFi.h>
#include <WiFiUdp.h>
#include <sys/time.h>                                                                                    
#include <time.h>


#define NTP0 "us.pool.ntp.org"
#define NTP1 "time.nist.gov"

/*

  Found this reference on setting TZ: http://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html

  Here are some example TZ values, including the appropriate Daylight Saving Time and its dates of applicability. 
  In North American Eastern Standard Time (EST) and Eastern Daylight Time (EDT), the normal offset from UTC is 5 hours; 
  since this is west of the prime meridian, the sign is positive. Summer time begins on Marchâ€™s second Sunday at 
  2:00am, and ends on Novemberâ€™s first Sunday at 2:00am.
  
*/

#define TZ "EST+5EDT,M3.2.0/2,M11.1.0/2"

// Replace with your network details
const char* ssid = "yourSSID";
const char* password = "yourPASSWORD";

//setting the addresses
IPAddress staticIP(10, 0, 0, 200);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

WiFiClient client;

///Are we currently connected?
boolean connected = false;

WiFiUDP udp;
// local port to listen for UDP packets
//Settings pertain to NTP
const int udpPort = 1337;
//NTP Time Servers
const char * udpAddress1 = "us.pool.ntp.org";
const char * udpAddress2 = "time.nist.gov";
char incomingPacket[255];
char replyPacket[] = "Hi there! Got the message :-)";

int DOW, MONTH, DATE, YEAR, HOUR, MINUTE, SECOND;
//String = (Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday};
String weekDay;

int lc = 0;
time_t tnow;

char strftime_buf[64];

String dtStamp(strftime_buf);

void setup(void)
{
  Serial.begin(115200);

  while (!Serial);

  WiFi.persistent( false ); // for time saving

  // Connecting to local WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS);
  WiFi.begin(ssid, password);
  delay(10);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nWiFi connected");
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Server IP:  ");
  Serial.println(WiFi.localIP());

  //set up TimeZone in local environment
  configTime(0, 0, udpAddress1, udpAddress2);
  setenv("TZ", "EST+5EDT,M3.2.0/2,M11.1.0/2", 3);   // this sets TZ to Indianapolis, Indiana
  tzset();

  Serial.print("wait for first valid timestamp ");

  while (time(nullptr) < 100000ul)
  {
    Serial.print(".");
    delay(5000);
  }
  
  Serial.println(" time synced");
  Serial.println("");

}

void loop()
{

  //udp only send data when connected
  if (connected)
  {
    //Send a packet
    udp.beginPacket(udpAddress1, udpPort);
    udp.printf("Seconds since boot: %u", millis() / 1000);
    udp.endPacket();
  }

  getDateTime();

  Serial.println(dtStamp);

  delay(2000);
}

String getDateTime()
{
  struct tm *ti;

  tnow = time(nullptr) + 1;
  //strftime(strftime_buf, sizeof(strftime_buf), "%c", localtime(&tnow));
  ti = localtime(&tnow);
  DOW = ti->tm_wday;
  YEAR = ti->tm_year + 1900;
  MONTH = ti->tm_mon + 1;
  DATE = ti->tm_mday;
  HOUR  = ti->tm_hour;
  MINUTE  = ti->tm_min;
  SECOND = ti->tm_sec;

  strftime(strftime_buf, sizeof(strftime_buf), "%a , %m/%d/%Y , %H:%M:%S %Z", localtime(&tnow));
  dtStamp = strftime_buf;
  return (dtStamp);

}
