/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        i2C_eeprom_m24m01.h [FREESCALE Coldfire V2 MCU]
    Project:     Single Chip Embedded Internet 
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    This file contains I2C EEPROM specific code
    It is declared as a header so that projects do not need to specify that it is not to be compiled.
    Its goal is to improve overall readability of the hardware interface.

    02.10.2011 Modify to suit revised storage space access               {1}

*/ 

#if defined _M24M01_0 || defined _M24M01_1

#ifdef _I2C_EEPROM_DEFINES
    #define PAGE_WRITE_LENGTH 128

    static unsigned char fnCheckI2CEEPROM(int iDeviceReference);

    static unsigned char ucI2C_EEPROM_Address[_M24M01_COUNT] = {0};
    static unsigned long ulI2C_EEPROM_pointer[_M24M01_COUNT];
#endif

    // This code is inserted to detect the presence of the I2C EEPROM device(s). If the first device is not detected the I2C interface is disabled.
    // If there are multiple devices, each will be recorded.
    //
#ifdef _CHECK_I2C_EEPROM
    {
        int iDeviceCount;
        int iDetectedSlaves = 0;
        // Initialise the I2C interface
        //
        POWER_UP(POWER_I2C);                                             // enable clock to module
        #if defined IIC0_ON_QS && defined _M52259
        PQSPAR |= (QS_I2C_SCL_0_FUNCTION | QS_I2C_SDA_0_FUNCTION);       // configure the SDA and SCL pins on QS
        #else
        PASPAR |= (AS_I2C_SCL_0_FUNCTION | AS_I2C_SDA_0_FUNCTION);       // configure the SDA and SCL pins on AS
        #endif
    #if BUS_CLOCK > 60000000                                             // 80MHz
        I2FDR = 0x32;                                                    // set about 400k with 80MHz bus frequency
    #elif BUS_CLOCK > 50000000                                           // 60MHz
        I2FDR = 0x0d;                                                    // set about 400k with 60MHz bus frequency
    #elif BUS_CLOCK > 40000000                                           // 50MHz
        I2FDR = 0x0b;                                                    // set about 400k with 50MHz bus frequency
    #else                                                                // assume 40MHz
        I2FDR = 0x0a;                                                    // set about 400k with 40MHz bus frequency
    #endif
        if (I2SR & IIC_IBB) {                                            // if bus is initially busy attempt to clear it
           I2CR = 0;
           I2CR = (IIC_IEN | IIC_MSTA);
           I2DR;                                                         // dummy read
           I2SR = 0;
           I2CR = 0;
        }
        I2CR = (IIC_IEN);                                                // enable IIC controller

        for (iDeviceCount = 0; iDeviceCount < _M24M01_COUNT; iDeviceCount++) { // test the presence of each possible chip and set its address
            ucI2C_EEPROM_Address[iDetectedSlaves] = fnCheckI2CEEPROM(iDeviceCount);
            if (ucI2C_EEPROM_Address[iDetectedSlaves] != 0) {
                ulI2C_EEPROM_pointer[iDetectedSlaves] = 0xffffffff;      // invalidate sequential pointer
                iDetectedSlaves++;
    #ifdef EXTENSION_FILE_COUNT_VARIABLE
                fnExtendFileSystem(I2C_EEPROM_DEVICE_SIZE);              // increase file system size accordingly
    #endif
            }
        }
        if (iDetectedSlaves == 0) {                                      // if no devices found disable the I2C interface
            POWER_DOWN(POWER_I2C);                                       // remove I2C power when no devices found
        }
    #ifdef STORAGE_REV_1                                                 // {1}
        else {
            static STORAGE_AREA_ENTRY i2c_eeprom_storage = {0};          // insert own storage area entry
            i2c_eeprom_storage.ptrNext = UserStorageListPtr;
            i2c_eeprom_storage.ptrMemoryStart = UserStorageListPtr->ptrMemoryEnd;
            i2c_eeprom_storage.ptrMemoryEnd = (i2c_eeprom_storage.ptrMemoryStart + (iDetectedSlaves * I2C_EEPROM_DEVICE_SIZE));
            i2c_eeprom_storage.ptrMemoryStart++;                         // compensate the fact that the end pointer is to the end of the last used address (see reason in STORAGE_AREA_ENTRY definition)
            i2c_eeprom_storage.ucStorageType = _STORAGE_I2C_EEPROM,      // type
            i2c_eeprom_storage.ucDeviceCount = (unsigned char)iDetectedSlaves;
            UserStorageListPtr = (STORAGE_AREA_ENTRY *)&i2c_eeprom_storage; // insert i2c eeprom as storage medium
        }
    #endif
    }
