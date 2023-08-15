#ifndef _SOFT_SPI_CONFIG_H_
#define _SOFT_SPI_CONFIG_H_


/*
    实现了自己的通用的，当前只实现了spi master的读写操作



    使用流程:
        1)声明spi_slaver_operation_struct 结构体，并调用spi_master_opmaster_eration_struct_init 初始化
        2）进行读写操作



*/

#include <stdint.h>



//硬件还是软件spi
#define SOFT_SPI                    1
#define HANDLE_SPI                  0

#define SPI_SLAVER_MAX_NUM          255




/// @brief spi master 1对1操作结构体
typedef struct _spi_slaver_operation_struct{

    uint8_t spi_mode;  //spi的模式
    void(*spi_cs_set_status)(uint8_t);//spi的片选信号
#if SOFT_SPI == 1 && HANDLE_SPI == 0
    
    void(*spi_sclk_set_status)(uint8_t);
    uint8_t(*spi_miso_get_status)(void);
    void(*spi_mosi_set_status)(uint8_t);
    void(*spi_delay_us)(uint16_t);
#endif

#if SOFT_SPI == 0 && HANDLE_SPI == 1
    void(*spi_init)(void);
    void(*spi_send_and_receive)(uint8_t*,uint8_t,uint8_t*);

#endif
}spi_slaver_operation_struct;


/// @brief  spi master 1对多 操作结构体
typedef struct _spi_masters_operation_struct{
    uint8_t spi_slaver_nums; //当前主站链接的从站数量
    spi_slaver_operation_struct * spi_slavers;//从站的操作结构体数组
    uint8_t spi_slaver_nums_avaiable;
}spi_masters_operation_struct;



#if SOFT_SPI == 1

/// @brief spi master-slaver 的初始化函数,只初始化一个从站
/// @param sos spi master的操作结构体
/// @param cpol_mode 决定spi master模式的变量
/// @param cpha_mode 决定spi master模式的变量2
/// @param spi_cs_set_status 设置spi的片选信号
/// @param spi_mosi_set_status 设置spi master的mosi 
/// @param spi_sclk_set_status 设置spi master的sclk
/// @param spi_miso_get_status 设置spi master的mios
/// @param spi_delay_us 设置spi master的延时函数
void spi_slaver_operation_struct_init(spi_slaver_operation_struct *sos,uint8_t cpol_mode,uint8_t cpha_mode,
                                                void *spi_cs_set_status,
                                                void *spi_mosi_set_status,
                                                void *spi_sclk_set_status,
                                                uint8_t *spi_miso_get_status,
                                                void *spi_delay_us);

#endif /*SOFT_SPI*/



/// @brief 初始化一主多从 spi操作结构体，
/// @param master 主站结构体
/// @param slaver_num 从站数量
void spi_masters_init(spi_masters_operation_struct *master,uint8_t slaver_num);



/// @brief 向 spi master 中注册 slaver ,必须确保slaver是已经初始化后的
/// @param master 
/// @param slaver 
void spi_masters_register_slaver(spi_masters_operation_struct* master , spi_slaver_operation_struct *slaver);


/// @brief 向指定从机写入数据
/// @param master 
/// @param  slaver_num 从机编号
/// @param data        写入数据   
/// @param data_length 数据长度
/// @param read_data   读取数据
void spi_masters_write_data(spi_masters_operation_struct *master,uint8_t slaver_num,uint8_t *write_data,uint8_t data_length,uint8_t *read_data);

void spi_masters_cs(spi_masters_operation_struct *master,uint8_t slaver_num,uint8_t cs_status);






#endif /*_SOFT_SPI_CONFIG_H_*/
