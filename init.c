
		// Inicializiruyem GPIO
		bcm2835_gpio_fsel(ADDR_A0, BCM2835_GPIO_FSEL_OUTP);
		bcm2835_gpio_write(ADDR_A0, HIGH); // close SPI communication
		bcm2835_gpio_fsel(ADDR_A1, BCM2835_GPIO_FSEL_OUTP);
		bcm2835_gpio_write(ADDR_A1, HIGH); // close SPI communication
		bcm2835_gpio_fsel(ADDR_A2, BCM2835_GPIO_FSEL_OUTP);
		bcm2835_gpio_write(ADDR_A2, HIGH); // close SPI communication
		bcm2835_gpio_fsel(ADDR_A3, BCM2835_GPIO_FSEL_OUTP);
		bcm2835_gpio_write(ADDR_A3, HIGH); // close SPI communication
		bcm2835_gpio_fsel(ADDR_A4, BCM2835_GPIO_FSEL_OUTP);
		bcm2835_gpio_write(ADDR_A4, HIGH); // close SPI communication
		
		// Inicializiruyem SPI
		bcm2835_spi_begin();
		bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);		// DEFAULT
		bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);						// DEFAULT
		bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);		// KHz
		bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);					// DEFAULT
		//bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);		// DEFAULT

		//**************************************
		address_a = (uint8_t)0x0;              // select address ***************
		//*************************************
