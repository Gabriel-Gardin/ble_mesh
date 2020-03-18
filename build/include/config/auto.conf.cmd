deps_config := \
	/home/gardin/esp/esp-mdf/esp-idf/components/app_trace/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/aws_iot/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/bt/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/driver/Kconfig \
	/home/gardin/esp/esp-mdf/components/third_party/esp-aliyun/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/esp32/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/esp_adc_cal/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/esp_event/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/esp_http_client/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/esp_http_server/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/ethernet/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/fatfs/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/freemodbus/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/freertos/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/heap/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/libsodium/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/log/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/lwip/Kconfig \
	/home/gardin/esp/esp-mdf/components/maliyun_linkkit/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/mbedtls/Kconfig \
	/home/gardin/esp/esp-mdf/components/mcommon/Kconfig \
	/home/gardin/esp/esp-mdf/components/mconfig/Kconfig \
	/home/gardin/esp/esp-mdf/components/mdebug/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/mdns/Kconfig \
	/home/gardin/esp/esp-mdf/components/mespnow/Kconfig \
	/home/gardin/esp/esp-mdf/components/third_party/miniz/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/mqtt/Kconfig \
	/home/gardin/esp/esp-mdf/components/mupgrade/Kconfig \
	/home/gardin/esp/esp-mdf/components/mwifi/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/nvs_flash/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/openssl/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/pthread/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/spi_flash/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/spiffs/Kconfig \
	/home/gardin/esp/esp-mdf/components/third_party/esp-aliyun/components/ssl/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/tcpip_adapter/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/vfs/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/wear_levelling/Kconfig \
	/home/gardin/esp/esp-mdf/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/gardin/esp/esp-mdf/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/gardin/Documents/dev/ESP-MDF/ble_mesh/main/Kconfig.projbuild \
	/home/gardin/esp/esp-mdf/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/gardin/esp/esp-mdf/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
