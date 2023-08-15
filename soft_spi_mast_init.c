#include "soft_spi_config.h"



/// @brief spi的模式
enum _spi_mode{
    CPOL_0_CPHA_0_MODE_0=0,
    CPOL_0_CPHA_1_MODE_0=1,
    CPOL_1_CPHA_0_MODE_0=2,
    CPOL_1_CPHA_0_MODE_1=3,
};




/// @brief spi 控制的io输出的电瓶高低

enum _status
{
    SPI_PIN_STATUS_DOWN=0,
    SPI_PIN_STATUS_UP,
};





void spi_slaver_operation_struct_init(spi_slaver_operation_struct *sos,uint8_t cpol_mode,uint8_t cpha_mode,
                                                void*spi_cs_set_status,
                                                void *spi_mosi_set_status,
                                                void*spi_sclk_set_status,
                                                void *spi_miso_get_status,
                                                void *spi_delay_us)
{
   
    if(cpol_mode==0&&cpha_mode==0)
        sos->spi_mode = CPOL_0_CPHA_0_MODE_0;
    else if(cpol_mode==0&&cpha_mode==1)
        sos->spi_mode = CPOL_0_CPHA_1_MODE_0;
    else if(cpol_mode==1&&cpha_mode==0)
        sos->spi_mode = CPOL_1_CPHA_0_MODE_0;
    else if(cpol_mode==1&&cpha_mode==1)
        sos->spi_mode = CPOL_1_CPHA_0_MODE_1;    


#if SOFT_SPI == 1 
    sos->spi_cs_set_status = spi_cs_set_status;
    sos->spi_mosi_set_status = spi_mosi_set_status;
    sos->spi_sclk_set_status = spi_sclk_set_status;
    sos->spi_miso_get_status = spi_miso_get_status;
    sos->spi_delay_us = spi_delay_us;
#endif
}


#if SOFT_SPI == 1

/* CPOL = 0, CPHA = 0, MSB first */
static uint8_t soft_spi_master_rw_mode0( spi_slaver_operation_struct *sos, uint8_t write_dat )
{
    uint8_t i, read_dat;
    for( i = 0; i < 8; i++ )
    {
        if( write_dat & 0x80 )
            (*sos->spi_mosi_set_status)(SPI_PIN_STATUS_UP); 
        else                    
            (*sos->spi_mosi_set_status)(SPI_PIN_STATUS_DOWN); 
        write_dat <<= 1;
        (*sos->spi_delay_us)(1);
        (*sos->spi_sclk_set_status)(SPI_PIN_STATUS_UP);  
        read_dat <<= 1;  
        if((*sos->spi_miso_get_status)() ) 
            read_dat++; 
		(*sos->spi_delay_us)(1);
        (*sos->spi_sclk_set_status)(SPI_PIN_STATUS_DOWN);  
        (*sos->spi_delay_us)(1);
    }
	
    return read_dat;
}






/* CPOL=0，CPHA=1, MSB first */
static uint8_t soft_spi_master_rw_mode1( spi_slaver_operation_struct *sos, uint8_t byte) 
{
    uint8_t i,Temp=0;
 
	for(i=0;i<8;i++)     // 循环8次
	{
		(*sos->spi_sclk_set_status)(SPI_PIN_STATUS_UP);     //拉高时钟
		if(byte&0x80)
        {
			(*sos->spi_mosi_set_status)(SPI_PIN_STATUS_UP);  //若最到位为高，则输出高
        }
		else      
		{
			 (*sos->spi_mosi_set_status)(SPI_PIN_STATUS_DOWN); ;   //若最到位为低，则输出低
		}
		byte <<= 1;     // 低一位移位到最高位
		(*sos->spi_delay_us)(1);
		(*sos->spi_sclk_set_status)(SPI_PIN_STATUS_DOWN);      //拉低时钟
		Temp <<= 1;     //数据左移
 
		if((*sos->spi_miso_get_status)())
			Temp++;     //若从从机接收到高电平，数据自加一
		(*sos->spi_delay_us)(1);
 
	}
	return (Temp);     //返回数据
}


