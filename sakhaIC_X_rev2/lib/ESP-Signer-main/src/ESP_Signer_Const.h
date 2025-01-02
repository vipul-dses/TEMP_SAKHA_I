
#ifndef ESP_SIGNER_CONST_H
#define ESP_SIGNER_CONST_H

#define ESP_SIGNER_DEFAULT_AUTH_TOKEN_PRE_REFRESH_SECONDS 1 * 60

#define ESP_SIGNER_DEFAULT_AUTH_TOKEN_EXPIRED_SECONDS 3600

#define ESP_SIGNER_DEFAULT_REQUEST_TIMEOUT 2000

#define ESP_SIGNER_DEFAULT_TS 1618971013

#define ESP_SIGNER_TIME_SYNC_INTERVAL 5000

#define ESP_SIGNER_MIN_TOKEN_GENERATION_ERROR_INTERVAL 5 * 1000

#define ESP_SIGNER_MIN_NTP_SERVER_SYNC_TIME_OUT 15 * 1000

#define ESP_SIGNER_MIN_TOKEN_GENERATION_BEGIN_STEP_INTERVAL 300

#define ESP_SIGNER_MIN_SERVER_RESPONSE_TIMEOUT 1 * 1000
#define ESP_SIGNER_DEFAULT_SERVER_RESPONSE_TIMEOUT 5 * 1000
#define ESP_SIGNER_MAX_SERVER_RESPONSE_TIMEOUT 60 * 1000

#define ESP_SIGNER_MIN_WIFI_RECONNECT_TIMEOUT 10 * 1000
#define ESP_SIGNER_MAX_WIFI_RECONNECT_TIMEOUT 5 * 60 * 1000

#include <Arduino.h>
#include "mbfs/MB_MCU.h"
#include "ESP_Signer_Error.h"
#include "ESP_Signer_Network.h"
#if __has_include(<FS.h>)
#include <FS.h>
#endif

#if defined(ESP32) && !defined(ESP_ARDUINO_VERSION) /* ESP32 core < v2.0.x */
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "FS_Config.h"
#include "mbfs/MB_FS.h"
#if defined(ESP32)
#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#endif

#if defined(ESP8266)

//__GNUC__
//__GNUC_MINOR__
//__GNUC_PATCHLEVEL__

#ifdef __GNUC__
#if __GNUC__ > 4 || __GNUC__ == 10
#include <string>
#define ESP8266_CORE_SDK_V3_X_X
#endif
#endif

#if __has_include(<core_esp8266_version.h>)
#include <core_esp8266_version.h>
#endif

#endif

#if defined __has_include

#if __has_include(<LwipIntfDev.h>)
#include <LwipIntfDev.h>
#endif

#if __has_include(<ENC28J60lwIP.h>) && defined(ESP_SIGNER_ENABLE_ESP8266_ENC28J60_ETH)
#define INC_ENC28J60_LWIP
#include <ENC28J60lwIP.h>
#define ESP_SIGNER_ESP8266_SPI_ETH_MODULE ENC28J60lwIP
#endif

#if __has_include(<W5100lwIP.h>) && defined(ESP_SIGNER_ENABLE_ESP8266_W5100_ETH)

#define INC_W5100_LWIP
// PIO compilation error
#include <W5100lwIP.h>
#define ESP_SIGNER_ESP8266_SPI_ETH_MODULE Wiznet5100lwIP
#endif

#if __has_include(<W5500lwIP.h>) && defined(ESP_SIGNER_ENABLE_ESP8266_W5500_ETH)
#define INC_W5500_LWIP
#include <W5500lwIP.h>
#define ESP_SIGNER_ESP8266_SPI_ETH_MODULE Wiznet5500lwIP
#endif

#endif

typedef enum
{
    esp_google_sheet_file_storage_type_undefined,
    esp_google_sheet_file_storage_type_flash,
    esp_google_sheet_file_storage_type_sd
} esp_google_sheet_file_storage_type;

typedef enum
{
    esp_signer_tcp_client_type_undefined,
    esp_signer_tcp_client_type_internal,
    esp_signer_tcp_client_type_external

} esp_signer_tcp_client_type;

