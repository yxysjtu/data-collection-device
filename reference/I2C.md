（1）主机发送数据
主机在检测到总线为“空闲状态”（即 SDA、SCL 线均为高电平）时，发送一个启动信号“S”，开始一次通信的开始；
主机接着发送一个从设备地址，它由7bit物理地址和1bit读写控制位R/w组成（此时R/W=0）；
相对应的从机收到命令字节后向主机回馈应答信号 ACK（ACK=0）；
主机收到从机的应答信号后开始发送第一个字节的数据；
从机收到数据后返回一个应答信号 ACK；
主机收到应答信号后再发送下一个数据字节；
当主机发送最后一个数据字节并收到从机的 ACK 后，通过向从机发送一个停止信号P结束本次通信并释放总线。从机收到P信号后也退出与主机之间的通信。

（2）主机接收数据
主机发送起始信号后，接着发送地址字节(其中R/W=1)；
对应的从机收到地址字节后，返回一个应答信号并向主机发送数据；
主机收到数据后向从机反馈一个应答信号ACK；
从机收到应答信号后再向主机发送下一个数据；
当主机完成接收数据后，向从机发送一个NAK，从机收到非应答信号后便停止发送；
主机发送非应答信号后，再发送一个停止信号，释放总线结束通信。

HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

