#include "SPI_FIFO_t3.h"

SPI_FIFO_t3::SPI_FIFO_t3()
{

}

SPI_FIFO_t3::SPI_FIFO_t3(const uint8_t csPin,const uint8_t dcPin,const uint8_t mosiPin,const uint8_t sclkPin,const uint8_t misoPin)
{
	postInstance(csPin,dcPin,mosiPin,sclkPin,misoPin);
}

void SPI_FIFO_t3::postInstance(const uint8_t csPin,const uint8_t dcPin,const uint8_t mosiPin,const uint8_t sclkPin,const uint8_t misoPin)
{
	_cs = csPin;
	_dc = dcPin;
	_mosi = mosiPin;
	_miso = misoPin;
	_sclk = sclkPin;
}

bool SPI_FIFO_t3::begin(SPISettings settings,bool avoidInit)
{
	_spiSettings = settings;
	_initError = 0xFF;
#if defined(__MK20DX128__) || defined(__MK20DX256__)//Teensy 3.0 -> 3.2
	if ((_mosi == 11 || _mosi == 7) && (_sclk == 13 || _sclk == 14) && (_miso == 255 || _miso == 12 || _miso == 8)) {
		if (!avoidInit) SPI.begin();
		SPI.setMOSI(_mosi);
		if (_miso != 255) SPI.setMISO(_miso);
		SPI.setSCK(_sclk);
		if (_dc != 255){
			if (SPI.pinIsChipSelect(_cs,_dc)){
				_pcs_data = SPI.setCS(_cs);
				_pcs_command = _pcs_data | SPI.setCS(_dc);
			} else {
				if (SPI.pinIsChipSelect(_cs)) {
					bitClear(_initError,1);
				} else {
					bitClear(_initError,2);
				}
			}
		} else {
			if (SPI.pinIsChipSelect(_cs)){
				_pcs_data = 0;
				_pcs_command = _pcs_data | SPI.setCS(_cs);
			} else {
				bitClear(_initError,1);
			}
		}
	} else {
		bitClear(_initError,0);
	}
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)//Teensy 3.5 -> 3.6
	if ((_mosi == 11 || _mosi == 7) && (_sclk == 13 || _sclk == 14) && (_miso == 255 || _miso == 12 || _miso == 8)) {
		_spiBus = 0;
		if (!avoidInit) SPI.begin();
		SPI.setMOSI(_mosi);
		if (_miso != 255) SPI.setMISO(_miso);
		SPI.setSCK(_sclk);
		if (_dc != 255){
			if (SPI.pinIsChipSelect(_cs,_dc)){
				_pcs_data = SPI.setCS(_cs);
				_pcs_command = _pcs_data | SPI.setCS(_dc);
			} else {
				if (SPI.pinIsChipSelect(_cs)) {
					bitClear(_initError,1);
				} else {
					bitClear(_initError,2);
				}
			}
		} else {
			if (SPI.pinIsChipSelect(_cs)){
				_pcs_data = 0;
				_pcs_command = _pcs_data | SPI.setCS(_cs);
			} else {
				bitClear(_initError,1);
			}
		}
	} else if ((_mosi == 0 || _mosi == 21 || _mosi == 59 || _mosi == 61) && (_sclk == 20 || _sclk == 32 || _sclk == 60) && (_miso == 255 || _miso == 1 || _miso == 5 || _miso == 59 || _miso == 61)){
		_spiBus = 1;
		if (_mosi == _miso){
			bitClear(_initError,4);
		}
		if (!avoidInit) SPI1.begin();
		SPI1.setMOSI(_mosi);
		if (_miso != 255) SPI1.setMISO(_miso);
		SPI1.setSCK(_sclk);
		if (_dc != 255){//Only 1 pin can be handled in SPI1
			if (_cs != _mosi && _cs != _sclk && _cs != _miso && !SPI1.pinIsChipSelect(_cs)){//cs cannot use these pin since are reserved to dc,mosi,sclk and miso
				pinMode(_cs, OUTPUT);//handle cs separately
				digitalWriteFast(_cs,HIGH);//disable cs
				if (SPI1.pinIsChipSelect(_dc)) {
					_pcs_data = 0;
					_pcs_command = _pcs_data | SPI1.setCS(_dc);
				} else {
					bitClear(_initError,2);
				}
			} else {
				bitClear(_initError,3);
			}
		} else {
			if (SPI1.pinIsChipSelect(_cs)) {
				_pcs_data = 0;
				_pcs_command = _pcs_data | SPI1.setCS(_cs);
			} else {
				bitClear(_initError,1);
			}
		}
	} else if ((_mosi == 44 || _mosi == 52) && (_sclk == 46 || _sclk == 53) && (_miso == 255 || _miso == 45 || _miso == 51)){
		_spiBus = 2;
		if (!avoidInit) SPI2.begin();
		SPI2.setMOSI(_mosi);
		if (_miso != 255) SPI2.setMISO(_miso);
		SPI2.setSCK(_sclk);
		if (_dc != 255){//Only 1 pin can be handled in SPI2
			if (_cs != _mosi && _cs != _sclk && _cs != _miso && !SPI2.pinIsChipSelect(_cs)){//cs cannot use these pin since are reserved to dc,mosi,sclk and miso
				pinMode(_cs, OUTPUT);//handle cs separately
				digitalWriteFast(_cs,HIGH);//disable cs
				if (SPI2.pinIsChipSelect(_dc)) {
					_pcs_data = 0;
					_pcs_command = _pcs_data | SPI2.setCS(_dc);
				} else {
					bitClear(_initError,2);
				}
			} else {
				bitClear(_initError,3);
			}
		} else {
			if (SPI2.pinIsChipSelect(_cs)) {
				_pcs_data = 0;
				_pcs_command = _pcs_data | SPI1.setCS(_cs);
			} else {
				bitClear(_initError,1);
			}
		}
	} else {
		bitClear(_initError,0);
	}
