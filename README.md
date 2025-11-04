ESP-NOW cho ESP32/ESP8266

Giao tiếp peer-to-peer không cần router Wi-Fi, độ trễ ~<1 ms (lý tưởng), tiết kiệm năng lượng. Phù hợp cảm biến, điều khiển thời gian thực và IoT cục bộ.

Điểm chính

⚡ Độ trễ rất thấp · 🔋 Tiêu thụ điện thấp

📡 Tầm xa ~200 m (lý tưởng) · 🧱 ~250 B mỗi gói

🔐 Hỗ trợ AES-128 theo từng peer

👥 Broadcast hoặc unicast nhiều thiết bị

Kiến trúc nhanh

ESP-NOW dùng 802.11 action frames. Mỗi node có MAC riêng. Vai trò linh hoạt: controller / device / combo. Thực tế chỉ cần đăng ký peer (MAC, kênh, mã hóa) rồi esp_now_send().

Ứng dụng

Cảm biến không dây · Nút bấm/điều khiển relay · Thu thập dữ liệu công nghiệp · Bài toán cần độ trễ thấp.

Ví dụ tối giản (Arduino)
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

Ghi chú nhanh

Gói >250 B: hãy chia nhỏ + gắn sequence ID.

Cố định kênh Wi-Fi giữa các node; quản lý danh sách peer để tránh tràn RAM.

Đồng tồn tại với Wi-Fi Internet được, miễn cùng kênh.
