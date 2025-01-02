
#ifndef CUSTOM_ESP_SSLCLIENT_FS_H
#define CUSTOM_ESP_SSLCLIENT_FS_H

#include "../../FS_Config.h"

#if defined(ESP_SIGNER_USE_PSRAM)
#if !defined(ESP_SSLCLIENT_USE_PSRAM)
#define ESP_SSLCLIENT_USE_PSRAM
#endif
#else
#undef ESP_SSLCLIENT_USE_PSRAM
#endif

#undef ESP_SSLCLIENT_ENABLE_DEBUG
#undef ESP_SSLCLIENT_ENABLE_SSL_ERROR_STRING

#endif
