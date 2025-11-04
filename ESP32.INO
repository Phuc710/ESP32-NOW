#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <esp_now.h>
#else
  #include <ESP8266WiFi.h>
  extern "C" { #include <espnow.h> }
#endif

uint8_t bcast[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

void onSend(const uint8_t*, esp_now_send_status_t s){ Serial.println(s==ESP_NOW_SEND_SUCCESS?"OK":"FAIL"); }
void onRecv(const uint8_t*, const uint8_t*, int len){ Serial.printf("Recv %dB\n", len); }

void setup() {
  Serial.begin(115200); WiFi.mode(WIFI_STA);
#ifdef ESP32
  if (esp_now_init()!=ESP_OK) { Serial.println("Init ERR"); return; }
  esp_now_register_send_cb(onSend); esp_now_register_recv_cb(onRecv);
  esp_now_peer_info_t p{}; memcpy(p.peer_addr,bcast,6); p.channel=0; p.encrypt=false; esp_now_add_peer(&p);
#else
  if (esp_now_init()!=0){ Serial.println("Init ERR"); return; }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb((esp_now_send_cb_t)onSend);
  esp_now_register_recv_cb((esp_now_recv_cb_t)onRecv);
  esp_now_add_peer(bcast, ESP_NOW_ROLE_COMBO, 0, NULL, 0);
#endif
}
void loop(){ const char msg[]="hello"; esp_now_send(bcast,(uint8_t*)msg,sizeof(msg)); delay(1000); }
