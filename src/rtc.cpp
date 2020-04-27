/** \file
 * Description: Provides function definitions to start an RTOS task and interface with an external RTC (MCP79510)
 * Part of the timing subsystem
 * \author: Primary: Shipra Vaidya, Secondary: Jesse Cannon
 * \date last modified: 4/26/2020
 */

#include "rtcdefine.h"

spi_device_handle_t rtc;

void rtc_config(void)
{
    esp_err_t ret;
    //spi_device_handle_t rtc;
    
    spi_bus_config_t buscfg;
    buscfg.mosi_io_num = HSPI_MOSI;
    buscfg.miso_io_num = HSPI_MISO;
    buscfg.sclk_io_num = HSPI_CLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = 5;

    spi_device_interface_config_t rtccfg;
    rtccfg.command_bits = 8;
    rtccfg.address_bits = 8;
    rtccfg.dummy_bits = 0;
    rtccfg.clock_speed_hz = 1000;
    //.duty_cycle_pos = 128,        //50% duty cycle
    rtccfg.mode = 3;
    rtccfg.spics_io_num = HSPI_CS;
    //.cs_ena_posttrans = 3,       ;
    rtccfg.queue_size = 3;

    ret = spi_bus_initialize(HSPI_HOST, &buscfg, 0);
    assert(ret == ESP_OK);
    ret = spi_bus_add_device(HSPI_HOST, &rtccfg, &rtc);
    assert(ret == ESP_OK);

	vTaskDelay(900 / portTICK_RATE_MS);
	ret = ST_StartRTCHandler();
	if (ret != 0) {
		ESP_LOGE(RTC_TAG, "%s RTC Handler failed, ret = %d\n", __func__, ret);
	}

	while(1) {
		vTaskDelay(1000 / portTICK_RATE_MS);
	}

}

esp_err_t readData(uint32_t addr, uint8_t *out)
{
	//spi_device_handle_t rtc;

    spi_transaction_t t_read;
	t_read.cmd = RTC_READ;
	t_read.flags = SPI_TRANS_USE_RXDATA;
	t_read.rxlength = 8;
	t_read.length = 8;
	t_read.addr = addr;

	esp_err_t ret = spi_device_transmit(rtc, &t_read); 
	*out = t_read.rx_data[0];
	return ret;
}

esp_err_t writeData(uint32_t addr, uint8_t data)
{
	//spi_device_handle_t rtc;
    
    spi_transaction_t t_write;
	t_write.cmd = RTC_WRITE; //Command is write
	t_write.flags = SPI_TRANS_USE_TXDATA;
	t_write.rxlength = 0;
	t_write.length = 8;
	t_write.addr = addr;

	t_write.tx_data[0] = data;
	esp_err_t ret = spi_device_transmit(rtc, &t_write); //Transmit
	return ret;
}

esp_err_t setTime(const struct tm *time)
{
    char buff[70];
	if (strftime(buff, sizeof buff, "%c", time))    {
		printf("Set Time %s\n", buff);
	}
	else	{
		ESP_LOGE(RTC_TAG, "setTime, couldn't print time being set.");
	}

	esp_err_t ret = ESP_FAIL;

    uint8_t sec = time->tm_sec;
	uint8_t min = time->tm_min;
	uint8_t hour = time->tm_hour;
	uint8_t day = time->tm_wday + 1; //RTC starts at 1, tm starts at 0
	uint8_t date = time->tm_mday; //1-31
	uint8_t month = time->tm_mon + 1; //RTC starts at 1, tm starts at 0
	uint8_t year = time->tm_year; //RTC is 0-99 for year
	
    if (year > 99) //tm is years since 1900, assume it's 2000 something so 100 off
	{
		year -= 100;
	}

    //convert to bcd
    sec = (sec / 10) << 4 | (sec % 10);
	min = (min / 10) << 4 | (min % 10);
	hour = (hour / 10) << 4 | (hour % 10);
	day = day & (0x07);
	date = (date / 10) << 4 | (date % 10);
	month = (month / 10) << 4 | (month % 10);
	year = (year / 10) << 4 | (year % 10);

    sec = sec | (ST); //start oscillator bit
	
    //check trimsign
	uint8_t trimsign = 0;
	ret = readData(RTCHOUR, &trimsign); //read trimsign so we don't change how RTC times itself
	if (ret != ESP_OK)	{
		ESP_LOGE(RTC_TAG, "%s readData failed, ret = %s\n", __func__, esp_err_to_name(ret));
	}
	hour = hour | (trimsign & 0x80); //keep trimsign the same
	day = day | VBATEN | OSCRUN; //enable backup battery and oscillator run flags

	//stop oscillator
	ret = writeData(RTCSEC, 0);
	if (ret != ESP_OK)
	{
		ESP_LOGE(RTC_TAG, "%s writeData failed, ret = %s\n", __func__, esp_err_to_name(ret));
	}

	//minutes
	ret = writeData(RTCMIN, min);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s writeData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//hours
	ret = writeData(RTCHOUR, hour);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s writeData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//days
	ret = writeData(RTCWKDAY, day);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s writeData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//date
	ret = writeData(RTCDATE, date);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s writeData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//month
	ret = writeData(RTCMTH, month);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s writeData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//year
	ret = writeData(RTCYEAR, year);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s writeData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//seconds
	ret = writeData(RTCSEC, sec);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s writeData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	return ret;
}