typedef enum
{
    esp_signer_cert_type_undefined = -1,
    esp_signer_cert_type_none = 0,
    esp_signer_cert_type_data,
    esp_signer_cert_type_file

} esp_signer_cert_type;

enum esp_signer_gauth_auth_token_status
{
    esp_signer_token_status_uninitialized,
    esp_signer_token_status_on_initialize,
    esp_signer_token_status_on_signing,
    esp_signer_token_status_on_request,
    esp_signer_token_status_on_refresh,
    esp_signer_token_status_ready,
    esp_signer_token_status_error
};

enum esp_signer_gauth_auth_token_type
{
    token_type_undefined,
    token_type_oauth2_access_token,
    token_type_refresh_token
};

enum esp_signer_gauth_jwt_generation_step
{
    esp_signer_gauth_jwt_generation_step_begin,
    esp_signer_gauth_jwt_generation_step_encode_header_payload,
    esp_signer_gauth_jwt_generation_step_sign,
    esp_signer_gauth_jwt_generation_step_exchange
};

enum esp_signer_request_method
{
    http_undefined,
    http_put,
    http_post,
    http_get,
    http_patch,
    http_delete
};

typedef enum
{
    esp_signer_mem_storage_type_undefined,
    esp_signer_mem_storage_type_flash,
    esp_signer_mem_storage_type_sd
} esp_signer_mem_storage_type;

struct esp_signer_no_eth_module_t
{
};

#ifndef ESP_SIGNER_ESP8266_SPI_ETH_MODULE
#define ESP_SIGNER_ESP8266_SPI_ETH_MODULE esp_signer_no_eth_module_t
#endif

struct esp_signer_wifi_credential_t
{
    MB_String ssid;
    MB_String password;
};

struct esp_signer_url_info_t
{
    MB_String host;
    MB_String uri;
    MB_String auth;
};

struct esp_signer_gauth_service_account_data_info_t
{
    MB_String client_email;
    MB_String client_id;
    MB_String project_id;
    MB_String private_key_id;
    const char *private_key = "";
};

struct esp_signer_gauth_service_account_file_info_t
{
    MB_String path;
    esp_signer_mem_storage_type storage_type = esp_signer_mem_storage_type_flash;
};

struct esp_signer_gauth_service_account_t
{
    struct esp_signer_gauth_service_account_data_info_t data;
    struct esp_signer_gauth_service_account_file_info_t json;
};

struct esp_signer_gauth_auth_token_error_t
{
    MB_String message;
    int code = 0;
};

struct esp_signer_gauth_auth_token_info_t
{
    MB_String auth_type;
    MB_String jwt;
    MB_String scope;
    unsigned long expires = 0;
    /* milliseconds count when last expiry time was set */
    unsigned long last_millis = 0;
    esp_signer_gauth_auth_token_type token_type = token_type_undefined;
    esp_signer_gauth_auth_token_status status = esp_signer_token_status_uninitialized;
    struct esp_signer_gauth_auth_token_error_t error;
};

typedef struct esp_signer_gauth_token_info_t
{
    esp_signer_gauth_auth_token_type type = token_type_undefined;
    esp_signer_gauth_auth_token_status status = esp_signer_token_status_uninitialized;
    struct esp_signer_gauth_auth_token_error_t error;
} TokenInfo;

struct esp_signer_gauth_token_signer_resources_t
{
    int step = 0;
    bool tokenTaskRunning = false;
    /* last token request milliseconds count */
    unsigned long lastReqMillis = 0;
    unsigned long preRefreshSeconds = ESP_SIGNER_DEFAULT_AUTH_TOKEN_PRE_REFRESH_SECONDS;
    unsigned long expiredSeconds = ESP_SIGNER_DEFAULT_AUTH_TOKEN_EXPIRED_SECONDS;
    /* request time out period (interval) */
    unsigned long reqTO = ESP_SIGNER_DEFAULT_REQUEST_TIMEOUT;
    MB_String customHeaders;
    MB_String pk;
    size_t hashSize = 32; // SHA256 size (256 bits or 32 bytes)
    size_t signatureSize = 256;

    char *hash = nullptr;
    unsigned char *signature = nullptr;
    MB_String encHeader;
    MB_String encPayload;
    MB_String encHeadPayload;
    MB_String encSignature;

    esp_signer_gauth_auth_token_info_t tokens;
};

