#include "Driver_SPI.h"
#include "OLED_SPI.h"
#include "oledfont.h"
#include "OLED_SHOW.h"

#define USE_SPI_DRIVER //ע�����н�����ģ��SPI��ʽ

static osSemaphoreId_t sem_SPI;
osMutexId_t mutex_OLED;


#ifdef USE_SPI_DRIVER
extern ARM_DRIVER_SPI Driver_SPI2;
static ARM_DRIVER_SPI * SPIdrv = &Driver_SPI2;
int32_t SPI_WriteBuf (const uint8_t *buf, uint32_t len) 
{
	SPIdrv->Send(buf, len);
	if (osSemaphoreAcquire (sem_SPI, 100) == osErrorTimeout) {
		return -1;
	}
	return len;
}
int32_t SPI_ReadBuf (uint8_t *buf, uint32_t len) 
{
	SPIdrv->Receive(buf, len);
	if (osSemaphoreAcquire (sem_SPI, 100) == osErrorTimeout) {
		return -1;
	}
	return len;
}

int32_t SPI_TransferBuf (uint8_t *buf_out, uint8_t *buf_in, uint32_t len) 
{
	len = SPIdrv->Transfer(buf_out, buf_in, len);
	if (osSemaphoreAcquire (sem_SPI, 100) == osErrorTimeout) {
		return -1;
	}
	return len;
}

void OLED_SPI_callback(uint32_t event)
{
    switch (event)
    {
    case ARM_SPI_EVENT_TRANSFER_COMPLETE:
        /* Success: Wakeup Thread */
		osSemaphoreRelease (sem_SPI); 
        break;
    case ARM_SPI_EVENT_DATA_LOST:
        /*  Occurs in slave mode when data is requested/sent by master
            but send/receive/transfer operation has not been started
            and indicates that data is lost. Occurs also in master mode
            when driver cannot transfer data fast enough. */
//        __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
        break;
    case ARM_SPI_EVENT_MODE_FAULT:
        /*  Occurs in master mode when Slave Select is deactivated and
            indicates Master Mode Fault. */
//        __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
        break;
    }
}
 
void SPI_Initialize (void) 
{
	sem_SPI = osSemaphoreNew(1, 0, NULL);
	
    SPIdrv->Initialize(OLED_SPI_callback);		//Initialize the SPI driver
    SPIdrv->PowerControl(ARM_POWER_FULL);	//Power up the SPI peripheral
	
    // Configure the SPI to Master, 8-bit mode @1000 kBits/sec 
    SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | 
					ARM_SPI_SS_MASTER_HW_OUTPUT | ARM_SPI_DATA_BITS(8), 1000000);
 
    SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);	//SS line = INACTIVE = HIGH
}


#else

#define OLED_CS(a)		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,(GPIO_PinState)a)
#define OLED_SCLK(a)	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,(GPIO_PinState)a)
#define OLED_SDIN(a)	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,(GPIO_PinState)a)
void OLED_WR_Byte(uint8_t dat)
{	
	int i;			  
	OLED_CS(0);
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK(0);
		if(dat&0x80)
			OLED_SDIN(1);
		else 
			OLED_SDIN(0);
		OLED_SCLK(1);
		dat<<=1;   
	}				 
	OLED_CS(1);
} 
int32_t SPI_WriteBuf (const uint8_t *buf, uint32_t len) 
{
	int i;
	for(i=0;i<len;i++)
		*buf++);
	return len;
}
#endif


uint8_t OLED_GRAM[8][128];	 //OLED���Դ�ÿҳ128�У�ÿ���ֽڿ��������8�е�ɫ��ʾֵ
//��ģ��ȡ�뷽ʽ��
//PC2LCD2002ȡģ��ʽ���ã�����+����ʽ+˳��+C51��ʽ
//   �ӵ�һ�п�ʼ����ÿȡ8������Ϊһ���ֽڣ���������8����Ͳ���8λ��
//   ȡģ˳���ǴӸߵ��ͣ�����һ������Ϊ���λ����*-------ȡΪ10000000

//��������


//�����Դ浽LCD		 
void OLED_Refresh_Gram(void)
{
	uint8_t cmd_data[]={
		0xb0,		//0xb0+i����ҳ��ַ��0~7��
		0x02,		//������ʾλ�á��е͵�ַ
		0x10,		//������ʾλ�á��иߵ�ַ   
	};
//	SPI_WriteBuf(cmd_data,sizeof(cmd_data));
//	OLED_RS(1);   	//���뷢����״̬  
//	SPI_WriteBuf(OLED_GRAM[0],sizeof(OLED_GRAM));
//	OLED_RS(0);   	//���뷢����״̬  
  uint8_t i;		    
	for(i=0;i<8;i++)  
	{  
		
    SPI_WriteBuf(cmd_data,sizeof(cmd_data));
    OLED_RS(1);
    SPI_WriteBuf(OLED_GRAM[i],sizeof(OLED_GRAM[i]));
		OLED_RS(0);
    cmd_data[0] += 1;
	}   
}
	  	  
