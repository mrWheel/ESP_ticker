/*
***************************************************************************  
**  Program : WeerliveClass.cpp
**  Copyright (c) 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

#include "WeerliveClass.h"

const char* Weerlive::apiHost = "http://weerlive.nl/api/weerlive_api_v2.php";

const char* dayNames[] = {"Zondag", "Maandag", "Dinsdag", "Woensdag", "Donderdag", "Vrijdag", "Zaterdag"};

Weerlive::Weerlive(WiFiClient& client) : client(client), apiUrl("") {}

/**
 * Initializes the Weerlive class by setting up the API URL with the provided key and city,
 * then starts the serial communication and configures filters.
 *
 * @param key The key for the API
 * @param city The city for which weather data is requested
 *
 * @throws None
 */
void Weerlive::setup(const char* key, const char* city) 
{
    apiUrl = String(apiHost) + "?key=" + key + "&locatie=" + city;
    Serial.begin(115200);
    delay(500);
    Serial.println("\nWeerlive setup");
    configureFilters();

} //  setup()

/**
 * Configures the filters for the Weerlive class.
 *
 * Sets up the filters for the 'liveweer' and 'wk_verw' objects to include specific
 * weather data.
 * The filters are used to determine which weather data to retrieve from the API.
 *
 * @throws None
 */
void Weerlive::configureFilters() 
{
    filter["liveweer"][0]["plaats"]   = true;
    filter["liveweer"][0]["time"]     = true;
    filter["liveweer"][0]["temp"]     = true;   //-- actuele temperatuur in graden Celsius
    filter["liveweer"][0]["gtemp"]    = false;  //-- gevoeld temperatuur
    filter["liveweer"][0]["samenv"]   = true;   //-- samenvatting
    filter["liveweer"][0]["lv"]       = true;   //-- relatieve luchtvochtigheid
    filter["liveweer"][0]["windr"]    = true;   //-- windrichting
    filter["liveweer"][0]["windrgr"]  = false;  //-- windrichting in graden
    filter["liveweer"][0]["windms"]   = false;  //-- windkracht in m/s
    filter["liveweer"][0]["windbft"]  = true;   //-- windkracht in bft
    filter["liveweer"][0]["windknp"]  = false;  //-- windsnelheid in knoppen
    filter["liveweer"][0]["windkmh"]  = false;  //-- windsnelheid in km/h
    filter["liveweer"][0]["luchtd"]   = true;   //-- luchtdruk
    filter["liveweer"][0]["ldmmhg"]   = false;  //-- luchtdruk in mmHg
    filter["liveweer"][0]["dauwp"]    = true;   //-- dauwpunt
    filter["liveweer"][0]["zicht"]    = false;  //-- zicht in meters
    filter["liveweer"][0]["gr"]       = true;   //-- globale (zonne)straling in Watt/M2
    filter["liveweer"][0]["verw"]     = false;  //-- korte dagverwachting
    filter["liveweer"][0]["sup"]      = false;  //-- zon op
    filter["liveweer"][0]["sunder"]   = true;   //-- zon onder
    filter["liveweer"][0]["image"]    = true;   //-- afbeeldingsnaam
    filter["liveweer"][0]["alarm"]    = true;   //-- Geldt er een weerwaarschuwing? 1 = ja, 0 = nee
    filter["liveweer"][0]["lkop"]     = true;   //-- Weerwaarschuwing korte omschrijving
    filter["liveweer"][0]["ltekst"]   = false;  //-- Langere beschrijving van de waarschuwing
    filter["liveweer"][0]["wrschklr"] = false;  //-- KNMI kleurcode voor jouw regio
    filter["liveweer"][0]["wrsch_g"]  = false;  //-- Moment waarop de eerstkomende KNMI-waarschuwing geldt
    filter["liveweer"][0]["wrsch_gts"]= false;  //-- Timestamp van wrsch_g
    filter["liveweer"][0]["wrsch_gc"] = false;  //-- KNMI kleurcode voor de eerstkomende waarschuwing
    //---- week verwachting ---
    for(int i = 0; i < 5; i++)
    {
      filter["wk_verw"][i]["dag"]       = true;   //-- datum van deze dag
      filter["wk_verw"][i]["image"]     = true;   //-- afbeeldingsnaamafbeeldingsnaam
      filter["wk_verw"][i]["max_temp"]  = true;   //-- maximum temperatuur voor de dag
      filter["wk_verw"][i]["min_temp"]  = true;   //-- minimum temperatuur voor de dag
      filter["wk_verw"][i]["windbft"]   = false;   //--  windkracht in Beaufort
      filter["wk_verw"][i]["windkmh"]   = false;   //-- windkracht in km/h
      filter["wk_verw"][i]["windms"]    = false;   //-- windkracht in m/s
      filter["wk_verw"][i]["windknp"]   = false;   //-- windkracht in knopen
      filter["wk_verw"][i]["windrgr"]   = false;   //-- windrichting in graden
      filter["wk_verw"][i]["windr"]     = false;   //-- windrichting
      filter["wk_verw"][i]["neersl_perc_dag"] = true;   //-- kans op neerslag per dag
      filter["wk_verw"][i]["zond_perc_dag"]   = false;   //-- kans op zon per dag
    }

} //  configureFilters()

