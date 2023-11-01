#include "GpsInterface.h"

#ifdef HAS_GPS

  #ifdef MARAUDER_FEBERIS_BOARD

    HardwareSerial Serial1(GPS_SERIAL_INDEX);
    TinyGPSPlus gps;


    void GpsInterface::begin() {

      Serial1.begin(9600, SERIAL_8N1, GPS_TX, GPS_RX);

      delay(4000);

      if (Serial1.available()) {
        Serial.println("GPS Attached Successfully");
        this->gps_enabled = true;
        while (Serial1.available())
          Serial1.read();
      } else {
        Serial.println("GPS Connection Unavailable");
      }
    }

    // Thanks JosephHewitt
    String GpsInterface::dt_string_from_gps(){
      //Return a datetime String using GPS data only.
      String datetime = "";
      if (gps.date.isValid() && gps.date.year() > 0){
        datetime += gps.date.year();
        datetime += "/";
        datetime += gps.date.month();
        datetime += "/";
        datetime += gps.date.day();
        datetime += " ";
        datetime += gps.time.hour();
        datetime += ":";
        datetime += gps.time.minute();
        datetime += ":";
        datetime += gps.time.second();
      }
      return datetime;
    }
    
    void GpsInterface::setGPSInfo() {
      this->good_fix = gps.satellites.isValid();
      this->num_sats = gps.satellites.value();

      this->datetime = this->dt_string_from_gps();

      this->lat = String((float)gps.location.lat(), 6);
      this->lon = String((float)gps.location.lng(), 6);
      long alt = 0;
      if (!gps.altitude.isValid()){
        alt = 0;
      } else {
        alt = gps.altitude.value();
      }
      this->altf = (float)alt / 1000;

      this->accuracy = 2.5 * ((float)gps.hdop.value()/10);

    }

    float GpsInterface::getAccuracy() {
      return this->accuracy;
    }

    String GpsInterface::getLat() {
      return this->lat;
    }

    String GpsInterface::getLon() {
      return this->lon;
    }

    float GpsInterface::getAlt() {
      return this->altf;
    }

    String GpsInterface::getDatetime() {
      return this->datetime;
    }

    String GpsInterface::getNumSatsString() {
      return (String)num_sats;
    }

    bool GpsInterface::getFixStatus() {
      return this->good_fix;
    }

    String GpsInterface::getFixStatusAsString() {
      if (this->getFixStatus())
        return "Yes";
      else
        return "No";
    }

    bool GpsInterface::getGpsModuleStatus() {
      return this->gps_enabled;
    }

    void GpsInterface::main() {
      while (Serial1.available()) {
        //Fetch the character one by one
        //char c = Serial1.read();
        //Serial.print(c);
        //Pass the character to the library
        //nmea.process(c);
        gps.encode(Serial1.read());
      }

      uint8_t num_sat = gps.satellites.value();

      if (gps.satellites.isValid()) {
        this->setGPSInfo();
      }

      if ((gps.satellites.isValid()) && (num_sat > 0)) {
        this->setGPSInfo();
      } else if ((!gps.satellites.isValid()) && (num_sat <= 0)) {
        this->setGPSInfo();
      }

      // if (millis() > 5000 && gps.charsProcessed() < 10) // uh oh
      // {
      //   Serial.println("ERROR: not getting any GPS data!");
      //   // dump the stream to Serial
      //   Serial.println("GPS stream dump:");
      //   while (true) // infinite loop
      //     if (Serial1.available()) // any data coming in?
      //       Serial.write(Serial1.read());

      // }

    }

  #else

    char nmeaBuffer[100];

    MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

    #ifndef GPS_SOFTWARE_SERIAL
      HardwareSerial Serial2(GPS_SERIAL_INDEX);
    #else
      EspSoftwareSerial::UART Serial2;
    #endif

    void GpsInterface::begin() {

      #ifndef GPS_SOFTWARE_SERIAL
        Serial2.begin(9600, SERIAL_8N1, GPS_TX, GPS_RX);
      #else
        Serial2.begin(9600, SWSERIAL_8N1, GPS_TX, GPS_RX);
      #endif

      MicroNMEA::sendSentence(Serial2, "$PSTMSETPAR,1201,0x00000042");
      MicroNMEA::sendSentence(Serial2, "$PSTMSAVEPAR");

      MicroNMEA::sendSentence(Serial2, "$PSTMSRR");

      delay(4000);

      if (Serial2.available()) {
        Serial.println("GPS Attached Successfully");
        this->gps_enabled = true;
        while (Serial2.available())
          Serial2.read();
      }
    }

    // Thanks JosephHewitt
    String GpsInterface::dt_string_from_gps(){
      //Return a datetime String using GPS data only.
      String datetime = "";
      if (nmea.isValid() && nmea.getYear() > 0){
        datetime += nmea.getYear();
        datetime += "-";
        datetime += nmea.getMonth();
        datetime += "-";
        datetime += nmea.getDay();
        datetime += " ";
        datetime += nmea.getHour();
        datetime += ":";
        datetime += nmea.getMinute();
        datetime += ":";
        datetime += nmea.getSecond();
      }
      return datetime;
    }

    void GpsInterface::setGPSInfo() {
      this->good_fix = nmea.isValid();
      this->num_sats = nmea.getNumSatellites();

      this->datetime = this->dt_string_from_gps();

      this->lat = String((float)nmea.getLatitude()/1000000, 7);
      this->lon = String((float)nmea.getLongitude()/1000000, 7);
      long alt = 0;
      if (!nmea.getAltitude(alt)){
        alt = 0;
      }
      this->altf = (float)alt / 1000;

      this->accuracy = 2.5 * ((float)nmea.getHDOP()/10);

      //nmea.clear();
    }

    float GpsInterface::getAccuracy() {
      return this->accuracy;
    }

    String GpsInterface::getLat() {
      return this->lat;
    }

    String GpsInterface::getLon() {
      return this->lon;
    }

    float GpsInterface::getAlt() {
      return this->altf;
    }

    String GpsInterface::getDatetime() {
      return this->datetime;
    }

    String GpsInterface::getNumSatsString() {
      return (String)num_sats;
    }

    bool GpsInterface::getFixStatus() {
      return this->good_fix;
    }

    String GpsInterface::getFixStatusAsString() {
      if (this->getFixStatus())
        return "Yes";
      else
        return "No";
    }

    bool GpsInterface::getGpsModuleStatus() {
      return this->gps_enabled;
    }

    void GpsInterface::main() {
      while (Serial2.available()) {
        //Fetch the character one by one
        char c = Serial2.read();
        //Serial.print(c);
        //Pass the character to the library
        nmea.process(c);
      }

      uint8_t num_sat = nmea.getNumSatellites();

      if ((nmea.isValid()) && (num_sat > 0))
        this->setGPSInfo();

      else if ((!nmea.isValid()) && (num_sat <= 0)) {
        this->setGPSInfo();
      }
    }

  #endif

#endif