//����OLED��ʾ    
void OLED_Display_On(void)
{
	uint8_t cmd_data[]={
	0X8D,  //SET DCDC����
	0X14,  //DCDC ON
	0XAF,  //DISPLAY ON
	};
	SPI_WriteBuf(cmd_data,sizeof(cmd_data));
}

//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	uint8_t cmd_data[]={
	0X8D,  //SET DCDC����
	0X10,  //DCDC OFF
	0XAE,  //DISPLAY OFF
	};
	SPI_WriteBuf(cmd_data,sizeof(cmd_data));
}

//�������� 
void OLED_Clear(void)  
{  
	memset(OLED_GRAM[0], 0, sizeof(OLED_GRAM));
	OLED_Refresh_Gram();//������ʾ
}

//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(int x,int y,uint8_t t)
{
	int pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)
		OLED_GRAM[pos][x]|=temp;
	else 
		OLED_GRAM[pos][x]&=~temp;	    
}

//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(int x1,int y1,int x2,int y2,uint8_t dot)  
{  
	int x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)
			OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 12/16/24
void OLED_ShowChar(int x,int y,uint8_t chr,uint8_t size,uint8_t mode)
{      			    
	uint8_t temp,t,t1;
	int y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
		else return;								//û�е��ֿ�
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
				OLED_DrawPoint(x,y,mode);
			else 
				OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n����
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(int x,int y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else 
				enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//��ʾ�ַ���
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(int x,int y,const uint8_t *p,uint8_t size,uint8_t mode)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(size/2)))
			{x=0;y+=size;}
        if(y>(64-size))
			{y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,mode);	 
        x+=size/2;
        p++;
    }  
}	

#define DrawOne_bit() 						\
		{									\
			if(temp&0x80)					\
				OLED_DrawPoint(x,y,mode);	\
			else 							\
				OLED_DrawPoint(x,y,!mode);	\
			temp<<=1;						\
		}
void OLED_DrawFont_OneGBK16(int x, int y, const uint8_t *p, int mode)
{
	//����һ�����ֵ�ͼ�Σ���ģ��ʼ��ַ��p
	uint8_t temp;
	int i,j;
	int y0=y;
	
	for(i=0;i<16;i++)
	{   
		y = y0;
		temp = *p++;
		for(j=0;j<8;j++)
		{
			DrawOne_bit();
			y++;
		}  
		
		y = y0+8;
		temp = *p++;
		for(j=0;j<8;j++)
		{
			DrawOne_bit();
			y++;
		}  	 
		x++;
    }          
}

void OLED_DrawFont_GBK16(int x, int y, uint8_t *s, int mode)
{
	int k;

	while(*s) 
	{	
		if((*s) < 0x80) //�ж��ַ��Ƿ��Ǻ���
		{
			OLED_ShowChar(x,y,*s,16,1);
			x+=8;
			s++;
		}
		else 
		{	//����Ǻ���
			for (k=0;k<sizeof(hzk16)/sizeof(FONT_GB16);k++) 
			{
				if ((hzk16[k].Index[0]==*(s))&&(hzk16[k].Index[1]==*(s+1)))
					OLED_DrawFont_OneGBK16(x,y, hzk16[k].Msk, mode);
			}
			x+=16;
			s = s+2;
		}
	}
}