#endif


#ifdef _I2C_EEPROM_INTERFACE
static unsigned char fnCheckI2CEEPROM(int iDeviceReference)
{
    const unsigned char EEPROM_address[_M24M01_COUNT] = {
        _M24M01_0,
    #if _M24M01_COUNT > 1
        _M24M01_1,
    #endif
    };

    unsigned char ucSlaveAddress = EEPROM_address[iDeviceReference];     // the address of the slave

    while (I2SR & IIC_IBB) {}                                            // while the bus is busy   

    I2SR = 0;
    I2CR = (IIC_IEN | IIC_MTX);                                          // set transmit mode
    I2CR = (IIC_IEN | IIC_MSTA | IIC_MTX);                               // set master mode to cause start condition to be sent
    I2DR = (ucSlaveAddress | 0x01);                                      // the slave address for read
    while (!(I2SR & IIC_IIF)) {                                          // wait for transfer to complete
    #ifdef _WINDOWS
        I2SR |= (IIC_ICF | IIC_IIF);
    #endif
    }
    if (I2SR & IIC_RXACK) {                                              // if the slave didn't acknowledge
        ucSlaveAddress = 0;                                              // return that no device is responding
    }
    I2CR = (IIC_IEN | IIC_MTX);                                          // send stop condition
    
    return ucSlaveAddress;                                               // return the slave address if there was a response, else return 0
}

static void fnSendStart(unsigned char ucSlaveAddress, unsigned long device_address)
{
    int iPoll = 0;
    do {
        I2SR = 0;
        if (iPoll != 0) {                                                // if the device is initially busy (due to previous write not yet completed)
            I2CR = (IIC_IEN | IIC_MSTA | IIC_MTX | IIC_RSTA);            // send repeated start
        }
        else {
            I2CR = (IIC_IEN | IIC_MTX);                                  // set transmit mode
            I2CR = (IIC_IEN | IIC_MSTA | IIC_MTX);                       // set master mode to cause start condition to be sent
        }
        I2DR = ucSlaveAddress;                                           // the slave address (including read/write and extended address bit)
        iPoll++;                                                         // count the number of times that the device needed to be polled
        while (!(I2SR & IIC_IIF)) {                                      // wait for transfer to complete
    #ifdef _WINDOWS
            fnSimIIC_devices(IIC_ADDRESS, I2DR);
            I2SR |= (IIC_ICF | IIC_IIF);
    #endif
        }
    } while (I2SR & IIC_RXACK);                                          // no acknowledge returned means that the device is busy completing a previous write operation

    if (!(ucSlaveAddress & 0x01)) {                                      // if write, send also the internal address
        I2SR = 0;
        I2DR = (unsigned char)(device_address >> 8);                     // set the address - msb
        while (!(I2SR & IIC_IIF)) {                                      // wait for transfer to complete
    #ifdef _WINDOWS
            fnSimIIC_devices(IIC_TX_DATA, I2DR);
            I2SR |= (IIC_ICF | IIC_IIF);
    #endif
        }
        I2SR = 0;
        I2DR = (unsigned char)(device_address);                          // set the address - lsb
        while (!(I2SR & IIC_IIF)) {                                      // wait for transfer to complete
    #ifdef _WINDOWS
            fnSimIIC_devices(IIC_TX_DATA, I2DR);
            I2SR |= (IIC_ICF | IIC_IIF);
    #endif
        }
    }
}

