//#include "jvpod.h"
//
////typedef DFMiniMp3<SoftwareSerial, Mp3Notify> DfMp3;
//
//class Mp3Notify
//{
//public:
//  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
//  {
//    if (source & DfMp3_PlaySources_Sd) 
//    {
//        Serial.print("SD Card, ");
//    }
//    if (source & DfMp3_PlaySources_Usb) 
//    {
//        Serial.print("USB Disk, ");
//    }
//    if (source & DfMp3_PlaySources_Flash) 
//    {
//        Serial.print("Flash, ");
//    }
//    Serial.println(action);
//  }
//  static void OnError([[maybe_unused]] DfMp3& mp3, uint16_t errorCode)
//  {
//    // see DfMp3_Error for code meaning
//    Serial.println();
//    Serial.print("Com Error ");
//    Serial.println(errorCode);
//  }
//  static void OnPlayFinished([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track)
//  {
//    Serial.print("Play finished for #");
//    Serial.println(track);  
//  }
//  static void OnPlaySourceOnline([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
//  {
//    PrintlnSourceAction(source, "online");
//  }
//  static void OnPlaySourceInserted([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
//  {
//    PrintlnSourceAction(source, "inserted");
//  }
//  static void OnPlaySourceRemoved([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
//  {
//    PrintlnSourceAction(source, "removed");
//  }
//};