typedef void (*TokenStatusCallback)(TokenInfo);

struct esp_signer_chunk_state_info
{
    int state = 0;
    int chunkedSize = 0;
    int dataLen = 0;
};

struct esp_signer_tcp_response_handler_t
{
    // the chunk index of all data that is being process
    int chunkIdx = 0;
    // the payload chunk index that is being process
    int pChunkIdx = 0;
    // the total bytes of http response payload to read
    int payloadLen = 0;
    // the total bytes of base64 decoded data from response payload
    int decodedPayloadLen = 0;
    // the current size of chunk data to read from client
    int chunkBufSize = 0;
    // the amount of http response payload that read,
    // compare with the content length header value for finishing check
    int payloadRead = 0;
    // status showed that the http headers was found and is being read
    bool isHeader = false;
    // status showed that the http headers was completely read
    bool headerEnded = false;
    // the prefered size of chunk data to read from client
    size_t defaultChunkSize = 0;
    // keep the auth token generation error
    struct esp_signer_gauth_auth_token_error_t error;
    // keep the http header or the first line of stream event data
    MB_String header;
    // time out checking for execution
    unsigned long dataTime = 0;
    // pointer to payload
    MB_String *payload = nullptr;
    // data is already in receive buffer (must be int)
    int bufferAvailable = 0;
    // data in receive buffer is base64 file data
    bool isBase64File = false;
    // the base64 encoded string downloaded anount
    int downloadByteLen = 0;
    // pad (=) length checking from tail of encoded string of file/blob data
    int base64PadLenTail = 0;
    // pad (=) length checking from base64 encoded string signature (begins with "file,base64, and "blob,base64,)
    // of file/blob data
    int base64PadLenSignature = 0;
    // the tcp client pointer
    Client *client = nullptr;
    // the chunk state info
    esp_signer_chunk_state_info chunkState;

public:
    int available()
    {
        if (client)
            return client->available();
        return false;
    }
};

struct esp_signer_server_response_data_t
{
    int httpCode = 0;
    // Must not be negative
    int payloadLen = 0;
    // The response content length, must not be negative as it uses to determine
    // the available data to read in event-stream
    // and content length specific read in http response
    int contentLen = 0;
    int chunkRange = 0;
    bool redirect = false;
    bool isChunkedEnc = false;
    bool noContent = false;
    MB_String location;
    MB_String contentType;
    MB_String connection;
    MB_String eventPath;
    MB_String eventType;
    MB_String eventData;
    MB_String etag;
    MB_String pushName;
    MB_String fbError;
    MB_String transferEnc;
};

template <typename T>
struct esp_signer_base64_io_t
{
    // the total bytes of data in output buffer
    int bufWrite = 0;
    // the size of output buffer
    size_t bufLen = 1024;
    // for file, the type of filesystem to write
    mbfs_file_type filetype = mb_fs_mem_storage_type_undefined;
    // for T array
    T *outT = nullptr;
    // for T vector
    MB_VECTOR<T> *outL = nullptr;
    // for client
    Client *outC = nullptr;
};

struct esp_signer_gauth_auth_cert_t
{
    const char *data = "";
    MB_String file;
    esp_signer_mem_storage_type file_storage = esp_signer_mem_storage_type_flash;
};

struct esp_signer_gauth_cfg_int_t
{
    bool processing = false;
    bool rtoken_requested = false;

    bool reconnect_wifi = false;
    unsigned long last_reconnect_millis = 0;
    unsigned long last_jwt_begin_step_millis = 0;
    unsigned long last_jwt_generation_error_cb_millis = 0;
    unsigned long last_request_token_cb_millis = 0;
    unsigned long last_stream_timeout_cb_millis = 0;
    unsigned long last_time_sync_millis = 0;
    unsigned long last_ntp_sync_timeout_millis = 0;
    bool clock_rdy = false;
    uint16_t email_crc = 0, password_crc = 0, client_email_crc = 0, project_id_crc = 0, priv_key_crc = 0;

    /* flag set when NTP time server synching has been started */
    bool clock_synched = false;
    float gmt_offset = 0;
    bool auth_uri = false;

    MB_String auth_token;
    MB_String refresh_token;
    MB_String client_id;
    MB_String client_secret;
};