#endif
	return _initError;
}

void SPI_FIFO_t3::setSpiSettings(SPISettings settings)
{
	_spiSettings = settings;
}

uint8_t SPI_FIFO_t3::getSPIbus(void)
{
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		return 0;
	#else
		return _spiBus;
	#endif
}


void SPI_FIFO_t3::startTransaction(void)
{
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.beginTransaction(_spiSettings);
	#else
		if (_spiBus == 0){
			SPI.beginTransaction(_spiSettings);
		} else if (_spiBus == 1){
			SPI1.beginTransaction(_spiSettings);
			if (_dc != 255) digitalWriteFast(_cs,LOW);
		} else {
			SPI2.beginTransaction(_spiSettings);
			if (_dc != 255) digitalWriteFast(_cs,LOW);
		}
	#endif
}

void SPI_FIFO_t3::endTransaction(void)
{
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.endTransaction();
	#else
		if (_spiBus == 0){
			SPI.endTransaction();
		} else if (_spiBus == 1){
			SPI1.endTransaction();
		} else {
			SPI2.endTransaction();
		}
	#endif
}


void SPI_FIFO_t3::waitFifoNotFull(void)
{
	uint32_t sr = 0;
	uint32_t tmp __attribute__((unused));
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		do {
			sr = KINETISK_SPI0.SR;
			if (sr & 0xF0) tmp = KINETISK_SPI0.POPR;// drain RX FIFO
		} while ((sr & (15 << 12)) > (3 << 12));//4Byte
	#else
		if (_spiBus == 0){
			do {
				sr = KINETISK_SPI0.SR;
				if (sr & 0xF0) tmp = KINETISK_SPI0.POPR;// drain RX FIFO
			} while ((sr & (15 << 12)) > (3 << 12));//4Byte
		} else if (_spiBus == 1){
			do {
				sr = KINETISK_SPI1.SR;
				if (sr & 0xF0) tmp = KINETISK_SPI1.POPR;
			} while ((sr & (15 << 12)) > (0 << 12));//1Byte
		} else {
			do {
				sr = KINETISK_SPI2.SR;
				if (sr & 0xF0) tmp = KINETISK_SPI2.POPR;
			} while ((sr & (15 << 12)) > (0 << 12));//1Byte
		}
	#endif
}

