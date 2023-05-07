#include "sht30.h"

/* ADDR Pin Conect to VSS */
 
#define    SHT30_ADDR_WRITE    0x44<<1         //10001000，根据用户手册的指示，这个就是SHT30的写入地址
#define    SHT30_ADDR_READ        (0x44<<1)+1        //10001011，根据用户手册的指示，这个就是SHT30的读取地址
extern I2C_HandleTypeDef hi2c1;//开启I2C3的接口，你用的哪个就改哪个
 
typedef enum				//命令的定义
{
    /* 软件复位命令 */
 
    SOFT_RESET_CMD = 0x30A2,
    /*
    单次测量模式
    命名格式：Repeatability_CS_CMD
    CS： Clock stretching
    */
    HIGH_ENABLED_CMD    = 0x2C06,
    MEDIUM_ENABLED_CMD  = 0x2C0D,
    LOW_ENABLED_CMD     = 0x2C10,
    HIGH_DISABLED_CMD   = 0x2400,
    MEDIUM_DISABLED_CMD = 0x240B,
    LOW_DISABLED_CMD    = 0x2416,
 
    /*
    周期测量模式
    命名格式：Repeatability_MPS_CMD
    MPS：measurement per second
    */
    HIGH_0_5_CMD   = 0x2032,
    MEDIUM_0_5_CMD = 0x2024,
    LOW_0_5_CMD    = 0x202F,
    HIGH_1_CMD     = 0x2130,
    MEDIUM_1_CMD   = 0x2126,
    LOW_1_CMD      = 0x212D,
    HIGH_2_CMD     = 0x2236,
    MEDIUM_2_CMD   = 0x2220,
    LOW_2_CMD      = 0x222B,
    HIGH_4_CMD     = 0x2334,
    MEDIUM_4_CMD   = 0x2322,
    LOW_4_CMD      = 0x2329,
    HIGH_10_CMD    = 0x2737,
    MEDIUM_10_CMD  = 0x2721,
    LOW_10_CMD     = 0x272A,
    /* 周期测量模式读取数据命令 */
    READOUT_FOR_PERIODIC_MODE = 0xE000,
} SHT30_CMD;
 
/**
 * @brief    向SHT30发送一条指令(16bit)
 * @param    cmd —— SHT30指令（在SHT30_MODE中枚举定义）
 * @retval    成功返回HAL_OK
*/
static uint8_t    SHT30_Send_Cmd(SHT30_CMD cmd)
{
    uint8_t cmd_buffer[2];
    cmd_buffer[0] = cmd >> 8;
    cmd_buffer[1] = cmd;
    return HAL_I2C_Master_Transmit(&hi2c1, SHT30_ADDR_WRITE, (uint8_t*)cmd_buffer, 2, 0xFFFF);
}
 
/**
 * @brief    复位SHT30
 * @param    none
 * @retval    none
*/
void SHT30_Reset(void)			
{
    SHT30_Send_Cmd(SOFT_RESET_CMD);
    HAL_Delay(20);
}
 
/**
 * @brief    初始化SHT30
 * @param    none
 * @retval    成功返回HAL_OK
 * @note    周期测量模式
*/
uint8_t SHT30_Init(void)
{
    return SHT30_Send_Cmd(MEDIUM_2_CMD);
}
 
/**
 * @brief    从SHT30读取一次数据
 * @param    dat —— 存储读取数据的地址（6个字节数组）
 * @retval    成功 —— 返回HAL_OK
*/
uint8_t SHT30_Read_Dat(uint8_t* dat)
{
    SHT30_Send_Cmd(READOUT_FOR_PERIODIC_MODE);
	HAL_Delay(20);
    return HAL_I2C_Master_Receive(&hi2c1, SHT30_ADDR_READ, dat, 6, 0xFFFF);
}
/*************************************************
 * 本驱动最大的难点，CRC验证；没看懂，懂行的来讲讲
 * 不管了会用就行
 */
 
#define CRC8_POLYNOMIAL 0x31
 
uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value)
{
    uint8_t  remainder;        //余数
    uint8_t  i = 0, j = 0;  //循环变量
 
    /* 初始化 */
    remainder = initial_value;
 
    for(j = 0; j < 2;j++)
    {
        remainder ^= message[j];
 
        /* 从最高位开始依次计算  */
        for (i = 0; i < 8; i++)
        {
            if (remainder & 0x80)
            {
                remainder = (remainder << 1)^CRC8_POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }
 
    /* 返回计算的CRC码 */
    return remainder;
}
/**
 * @brief    将SHT30接收的6个字节数据进行CRC校验，并转换为温度值和湿度值
 * @param    dat  —— 存储接收数据的地址（6个字节数组）
 * @retval    校验成功  —— 返回0
 *             校验失败  —— 返回1，并设置温度值和湿度值为0
*/
uint8_t SHT30_Dat_To_Float(uint8_t* const dat, float *temperature,float *humidity)
{
    uint16_t recv_temperature = 0;
    uint16_t recv_humidity = 0;
 
    /* 校验温度数据和湿度数据是否接收正确 */
    if(CheckCrc8(dat, 0xFF) != dat[2] || CheckCrc8(&dat[3], 0xFF) != dat[5])
        return 1;
 
    /* 转换温度数据 */
    recv_temperature = ((uint16_t)dat[0]<<8)|dat[1];
    *temperature = -45 + 175*((float)recv_temperature/65535);
 
    /* 转换湿度数据 */
    recv_humidity = ((uint16_t)dat[3]<<8)|dat[4];
    *humidity = 100 * ((float)recv_humidity / 65535);
 
    return 0;
}