struct esp_signer_gauth_client_timeout_t
{
    // WiFi reconnect timeout (interval) in ms (10 sec - 5 min) when WiFi disconnected.
    uint16_t wifiReconnect = 10 * 1000;

    // Socket connection and ssl handshake timeout in ms (1 sec - 1 min).
    unsigned long socketConnection = 10 * 1000;

    // unused.
    unsigned long sslHandshake = 0;

    // Server response read timeout in ms (1 sec - 1 min).
    unsigned long serverResponse = 10 * 1000;

    uint16_t tokenGenerationBeginStep = 300;

    uint16_t tokenGenerationError = 5 * 1000;

    uint16_t ntpServerRequest = 15 * 1000;
};

typedef struct esp_signer_gauth_spi_ethernet_module_t
{
#if defined(ESP8266) && defined(ESP8266_CORE_SDK_V3_X_X)
#ifdef INC_ENC28J60_LWIP
    ENC28J60lwIP *enc28j60 = nullptr;
#endif
#ifdef INC_W5100_LWIP
    Wiznet5100lwIP *w5100 = nullptr;
#endif
#ifdef INC_W5500_LWIP
    Wiznet5500lwIP *w5500 = nullptr;
#endif
#elif defined(INC_CYW43_LWIP)

#endif
} SPI_ETH_Module;

class esp_signer_wifi
{
    friend class GAuth_TCP_Client;

public:
    esp_signer_wifi(){};
    ~esp_signer_wifi()
    {
        clearAP();
        clearMulti();
    };
    void addAP(const String &ssid, const String &password)
    {
        esp_signer_wifi_credential_t data;
        data.ssid = ssid;
        data.password = password;
        credentials.push_back(data);
    }
    void clearAP()
    {
        credentials.clear();
    }
    size_t size() { return credentials.size(); }

    esp_signer_wifi_credential_t operator[](size_t index)
    {
        return credentials[index];
    }

private:
    MB_List<esp_signer_wifi_credential_t> credentials;
#if defined(ESP_SIGNER_HAS_WIFIMULTI)
    WiFiMulti *multi = nullptr;
#endif
    void reconnect()
    {
        if (credentials.size())
        {
            disconnect();
            connect();
        }
    }

    void connect()
    {
#if defined(ESP_SIGNER_HAS_WIFIMULTI)

        clearMulti();
        multi = new WiFiMulti();
        for (size_t i = 0; i < credentials.size(); i++)
            multi->addAP(credentials[i].ssid.c_str(), credentials[i].password.c_str());

        if (credentials.size() > 0)
            multi->run();

#elif defined(ESP_SIGNER_WIFI_IS_AVAILABLE)
        WiFi.begin((CONST_STRING_CAST)credentials[0].ssid.c_str(), credentials[0].password.c_str());
#endif
    }

    void disconnect()
    {
#if defined(ESP_SIGNER_WIFI_IS_AVAILABLE)
        WiFi.disconnect();
#endif
    }

    void clearMulti()
    {
#if defined(ESP_SIGNER_HAS_WIFIMULTI)
        if (multi)
            delete multi;
        multi = nullptr;
#endif
    }
};

struct esp_signer_gauth_cfg_t
{
    uint32_t mb_ts = 0;

    struct esp_signer_gauth_service_account_t service_account;
    float time_zone = 0;
    struct esp_signer_gauth_auth_cert_t cert;
    struct esp_signer_gauth_token_signer_resources_t signer;
    struct esp_signer_gauth_cfg_int_t internal;
    TokenStatusCallback token_status_callback = NULL;
    esp_signer_gauth_spi_ethernet_module_t spi_ethernet_module;
    struct esp_signer_gauth_client_timeout_t timeout;

    MB_String api_key;
    MB_String client_id;
    MB_String client_secret;

    esp_signer_gauth_token_info_t tokenInfo;
    esp_signer_gauth_auth_token_error_t error;

    struct esp_signer_wifi wifi;
};

#if !defined(__AVR__)
typedef std::function<void(void)> callback_function_t;
#endif

typedef void (*ESP_Signer_NetworkConnectionRequestCallback)(void);
typedef void (*ESP_Signer_NetworkStatusRequestCallback)(void);
typedef void (*ESP_Signer_ResponseCallback)(const char *);

