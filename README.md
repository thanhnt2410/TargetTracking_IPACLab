# TargetTracking_IPACLab
Đồ án định vị người trong không gian kín và mô phỏng tập huấn cứu hoả. Sử dụng định vị bằng sóng wifi và cảm biến IMU đọc giá trị toạ độ. Dữ liệu được gửi lên sever quản lý
Thư mục ESP07: Nạp code kết nối wifi và scan tín hiệu RSSI gửi lên Serial theo giá trị abs(RSSI)
Thư mục esp32: Code chính cho việc thu dữ liệu từ ESP07 và các giá trị BNO055, biến trở và đẩy lên MQTT server.
Lưu ý: Sử dụng MUX 74HC153 để quét 4 kênh UART của ESP07 để nhận dữ liệu. 
DỮ liệu trả về từ Server hiển thị lên màn hình là vị trí của người và vị trí của các ngọn lửa. 
//-----------------English--------------------------------------
Project for locating people in closed spaces and simulating firefighting training. Using wifi positioning and IMU sensor to read coordinate values. Data is sent to the management server
ESP07 folder: Load the wifi connection code and scan the RSSI signal and send it to Serial according to the value abs(RSSI)
Folder esp32: Main code for collecting data from ESP07 and BNO055 values, potentiometer and pushing to MQTT server.
Note: Use MUX 74HC153 to scan 4 UART channels of ESP07 to receive data. 
The data returned from the Server displayed on the screen is the location of people and the location of fires.