void SPI_FIFO_t3::waitFifoEmpty(void)
{
	uint32_t sr = 0;
	uint32_t tmp __attribute__((unused));
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		do {
			sr = KINETISK_SPI0.SR;
			if (sr & 0xF0) tmp = KINETISK_SPI0.POPR;  // drain RX FIFO
		} while ((sr & 0xF0F0) > 0);         // wait both RX & TX empty
	#else
		if (_spiBus == 0){
			do {
				sr = KINETISK_SPI0.SR;
				if (sr & 0xF0) tmp = KINETISK_SPI0.POPR;  // drain RX FIFO
			} while ((sr & 0xF0F0) > 0);         // wait both RX & TX empty
		} else if (_spiBus == 1){
			do {
				sr = KINETISK_SPI1.SR;
				if (sr & 0xF0) tmp = KINETISK_SPI1.POPR;  // drain RX FIFO
			} while ((sr & 0xF0F0) > 0);         // wait both RX & TX empty
		} else {
			do {
				sr = KINETISK_SPI2.SR;
				if (sr & 0xF0) tmp = KINETISK_SPI2.POPR;  // drain RX FIFO
			} while ((sr & 0xF0F0) > 0);         // wait both RX & TX empty
		}
	#endif
}

void SPI_FIFO_t3::waitTransmitComplete(uint32_t mcr)
{
	uint32_t tmp __attribute__((unused));
	uint32_t sr = 0;
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		while (1) {
			sr = KINETISK_SPI0.SR;
			if (sr & SPI_SR_EOQF) break;  			   // wait for last transmit
			if (sr & 0xF0) tmp = KINETISK_SPI0.POPR;  // drain RX FIFO
		}
		KINETISK_SPI0.SR = SPI_SR_EOQF;
		SPI0_MCR = mcr;
		while (KINETISK_SPI0.SR & 0xF0) {tmp = KINETISK_SPI0.POPR;}
	#else
		if (_spiBus == 0){
			while (1) {
				sr = KINETISK_SPI0.SR;
				if (sr & SPI_SR_EOQF) break;  			   // wait for last transmit
				if (sr & 0xF0) tmp = KINETISK_SPI0.POPR;  // drain RX FIFO
			}
			KINETISK_SPI0.SR = SPI_SR_EOQF;
			SPI0_MCR = mcr;
			while (KINETISK_SPI0.SR & 0xF0) {tmp = KINETISK_SPI0.POPR;}
		} else if (_spiBus == 1){
			while (1) {
				sr = KINETISK_SPI1.SR;
				if (sr & SPI_SR_EOQF) break;  			   // wait for last transmit
				if (sr & 0xF0) tmp = KINETISK_SPI1.POPR;  // drain RX FIFO
			}
			KINETISK_SPI1.SR = SPI_SR_EOQF;
			SPI1_MCR = mcr;
			while (KINETISK_SPI1.SR & 0xF0) {tmp = KINETISK_SPI1.POPR;}
		} else {
			while (1) {
				sr = KINETISK_SPI2.SR;
				if (sr & SPI_SR_EOQF) break;  			   // wait for last transmit
				if (sr & 0xF0) tmp = KINETISK_SPI2.POPR;  // drain RX FIFO
			}
			KINETISK_SPI2.SR = SPI_SR_EOQF;
			SPI2_MCR = mcr;
			while (KINETISK_SPI2.SR & 0xF0) {tmp = KINETISK_SPI2.POPR;}
		}
	#endif
}

