


#include <ELECHOUSE_CC1101.h>
#include <Arduino.h>
#include <SPI.h>

/****************************************************************/
#define 	WRITE_BURST     	0x40						//write burst
#define 	READ_SINGLE     	0x80						//read single
#define 	READ_BURST      	0xC0						//read burst
#define 	BYTES_IN_RXFIFO     0x7F  						//byte number in RXfifo

/****************************************************************/
byte PaTabel[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};
#ifdef ESP32
	SPIClass SPI_T(HSPI);
#endif
/****************************************************************
*FUNCTION NAME:SpiInit
*FUNCTION     :spi communication initialization
*INPUT        :none
*OUTPUT       :none
****************************************************************/

static const int spiClk = 1000000;

void ELECHOUSE_CC1101::SpiInit(void)
{
	#ifdef ESP32
	  // initialize the SPI pins
	  pinMode(SCK_PIN, OUTPUT);
	  pinMode(MOSI_PIN, OUTPUT);
	  pinMode(MISO_PIN, INPUT);
	  pinMode(SS_PIN, OUTPUT);
    #elif ESP8266

	#endif

  // enable SPI Master, MSB, SPI mode 0, FOSC/4
  // SpiMode(0);
}
/****************************************************************
*FUNCTION NAME:SpiMode
*FUNCTION     :set spi mode
*INPUT        :        config               mode
			   (0<<CPOL) | (0 << CPHA)		 0
			   (0<<CPOL) | (1 << CPHA)		 1
			   (1<<CPOL) | (0 << CPHA)		 2
			   (1<<CPOL) | (1 << CPHA)		 3
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SpiMode(byte config)
{
  // byte tmp;

  // // enable SPI master with configuration byte specified
  // SPCR = 0;
  // SPCR = (config & 0x7F) | (1<<SPE) | (1<<MSTR);
  // tmp = SPSR;
  // tmp = SPDR;
}

/****************************************************************
*FUNCTION NAME:SpiTransfer
*FUNCTION     :spi transfer
*INPUT        :value: data to send
*OUTPUT       :data to receive
****************************************************************/
byte ELECHOUSE_CC1101::SpiTransfer(byte value)
{
  // SPDR = value;
  // while (!(SPSR & (1<<SPIF))) ;
  // return SPDR;
}

/****************************************************************
*FUNCTION NAME: GDO_Set()
*FUNCTION     : set GDO0,GDO2 pin
*INPUT        : none
*OUTPUT       : none
****************************************************************/
void ELECHOUSE_CC1101::GDO_Set (void)
{
	pinMode(GDO0,INPUT);
	// pinMode(GDO2, INPUT);
}

/****************************************************************
*FUNCTION NAME:Reset
*FUNCTION     :CC1101 reset //details refer datasheet of CC1101/CC1100//
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::Reset (void)
{
  #ifdef ESP8266 	
  SpiStrobe(CC1101_SRES);
  delay(10);
  //                     // Deselect CC1101
  // delayMicroseconds(5);
  // spi.begin();                      // Select CC1101
  // delayMicroseconds(10);
  //                     // Deselect CC1101
  // delayMicroseconds(41);
  // spi.begin();                      // Select CC1101

  // delay(10);                          // Wait until MISO goes low
  // spi.transfer(CC1101_SRES);                // Send reset command strobe
  // delay(10);                          // Wait until MISO goes low

  //                     // Deselect CC1101
  #elif ESP32
	  SpiStrobe(CC1101_SRES);
	  delay(10);
  #endif
}

/****************************************************************
*FUNCTION NAME:Init
*FUNCTION     :CC1101 initialization
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::Init(void)
{
	#ifdef ESP32
	pinMode(SS_PIN, OUTPUT);
	SPI_T.begin(SCK_PIN,MISO_PIN,MOSI_PIN,SS_PIN);				    //spi initialization  //CLK,MISO,MOIS,SS
	#elif ESP8266
		spi.begin();
		spi.setFrequency(spiClk);
		delay(10);  /* begin SPI */
		Serial.println("ESP8266 found");
	#endif
	GDO_Set();										//GDO set
	Reset();										//CC1101 reset
	RegConfigSettings(F_433);						//CC1101 register config
	SpiWriteBurstReg(CC1101_PATABLE,PaTabel,8);		//CC1101 PATABLE config
}