typedef esp_signer_gauth_cfg_t SignerConfig;

static const unsigned char esp_signer_base64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char esp_signer_gauth_pgm_str_1[] PROGMEM = "type";
static const char esp_signer_gauth_pgm_str_2[] PROGMEM = "service_account";
static const char esp_signer_gauth_pgm_str_3[] PROGMEM = "project_id";
static const char esp_signer_gauth_pgm_str_4[] PROGMEM = "private_key_id";
static const char esp_signer_gauth_pgm_str_5[] PROGMEM = "private_key";
static const char esp_signer_gauth_pgm_str_6[] PROGMEM = "client_email";
static const char esp_signer_gauth_pgm_str_7[] PROGMEM = "client_id";
static const char esp_signer_gauth_pgm_str_8[] PROGMEM = "securetoken";
static const char esp_signer_gauth_pgm_str_9[] PROGMEM = "grantType";
static const char esp_signer_gauth_pgm_str_10[] PROGMEM = "refresh_token";
static const char esp_signer_gauth_pgm_str_11[] PROGMEM = "refreshToken";
static const char esp_signer_gauth_pgm_str_12[] PROGMEM = "/v1/token?Key=";
static const char esp_signer_gauth_pgm_str_13[] PROGMEM = "application/json";
static const char esp_signer_gauth_pgm_str_14[] PROGMEM = "error/code";
static const char esp_signer_gauth_pgm_str_15[] PROGMEM = "error/message";
static const char esp_signer_gauth_pgm_str_16[] PROGMEM = "id_token";
static const char esp_signer_gauth_pgm_str_18[] PROGMEM = "refresh_token";
static const char esp_signer_gauth_pgm_str_19[] PROGMEM = "expires_in";
static const char esp_signer_gauth_pgm_str_20[] PROGMEM = "alg";
static const char esp_signer_gauth_pgm_str_21[] PROGMEM = "RS256";
static const char esp_signer_gauth_pgm_str_22[] PROGMEM = "typ";
static const char esp_signer_gauth_pgm_str_23[] PROGMEM = "JWT";
static const char esp_signer_gauth_pgm_str_24[] PROGMEM = "iss";
static const char esp_signer_gauth_pgm_str_25[] PROGMEM = "sub";
static const char esp_signer_gauth_pgm_str_26[] PROGMEM = "https://";
static const char esp_signer_gauth_pgm_str_27[] PROGMEM = "oauth2";
static const char esp_signer_gauth_pgm_str_28[] PROGMEM = "/";
static const char esp_signer_gauth_pgm_str_29[] PROGMEM = "token";
static const char esp_signer_gauth_pgm_str_30[] PROGMEM = "aud";
static const char esp_signer_gauth_pgm_str_31[] PROGMEM = "iat";
static const char esp_signer_gauth_pgm_str_32[] PROGMEM = "exp";
static const char esp_signer_gauth_pgm_str_33[] PROGMEM = "scope";
static const char esp_signer_gauth_pgm_str_34[] PROGMEM = "https://www.googleapis.com/auth/drive.metadata https://www.googleapis.com/auth/drive.appdata https://www.googleapis.com/auth/spreadsheets https://www.googleapis.com/auth/drive https://www.googleapis.com/auth/drive.file";
static const char esp_signer_gauth_pgm_str_35[] PROGMEM = ".";
static const char esp_signer_gauth_pgm_str_36[] PROGMEM = "www";
static const char esp_signer_gauth_pgm_str_37[] PROGMEM = "client_secret";
static const char esp_signer_gauth_pgm_str_38[] PROGMEM = "grant_type";
static const char esp_signer_gauth_pgm_str_39[] PROGMEM = "urn:ietf:params:oauth:grant-type:jwt-bearer";
static const char esp_signer_gauth_pgm_str_40[] PROGMEM = "assertion";
static const char esp_signer_gauth_pgm_str_41[] PROGMEM = "oauth2";
static const char esp_signer_gauth_pgm_str_42[] PROGMEM = "error";
static const char esp_signer_gauth_pgm_str_43[] PROGMEM = "error_description";
static const char esp_signer_gauth_pgm_str_44[] PROGMEM = "access_token";
static const char esp_signer_gauth_pgm_str_45[] PROGMEM = "Bearer ";
static const char esp_signer_gauth_pgm_str_46[] PROGMEM = "https://www.googleapis.com/auth/cloud-platform";

