ESP-NOW – Giao thức không dây siêu tiết kiệm cho ESP32/ESP8266

Giao tiếp peer-to-peer, độ trễ cực thấp (~<1 ms trong điều kiện lý tưởng), không cần router Wi-Fi, phù hợp cho cảm biến, điều khiển thời gian thực và thiết bị IoT công suất thấp.

Mục lục

ESP-NOW là gì?

Điểm nổi bật

Kiến trúc & cách hoạt động

Ứng dụng điển hình

Bắt đầu nhanh

Arduino (ESP32/ESP8266)

ESP-IDF (ESP32)

So sánh nhanh

Thách thức thường gặp & gợi ý giải pháp

Câu hỏi thường gặp

Đóng góp

Giấy phép

ESP-NOW là gì?

ESP-NOW là giao thức truyền thông không dây do Espressif phát triển dành cho ESP32 và ESP8266. Thay vì phải kết nối tới Access Point (AP) như Wi-Fi truyền thống, các thiết bị ESP giao tiếp trực tiếp với nhau (peer-to-peer), giúp giảm độ trễ, tiết kiệm năng lượng, và đơn giản hoá cấu hình mạng.

Điểm nổi bật

⚡ Độ trễ cực thấp: ~<1 ms (lý tưởng)

🔋 Tiết kiệm năng lượng: thấp hơn đáng kể so với Wi-Fi thông thường

📡 Tầm xa: đến ~200 m (không vật cản, cấu hình ăng-ten phù hợp)

🧱 Kích thước dữ liệu: ~250 byte mỗi gói

🔐 Mã hoá: AES-128 (ESP32), chia sẻ key theo peer

👥 Nhiều peer: gửi broadcast hoặc unicast đến nhiều thiết bị

Lưu ý: giá trị thực tế phụ thuộc môi trường, nhiễu RF, kiểu ăng-ten, nguồn, firmware, v.v.

Kiến trúc & cách hoạt động

ESP-NOW sử dụng 802.11 action frames để truyền dữ liệu. Mỗi thiết bị có địa chỉ MAC duy nhất. Về logic, có thể hình dung 3 vai trò:

Controller: khởi phát/gửi lệnh, điều phối

Slave/Device: phản hồi/nhận lệnh

Combo: vừa gửi vừa nhận

Trên thực tế, bạn chỉ cần đăng ký peer theo MAC, cấu hình kênh, mã hoá (nếu cần) và sử dụng API esp_now_send() / callback nhận dữ liệu.

Ứng dụng điển hình

🌡️ Mạng cảm biến không dây: nhiệt độ, độ ẩm, chuyển động, cửa từ…

🎛️ Điều khiển thời gian thực: đèn, relay, động cơ, robot mini…

🏭 Giám sát công nghiệp: thu thập dữ liệu cục bộ, phản hồi nhanh

⏱️ Hệ thống cần độ trễ thấp: nút bấm không dây, game controller DIY…

Bắt đầu nhanh
Arduino (ESP32/ESP8266)

Gửi broadcast tối giản

#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <esp_now.h>
#else
  #include <ESP8266WiFi.h>
  extern "C" {
    #include <espnow.h>
  }
#endif

// Broadcast MAC (gửi tới mọi thiết bị lắng nghe)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

struct Payload {
  uint32_t counter;
  float temperature;
} __attribute__((packed));

#ifdef ESP32
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.printf("Send to %02X:%02X:%02X:%02X:%02X:%02X => %s\n",
                mac_addr[0], mac_addr[1], mac_addr[2],
                mac_addr[3], mac_addr[4], mac_addr[5],
                status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}