esp_err_t getTime(struct tm *outTime)
{
	esp_err_t ret = ESP_FAIL;
	
	uint8_t sec = 0;
	uint8_t min = 0;
	uint8_t hour = 0;
	uint8_t day = 0;
	uint8_t date = 0;
	uint8_t month = 0;
	uint8_t year = 0;

	//seconds
	ret = readData(RTCSEC, &sec);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s readData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//minutes
	ret = readData(RTCMIN, &min);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s readData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//hours
	ret = readData(RTCHOUR, &hour);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s readData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//weekday
	ret = readData(RTCWKDAY, &day);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s readData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//date
	ret = readData(RTCDATE, &date);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s readData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//month
	ret = readData(RTCMTH, &month);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s readData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//year
	ret = readData(RTCYEAR, &year);
	if (ret != ESP_OK) {
		ESP_LOGE(RTC_TAG, "%s readData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		return ret;
	}

	//bcd to decimal
	sec = ((sec & 0x70) >> 4) * 10 + (sec & 0x0F);
	min = ((min & 0x70) >> 4) * 10 + (min & 0x0F);
	hour = ((hour & 0x30) >> 4) * 10 + (hour & 0x0F);
	day = (day & 0x07);
	date = ((date & 0x70) >> 4) * 10 + (date & 0x0F);
	month = ((month & 0x10) >> 4) * 10 + (month & 0x0F);
	year = ((year & 0x70) >> 4) * 10 + (year & 0x0F);

	//adjustments
	day -= 1;
	month -= 1;
	year += 100;

	outTime->tm_sec = sec;
	outTime->tm_min = min;
	outTime->tm_hour = hour;
	outTime->tm_wday = day;
	outTime->tm_mday = date;
	outTime->tm_mon = month;
	outTime->tm_year = year;

	return ret;
}

void RTCHandler(void *pvParms) 
{
	esp_err_t ret = ESP_OK;
	struct tm date;

	currTime = &date;
	
	uint8_t rxData = 0;
	ret = readData(RTCWKDAY, &rxData);
	if (ret != ESP_OK)
	{
		ESP_LOGE(RTC_TAG, "%s readData failed, ret = %s\n", __func__, esp_err_to_name(ret));
	}

	//first time running
	if (!(rxData & OSCRUN))
	{
		ESP_LOGI(RTC_TAG, "RTC not running, starting now...");
		uint8_t txData = SQWEN;
		ret = writeData(CONTROL, txData);
		if(ret != ESP_OK){
			ESP_LOGE(RTC_TAG, "%s writeData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		}
		
		//enable backup battery
		txData = rxData | VBATEN;
		ret = writeData(RTCWKDAY, txData);
		if (ret != ESP_OK) {
			ESP_LOGE(RTC_TAG, "%s writeData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		}
		
		//set start oscillator bit
		txData = ST;
		ret = writeData(RTCSEC, txData); //start oscillator
		if(ret != ESP_OK){
			ESP_LOGE(RTC_TAG, "%s writeData failed, ret = %s\n", __func__, esp_err_to_name(ret));
		}
	}

	else
	{
		ESP_LOGI(RTC_TAG, "RTC already running, no need to configure.");
	}

	//read time from RTC to set local time
	ret = getTime(&date);
	if (ret != ESP_OK)
	{
		ESP_LOGE(RTC_TAG, "getTime failed, ret=%s", esp_err_to_name(ret));
	}
	time_t tmpTime = mktime(&date);
	struct timeval toSet = {
			.tv_sec = tmpTime,
			.tv_usec = 0
	};
	settimeofday(&toSet, NULL);

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 100; //testing 1 second interval
	xLastWakeTime = xTaskGetTickCount();
	while(1){
		//read time from RTC periodically

		ret = getTime(&date);
		if (ret != ESP_OK)
		{
			ESP_LOGE(RTC_TAG, "getTime failed, ret=%s", esp_err_to_name(ret));
		}
		else
		{
			//print RTC vs ESP time
			char buff[64];
			if (strftime(buff, sizeof(buff), "%c", &date)) {
				printf("(GMT): %s\n", buff);
				time_t t = time(NULL);
				localtime_r(&t, &date);
				if (strftime(buff, sizeof(buff), "%c", &date)) {
					printf("Local: %s\n", buff);
				}

			}
			else {
				ESP_LOGE(RTC_TAG, "Couldn't convert time to string");
			}
		}
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
	RTC_Handle = NULL;
	vTaskDelete(NULL);
}

//start RTC task
esp_err_t ST_StartRTCHandler(void) {
	esp_err_t ret = ESP_OK;
	if(RTC_Handle != NULL){
		ret = ESP_FAIL;
		return ret; //return fail if task is already defined
	}
	BaseType_t xReturned;
	//create task, passing the spi device handle through. stack depth of 4096, priority of 25-5=20, handle stored at RTC_Handle
	xReturned = xTaskCreate(&RTCHandler, "ST_RTCHandler", 4096, NULL, configMAX_PRIORITIES - 5, &RTC_Handle);
	if(xReturned != pdPASS){
		ret = ESP_FAIL;
		return ret;					//return fail if task could not be created
	}
	return ret;
}