/****************************************************************
*FUNCTION NAME:Init
*FUNCTION     :CC1101 initialization
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::Init(byte f)
{
	#ifdef ESP32
		pinMode(SS_PIN, OUTPUT);
		SPI_T.begin(SCK_PIN,MISO_PIN,MOSI_PIN,SS_PIN);										//spi initialization
	#elif ESP8266
		spi.begin();
		pinMode(SS_PIN, OUTPUT);
		Serial.println("ESP8266 found");
	#endif

	GDO_Set();										//GDO set
	Reset();										//CC1101 reset
	RegConfigSettings(f);							//CC1101 register config
	// SpiWriteBurstReg(CC1101_PATABLE,PaTabel,8);		//CC1101 PATABLE config
	// SpiWriteReg(CC1101_PATABLE, 0x60);
	SpiWriteBurstReg(CC1101_PATABLE,PaTabel,8);		//CC1101 PATABLE config
}


/****************************************************************
*FUNCTION NAME:SpiWriteReg
*FUNCTION     :CC1101 write data to register
*INPUT        :addr: register address; value: register value
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SpiWriteReg(byte addr, byte value)
{
	   // Serial.println("...");
	   // Serial.print(addr,HEX);
	   // Serial.print(" ");
	   // Serial.println(value,HEX);
	#ifdef ESP32
		SPI_T.beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
		digitalWrite(SS_PIN, LOW);
		SPI_T.transfer(addr);
		SPI_T.transfer(value);
		
		SPI_T.endTransaction();
	#elif ESP8266
		digitalWrite(SS_PIN, LOW);
		spi.transfer(addr);
		spi.transfer(value);
			
	#endif
		digitalWrite(SS_PIN, HIGH);
	// SpiTransfer(addr);
	// SpiTransfer(value);
}

/****************************************************************
*FUNCTION NAME:SpiWriteBurstReg
*FUNCTION     :CC1101 write burst data to register
*INPUT        :addr: register address; buffer:register value array; num:number to write
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SpiWriteBurstReg(byte addr, byte *buffer, byte num)
{
	byte i, temp;
	temp = addr | WRITE_BURST;
	#ifdef ESP32
	SPI_T.beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
	digitalWrite(SS_PIN, LOW);
	SPI_T.transfer(temp);
	#elif ESP8266
	digitalWrite(SS_PIN, LOW);
		spi.transfer(temp);
	#endif
	// Serial.print(temp,HEX);
	// Serial.print(" ");
    for (i = 0; i < num; i++)
 	{
 		#ifdef ESP32
 			SPI_T.transfer(buffer[i]);
 		#elif ESP8266
 			spi.transfer(buffer[i]);
 		#endif
 	    // Serial.print(buffer[i],HEX);
	    // Serial.print(" ");
    }
    // Serial.println();
    #ifdef ESP32
	   
	    SPI_T.endTransaction();
    #elif ESP8266
    	
    #endif
	digitalWrite(SS_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:SpiStrobe
*FUNCTION     :CC1101 Strobe
*INPUT        :strobe: command; //refer define in CC1101.h//
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SpiStrobe(byte strobe)
{
	#ifdef ESP32
		SPI_T.beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
		digitalWrite(SS_PIN, LOW);
		SPI_T.transfer(strobe);
		
	    SPI_T.endTransaction();
	#elif ESP8266
	    digitalWrite(SS_PIN, LOW);
		spi.transfer(strobe);
		
	#endif
		digitalWrite(SS_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:SpiReadReg
*FUNCTION     :CC1101 read data from register
*INPUT        :addr: register address
*OUTPUT       :register value
****************************************************************/
byte ELECHOUSE_CC1101::SpiReadReg(byte addr) 
{

	byte temp, value;
    temp = addr | READ_BURST;
    #ifdef ESP32
	    SPI_T.beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
		digitalWrite(SS_PIN, LOW);
		SPI_T.transfer(temp);
		// SpiTransfer(temp);
		value =SPI_T.transfer(0);
		
	    SPI_T.endTransaction();
    #elif ESP8266
	    // spi.begin();
	    digitalWrite(SS_PIN, LOW);
	    spi.transfer(temp);
	    // delay(10);
		value = spi.transfer(0x00);
		
    #endif
		digitalWrite(SS_PIN, HIGH);
	return value;
}

