/**
 ****************************************************************************************
 *
 * \file SSD1322_HW_Driver.cpp
 *
 * \brief Hardware dependent functions for SSD1322 OLED display.
 *
 * This file contains functions that rely on hardware of used MCU. In this example functions
 * are filled with STM32F411RE hardware implementation. To use this library on any other MCU
 * you just have to provide its hardware implementations of functions from this file and higher
 * level functions should work without modification.
 *
 * Copyright (C) 2020 Wojciech Klimek
 * MIT license:
 * https://github.com/wjklimek1/SSD1322_OLED_library
 *
 * Modified for use in Arduino by Eric Hazen
 ****************************************************************************************
 */

//=================== Include hardware HAL libraries =====================//
#include <Arduino.h>
#include <SPI.h>
#include "SSD1322_HW_Driver.h"

//
// define our pins
//
const int OLED_DC_PIN = 5;	/* data/control */
//const int OLED_nRST_PIN = 6;	/* reset */
//const int OLED_nCS_PIN = 4;	/* chip select */
const int OLED_nRST_PIN = 4;	/* reset */
const int OLED_nCS_PIN = 6;	/* chip select */

// #define OUR_SPI_SETTINGS SPISettings(10000000,MSBFIRST,SPI_MODE3)
#define OUR_SPI_SETTINGS SPISettings(4000000,MSBFIRST,SPI_MODE0)

//====================== Initialize Hardware ======================//
void SSD1322_HW_Init() {
  pinMode( OLED_DC_PIN, OUTPUT);
  pinMode( OLED_nRST_PIN, OUTPUT);
  pinMode( OLED_nCS_PIN, OUTPUT);

  digitalWrite( OLED_DC_PIN, LOW);
  digitalWrite( OLED_nRST_PIN, HIGH);
  digitalWrite( OLED_nCS_PIN, HIGH);

  SPI.begin();
}

//====================== CS pin low ========================//
/**
 *  @brief Drives CS (Chip Select) pin of SPI interface low.
 *
 *  CS pin may be also signed as "SS" or "NSS"
 */
void SSD1322_HW_drive_CS_low()
{
  digitalWrite( OLED_nCS_PIN, LOW);
}

//====================== CS pin high ========================//
/**
 *  @brief Drives CS (Chip Select) pin of SPI interface high.
 *
 *  CS pin may be also signed as "SS" or "NSS"
 */
void SSD1322_HW_drive_CS_high()
{
  digitalWrite( OLED_nCS_PIN, HIGH);
}

//====================== DC pin low ========================//
/**
 *  @brief Drives DC (Data/Command) pin of OLED driver low.
 *
 *  High state is for data and low state is for command.
 */
void SSD1322_HW_drive_DC_low()
{
  digitalWrite( OLED_DC_PIN, LOW);
}

//====================== DC pin high ========================//
/**
 *  @brief Drives DC (Data/Command) pin of of OLED driver high.
 *
 *  High state is for data and low state is for command.
 */
void SSD1322_HW_drive_DC_high()
{
  digitalWrite( OLED_DC_PIN, HIGH);
}

//====================== RESET pin low ========================//
/**
 *  @brief Drives RESET pin of of OLED driver low.
 *
 *  Logic low on RESET resets OLED driver.
 */
void SSD1322_HW_drive_RESET_low()
{
  digitalWrite( OLED_nRST_PIN, LOW);
}

//====================== RESET pin high ========================//
/**
 *  @brief Drives RESET pin of of OLED driver high.
 *
 *  Logic low on RESET resets OLED driver.
 */
void SSD1322_HW_drive_RESET_high()
{
  digitalWrite( OLED_nRST_PIN, HIGH);
}

//====================== Send single SPI byte ========================//
/**
 *  @brief Transmits single byte through SPI interface.
 *
 *  @param[in] byte_to_transmit byte that will be transmitted through SPI interface
 */
void SSD1322_HW_SPI_send_byte(uint8_t byte_to_transmit)
{
  SPI.beginTransaction(OUR_SPI_SETTINGS);
  SPI.transfer( byte_to_transmit);
  SPI.endTransaction();
}

//====================== Send array of SPI bytes ========================//
/**
 *  @brief Transmits array of bytes through SPI interface.
 *
 *  @param[in] array_to_transmit array of bytes that will be transmitted through SPI interface
 *  @param[in] array_size amount of bytes to transmit
 */
void SSD1322_HW_SPI_send_array(uint8_t *array_to_transmit, uint32_t array_size)
{
  SPI.beginTransaction(OUR_SPI_SETTINGS);
  for( uint32_t i=0; i<array_size; i++)
    SPI.transfer( array_to_transmit[i]);
  SPI.endTransaction();
}

//====================== Milliseconds delay ========================//
/**
 *  @brief Wait for x milliseconds.
 *
 *  NOTE: This function is only used in initialization sequence to pull reset down and high
 *  and later to wait for Vdd stabilization. It takes time ONLY on startup (around 200ms)
 *  and is NOT USED during normal operation.
 *
 *  @param[in] milliseconds time to wait
 */
void SSD1322_HW_msDelay(uint32_t milliseconds)
{
  for( int i=0; i<milliseconds; i++)
    delay(1);
}