void SPI_FIFO_t3::clearFifoBuffer(bool dataMode)
{
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		// Push 4 bytes
		if (dataMode){
			KINETISK_SPI0.PUSHR = 0 | (_pcs_data << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_CONT;
			waitFifoEmpty();    // wait for both queues to be empty.
			KINETISK_SPI0.PUSHR = 0 | (_pcs_data << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_CONT;
			KINETISK_SPI0.PUSHR = 0 | (_pcs_data << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_CONT;
			KINETISK_SPI0.PUSHR = 0 | (_pcs_data << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_EOQ;
		} else {
			KINETISK_SPI0.PUSHR = 0 | (_pcs_command << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_CONT;
			waitFifoEmpty();    // wait for both queues to be empty.
			KINETISK_SPI0.PUSHR = 0 | (_pcs_command << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_CONT;
			KINETISK_SPI0.PUSHR = 0 | (_pcs_command << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_CONT;
			KINETISK_SPI0.PUSHR = 0 | (_pcs_command << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_EOQ;
		}
		while ((KINETISK_SPI0.SR & SPI_SR_EOQF) == 0);// Wait for End of Queue
		KINETISK_SPI0.SR = SPI_SR_EOQF;  // make sure it is clear
	#else
		if (_spiBus == 0){
			// Push 4 bytes
			if (dataMode){
				KINETISK_SPI0.PUSHR = 0 | (_pcs_data << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_CONT;
				waitFifoEmpty();    // wait for both queues to be empty.
				KINETISK_SPI0.PUSHR = 0 | (_pcs_data << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_CONT;
				KINETISK_SPI0.PUSHR = 0 | (_pcs_data << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_CONT;
				KINETISK_SPI0.PUSHR = 0 | (_pcs_data << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_EOQ;
			} else {
				KINETISK_SPI0.PUSHR = 0 | (_pcs_command << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_CONT;
				waitFifoEmpty();    // wait for both queues to be empty.
				KINETISK_SPI0.PUSHR = 0 | (_pcs_command << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_CONT;
				KINETISK_SPI0.PUSHR = 0 | (_pcs_command << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_CONT;
				KINETISK_SPI0.PUSHR = 0 | (_pcs_command << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_EOQ;
			}
			while ((KINETISK_SPI0.SR & SPI_SR_EOQF) == 0);// Wait for End of Queue
			KINETISK_SPI0.SR = SPI_SR_EOQF;  // make sure it is clear
		} else if (_spiBus == 1){
			// Push 1 byte
			if (dataMode){
				KINETISK_SPI1.PUSHR = 0 | (_pcs_data << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_EOQ;
			} else {
				KINETISK_SPI1.PUSHR = 0 | (_pcs_command << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_EOQ;
			}
			// Wait for End of Queue
			while ((KINETISK_SPI1.SR & SPI_SR_EOQF) == 0);// Wait for End of Queue
			KINETISK_SPI1.SR = SPI_SR_EOQF;  // make sure it is clear
		} else {
			// Push 1 byte
			if (dataMode){
				KINETISK_SPI2.PUSHR = 0 | (_pcs_data << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_EOQ;
			} else {
				KINETISK_SPI2.PUSHR = 0 | (_pcs_command << 16) | SPI_PUSHR_CTAS(0)| SPI_PUSHR_EOQ;
			}
			while ((KINETISK_SPI2.SR & SPI_SR_EOQF) == 0);// Wait for End of Queue
			KINETISK_SPI2.SR = SPI_SR_EOQF;  // make sure it is clear
		}
	#endif
}

void SPI_FIFO_t3::waitTransmitComplete(void)
{
	uint32_t tmp __attribute__((unused));
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		while (!(KINETISK_SPI0.SR & SPI_SR_TCF)); 		// wait until final output done
		tmp = KINETISK_SPI0.POPR;                  		// drain the final RX FIFO word
	#else
		if (_spiBus == 0){
			while (!(KINETISK_SPI0.SR & SPI_SR_TCF)); 
			tmp = KINETISK_SPI0.POPR;
		} else if (_spiBus == 1){
			while (!(KINETISK_SPI1.SR & SPI_SR_TCF));
			tmp = KINETISK_SPI1.POPR;
		} else {
			while (!(KINETISK_SPI2.SR & SPI_SR_TCF));
			tmp = KINETISK_SPI2.POPR;
		}
	#endif
}


void SPI_FIFO_t3::writeByte_cont(uint8_t val,bool dataMode)
{
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		if (dataMode){
			KINETISK_SPI0.PUSHR = val | (_pcs_data << 16) 	 | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
		} else {
			KINETISK_SPI0.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
		}
	#else
		if (_spiBus == 0){
			if (dataMode){
				KINETISK_SPI0.PUSHR = val | (_pcs_data << 16) 	 | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
			} else {
				KINETISK_SPI0.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
			}
		} else if (_spiBus == 1){
			KINETISK_SPI1.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
		} else {
			KINETISK_SPI2.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
		}
	#endif
	waitFifoNotFull();
}

void SPI_FIFO_t3::writeByte_last(uint8_t val,bool dataMode)
{
	uint32_t mcr = 0;
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		mcr = SPI0_MCR;
		if (dataMode){
			KINETISK_SPI0.PUSHR = val | (_pcs_data << 16) 	 | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
		} else {
			KINETISK_SPI0.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
		}
	#else
		if (_spiBus == 0){
			mcr = SPI0_MCR;
			if (dataMode){
				KINETISK_SPI0.PUSHR = val | (_pcs_data << 16) 	 | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
			} else {
				KINETISK_SPI0.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
			}
		} else if (_spiBus == 1){
			mcr = SPI1_MCR;
			KINETISK_SPI1.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
		} else {
			mcr = SPI2_MCR;
			KINETISK_SPI2.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
		}
	#endif
	waitTransmitComplete(mcr);
	#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
		if (_dc != 255) digitalWriteFast(_cs,HIGH);
	#endif
}

void SPI_FIFO_t3::writeWord_cont(uint16_t val,bool dataMode)
{
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		if (dataMode){
			KINETISK_SPI0.PUSHR = val | (_pcs_data << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT;
		} else {
			KINETISK_SPI0.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT;
		}
	#else
		if (_spiBus == 0){
			if (dataMode){
				KINETISK_SPI0.PUSHR = val | (_pcs_data << 16) 	 | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT;
			} else {
				KINETISK_SPI0.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT;
			}
		} else if (_spiBus == 1){
			KINETISK_SPI1.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT;
		} else {
			KINETISK_SPI2.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT;
		}
	#endif
	waitFifoNotFull();
}

void SPI_FIFO_t3::writeWord_last(uint16_t val,bool dataMode)
{
	uint32_t mcr = 0;
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		mcr = SPI0_MCR;
		if (dataMode){
			KINETISK_SPI0.PUSHR = val | (_pcs_data << 16) 	 | SPI_PUSHR_CTAS(1) | SPI_PUSHR_EOQ;
		} else {
			KINETISK_SPI0.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_EOQ;
		}
	#else
		if (_spiBus == 0){
			mcr = SPI0_MCR;
			if (dataMode){
				KINETISK_SPI0.PUSHR = val | (_pcs_data << 16) 	 | SPI_PUSHR_CTAS(1) | SPI_PUSHR_EOQ;
			} else {
				KINETISK_SPI0.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_EOQ;
			}
		} else if (_spiBus == 1){
			mcr = SPI1_MCR;
			KINETISK_SPI1.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_EOQ;
		} else {
			mcr = SPI2_MCR;
			KINETISK_SPI2.PUSHR = val | (_pcs_command << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_EOQ;
		}
	#endif
	waitTransmitComplete(mcr);
	#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
		if (_dc != 255) digitalWriteFast(_cs,HIGH);
	#endif
}

uint8_t SPI_FIFO_t3::readByte_cont(bool dataMode)
{
	uint8_t dummy __attribute__((unused));
	clearFifoBuffer(dataMode);
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		dummy = KINETISK_SPI0.POPR;
		dummy = KINETISK_SPI0.POPR;
		dummy = KINETISK_SPI0.POPR;
		return KINETISK_SPI0.POPR;
	#else
		if (_spiBus == 0){
			dummy = KINETISK_SPI0.POPR;
			dummy = KINETISK_SPI0.POPR;
			dummy = KINETISK_SPI0.POPR;
			return KINETISK_SPI0.POPR;
		} else if (_spiBus == 1){
			return KINETISK_SPI1.POPR;
		} else {
			return KINETISK_SPI2.POPR;
		}
	#endif
		return 0;
}

uint16_t SPI_FIFO_t3::readWord_cont(bool dataMode)
{
	uint8_t dummy __attribute__((unused));
	uint8_t a = 0;
	uint8_t b = 0;
	clearFifoBuffer(dataMode);
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		dummy = KINETISK_SPI0.POPR;
		dummy = KINETISK_SPI0.POPR;
		a = KINETISK_SPI0.POPR;
		b = KINETISK_SPI0.POPR;
		return ((b << 8) | a);
	#else
		if (_spiBus == 0){
			dummy = KINETISK_SPI0.POPR;
			dummy = KINETISK_SPI0.POPR;
			a = KINETISK_SPI0.POPR;
			b = KINETISK_SPI0.POPR;
		} else if (_spiBus == 1){
			a = KINETISK_SPI1.POPR;
			b = KINETISK_SPI1.POPR;
		} else {
			a = KINETISK_SPI2.POPR;
			b = KINETISK_SPI2.POPR;
		}
		return ((b << 8) | a);
	#endif
}

int SPI_FIFO_t3::getInterruptNumber(uint8_t pin)
{
	int intNum = digitalPinToInterrupt(pin);
	if (intNum != NOT_AN_INTERRUPT) {
		#if defined(__MK20DX128__) || defined(__MK20DX256__)
			SPI.usingInterrupt(intNum);
		#else
			if (_spiBus == 0){
				SPI.usingInterrupt(intNum);
			} else if (_spiBus == 1){
				SPI1.usingInterrupt(intNum);
			} else {
				SPI2.usingInterrupt(intNum);
			}
		#endif
		return intNum;
	}
	return 255;
}

void SPI_FIFO_t3::usingInterrupt(uint8_t n) 
{
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.usingInterrupt(n); 
	#else
		if (_spiBus == 0){
			SPI.usingInterrupt(n); 
		} else if (_spiBus == 1){	
			SPI1.usingInterrupt(n); 
		} else {
			SPI2.usingInterrupt(n); 
		}
	#endif
}

void SPI_FIFO_t3::usingInterrupt(IRQ_NUMBER_t interruptName) 
{
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.usingInterrupt(interruptName); 
	#else
		if (_spiBus == 0){
			SPI.usingInterrupt(interruptName); 
		} else if (_spiBus == 1){	
			SPI1.usingInterrupt(interruptName); 
		} else {
			SPI2.usingInterrupt(interruptName); 
		}
	#endif
}

void SPI_FIFO_t3::notUsingInterrupt(IRQ_NUMBER_t interruptName) 
{
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		SPI.notUsingInterrupt(interruptName); 
	#else
		if (_spiBus == 0){
			SPI.notUsingInterrupt(interruptName); 
		} else if (_spiBus == 1){	
			SPI1.notUsingInterrupt(interruptName); 
		} else {
			SPI2.notUsingInterrupt(interruptName); 
		}
	#endif
}

/*

void SPI_FIFO::printPacket(uint16_t data, uint8_t count)
{
  for (int i = count - 1; i >= 0; i--) {
    if (bitRead(data, i) == 1) {
      Serial.print("1");
    }
    else {
      Serial.print("0");
    }
  }
  Serial.print(" -> 0x");
  if (count == 8) {
    Serial.print((byte)data, HEX);
  } else {
    Serial.print(data, HEX);
  }
  Serial.print("\n");
}
*/