#ifdef STORAGE_REV_1
static int fnReadI2C_eeprom(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    MAX_FILE_LENGTH data_read = ptrAccessDetails->BlockLength;
    unsigned long device_address = ptrAccessDetails->ulOffset;
    int iDeviceNumber = ptrAccessDetails->ucDeviceNumber;
    // Read the amount of data specified from the starting address to a return buffer
    //
    if (ucI2C_EEPROM_Address[iDeviceNumber] == 0) {                      // check that device exists
        uMemset(ptrBuffer, 0x00, data_read);
        return -1;                                                       // error - no device exists so buffer filled with 0
    }

    while (I2SR & IIC_IBB) {}                                            // while the bus is busy (could still be sending stop condition from previous operation)

    if (ulI2C_EEPROM_pointer[iDeviceNumber] != device_address) {         // check whether a sequential read can be performed
        ulI2C_EEPROM_pointer[iDeviceNumber] = device_address;            // synchronise pointer so that sequential read may be possible in the future

        fnSendStart((unsigned char)(ucI2C_EEPROM_Address[iDeviceNumber] | (unsigned char)(0x02 & (device_address >> 15))), device_address); // send start condition and address for write

        I2SR = 0;
        I2CR = (IIC_IEN | IIC_MSTA | IIC_MTX | IIC_RSTA);                // send a repeated start
        I2DR = (ucI2C_EEPROM_Address[iDeviceNumber] | 0x01); // the slave address for read
        while (!(I2SR & IIC_IIF)) {                                      // wait for transfer to complete
    #ifdef _WINDOWS
            fnSimIIC_devices(IIC_ADDRESS, I2DR);
            I2SR |= (IIC_ICF | IIC_IIF);
    #endif
        }
    }
    else {                                                               // sequential read is possible
        fnSendStart((unsigned char)(ucI2C_EEPROM_Address[iDeviceNumber] | 0x01), 0); // send start condition and address for read
    }

    I2SR = 0;
    if (data_read == 1) {
        I2CR = (IIC_IEN | IIC_MSTA | IIC_TXAK);                          // single read byte will not be acknowledged
    }
    else {
        I2CR = (IIC_IEN | IIC_MSTA);                                     // multi-byte reads are initially acknowledge
    }
    *ptrBuffer = I2DR;                                                   // dummy read to initiate the read data transfer
        
    while (data_read--) {
        device_address++;
        if (++ulI2C_EEPROM_pointer[iDeviceNumber] >= I2C_EEPROM_DEVICE_SIZE) {
            ulI2C_EEPROM_pointer[iDeviceNumber] = 0;                     // pointer overflow occurs at last address in device
            iDeviceNumber++;                                             // move to next device
            device_address = 0;
        }
        while (!(I2SR & IIC_IIF)) {                                      // wait for transfer to complete
#ifdef _WINDOWS
            I2DR = fnSimIIC_devices(IIC_RX_DATA, I2DR);
            I2SR |= (IIC_ICF | IIC_IIF);
#endif
        }
        I2SR = 0;
        if (data_read == 1) {
            I2CR = (IIC_IEN | IIC_MSTA | IIC_TXAK);                      // final read byte will not be acknowledged
        }
        *ptrBuffer++ = I2DR;                                             // read received byte
    }
    while (!(I2SR & IIC_IIF)) {                                          // wait for transfer to complete
#ifdef _WINDOWS
        I2SR |= (IIC_ICF | IIC_IIF);
#endif
    }
    I2CR = (IIC_IEN | IIC_TXAK);                                         // send stop condition
    return 0;                                                            // success
}
#else
static int fnReadI2C_eeprom(unsigned char *ptrAddress, unsigned char *ptrBuffer, MAX_FILE_LENGTH DataLength)
{
    // Read the amount of data specified from the starting address to a return buffer
    //
    CAST_POINTER_ARITHMETIC device_address = (CAST_POINTER_ARITHMETIC)ptrAddress;
    int iDeviceNumber = 0;
    device_address -= I2C_EEPROM_START_ADDRESS;
    while (device_address >= I2C_EEPROM_DEVICE_SIZE) {                   // determine the device in which the address starts
        device_address -= I2C_EEPROM_DEVICE_SIZE;
        iDeviceNumber++;
    }    
    while (DataLength != 0) {                                            // support read in multiple devices
        MAX_FILE_LENGTH data_read = DataLength;
        if (data_read > (I2C_EEPROM_DEVICE_SIZE - device_address)) {     // check how much data could be read from the present device
            data_read = (I2C_EEPROM_DEVICE_SIZE - device_address);       // limit the read to maximum in present device
        }
        if (ucI2C_EEPROM_Address[iDeviceNumber] == 0) {                  // check that device exists
            uMemset(ptrBuffer, 0x00, DataLength);
            return -1;                                                   // error - no device exists so buffer filled with 0
        }

        while (I2SR & IIC_IBB) {}                                        // while the bus is busy (could still be sending stop condition from previous operation)

        if (ulI2C_EEPROM_pointer[iDeviceNumber] != device_address) {     // check whether a sequential read can be performed
            ulI2C_EEPROM_pointer[iDeviceNumber] = device_address;        // synchronise pointer so that sequential read may be possible in the future

            fnSendStart((unsigned char)(ucI2C_EEPROM_Address[iDeviceNumber] | (unsigned char)(0x02 & (device_address >> 15))), device_address); // send start condition and address for write

            I2SR = 0;
            I2CR = (IIC_IEN | IIC_MSTA | IIC_MTX | IIC_RSTA);            // send a repeated start
            I2DR = (ucI2C_EEPROM_Address[iDeviceNumber] | 0x01);         // the slave address for read
            while (!(I2SR & IIC_IIF)) {                                  // wait for transfer to complete
    #ifdef _WINDOWS
                fnSimIIC_devices(IIC_ADDRESS, I2DR);
                I2SR |= (IIC_ICF | IIC_IIF);
    #endif
            }
        }
        else {                                                           // sequential read is possible
            fnSendStart((unsigned char)(ucI2C_EEPROM_Address[iDeviceNumber] | 0x01), 0); // send start condition and address for read
        }

        I2SR = 0;
        if (data_read == 1) {
            I2CR = (IIC_IEN | IIC_MSTA | IIC_TXAK);                      // single read byte will not be acknowledged
        }
        else {
            I2CR = (IIC_IEN | IIC_MSTA);                                 // multi-byte reads are initially acknowledge
        }
        *ptrBuffer = I2DR;                                               // dummy read to initiate the read data transfer
            
        while (data_read--) {
            device_address++;
            if (++ulI2C_EEPROM_pointer[iDeviceNumber] >= I2C_EEPROM_DEVICE_SIZE) {
                ulI2C_EEPROM_pointer[iDeviceNumber] = 0;                 // pointer overflow occurs at last address in device
                iDeviceNumber++;                                         // move to next device
                device_address = 0;
            }
            DataLength--;
            while (!(I2SR & IIC_IIF)) {                                  // wait for transfer to complete
    #ifdef _WINDOWS
                I2DR = fnSimIIC_devices(IIC_RX_DATA, I2DR);
                I2SR |= (IIC_ICF | IIC_IIF);
    #endif
            }
            I2SR = 0;
            if (data_read == 1) {
                I2CR = (IIC_IEN | IIC_MSTA | IIC_TXAK);                  // final read byte will not be acknowledged
            }
            *ptrBuffer++ = I2DR;                                         // read received byte
        }
        while (!(I2SR & IIC_IIF)) {                                      // wait for transfer to complete
    #ifdef _WINDOWS
            I2SR |= (IIC_ICF | IIC_IIF);
    #endif
        }
        I2CR = (IIC_IEN | IIC_TXAK);                                     // send stop condition
    }
    return 0;                                                            // success
}
#endif