/* CPOL=1，CPHA=0, MSB first */
static uint8_t soft_spi_master_rw_mode2( spi_slaver_operation_struct *sos, uint8_t byte) 
{
    uint8_t i,Temp=0;
 
	for(i=0;i<8;i++)     // 循环8次
	{
		if(byte&0x80)
        {
			(*sos->spi_mosi_set_status)(SPI_PIN_STATUS_UP);  //若最到位为高，则输出高
        }
		else      
		{
			 (*sos->spi_mosi_set_status)(SPI_PIN_STATUS_DOWN); ;   //若最到位为低，则输出低
		}
		byte <<= 1;     // 低一位移位到最高位
		(*sos->spi_delay_us)(1);
		(*sos->spi_sclk_set_status)(SPI_PIN_STATUS_DOWN);;     //拉低时钟
		Temp <<= 1;     //数据左移
 
		if((*sos->spi_miso_get_status)())
			Temp++;     //若从从机接收到高电平，数据自加一
		(*sos->spi_delay_us)(1);
		(*sos->spi_sclk_set_status)(SPI_PIN_STATUS_UP);     //拉高时钟
		
	}
	return (Temp);     //返回数据
}
 
 
/* CPOL = 1, CPHA = 1, MSB first */
static uint8_t soft_spi_master_rw_mode3(  spi_slaver_operation_struct *sos, uint8_t write_dat )
{
    uint8_t i, read_dat;
    for( i = 0; i < 8; i++ )
    {
		(*sos->spi_sclk_set_status)(SPI_PIN_STATUS_DOWN);; 
        if( write_dat & 0x80 )
            (*sos->spi_mosi_set_status)(SPI_PIN_STATUS_UP);  
        else                    
            (*sos->spi_mosi_set_status)(SPI_PIN_STATUS_DOWN); ;  
        write_dat <<= 1;
        (*sos->spi_delay_us)(1);
        (*sos->spi_sclk_set_status)(SPI_PIN_STATUS_UP); 
        read_dat <<= 1;  
        if( (*sos->spi_miso_get_status)() ) 
            read_dat++; 
        (*sos->spi_delay_us)(1);
    }
    return read_dat;
}
#endif

/// @brief Spi读写模式1
/// @param sos 
/// @param write_dat 
/// @return 
static uint8_t soft_spi_master_rw_one_Bit(  spi_slaver_operation_struct *sos, uint8_t write_dat )
{
    uint8_t read_data;
    #if SOFT_SPI == 1 && HANDLE_SPI ==0
    switch (sos->spi_mode)
    {
    case CPOL_0_CPHA_0_MODE_0:
        read_data = soft_spi_master_rw_mode0(sos,write_dat);
        break;
    case CPOL_0_CPHA_1_MODE_0:
        read_data = soft_spi_master_rw_mode1(sos,write_dat);
        break;
    case CPOL_1_CPHA_0_MODE_0:
        read_data = soft_spi_master_rw_mode2(sos,write_dat);
        break;
    case CPOL_1_CPHA_0_MODE_1:
        read_data = soft_spi_master_rw_mode3(sos,write_dat);
        break;
    default:
        break;
    }
    #elif #if SOFT_SPI == 0 && HANDLE_SPI == 1
        (*sos->spi_send_and_receive)(write_dat,read_data);

    #endif


    return read_data ;
      
}

/// @brief 软件spi master 读写函数
/// @param sos spi master的操作结构体
/// @param write_datas 需要写入的数据
/// @param data_lenght 数据长度
/// @param read_data 需要读取的数据
/// @return //返回的数据
static soft_spi_master_rw(spi_slaver_operation_struct *sos, uint8_t* write_datas,uint8_t data_length,uint8_t* read_data)
{
    for(int i=0;i<data_length;i++)
    {
        read_data[i]=soft_spi_master_rw_one_Bit(sos,write_datas[i];)
    }
}


void spi_masters_init(spi_masters_operation_struct *master,uint8_t slaver_num)
{
    master->spi_slaver_nums = slaver_num;

    master->spi_slavers = (spi_slaver_operation_struct *)malloc( sizeof(spi_slaver_operation_struct)*slaver_num );

    master->spi_slaver_nums_avaiable = slaver_num;
}

void spi_masters_register_slaver(spi_masters_operation_struct* master , spi_slaver_operation_struct *slaver)
{
    if(master->spi_slaver_nums_avaiable < 0)
        return;//当前spi 支持的slaver以达到最大值
    
   

    uint8_t num = master->spi_slaver_nums - master->spi_slaver_nums_avaiable;
    master->spi_slavers[num] = &slaver;

     master->spi_slaver_nums_avaiable--;
    
}

void spi_masters_write_data(spi_masters_operation_struct *master,uint8_t slaver_num,uint8_t *write_data,uint8_t data_length,uint8_t *read_data)
{
    if(master->spi_slavers == NULL||master->spi_slaver_nums<=0)//master未初始化错位
        return ;
    if(master->spi_slaver_nums<slaver_num)//slaver—_num 越界错误
        return;

    spi_slaver_operation_struct temp = master->spi_slavers[slaver_num]; 
#if SOFT_SPI == 1 && HANDLE_SPI == 0

    
    soft_spi_master_rw(&temp,write_data,data_length,read_data);
    
#else

    temp.spi_send_and_receive(write_data,data_length,read_data)

#endif

}


void spi_masters_cs(spi_masters_operation_struct *master,uint8_t slaver_num,uint8_t cs_status)
{
     if(master->spi_slavers == NULL||master->spi_slaver_nums<=0)//master未初始化错位
        return ;
    if(master->spi_slaver_nums<slaver_num)//slaver—_num 越界错误
        return;

    spi_slaver_operation_struct temp = master->spi_slavers[slaver_num];

    temp.spi_cs_set_status(cs_status);


}