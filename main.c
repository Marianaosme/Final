// 3
// a)
void spi_bus_init(void)
{
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000,  
        .mode = 0,                   
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 1
    };

    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(SPI2_HOST, &devcfg, &mcp4132);
}


//b)

void mcp4132_write_register(uint8_t address, uint8_t value)
{
    spi_transaction_t t;

    uint8_t tx_data[2];
    tx_data[0] = (address << 4);
    tx_data[1] = value;

    memset(&t, 0, sizeof(t));
    t.length = 16;   
    t.tx_buffer = tx_data;

    spi_device_transmit(mcp4132, &t);
}



//c)

uint16_t mcp4132_read_register(uint8_t address)
{
    spi_transaction_t t;
    uint8_t tx_data[2];
    uint8_t rx_data[2];

    tx_data[0] = (address << 4) | 0x0C;
    tx_data[1] = 0x00;

    memset(&t, 0, sizeof(t));
    t.length = 16;
    t.tx_buffer = tx_data;
    t.rx_buffer = rx_data;

    spi_device_transmit(mcp4132, &t);
    uint16_t value = ((rx_data[0] & 0x03) << 8) | rx_data[1];

    return value;
}

//4
//a)
void mcp4132_set_wiper(uint8_t n)
{
    if (n > 128)
    {
        return;
    }

    mcp4132_write_register(0x00, n);
}

//b) 
void mcp4132_set_cutoff_frequency(float fc)
{
    if (fc <= 0)
    {
        return;
    }
    float Rwb = 1.0f / (2.0f * M_PI * fc * C_FILTER);

    uint8_t N = (uint8_t)((Rwb * 128.0f) / RAB);

    if (N > 128)
    {
        N = 128;
    }

    mcp4132_set_wiper(N);
} 