#ifdef STORAGE_REV_1
static int fnWriteI2C_eeprom(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    // Read the amount of data specified from the starting address to a return buffer
    //
    CAST_POINTER_ARITHMETIC device_address = ptrAccessDetails->ulOffset;
    int iDeviceNumber = ptrAccessDetails->ucDeviceNumber;
    MAX_FILE_LENGTH data_write = ptrAccessDetails->BlockLength;
    unsigned char ucPageRemain = (PAGE_WRITE_LENGTH - (unsigned char)(device_address % PAGE_WRITE_LENGTH)); // remaining line length
    if (data_write > ucPageRemain) {                                     // limit to remaining line length
        data_write = ucPageRemain;
    }
    while (device_address >= I2C_EEPROM_DEVICE_SIZE) {                   // determine the device in which the address starts
        device_address -= I2C_EEPROM_DEVICE_SIZE;
        iDeviceNumber++;
    }
    if (ucI2C_EEPROM_Address[iDeviceNumber] == 0) {                      // check that device exists
        return -1;                                                       // error - no device exists
    }

    while (I2SR & IIC_IBB) {}                                            // while the bus is busy (could still be sending stop condition from previous operation)

    fnSendStart((unsigned char)(ucI2C_EEPROM_Address[iDeviceNumber] | (unsigned char)(0x02 & (device_address >> 15))), device_address); // send start condition and address for write
        
    while (data_write--) {
        I2SR = 0;
        I2DR = *ptrBuffer++;                                             // write data byte
        device_address++;
        while (!(I2SR & IIC_IIF)) {                                      // wait for transfer to complete
    #ifdef _WINDOWS
            fnSimIIC_devices(IIC_TX_DATA, I2DR);
            I2SR |= (IIC_ICF | IIC_IIF);
    #endif
        }
    }
    I2CR = (IIC_IEN | IIC_TXAK);                                     // send stop condition
    return 0;                                                            // success
}
#else
static int fnWriteI2C_eeprom(unsigned char *ptrAddress, unsigned char *ptrBuffer, MAX_FILE_LENGTH DataLength)
{
    // Read the amount of data specified from the starting address to a return buffer
    //
    CAST_POINTER_ARITHMETIC device_address = (CAST_POINTER_ARITHMETIC)ptrAddress;
    int iDeviceNumber = 0;
    device_address -= I2C_EEPROM_START_ADDRESS;
    while (DataLength != 0) {                                            // support read in multiple devices
        MAX_FILE_LENGTH data_write = DataLength;
        unsigned char ucPageRemain = (PAGE_WRITE_LENGTH - (unsigned char)(device_address % PAGE_WRITE_LENGTH)); // remaining line length
        if (data_write > ucPageRemain) {                                 // limit to remaining line length
            data_write = ucPageRemain;
        }
        while (device_address >= I2C_EEPROM_DEVICE_SIZE) {               // determine the device in which the address starts
            device_address -= I2C_EEPROM_DEVICE_SIZE;
            iDeviceNumber++;
        }
        if (ucI2C_EEPROM_Address[iDeviceNumber] == 0) {                  // check that device exists
            return -1;                                                   // error - no device exists
        }

        while (I2SR & IIC_IBB) {}                                        // while the bus is busy (could still be sending stop condition from previous operation)

        fnSendStart((unsigned char)(ucI2C_EEPROM_Address[iDeviceNumber] | (unsigned char)(0x02 & (device_address >> 15))), device_address); // send start condition and address for write
            
        while (data_write--) {
            I2SR = 0;
            I2DR = *ptrBuffer++;                                         // write data byte
            device_address++;
            DataLength--;
            while (!(I2SR & IIC_IIF)) {                                  // wait for transfer to complete
    #ifdef _WINDOWS
                fnSimIIC_devices(IIC_TX_DATA, I2DR);
                I2SR |= (IIC_ICF | IIC_IIF);
    #endif
            }
        }
        I2CR = (IIC_IEN | IIC_TXAK);                                     // send stop condition
    }
    return 0;                                                            // success
}
#endif

