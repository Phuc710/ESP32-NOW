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
