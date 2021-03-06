#include "DHT.h"
#include <Time.h>
#include <TimeAlarms.h>
#include <LiquidCrystal.h>
#include "etherShield.h"
#include <stdlib.h>
#include <EEPROM.h>
#include "Schedule.h"
#include "stdio.h"

#define BTN    2

#define DHTTYPE DHT11
#define DHTPIN 9

#define LCD_RS 1
#define LCD_EN 8
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7
#define LCD_BL 0

#define WATER_DURATION 36000 //ten mins

static uint8_t mymac[6] = { 0x54, 0x55, 0x58, 0x10, 0x00, 0x24 };
static uint8_t myip[4]  = { 192, 168, 1, 15 };

#define BUFFER_SIZE 300
static uint8_t buf[BUFFER_SIZE + 1];

#define RESPONSE_BUF 350
static char responseBuf[RESPONSE_BUF];

EtherShield es = EtherShield();
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Schedule schedule(0);

long last_lcd_backlight = 0
   , last_sensor_update = 0;

void lcd_backlight() {
  last_lcd_backlight = millis();
  digitalWrite(LCD_BL, HIGH);
}

void setup() {
  Serial.begin(9600);
  schedule.fetch();
  schedule.time(9, 30, 55, 18, 7, 2012);
  pinMode(A4, OUTPUT);
  digitalWrite(A4, LOW);
  es.ES_enc28j60Init(mymac);
  // change clkout from 6.25MHz to 12.5MHz
  es.ES_enc28j60clkout(2);
  delay(10);

  /* Magjack leds configuration, see enc28j60 datasheet, page 11 */
  // LEDA=greed LEDB=yellow
  //
  // 0x880 is PHLCON LEDB=on, LEDA=on
  // enc28j60PhyWrite(PHLCON,0b0000 1000 1000 00 00);
  es.ES_enc28j60PhyWrite(PHLCON, 0x880);
  delay(500);
  //
  // 0x990 is PHLCON LEDB=off, LEDA=off
  // enc28j60PhyWrite(PHLCON,0b0000 1001 1001 00 00);
  es.ES_enc28j60PhyWrite(PHLCON, 0x990);
  delay(500);
  //
  // 0x880 is PHLCON LEDB=on, LEDA=on
  // enc28j60PhyWrite(PHLCON,0b0000 1000 1000 00 00);
  es.ES_enc28j60PhyWrite(PHLCON, 0x880);
  delay(500);
  //
  // 0x990 is PHLCON LEDB=off, LEDA=off
  // enc28j60PhyWrite(PHLCON,0b0000 1001 1001 00 00);
  es.ES_enc28j60PhyWrite(PHLCON, 0x990);
  delay(500);
  //
  // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
  // enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
  es.ES_enc28j60PhyWrite(PHLCON, 0x476);
  delay(100);

  //init the ethernet/ip layer:
  es.ES_init_ip_arp_udp_tcp(mymac, myip, 81);


  pinMode(BTN, INPUT);
  pinMode(LCD_BL, OUTPUT);
  attachInterrupt(0, lcd_backlight, RISING);
  dht.begin();
  lcd.begin(16, 2);
  lcd.print("Temperature    C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity       %");
  last_lcd_backlight = 0;
  last_sensor_update = 0;
}

float h = -1;
float t = -1;

void digits(int digits, char *buf)
{
  if(digits < 10)
    sprintf(buf, "0%d", digits);
  else 
    sprintf(buf, "%d", digits);
}

int parseTime(char *time_buf, int *hour, int *min, int *sec) {
  if(sscanf(time_buf, "%d:%d:%d", hour, min, sec) == EOF) return -1;
  if(!(*hour <= 23 && *hour >= 0) || !(*min <= 60 && *min >= 0) || !(*sec <= 60 && *sec >= 0)) return -1; 
  return 0;
}