// This delete routine is specifically for the uFileSystem since it only deletes (sets 0xff) the length bytes at each file boundary,
// which improves efficiency. If the content is to be set to 0xff a write should be performed instead
//
#ifdef STORAGE_REV_1
static int fnDeleteI2C_eeprom(ACCESS_DETAILS *ptrAccessDetails)
{
    const unsigned long ulEmpty = 0xffffffff;
    ACCESS_DETAILS AccessDetails;
    MAX_FILE_LENGTH DataLength = ptrAccessDetails->BlockLength;
    AccessDetails.ucDeviceNumber = ptrAccessDetails->ucDeviceNumber;
    AccessDetails.ulOffset = ptrAccessDetails->ulOffset;
    AccessDetails.BlockLength = sizeof(ulEmpty);

    while (DataLength != 0) {
        if (fnWriteI2C_eeprom(&AccessDetails, (unsigned char *)&ulEmpty) < 0) {
            return -1;
        }
        if (DataLength < EXTENSION_FILE_SIZE) {
            break;
        }
        DataLength -= EXTENSION_FILE_SIZE;
        AccessDetails.ulOffset += EXTENSION_FILE_SIZE;
    }
    return 0;
}
#else
static int fnDeleteI2C_eeprom(unsigned char *ptrAddress, MAX_FILE_LENGTH DataLength)
{
    const unsigned long ulEmpty = 0xffffffff;
    unsigned char *ptrOriginalAddress = ptrAddress;
    // Set to file boundary and adjust length to suit
    //
    ptrAddress = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrAddress & ~(EXTENSION_FILE_SIZE - 1));
    DataLength += (ptrOriginalAddress - ptrAddress);

    while (DataLength != 0) {
        if (fnWriteI2C_eeprom(ptrAddress, (unsigned char *)&ulEmpty, sizeof(ulEmpty)) < 0) {
            return -1;
        }
        if (DataLength < EXTENSION_FILE_SIZE) {
            break;
        }
        DataLength -= EXTENSION_FILE_SIZE;
        ptrAddress += EXTENSION_FILE_SIZE;
    }
    return 0;
}
#endif
#endif

#endif

