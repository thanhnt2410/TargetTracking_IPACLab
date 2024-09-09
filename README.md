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

  Link youtube demo: https://youtu.be/UA2SsdpW9Xs?si=mJk__f9eAycKoT8m


  ![LCD](https://github.com/user-attachments/assets/8157ddf3-649c-460a-afeb-1a26d19b6f25)
  Hardware: 
![image](https://github.com/user-attachments/assets/06030b6f-6320-46bc-8173-ff43eb04fa69)
  Lăng phun/device
    ![image](https://github.com/user-attachments/assets/1ae2a172-16ab-47de-a071-7aef733e6af7)
  Mạch: ![image](https://github.com/user-attachments/assets/c20b6c7c-5cd6-4a63-a376-b40c0659a513)
  ![image](https://github.com/user-attachments/assets/5963cb66-4495-4397-a9e9-033cf7d469a4)
![image](https://github.com/user-attachments/assets/b917a41d-45e1-40f8-a117-ce0be9ae2141)
![image](https://github.com/user-attachments/assets/186edc9a-589f-4fc0-9460-3a37ae02482f)

