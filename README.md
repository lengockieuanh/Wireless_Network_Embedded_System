## Wireless_Network_Embedded_System
# Giới thiệu chủ đề
Triển khai mô hình truyền dữ liệu sức khỏe thông qua hệ thống nhúng mạng không dây:
  - Thiết lập mã nguồn cho việc đo nhịp tim và gửi dữ liệu trên ESP32.
  - Cấu hình thiết bị máy ảo sử dụng để chạy hệ điều hành TinyCore.
  - Cấu hình thiết lập mạng MANET sử dụng giao thức OLSR.
  - Triển khai giao thức SMTP.
# Công cụ triển khai
### 1. Hệ điều hành TinyCore
  - Một bản phân phối của Linux
  - Dung lượng rất nhỏ: khoảng 16MB giúp tiết kiệm tài nguyên
  - Không yêu cầu các cài đặt phức tạp, có thể chạy trực tiếp từ RAM
  - Dễ dàng mở rộng, cài đặt các ứng dụng và các gói phần mềm
  - Chạy giao thức OLSR
### 2. OLSR
  - OLSRd (Optimized Link State Routing daemon) là một phần mềm thực hiện giao thức OLSR trên các hệ thống thực tế. 
  - Đây là một ứng dụng mã nguồn mở chạy trên hệ điều hành Linux cho phép các thiết bị sử dụng giao thức OLSR để thực hiện định tuyến trong mạng ad-hoc.
### 3. USB WiFi TL-WN722N
  - USB WiFi adapter TP-Link TL-WN722N là một adapter không dây phổ biến, thường được sử dụng trong các ứng dụng như tạo kết nối mạng trên các thiết bị không có WiFi tích hợp.
  - Có khả năng hỗ trợ chế độ monitor mode (giám sát gói tin).
### 4. Kit WiFi BLE ESP32
  - Hỗ trợ kết nối WiFi và Bluetooth Low Energy
  - Kit này dùng để kết nối đến cảm biến MAX30102 để thu thập dữ liệu và gửi  qua các kết nối Wi-Fi.
### 5. Cảm biến đo nhịp tim và nồng độ oxi trong máu MAX30102
  - Là một module đo nhịp tim và nồng độ oxy trong máu (SpO2) phổ biến, được sử dụng rộng rãi trong các ứng dụng y tế và thiết bị đeo thông minh. 
  - Cảm biến này được thiết kế để cung cấp dữ liệu chính xác về nhịp tim và SpO2 thông qua phương pháp quang học.
# Mô hình triển khai
![WENS_Topology](https://drive.google.com/file/d/1ODo5YIaZ1SP5KnrujwHjDHxdqVnJR11T/view?usp=sharing)

     



