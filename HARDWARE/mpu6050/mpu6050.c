#include "mpu6050.h"
#include "delay.h"
#include "usart1.h"   
/*********************************************************************************************************
* 函 数 名 : Mpu6050_Init
* 功能说明 : 初始化MPU6050
* 形    参 : 无
* 返 回 值 : 0：成功；其他：错误代码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Mpu6050_Init(void)
{ 
	unsigned char res;
	// GPIO_InitTypeDef GPIO_InitStructure;            	 	
	// RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	// GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;	
	// GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT; 
	// GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;   
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	// GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; 
	// GPIO_Init(GPIOB, &GPIO_InitStructure);          
	// GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7); 
	
	Mpu6050_WriteByte(MPU_PWR_MGMT1_REG, 0X80);	//复位MPU6050
    delay_ms(500);
	delay_ms(500);
	Mpu6050_WriteByte(MPU_PWR_MGMT1_REG, 0X00);	//唤醒MPU6050 
	Mpu6050_SetGyroFsr(3);						//陀螺仪传感器,±2000dps
	Mpu6050_SetAccelFsr(0);						//加速度传感器,±2g
	Mpu6050_SetRate(50);						//设置采样率50Hz
	Mpu6050_WriteByte(MPU_INT_EN_REG, 0X00);	//关闭所有中断
	Mpu6050_WriteByte(MPU_USER_CTRL_REG, 0X00);	//I2C主模式关闭
	Mpu6050_WriteByte(MPU_FIFO_EN_REG, 0X00);	//关闭FIFO
	Mpu6050_WriteByte(MPU_INTBP_CFG_REG, 0X80);	//INT引脚低电平有效
	res = Mpu6050_ReadByte(MPU_DEVICE_ID_REG);

	if(res == MPU_ADDR){							//器件ID正确
		Mpu6050_WriteByte(MPU_PWR_MGMT1_REG, 0X01);	//设置CLKSEL,PLL X轴为参考
		Mpu6050_WriteByte(MPU_PWR_MGMT2_REG, 0X00);	//加速度与陀螺仪都工作
		Mpu6050_SetRate(50);						//设置采样率为50Hz
 	}else{
		return 1;
	}
	
	return 0;
}
/*********************************************************************************************************
* 函 数 名 : Mpu6050_SetGyroFsr
* 功能说明 : 设置MPU6050陀螺仪传感器满量程范围
* 形    参 : fsr：0，±250dps；1，±500dps；2，±1000dps；3，±2000dps
* 返 回 值 : 0：成功；其他：失败
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Mpu6050_SetGyroFsr(unsigned char fsr)
{
	return Mpu6050_WriteByte(MPU_GYRO_CFG_REG, fsr<<3);		//设置陀螺仪满量程范围  
}
/*********************************************************************************************************
* 函 数 名 : Mpu6050_SetAccelFsr
* 功能说明 : 设置MPU6050加速度传感器满量程范围
* 形    参 : fsr：0，±2g；1，±4g；2，±8g；3，±16g
* 返 回 值 : 0：成功；其他：失败
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Mpu6050_SetAccelFsr(unsigned char fsr)
{
	return Mpu6050_WriteByte(MPU_ACCEL_CFG_REG, fsr<<3);	//设置加速度传感器满量程范围  
}
/*********************************************************************************************************
* 函 数 名 : Mpu6050_SetLPF
* 功能说明 : 设置MPU6050的数字低通滤波器
* 形    参 : lpf：数字低通滤波频率(Hz)
* 返 回 值 : 0：成功；其他：失败
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Mpu6050_SetLPF(unsigned short lpf)
{
	unsigned char data = 0;
	
	if(lpf >= 188){
		data = 1;
	}else if(lpf >= 98){
		data = 2;
	}else if(lpf >= 42){
		data = 3;
	}else if(lpf >= 20){
		data = 4;
	}else if(lpf >= 10){
		data = 5;
	}else{
		data = 6; 
	}
	
	return Mpu6050_WriteByte(MPU_CFG_REG, data);	//设置数字低通滤波器  
}
/*********************************************************************************************************
* 函 数 名 : Mpu6050_SetRate
* 功能说明 : 设置MPU6050的采样率(假定Fs=1KHz)
* 形    参 : rat：4~1000(Hz)
* 返 回 值 : 0：成功；其他：失败
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Mpu6050_SetRate(unsigned short rate)
{
	unsigned char data;
	
	if(rate > 1000){
		rate = 1000;
	}
	
	if(rate < 4){
		rate = 4;
	}
	
	data = 1000/rate - 1;
	data = Mpu6050_WriteByte(MPU_SAMPLE_RATE_REG, data);	//设置数字低通滤波器
	
 	return Mpu6050_SetLPF(rate/2);								//自动设置LPF为采样率的一半
}
/*********************************************************************************************************
* 函 数 名 : usart1_report_imu
* 功能说明 : 通过串口一上报姿态数据
* 形    参 : roll：横滚角；pitch：俯仰角；yaw：航向角；
* 返 回 值 : 无
* 备    注 : 此函数上报至匿名上位机（v7），传入的角度数据应扩大100倍
*********************************************************************************************************/ 
void usart1_report_imu(short roll, short pitch, short yaw)
{
	unsigned char tbuf[20] = {0}; 
	unsigned char cnt = 0, i = 0;
	unsigned char sumcheck = 0;
	unsigned char addcheck = 0;
	
	tbuf[cnt++] = 0xaa;		//帧头
	tbuf[cnt++] = 0xff;		//目标地址
	tbuf[cnt++] = 0x03;		//功能码，0x03：飞控姿态，欧拉角模式
	tbuf[cnt++] = 0x07;		//数据长度固定7个
	
	tbuf[cnt++] = BYTE0(roll);
	tbuf[cnt++] = BYTE1(roll);
	
	tbuf[cnt++] = BYTE0(pitch);
	tbuf[cnt++] = BYTE1(pitch);
	
	tbuf[cnt++] = BYTE0(yaw);
	tbuf[cnt++] = BYTE1(yaw);
	
	tbuf[cnt++] = 255;			//融合状态
	
	for(i=0; i<(tbuf[3]+4); i++){
		sumcheck += tbuf[i]; 	//从帧头开始，对每一字节进行求和，直到DATA区结束
		addcheck += sumcheck; 	//每一字节的求和操作，进行一次sumcheck的累加
	}
	tbuf[cnt++] = sumcheck;		//和校验
	tbuf[cnt++] = addcheck;		//附加校验
	
	usart1_send_pack(tbuf, cnt);
} 
/*********************************************************************************************************
* 函 数 名 : Mpu6050_GetTemperature
* 功能说明 : 读取MPU6050温度值
* 形    参 : 无
* 返 回 值 : 温度值（扩大了100倍）
* 备    注 : 无
*********************************************************************************************************/ 
short Mpu6050_GetTemperature(void)
{
    unsigned char buf[2]; 
    short raw;
	float temp;
	
	Mpu6050_ReadLen(MPU_ADDR, MPU_TEMP_OUTH_REG, 2, buf); 
    raw = ((unsigned short)buf[0]<<8) | buf[1];  
    temp = 36.53 + ((double)raw)/340;  
	
    return temp * 100;
}
/*********************************************************************************************************
* 函 数 名 : Mpu6050_GetGyroscope
* 功能说明 : 读取MPU6050陀螺仪值（原始值）
* 形    参 : gx、gy、gz：陀螺仪x、y、z轴的原始读数（带符号）
* 返 回 值 : 0：成功；其他：失败
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Mpu6050_GetGyroscope(short *gx, short *gy, short *gz)
{
    unsigned char buf[6], res;  
	
	res = Mpu6050_ReadLen(MPU_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
	if(res == 0){
		*gx = ((unsigned short)buf[0]<<8) | buf[1];  
		*gy = ((unsigned short)buf[2]<<8) | buf[3];  
		*gz = ((unsigned short)buf[4]<<8) | buf[5];
	}
 	
    return res;
}
/*********************************************************************************************************
* 函 数 名 : Mpu6050_GetAccelerometer
* 功能说明 : 读取MPU6050加速度值（原始值）
* 形    参 : ax、ay、az：加速度计x、y、z轴的原始读数（带符号）
* 返 回 值 : 0：成功；其他：失败
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Mpu6050_GetAccelerometer(short *ax,short *ay,short *az)
{
    unsigned char buf[6], res;  
	
	res = Mpu6050_ReadLen(MPU_ADDR,MPU_ACCEL_XOUTH_REG, 6, buf);
	if(res == 0){
		*ax = ((unsigned short)buf[0]<<8) | buf[1];  
		*ay = ((unsigned short)buf[2]<<8) | buf[3];  
		*az = ((unsigned short)buf[4]<<8) | buf[5];
	} 	
	
    return res;
}
/*********************************************************************************************************
* 函 数 名 : Mpu6050_WriteLen
* 功能说明 : IIC连续写
* 形    参 : addr：器件地址；reg：寄存器地址；len：写入长度；buf：数据区；
* 返 回 值 : 0：成功；其他：失败
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Mpu6050_WriteLen(unsigned char addr, unsigned char reg, unsigned char len, unsigned char *buf)
{
	unsigned char i; 
	
    IIC_Start(); 
	IIC_WriteByte((addr<<1) | 0);	//发送器件地址+写命令	
	if(IIC_Wait_ACK()){				//等待应答
		IIC_Stop();		 
		return 1;		
	}
	
    IIC_WriteByte(reg);				//写寄存器地址
    IIC_Wait_ACK();					//等待应答
	for(i=0; i<len; i++){
		IIC_WriteByte(buf[i]);		//发送数据
		if(IIC_Wait_ACK()){			//等待ACK
			IIC_Stop();	 
			return 1;		 
		}		
	}    
    IIC_Stop();
	
	return 0;	
} 
/*********************************************************************************************************
* 函 数 名 : Mpu6050_ReadLen
* 功能说明 : IIC连续读
* 形    参 : addr：器件地址；reg：寄存器地址；len：写入长度；buf：数据区；
* 返 回 值 : 0：成功；其他：失败
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Mpu6050_ReadLen(unsigned char addr, unsigned char reg, unsigned char len, unsigned char *buf)
{ 
 	IIC_Start(); 
	IIC_WriteByte((addr<<1) | 0);	//发送器件地址+写命令	
	if(IIC_Wait_ACK()){				//等待应答
		IIC_Stop();		 
		return 1;		
	}
	
    IIC_WriteByte(reg);				//写寄存器地址
    IIC_Wait_ACK();					//等待应答
    IIC_Start();
	IIC_WriteByte((addr<<1) | 1);	//发送器件地址+读命令	
    IIC_Wait_ACK();					//等待应答 
	while(len){
		if(len==1){
			*buf=IIC_ReadByte(1);	//读数据,发送nACK 
		}else{
			*buf=IIC_ReadByte(0);	//读数据,发送ACK  
		}
		len--;
		buf++; 
	}    
    IIC_Stop();						//产生一个停止条件 
	
	return 0;	
}
/*********************************************************************************************************
* 函 数 名 : Mpu6050_WriteByte
* 功能说明 : IIC写一个字节 
* 形    参 : reg：寄存器地址；data：写入的数据；
* 返 回 值 : 0：成功；其他：失败
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Mpu6050_WriteByte(unsigned char reg,unsigned char data) 				 
{ 
    IIC_Start(); 
	IIC_WriteByte((MPU_ADDR<<1) | 0);	//发送器件地址+写命令	
	if(IIC_Wait_ACK()){					//等待应答
		IIC_Stop();		 
		return 1;		
	}
	
    IIC_WriteByte(reg);					//写寄存器地址
    IIC_Wait_ACK();						//等待应答 
	IIC_WriteByte(data);				//发送数据
	if(IIC_Wait_ACK()){					//等待ACK
		IIC_Stop();	 
		return 1;		 
	}		 
    IIC_Stop();	
	
	return 0;
}
/*********************************************************************************************************
* 函 数 名 : Mpu6050_ReadByte
* 功能说明 : IIC读一个字节 
* 形    参 : reg：寄存器地址
* 返 回 值 : 读到的数据
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Mpu6050_ReadByte(unsigned char reg)
{
	unsigned char res;
	
    IIC_Start(); 
	IIC_WriteByte((MPU_ADDR<<1) | 0);	//发送器件地址+写命令	
	IIC_Wait_ACK();						//等待应答 
    IIC_WriteByte(reg);					//写寄存器地址
    IIC_Wait_ACK();						//等待应答
    IIC_Start();
	IIC_WriteByte((MPU_ADDR<<1) | 1);	//发送器件地址+读命令	
    IIC_Wait_ACK();						//等待应答 
	res=IIC_ReadByte(1);				//读取数据,发送nACK 
    IIC_Stop();							//产生一个停止条件 
	
	return res;		
}


