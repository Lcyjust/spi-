# spi-
## 实现了通用的spi库，支持软件spi与硬件spi,支持一主多从
## 实现了sd库

### 使用流程,伪代码
example:
   spi_slaver_operation_struct slaver;
    spi_slaver_operation_struct_init(&slaver,1,1,cs,mosi,sck,miso,delay_us);
    SD_SPI_init(&slaver);
    MX_FATFS_Init();
    f_mount(&fs,"0",1);
    f_open(&file, "/file.c", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    f_write(&file,buff,strlen((const char*)buff),&data);
    f_close(&file);  //关闭文件