/****************************************************************
*FUNCTION NAME:SpiReadBurstReg
*FUNCTION     :CC1101 read burst data from register
*INPUT        :addr: register address; buffer:array to store register value; num: number to read
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SpiReadBurstReg(byte addr, byte *buffer, byte num)
{
	byte i,temp;

	temp = addr | READ_BURST;
	#ifdef ESP32
		SPI_T.beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
		digitalWrite(SS_PIN, LOW);
		SPI_T.transfer(temp);
		for(i=0;i<num;i++)
		{
			buffer[i]=SPI_T.transfer(0);
		}
	    SPI_T.endTransaction();
	#elif ESP8266
	   	digitalWrite(SS_PIN, LOW);
		spi.transfer(temp);
		for(i=0;i<num;i++)
		{
			buffer[i]=spi.transfer(0);
		}
		
	#endif
		digitalWrite(SS_PIN, HIGH);
		// SpiTransfer(temp);

	
}

/****************************************************************
*FUNCTION NAME:SpiReadStatus
*FUNCTION     :CC1101 read status register
*INPUT        :addr: register address
*OUTPUT       :status value
****************************************************************/
byte ELECHOUSE_CC1101::SpiReadStatus(byte addr) 
{
	byte value,temp;
	temp = addr | READ_BURST;
	#ifdef ESP32
		SPI_T.beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
		digitalWrite(SS_PIN, LOW);
		SPI_T.transfer(temp);
		// SpiTransfer(temp);
		// value=SpiTransfer(0);
		value = SPI_T.transfer(0);
		digitalWrite(SS_PIN, HIGH);
		SPI_T.endTransaction();
	#elif ESP8266
		digitalWrite(SS_PIN, LOW);
		spi.transfer(temp);
		value = spi.transfer(0);
		
	#endif
		digitalWrite(SS_PIN, HIGH);
	return value;
}