static const char esp_signer_pgm_str_1[] PROGMEM = "\r\n";
static const char esp_signer_pgm_str_2[] PROGMEM = ".";
static const char esp_signer_pgm_str_3[] PROGMEM = "googleapis.com";
static const char esp_signer_pgm_str_4[] PROGMEM = "Host: ";
static const char esp_signer_pgm_str_5[] PROGMEM = "Content-Type: ";
static const char esp_signer_pgm_str_6[] PROGMEM = "Content-Length: ";
static const char esp_signer_pgm_str_7[] PROGMEM = "User-Agent: ESP\r\n";
static const char esp_signer_pgm_str_8[] PROGMEM = "Connection: keep-alive\r\n";
static const char esp_signer_pgm_str_9[] PROGMEM = "Connection: close\r\n";
static const char esp_signer_pgm_str_10[] PROGMEM = "GET";
static const char esp_signer_pgm_str_11[] PROGMEM = "POST";
static const char esp_signer_pgm_str_12[] PROGMEM = "PATCH";
static const char esp_signer_pgm_str_13[] PROGMEM = "DELETE";
static const char esp_signer_pgm_str_14[] PROGMEM = "PUT";
static const char esp_signer_pgm_str_15[] PROGMEM = " ";
static const char esp_signer_pgm_str_16[] PROGMEM = " HTTP/1.1\r\n";
static const char esp_signer_pgm_str_17[] PROGMEM = "Authorization: ";
static const char esp_signer_pgm_str_18[] PROGMEM = "Bearer ";
static const char esp_signer_pgm_str_19[] PROGMEM = "true";
static const char esp_signer_pgm_str_20[] PROGMEM = "Connection: ";
static const char esp_signer_pgm_str_21[] PROGMEM = "Content-Type: ";
static const char esp_signer_pgm_str_22[] PROGMEM = "Content-Length: ";
static const char esp_signer_pgm_str_23[] PROGMEM = "ETag: ";
static const char esp_signer_pgm_str_24[] PROGMEM = "Transfer-Encoding: ";
static const char esp_signer_pgm_str_25[] PROGMEM = "chunked";
static const char esp_signer_pgm_str_26[] PROGMEM = "Location: ";
static const char esp_signer_pgm_str_27[] PROGMEM = "HTTP/1.1 ";
static const char esp_signer_pgm_str_28[] PROGMEM = "?";
static const char esp_signer_pgm_str_29[] PROGMEM = "&";
static const char esp_signer_pgm_str_30[] PROGMEM = "=";
static const char esp_signer_pgm_str_31[] PROGMEM = "/";
static const char esp_signer_pgm_str_32[] PROGMEM = "https://";
static const char esp_signer_pgm_str_33[] PROGMEM = "https://%[^/]/%s";
static const char esp_signer_pgm_str_34[] PROGMEM = "http://%[^/]/%s";
static const char esp_signer_pgm_str_35[] PROGMEM = "%[^/]/%s";
static const char esp_signer_pgm_str_36[] PROGMEM = "%[^?]?%s";
static const char esp_signer_pgm_str_37[] PROGMEM = "auth=";
static const char esp_signer_pgm_str_38[] PROGMEM = "%[^&]";
static const char esp_signer_pgm_str_39[] PROGMEM = "undefined";
static const char esp_signer_pgm_str_40[] PROGMEM = "OAuth2.0 access token";
static const char esp_signer_pgm_str_41[] PROGMEM = "uninitialized";
static const char esp_signer_pgm_str_42[] PROGMEM = "on initializing";
static const char esp_signer_pgm_str_43[] PROGMEM = "on signing";
static const char esp_signer_pgm_str_44[] PROGMEM = "on exchange request";
static const char esp_signer_pgm_str_45[] PROGMEM = "on refreshing";
static const char esp_signer_pgm_str_46[] PROGMEM = "error";
static const char esp_signer_pgm_str_47[] PROGMEM = "code: ";
static const char esp_signer_pgm_str_48[] PROGMEM = ", message: ";
static const char esp_signer_pgm_str_49[] PROGMEM = "ready";

#endif