/**
 * Makes a request to the Weerlive API and processes the response.
 *
 * @return a pointer to a C-style string containing the processed weather data.
 *
 * @throws None.
 */
const char* Weerlive::request() 
{
    if (apiUrl.isEmpty()) 
    {
        weerliveText = "API URL not set";
        Serial.println(weerliveText);
        return weerliveText.c_str();
    }

    Serial.println("Weerlive request");
    #ifdef DEBUG
      Serial.println(apiUrl);
    #endif  // DEBUG
    HTTPClient http;
    http.begin(client, apiUrl);
    int httpCode = http.GET();

    if (httpCode > 0) 
    {
      #ifdef DEBUG
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        Serial.print("HTTP Response code: ");
        Serial.println(httpCode);
      #endif  // DEBUG
        payload = http.getString();
        if (httpCode == 429) 
        {
          weerliveText = "httpCode [429]Too many requests";
          #ifdef DEBUG
            Serial.println(weerliveText);
          #endif  // DEBUG
          return weerliveText.c_str();
        }

      #ifdef DEBUG
        Serial.println(payload);
      #endif  // DEBUG

      DeserializationError error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
      if (error) 
      {
        weerliveText = "JSON Deserialization failed: " + String(error.f_str());
        Serial.println(weerliveText);
        return weerliveText.c_str();
      }

      weerliveText = "";

      Serial.println("\nProcessliveweer ......  \n");
      JsonArray liveweerArray = doc["liveweer"];
      alarmInd = -1;
      for (JsonObject weather : liveweerArray) 
      {
        for (JsonPair kv : weather) 
        {
          #ifdef DEBUG
            Serial.print(kv.key().c_str());
            Serial.print(": ");
            Serial.println(kv.value().as<String>());
          #endif  // DEBUG
          if (kv.key() == "plaats" && filter["liveweer"][0]["plaats"]) {
              weerliveText += kv.value().as<String>();
          } else if (kv.key() == "time" && filter["liveweer"][0]["time"]) {
              weerliveText += ", " + kv.value().as<String>();
          } else if (kv.key() == "temp" && filter["liveweer"][0]["temp"]) {
              weerliveText += " " + kv.value().as<String>() + "°C";
          } else if (kv.key() == "gtemp" && filter["liveweer"][0]["gtemp"]) {
              weerliveText += ", gevoelstemperatuur " + kv.value().as<String>() + "°C";
          } else if (kv.key() == "samenv" && filter["liveweer"][0]["samenv"]) {
              weerliveText += ", weersgesteldheid: " + kv.value().as<String>();
          } else if (kv.key() == "lv" && filter["liveweer"][0]["lv"]) {
              weerliveText += ", luchtvochtigheid " + kv.value().as<String>() + "%";
          } else if (kv.key() == "windr" && filter["liveweer"][0]["windr"]) {
              weerliveText += ", windrichting " + kv.value().as<String>();
          } else if (kv.key() == "windrgr" && filter["liveweer"][0]["windrgr"]) {
              weerliveText += ", windrgr " + kv.value().as<String>() + "°";
          } else if (kv.key() == "windms" && filter["liveweer"][0]["windms"]) {
              weerliveText += ", wind " + kv.value().as<String>() + " m/s";
          } else if (kv.key() == "windbft" && filter["liveweer"][0]["windbft"]) {
              weerliveText += ", wind " + kv.value().as<String>() + " bft";
          } else if (kv.key() == "windknp" && filter["liveweer"][0]["windknp"]) {
              weerliveText += ", wind " + kv.value().as<String>() + " kts";
          } else if (kv.key() == "windkmh" && filter["liveweer"][0]["windkmh"]) {
              weerliveText += ", wind " + kv.value().as<String>() + " km/h";
          } else if (kv.key() == "luchtd" && filter["liveweer"][0]["luchtd"]) {
              weerliveText += ", luchtdruk " + kv.value().as<String>() + " hPa";
          } else if (kv.key() == "ldmmhg" && filter["liveweer"][0]["ldmmhg"]) {
              weerliveText += ", luchtdruk " + kv.value().as<String>() + " mmHg";
          } else if (kv.key() == "dauwp" && filter["liveweer"][0]["dauwp"]) {
              weerliveText += ", dauwpunt " + kv.value().as<String>() + "°C";
          } else if (kv.key() == "zicht" && filter["liveweer"][0]["zicht"]) {
              weerliveText += ", zicht " + kv.value().as<String>() +" m";
          } else if (kv.key() == "gr" && filter["liveweer"][0]["gr"]) {
              weerliveText += ", globale (zonne)straling " + kv.value().as<String>() + " Watt/M2";
          } else if (kv.key() == "verw" && filter["liveweer"][0]["verw"]) {
              weerliveText += ", dagverwachting: " + kv.value().as<String>();
          } else if (kv.key() == "sup" && filter["liveweer"][0]["sup"]) {
              weerliveText += ", zon op " + kv.value().as<String>();
          } else if (kv.key() == "sunder" && filter["liveweer"][0]["sunder"]) {
              weerliveText += ", zon onder " + kv.value().as<String>();
          } else if (kv.key() == "image" && filter["liveweer"][0]["image"]) {
              weerliveText += ", " + kv.value().as<String>();
          } else if (kv.key() == "alarm" && filter["liveweer"][0]["alarm"]) {
              alarmInd = kv.value().as<String>().toInt();
          } else if (kv.key() == "lkop" && filter["liveweer"][0]["lkop"] && (alarmInd == 1)) {
              weerliveText += ", waarschuwing: " + kv.value().as<String>();
          } else if (kv.key() == "ltekst" && filter["liveweer"][0]["ltekst"] && (alarmInd == 1)) { 
              weerliveText += ", waarschuwing: " + kv.value().as<String>();
          } else if (kv.key() == "wrschklr" && filter["liveweer"][0]["wrschklr"]) {
              weerliveText += ", KNMI kleurcode " + kv.value().as<String>();
          } else if (kv.key() == "wrsch_g" && filter["liveweer"][0]["wrsch_g"] && (alarmInd == 1)) { 
              weerliveText += ", waarschuwing:" + kv.value().as<String>();
          } else if (kv.key() == "wrsch_gts" && filter["liveweer"][0]["wrsch_gts"] && (alarmInd == 1)) {
              weerliveText += ", Timestamp van waarschuwing " + kv.value().as<String>();
          } else if (kv.key() == "wrsch_gc" && filter["liveweer"][0]["wrsch_gc"] && (alarmInd == 1)) {
              weerliveText += ", kleurcode eerstkomende waarschuwing " + kv.value().as<String>();
          } 
          else 
          {
          //weerliveText += ", [" + String(kv.key().c_str()) + "] = " + kv.value().as<String>();
          }
        }
      }

      Serial.println("\nProcess wk_verw ......  \n");
      // Process the wk_verw array
      JsonArray wk_verwArray = doc["wk_verw"];
      for (JsonVariant v : wk_verwArray) 
      {
        JsonObject obj = v.as<JsonObject>();
        for (JsonPair kv : obj) 
        {
          #ifdef DEBUG
            Serial.print(kv.key().c_str());
            Serial.print(": ");
            Serial.println(kv.value().as<String>());
          #endif  // DEBUG
          if (kv.key() == "dag" && filter["wk_verw"][0]["dag"]) {
              //weerliveText += " ["+ kv.value().as<String>() +"]";
              weerliveText += ", "+ String(dateToDayName(kv.value().as<String>().c_str()));
          } else if (kv.key() == "image" && filter["wk_verw"][0]["image"]) {
              weerliveText += " " + kv.value().as<String>();
          } else if (kv.key() == "max_temp" && filter["wk_verw"][0]["max_temp"]) {
              weerliveText += " max " + kv.value().as<String>() + "°C";
          } else if (kv.key() == "min_temp" && filter["wk_verw"][0]["min_temp"]) {
              weerliveText += " min " + kv.value().as<String>() + "°C";
          } else if (kv.key() == "windbft" && filter["wk_verw"][0]["windbft"]) {
              weerliveText += " wind " + kv.value().as<String>() + " bft";
          } else if (kv.key() == "windkmh" && filter["wk_verw"][0]["windkmh"]) {
              weerliveText += " wind " + kv.value().as<String>() + " km/h";
          } else if (kv.key() == "windms" && filter["wk_verw"][0]["windms"]) {
              weerliveText += " wind " + kv.value().as<String>() + " m/s";
          } else if (kv.key() == "windknp" && filter["wk_weer"][0]["windknp"]) {
              weerliveText += " wind " + kv.value().as<String>() + " kts";
          } else if (kv.key() == "windrgr" && filter["wk_verw"][0]["windrgr"]) {
              weerliveText += " windrichting " + kv.value().as<String>() + "°";
          } else if (kv.key() == "windr" && filter["wk_verw"][0]["windr"]) {
              weerliveText += " windrichting " + kv.value().as<String>();
          } else if (kv.key() == "neersl_perc_dag" && filter["wk_verw"][0]["neersl_perc_dag"]) {
              weerliveText += " neerslag kans " + kv.value().as<String>() + "%";
          } else if (kv.key() == "zond_perc_dag" && filter["wk_verw"][0]["zond_perc_dag"]) {
              weerliveText += " zon kans " + kv.value().as<String>() + "%";
          } 
          else 
          {
          //weerliveText += ", [" + String(kv.key().c_str()) + "] = " + kv.value().as<String>();
          }
        }
      }
      #ifdef DEBUG
        Serial.printf("\nweerliveText is [%s]\n\n", weerliveText.c_str());
      #endif  // DEBUG
    } 
    else 
    {
      weerliveText = "Error [" + String(httpCode) + "] on HTTP request";
      Serial.println(weerliveText);
    }

    http.end();
    return weerliveText.c_str();
} //  request()

/**
 * Takes a date string in the format "day-month-year" and converts it to the 
 * corresponding day of the week using Zeller’s Congruence
 * 
 * @param date_str The date string in the format "day-month-year"
 *
 * @return A const char* representing the day of the week
 *
 * @throws None
 */
const char* Weerlive::dateToDayName(const char* date_str) 
{
    int day, month, year;
    sscanf(date_str, "%d-%d-%d", &day, &month, &year);

    if (month < 3) 
    {
        month += 12;
        year -= 1;
    }

    int K = year % 100;
    int J = year / 100;
    int f = day + 13 * (month + 1) / 5 + K + K / 4 + J / 4 + 5 * J;
    int dayOfWeek = f % 7;

    // Adjust dayOfWeek to fit our dayNames array (0 = Saturday, 1 = Sunday, ..., 6 = Friday)
    dayOfWeek = (dayOfWeek + 6) % 7;

    return dayNames[dayOfWeek];
}


/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
****************************************************************************
*/