int parseDate(char *date_buf, int *day, int *month, int *year) {
  if(sscanf(date_buf, "%d/%d/%d", day, month, year) == EOF) return -1;
  if(!(*day <= 31 && *day >= 1) || !(*month <= 12 && *month >= 1) || !(*year >= 2012)) return -1;
  return 0;
}

uint16_t writeStrToBuf(char* from, uint8_t* to, uint16_t plen) {
  int i = 0;
  while (from[i]) {
      to[TCP_CHECKSUM_L_P + 3 + plen] = from[i++];
      plen++;
  }
  return plen;
}

char* respWrongFormat = "Error: Wrong format. Refer to \"help\"\n";

void loop() {
  Alarm.delay(0); 
  if ((millis() - last_lcd_backlight) > 5000) {
    digitalWrite(LCD_BL, LOW);
  }
  if ((millis() - last_sensor_update) > 2000) {
    h = dht.readHumidity();
    t = dht.readTemperature();
    last_sensor_update = millis();
  }
  if (isnan(t) || isnan(h)) {
    // error
  } else {
    lcd.setCursor(13, 0);
    lcd.print((int)t);
    lcd.setCursor(13, 1);
    lcd.print((int)h);
  }
      
  uint16_t plen, dat_p;
  plen = es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf);

  /*plen will ne unequal to zero if there is a valid packet (without crc error) */
  if (plen == 0) {
      return;
  }

  // arp is broadcast if unknown but a host may also verify the mac address by sending it to a unicast address.
  if (es.ES_eth_type_is_arp_and_my_ip(buf,plen)) {
      es.ES_make_arp_answer_from_request(buf);
      return;
  }

  // check if ip packets are for us:
  if(es.ES_eth_type_is_ip_and_my_ip(buf,plen) == 0) {
      return;
  }

  if(buf[IP_PROTO_P] == IP_PROTO_ICMP_V && buf[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V) {
      es.ES_make_echo_reply_from_request(buf,plen);
      return;
  }

  // tcp port start, compare only the lower byte
  char isMyPort = buf[IP_PROTO_P] == IP_PROTO_TCP_V &&
                  buf[TCP_DST_PORT_H_P] == 0 &&
                  buf[TCP_DST_PORT_L_P] == 81;
  if (isMyPort) {
      char sendFin = 0;
      if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V) {
          // make_tcp_synack_from_syn does already send the syn,ack
          es.ES_make_tcp_synack_from_syn(buf);
          return;
      }
      if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V){
          es.ES_init_len_info(buf); // init some data structures
          dat_p=es.ES_get_tcp_data_pointer();
          if (dat_p==0){ // we can possibly have no data, just ack:
              if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V){
                  es.ES_make_tcp_ack_from_any(buf);
              }
              return;
          }
          if (strncmp("help", (char *)&(buf[dat_p]), 4) == 0) {
              plen = es.ES_fill_tcp_data_p(buf, 0, PSTR("\
               info                : get temperature and time information\n \
              start watering      : open water valve\n \
              stop watering       : close water valve\n \
              set time hh:mm:ss   : set system time\n \
              set time dd:mm:yyyy : set system date\n \
              exit                : exit telnet connection\n"));
          }
          else if (strncmp("info", (char *)&(buf[dat_p]), 4) == 0) {
              sprintf(responseBuf,
                  "Temp: %d\nHumi: %d\n%d:%d:%d %d/%d/%d\n",
                  (int)t, (int)h,
                  schedule.hour(), schedule.minute(), schedule.second(),
                  schedule.day(), schedule.month(), schedule.year());
              plen = writeStrToBuf(responseBuf, buf, 0);
          }
          else if (strncmp("start watering", (char *)&(buf[dat_p]), 14) == 0) {
            digitalWrite(A4, HIGH);
            plen = es.ES_fill_tcp_data_p(buf, 0, PSTR("better hold on to your umbrella!\n"));
          }
          else if (strncmp("stop watering", (char *)&(buf[dat_p]), 13) == 0) {
            digitalWrite(A4, LOW);
            plen = es.ES_fill_tcp_data_p(buf, 0, PSTR("look at the rainbow!\n"));
          }
          else if (strncmp("set time", (char *)&(buf[dat_p]), 8) == 0) {
            char *time_buf = (char *)&(buf[dat_p+9]);
            int hour, min, sec;
            if (!parseTime(time_buf, &hour, &min, &sec)) {
              schedule.time(hour, min, sec);
              sprintf(responseBuf, "Time is set to %d:%d:%d\n", hour, min, sec);
            }
            else {
              sprintf(responseBuf, "Time could not be set\n");
            }
            plen = writeStrToBuf(responseBuf, buf, 0);
          }
          else if (strncmp("set date", (char *)&(buf[dat_p]), 8) == 0) {
            char *date_buf = (char *)&(buf[dat_p+9]);
            int day, month, year;
            if (parseDate(date_buf, &day, &month, &year) != -1) {
              schedule.date(day, month, year);
              sprintf(responseBuf, "Date is set to %d/%d/%d\n", day, month, year);
            }
            else {
              sprintf(responseBuf, "Date could not be set\n");
            }
            plen = writeStrToBuf(responseBuf, buf, 0);
          }
          else if (strncmp("schedules", (char *)&(buf[dat_p]), 9) == 0) {
            schedule.toString(responseBuf);
            plen = writeStrToBuf(responseBuf, buf, 0);
          }
          else if (strncmp("set schedule", (char*)&(buf[dat_p]), 12) == 0) {
            char* params = (char *)&(buf[dat_p + 13]);
            char day[20];
            unsigned int hours, mins, duration;
            char ret = sscanf(params, "%19s %d:%d %d", day, &hours, &mins, &duration);
            if (ret != 4) {
              plen = writeStrToBuf(respWrongFormat, buf, 0);
            }
            else {
              char dayIndex = Schedule::dayIndex(day);
              if (dayIndex == -1) {
                sprintf(responseBuf, "Day %s does not exist\n", day);
                plen = writeStrToBuf(responseBuf, buf, 0);
              }
              else {
                WaterRule& dayRule = schedule.get(dayIndex);
                dayRule.set(hours, mins, duration);
                schedule.storeDay(dayIndex);                
                sprintf(responseBuf, "Enabled rule: %s at %d:%d for %d minutes\n", day, hours, mins, duration);
                plen = writeStrToBuf(responseBuf, buf, 0);
              }
            }
          }
          else if (strncmp("disable", (char *)&(buf[dat_p]), 7) == 0) {
            char *params = (char *)&(buf[dat_p + 7]);
            char day[20];
            char ret = sscanf(params, "%19s", day);
            if (ret != 1) {
              plen = writeStrToBuf(respWrongFormat, buf, 0);
            }
            else {
              char dayIndex = Schedule::dayIndex(day);
              if (dayIndex == -1) {
                sprintf(responseBuf, "Day %s does not exist\n\n", day);
                plen = writeStrToBuf(responseBuf, buf, 0);
              }
              else {
                WaterRule& dayRule = schedule.get(dayIndex);
                dayRule.setEnabled(false);
                schedule.storeDay(dayIndex);
                sprintf(responseBuf, "Disabled day %s\n", day);
                plen = writeStrToBuf(responseBuf, buf, 0);
              }
            }
          }
          else if (strncmp("exit", (char *)&(buf[dat_p]), 4) == 0) {
            plen = es.ES_fill_tcp_data_p(buf, 0, PSTR("goodbye  ;)\n"));
            sendFin = 1;
          }
          else {
            plen = es.ES_fill_tcp_data_p(buf, 0, PSTR("Unknown command!\n"));
          }
          es.ES_make_tcp_ack_from_any(buf);
          es.ES_make_tcp_ack_with_data(buf,plen, sendFin);
     }
  }
  
}