/****************************************************************
*FUNCTION NAME:RegConfigSettings
*FUNCTION     :CC1101 register config //details refer datasheet of CC1101/CC1100//
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::RegConfigSettings(byte f) 
{
    SpiWriteReg(CC1101_FSCTRL1,  0x08);
    SpiWriteReg(CC1101_FSCTRL0,  0x00);
	
    switch(f)
    {
      case F_868:
      	SpiWriteReg(CC1101_FREQ2,    F2_868);
      	SpiWriteReg(CC1101_FREQ1,    F1_868);
      	SpiWriteReg(CC1101_FREQ0,    F0_868);
        break;
      case F_915:
        SpiWriteReg(CC1101_FREQ2,    F2_915);
        SpiWriteReg(CC1101_FREQ1,    F1_915);
        SpiWriteReg(CC1101_FREQ0,    F0_915);
        break;
	  case F_433:
        SpiWriteReg(CC1101_FREQ2,    F2_433);
        SpiWriteReg(CC1101_FREQ1,    F1_433);
        SpiWriteReg(CC1101_FREQ0,    F0_433);
        break;
	  default: // F must be set
	  	break;
	}
	SpiWriteReg(CC1101_IOCFG0,   0x06);  	//asserts when sync word has been sent/received, and de-asserts at the end of the packet 
    SpiWriteReg(CC1101_MDMCFG4,  0x5B);
    SpiWriteReg(CC1101_MDMCFG3,  0xF8);
    SpiWriteReg(CC1101_MDMCFG2,  0x03);
    SpiWriteReg(CC1101_MDMCFG1,  0x22);
    SpiWriteReg(CC1101_MDMCFG0,  0xF8);
    SpiWriteReg(CC1101_CHANNR,   0x00);
    SpiWriteReg(CC1101_DEVIATN,  0x47);
    SpiWriteReg(CC1101_FREND1,   0xB6);
    SpiWriteReg(CC1101_FREND0,   0x10);
    SpiWriteReg(CC1101_MCSM0 ,   0x18);
    SpiWriteReg(CC1101_FOCCFG,   0x1D);
    SpiWriteReg(CC1101_BSCFG,    0x1C);
    SpiWriteReg(CC1101_AGCCTRL2, 0xC7);
	SpiWriteReg(CC1101_AGCCTRL1, 0x00);
    SpiWriteReg(CC1101_AGCCTRL0, 0xB2);
    SpiWriteReg(CC1101_FSCAL3,   0xEA);
	SpiWriteReg(CC1101_FSCAL2,   0x2A);
	SpiWriteReg(CC1101_FSCAL1,   0x00);
    SpiWriteReg(CC1101_FSCAL0,   0x11);
    SpiWriteReg(CC1101_FSTEST,   0x59);
    SpiWriteReg(CC1101_TEST2,    0x81);
    SpiWriteReg(CC1101_TEST1,    0x35);
    SpiWriteReg(CC1101_TEST0,    0x09);
    SpiWriteReg(CC1101_IOCFG2,   0x0B); 	//serial clock.synchronous to the data in synchronous serial mode
    
    SpiWriteReg(CC1101_PKTCTRL1, 0x04);		//two status bytes will be appended to the payload of the packet,including RSSI LQI and CRC OK
											//No address check
    SpiWriteReg(CC1101_PKTCTRL0, 0x05);		//whitening off;CRC Enable£»variable length packets, packet length configured by the first byte after sync word
    SpiWriteReg(CC1101_ADDR,     0x00);		//address used for packet filtration.
    SpiWriteReg(CC1101_PKTLEN,   0x3D); 	//61 bytes max length
}

/****************************************************************
*FUNCTION NAME:SendData
*FUNCTION     :use CC1101 send data
*INPUT        :txBuffer: data array to send; size: number of data to send, no more than 61
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SendData(byte *txBuffer,byte size)
{
	// Serial.println("Hello1");
	SpiWriteReg(CC1101_TXFIFO,size);
	SpiWriteBurstReg(CC1101_TXFIFO,txBuffer,size);			//write data to send
	SpiStrobe(CC1101_STX);									//start send	
    
    while (!digitalRead(GDO0)){delay(1);}								// Wait for GDO0 to be set -> sync transmitted  
    // Serial.println("Hello2");
    while (digitalRead(GDO0)){delay(1);}								// Wait for GDO0 to be cleared -> end of packet
	// Serial.println("Hello3");
	SpiStrobe(CC1101_SFTX);									//flush TXfifo
	Serial.println(">>");
}

/****************************************************************
*FUNCTION NAME:SetReceive
*FUNCTION     :set CC1101 to receive state
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SetReceive(void)
{
	SpiStrobe(CC1101_SRX);
}

/****************************************************************
*FUNCTION NAME:CheckReceiveFlag
*FUNCTION     :check receive data or not
*INPUT        :none
*OUTPUT       :flag: 0 no data; 1 receive data 
****************************************************************/
byte ELECHOUSE_CC1101::CheckReceiveFlag(void)
{
	if(digitalRead(GDO0))			//receive data
	{
		while (digitalRead(GDO0)){delay(1);}
		return 1;
	}
	else							// no data
	{
		return 0;
	}
}


/****************************************************************
*FUNCTION NAME:ReceiveData
*FUNCTION     :read data received from RXfifo
*INPUT        :rxBuffer: buffer to store data
*OUTPUT       :size of data received
****************************************************************/
byte ELECHOUSE_CC1101::ReceiveData(byte *rxBuffer)
{
	byte size;
	byte status[2];

	if(SpiReadStatus(CC1101_RXBYTES) & BYTES_IN_RXFIFO)
	{
		size = SpiReadReg(CC1101_RXFIFO);
		SpiReadBurstReg(CC1101_RXFIFO,rxBuffer,size);
		SpiReadBurstReg(CC1101_RXFIFO,status,2);
		SpiStrobe(CC1101_SFRX);
		return size;
	}
	else
	{
		SpiStrobe(CC1101_SFRX);
		return 0;
	}
	
}

ELECHOUSE_CC1101 ELECHOUSE_cc1101;