void onRecv(const uint8_t *mac, const uint8_t *data, int len) {
  Serial.printf("Recv %d bytes from %02X:%02X:%02X:%02X:%02X:%02X\n",
                len, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
#else // ESP8266
void onSent(uint8_t *mac_addr, uint8_t status) {
  Serial.printf("Send => %s\n", status == 0 ? "OK" : "FAIL");
}
void onRecv(uint8_t *mac, uint8_t *data, uint8_t len) {
  Serial.printf("Recv %u bytes\n", len);
}
#endif

void setup() {
  Serial.begin(115200);

#ifdef ESP32
  WiFi.mode(WIFI_STA);  // Bắt buộc ở STA/STA+AP
  if (esp_now_init() != ESP_OK) {
    Serial.println("Lỗi khởi tạo ESP-NOW");
    return;
  }
  esp_now_register_send_cb(onSent);
  esp_now_register_recv_cb(onRecv);

  esp_now_peer_info_t peer{};
  memcpy(peer.peer_addr, broadcastAddress, 6);
  peer.channel = 0;      // 0 = kênh hiện tại
  peer.encrypt = false;  // bật true nếu dùng key
  if (esp_now_add_peer(&peer) != ESP_OK) {
    Serial.println("Thêm peer thất bại");
    return;
  }
#else // ESP8266
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Lỗi khởi tạo ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(onSent);
  esp_now_register_recv_cb(onRecv);

  if (esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 0, NULL, 0) != 0) {
    Serial.println("Thêm peer thất bại");
    return;
  }
#endif
}

void loop() {
  static uint32_t cnt = 0;
  Payload p{++cnt, 25.3f};

#ifdef ESP32
  esp_err_t ok = esp_now_send(broadcastAddress, (uint8_t*)&p, sizeof(p));
  if (ok != ESP_OK) Serial.printf("Send error: %d\n", ok);
#else
  uint8_t ok = esp_now_send(broadcastAddress, (uint8_t*)&p, sizeof(p));
  if (ok != 0) Serial.printf("Send error: %u\n", ok);
#endif

  delay(1000);
}


Gợi ý nhận unicast: thay broadcastAddress bằng MAC đích của node nhận (ví dụ uint8_t peerMac[] = {0x24,0x6F,0x28,0xAA,0xBB,0xCC};) và thêm peer tương ứng.

ESP-IDF (ESP32)
// CMakeLists.txt đã thêm component esp_wifi, esp_event, esp_now
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_now.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "espnow-demo";
static uint8_t bcast[] = {0xff,0xff,0xff,0xff,0xff,0xff};

typedef struct __attribute__((packed)) {
    uint32_t counter;
    float temperature;
} payload_t;

static void send_cb(const uint8_t *mac, esp_now_send_status_t status) {
    ESP_LOGI(TAG, "Send to %02x:%02x:%02x:%02x:%02x:%02x => %s",
             mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],
             status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

static void recv_cb(const uint8_t *mac, const uint8_t *data, int len) {
    ESP_LOGI(TAG, "Recv %d bytes", len);
}

void app_main(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(send_cb));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(recv_cb));

    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, bcast, 6);
    peer.channel = 0;
    peer.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peer));

    payload_t p = {.counter = 1, .temperature = 23.5f};
    while (1) {
        ESP_ERROR_CHECK(esp_now_send(bcast, (uint8_t*)&p, sizeof(p)));
        vTaskDelay(pdMS_TO_TICKS(1000));
        p.counter++;
    }
}

So sánh nhanh
Giao thức	Tầm xa	Độ trễ	Năng lượng	Tốc độ dữ liệu	Hạ tầng
ESP-NOW	~200 m (lý tưởng)	Rất thấp	Thấp	~250 B/gói (nhiều gói/giây)	Không cần router
Bluetooth (Classic/LE)	Ngắn–trung bình	Thấp–TB	Thấp	TB	Không cần router
LoRa	Rất xa	Cao	Rất thấp	Thấp	Không cần router
Wi-Fi	Trung bình	Thấp–TB	Cao	Cao	Cần router/AP

ESP-NOW mạnh ở độ trễ thấp + cấu hình tối giản. Nếu cần siêu tầm xa/siêu tiết kiệm, cân nhắc LoRa; nếu cần thông lượng cao & Internet, dùng Wi-Fi.

Thách thức thường gặp & gợi ý giải pháp

Giới hạn ~250 byte/gói
→ Chunking/fragmentation: chia dữ liệu lớn thành nhiều gói, kèm sequence ID và checksum để ghép lại an toàn.

Đồng bộ kênh Wi-Fi
→ Nên cố định kênh cho các node (tránh quét), hoặc đảm bảo tất cả hoạt động trên cùng kênh khi dùng AP song song.

Quản lý danh sách peer
→ Thêm/xoá peer đúng lúc; tránh lưu trữ dư thừa gây tràn bộ nhớ. Tận dụng broadcast khi phù hợp.

Nhiễu & môi trường
→ Điều chỉnh tốc độ gửi, thêm ACK/Retry ở tầng ứng dụng, chọn ăng-ten tốt, kiểm tra nguồn.

Mã hoá & khoá
→ Dùng AES-128 theo peer. Quản lý key an toàn (không hard-code lên public repo; cân nhắc build flags/secure storage).