//��ʼ��SSD1306					   
void OLED_Init(void)
{ 	
  
//  uint8_t init_data[]={
//		0xAE, //�ر���ʾ
////����������������˫�ֽ����һ�������ֽں������һ���ֽڵĲ���	
//		0xD5, //����ʱ�ӷ�Ƶ����,��Ƶ��
//		80,   //[3:0],��Ƶ����;[7:4],��Ƶ��
//		
//		0xA8, //��������·��
//		0X3F, //Ĭ��0X3F(1/64) 
//		
//		0xD3, //������ʾƫ��
//		0X00, //Ĭ��Ϊ0

//		0x8D, //��ɱ�����
//		0x14, //bit2�����뿪����ȱʡ�ǹر�
//		
//		0x20, //�����ڴ��ַģʽ Ϊ�����ַģʽ
//		0x00, //[1:0],00�������ַģʽ;01�������ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
//		
//		0xDA, //����COMӲ����������
//		0x12, //[5:4]����
////�ɵ��ڶԱȶ�ֵ
//		0x81, //�Աȶ�����
//		0xEF, //1~255;Ĭ��0X7F (��������,Խ��Խ��)
//		
//		0xD9, //����Ԥ�������
//		0xf1, //[3:0],PHASE 1;[7:4],PHASE 2;
//		
//		0xDB, //����VCOMH�ĵ�ѹ
//		0x30, //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
//		
////���涼�ǵ��ֽ�����
//		0x40, //������ʾ��ʼ�� [5:0],����.
//		0xA1, //���ض�������,bit0:0,0->0;1,0->127;
//		0xC0, //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
//		0xA4, //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
////�ɵ���Ϊ������ʾ		
//		0xA6, //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
//		0xAF, //������ʾ	 
//	};
	uint8_t init_data[]={
    0xAE,//--turn off oled panel
    0x02,//---set low column address
    0x10,//---set high column address
    0x40,//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    0x81,//--set contrast control register
    0xCF, // Set SEG Output Current Brightness
    0xA1,//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
    0xC0,//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
    0xA6,//--set normal display
    0xA8,//--set multiplex ratio(1 to 64)
    0x3f,//--1/64 duty
    0xD3,//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    0x00,//-not offset
    0xd5,//--set display clock divide ratio/oscillator frequency
    0x80,//--set divide ratio, Set Clock as 100 Frames/Sec
    0xD9,//--set pre-charge period
    0xF1,//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    0xDA,//--set com pins hardware configuration
    0x12,
    0xDB,//--set vcomh
    0x40,//Set VCOM Deselect Level
    0x20,//-Set Page Addressing Mode (0x00/0x01/0x02)
    0x02,//
    0x8D,//--set Charge Pump enable/disable
    0x14,//--set(0x10) disable
    0xA4,// Disable Entire Display On (0xa4/0xa5)
    0xA6,// Disable Inverse Display On (0xa6/a7) 
    0xAF,//--turn on oled panel
    
    0xAF,/*display ON*/ 
	};
	
	GPIO_InitTypeDef GPIO_Initure;               
  __HAL_RCC_GPIOB_CLK_ENABLE();  
	
#ifdef USE_SPI_DRIVER
	SPI_Initialize();
	
	//ʹ��SPI Driver �������ʱ������MISO��PA6��Ϊ��λ�ź�GPIO���
  GPIO_Initure.Pin=GPIO_PIN_11;           //PA6 OLED_RST
#else
    GPIO_Initure.Pin= GPIO_PIN_12 |GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
#endif
  GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  
  GPIO_Initure.Pull=GPIO_PULLUP;        
  GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;            
  HAL_GPIO_Init(GPIOB,&GPIO_Initure);   
	
  GPIO_Initure.Pin=GPIO_PIN_14 ;           //PC5  OLED_RS
  GPIO_Initure.Pull=GPIO_PULLDOWN;        
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);   
	
	OLED_RST(0);				//��ʾ  ϵͳ��λ
	osDelay(100);
	OLED_RST(1); 
	osDelay(100);
	
	OLED_RS(0);	 				//���뷢����״̬  0,����;1,����
	SPI_WriteBuf(init_data,sizeof(init_data));

	OLED_Clear();
  mutex_OLED = osSemaphoreNew(1, 1, NULL);
}  

void OLED_SPI_Test (void *argument)
{
  uint8_t t;

//	OLED_Init();				//��ʼ��OLED

	OLED_DrawFont_GBK16(0,0,"���������ѧԺ",0);	
	OLED_DrawFont_GBK16(0,16,"2020��7��28��",1);	
	OLED_ShowString(102,22,"OK",24,1);  
	OLED_ShowString(12,36, "0.96' OLED",16,1);  
 	OLED_ShowString(0,52,"ASCII:",12,1);  
 	OLED_ShowString(64,52,"CODE:",12,1);
//  OLED_Clear();

	OLED_Refresh_Gram();		//������ʾ��OLED	 
	
	t=' ';  
	while(1) 
	{		
		OLED_ShowChar(36,52,t,12,1);//��ʾASCII�ַ�	
		OLED_ShowNum(96,52,t,3,12);	//��ʾASCII�ַ�����ֵ    
		OLED_Refresh_Gram();        //������ʾ��OLED
		t++;
		if(t>'~')t=' ';  
		osDelay(500);
//    OLED_Clear();
	}

}
