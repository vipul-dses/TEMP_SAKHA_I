#include <firebaseLib.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <mbedtls/base64.h>
#include <base64.h>
#include <time.h>

#define DEBUG

// NTP server
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

const char *accessToken = "";
unsigned long lastAccessTokenGeneratedAt = 0;
const char *client_email = "firebase-adminsdk-y9kzk@dnyandases.iam.gserviceaccount.com";
const char private_key_base64[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCj+KyBecZ6gOHq\nTM13GcT8AD92CK4Top0c7KA1iLzcqUDtidVXGPehjpiMlaTyAwfYD6x5wF4BKEpb\n+79GlKo8ZRkVjpgYveuScS5DwDSzJK2jh4RyHBz+TvAQDrsqkL1jy0L+wHcBbNMA\nM0SW38whiSNgAnbqAXc08c6KkpgsaHTDDP+sJGfM//YDJLnzV5cj3GFfGnr0VpA+\n8XgURoAga6oQ3Uj8CbXV5BgvJ57mA7yewnjP9dtW7IeHAEfZGQuhWGjrTnOnXzfs\nZngklbXQP9he6JtyZ9pOtggubcgQ8AZvkde6hUtgm7zVuXBzZJqtBZfV6bstzOXG\nOlpbRAejAgMBAAECggEADivPvODpZmz9GWLJ3yE7BQyAb5MQeGH1ciqBLh9DM07t\nbZob9gsSW8QXKw9D0iaw5ttz7JHTjgYNZyW6wj7W96FFKbfJLVo8octmVDSoWVMy\nLo/nwnTrjI8F8F1Vj7gypg1LHPlFT7o4AixsIR32Zm+Tvxh3boHx64F6eD9zJ2M8\nPoobzCjdz/bPg5bviXAwaAjD3hbaIDUg4Bbl0WBYCb/i5ZJ1yjL6nKiFyohfrUDB\nzY9+sTxYfD/Q58p8E6hGvdYX718Z9w/LNHS7SK1UvjFRGQRAckOLpvSFkp9WqbFm\n0FwC+6nvYg0ykaAdrHSozRTPYQdRtJgZcFIBra7JAQKBgQDXfPzMT/UA04VPR2h3\nw7WUwai4qT2YynLPCycLUOyusSvnQzNfEr0L29Qm2mTiQXiwr50U804oEb0utv4O\nQphmB2c8lsz0ROansNcmaakqoJg3ObEbMLc8HnKOZ+WixnX/0LaECT6KN/7GdcEi\nvM0ha/UqnprXAU9bPqimauf+vwKBgQDCzEjdBTHYHLSHKs+khPqDdVAMMnxU093r\nohVd7QQPK6L8aObefd/40Wxln1RawuV8GcEKKBagzXzao2GB/P67BR0GeoP339xB\n8FqKQQsIfu3O9fgodZPJGo+QmAcAuXq54rTZIYGN/tL9qONdDf0zwHLv2EVd2ZtH\nnjFuh3rUHQKBgQCkcQEJeUriYogsw7V5D6SV7GMw6BXVb0jFryEQhaTYeuOiC6o8\nNzSExDDVpXuvHX9YdetNfY57DW5tI8AkabWaxrOFjR7CmYHbHhxRd/hcS0lde20n\n7rkl66ZH/glDeme53DAr+lH7cFfAY0QLmkB/7L6e04Id6SYB34Mb3CFGSQKBgFph\nPAFIMrspfl0kcNjNVgn+ORifp+4XquRz/GPq3zJGtcVUyqE6V9tzF08DILlHqf/t\nVxJmTTFoheBPY6NTBcEx+yiRWX99Xw08/lUdHohfk+itWOk0vE/XQfr9fmpIpHdw\nhyYjFKAUNkK4xFzda7hCN1Guz2he2VveOuMkesnFAoGBAIgYfqtKTuG3aX3JGofp\nNS8svrznCsSQy70kkQONZaXSxl3u+oTsKhaFjunyLGzspBqph1KwhKxKdmz/Qn4Y\nVoZwyHLaDC9de2tPO7L/zstjgIRr6+c+u/CKA2ljOayMjNxEMjVH2nKCXj9DEOqn\n/MzRnTs71easBx37HAR189l2\n-----END PRIVATE KEY-----\n";

const char *FCM_SERVER = "fcm.googleapis.com";
const int FCM_PORT = 443; // Use port 80 for HTTP

const char *rootCACertificate = "-----BEGIN CERTIFICATE-----\n"
                                "MIIFVzCCAz+gAwIBAgINAgPlk28xsBNJiGuiFzANBgkqhkiG9w0BAQwFADBHMQsw\n"
                                "CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n"
                                "MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw\n"
                                "MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n"
                                "Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEBAQUA\n"
                                "A4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaMf/vo\n"
                                "27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vXmX7w\n"
                                "Cl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7zUjw\n"
                                "TcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0Pfybl\n"
                                "qAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtcvfaH\n"
                                "szVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4Zor8\n"
                                "Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUspzBmk\n"
                                "MiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOORc92\n"
                                "wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYWk70p\n"
                                "aDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+DVrN\n"
                                "VjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgFlQID\n"
                                "AQABo0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E\n"
                                "FgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBAJ+qQibb\n"
                                "C5u+/x6Wki4+omVKapi6Ist9wTrYggoGxval3sBOh2Z5ofmmWJyq+bXmYOfg6LEe\n"
                                "QkEzCzc9zolwFcq1JKjPa7XSQCGYzyI0zzvFIoTgxQ6KfF2I5DUkzps+GlQebtuy\n"
                                "h6f88/qBVRRiClmpIgUxPoLW7ttXNLwzldMXG+gnoot7TiYaelpkttGsN/H9oPM4\n"
                                "7HLwEXWdyzRSjeZ2axfG34arJ45JK3VmgRAhpuo+9K4l/3wV3s6MJT/KYnAK9y8J\n"
                                "ZgfIPxz88NtFMN9iiMG1D53Dn0reWVlHxYciNuaCp+0KueIHoI17eko8cdLiA6Ef\n"
                                "MgfdG+RCzgwARWGAtQsgWSl4vflVy2PFPEz0tv/bal8xa5meLMFrUKTX5hgUvYU/\n"
                                "Z6tGn6D/Qqc6f1zLXbBwHSs09dR2CQzreExZBfMzQsNhFRAbd03OIozUhfJFfbdT\n"
                                "6u9AWpQKXCBfTkBdYiJ23//OYb2MI3jSNwLgjt7RETeJ9r/tSQdirpLsQBqvFAnZ\n"
                                "0E6yove+7u7Y/9waLd64NnHi/Hm3lCXRSHNboTXns5lndcEZOitHTtNCjv0xyBZm\n"
                                "2tIMPNuzjsmhDYAPexZ3FL//2wmUspO8IFgV6dtxQ/PeEMMA3KgqlbbC1j+Qa3bb\n"
                                "bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c\n"
                                "-----END CERTIFICATE-----";

void ntpInit()
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    while (time(nullptr) < 8 * 3600 * 2)
    {
        Serial.println("Waiting for NTP time sync...");
        delay(1000);
    }
}

String base64UrlEncode(const unsigned char *input, size_t length)
{
    String encoded = base64::encode(input, length);
    encoded.replace("+", "-");
    encoded.replace("/", "_");
    encoded.replace("=", "");
    return encoded;
}

String decodeBase64(const char *input)
{
    size_t input_len = strlen(input);
    size_t output_len = 0;
    unsigned char *output = (unsigned char *)malloc((input_len * 3) / 4);

    int ret = mbedtls_base64_decode(output, (input_len * 3) / 4, &output_len, (const unsigned char *)input, input_len);
    if (ret != 0)
    {
        Serial.printf("Failed to decode base64: -0x%04X\n", -ret);
        free(output);
        return "";
    }

    String decoded = String((char *)output);
    free(output);
    return decoded;
}

const char *createJwt()
{
    // Create header
    DynamicJsonDocument header(1024);
    header["alg"] = "RS256";
    header["typ"] = "JWT";
    String headerStr;
    serializeJson(header, headerStr);

    // Create payload
    DynamicJsonDocument payload(1024);
    payload["iss"] = client_email;
    payload["sub"] = client_email;
    payload["aud"] = "https://oauth2.googleapis.com/token";
    unsigned long now = time(nullptr);
    unsigned long exp = now + 3600;
    payload["iat"] = now;
    payload["exp"] = exp;
    payload["scope"] = "https://www.googleapis.com/auth/firebase.database https://www.googleapis.com/auth/userinfo.email https://www.googleapis.com/auth/firebase.messaging";
    String payloadStr;
    serializeJson(payload, payloadStr);

    // Base64 encode header and payload
    String encodedHeader = base64UrlEncode((const unsigned char *)headerStr.c_str(), headerStr.length());
    String encodedPayload = base64UrlEncode((const unsigned char *)payloadStr.c_str(), payloadStr.length());
    String signatureBase = encodedHeader + "." + encodedPayload;

    // Sign the JWT
    byte sha256Result[32];
    mbedtls_md_context_t ctx;
    mbedtls_pk_context pk;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    mbedtls_pk_init(&pk);
    mbedtls_md_init(&ctx);

    // Load the private key
    // String privateKeyPEM = decodeBase64(private_key_base64);
    const char *privateKey = private_key_base64;

    int pkL = strlen(privateKey);
    Serial.printf("PK Length: %d\n", pkL);
    int ret = mbedtls_pk_parse_key(&pk, (const unsigned char *)privateKey, pkL + 1, NULL, 0);
    if (ret != 0)
    {
        Serial.printf("Failed to parse key: -0x%04X\n", -ret);
        return "";
    }

    ret = mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
    if (ret != 0)
    {
        Serial.printf("Failed to setup md context: -0x%04X\n", -ret);
        return "";
    }

    ret = mbedtls_md_starts(&ctx);
    if (ret != 0)
    {
        Serial.printf("Failed to start md context: -0x%04X\n", -ret);
        return "";
    }

    ret = mbedtls_md_update(&ctx, (const unsigned char *)signatureBase.c_str(), signatureBase.length());
    if (ret != 0)
    {
        Serial.printf("Failed to update md context: -0x%04X\n", -ret);
        return "";
    }

    ret = mbedtls_md_finish(&ctx, sha256Result);
    if (ret != 0)
    {
        Serial.printf("Failed to finish md context: -0x%04X\n", -ret);
        return "";
    }

    unsigned char sig[512];
    size_t sig_len;
    ret = mbedtls_pk_sign(&pk, md_type, sha256Result, 0, sig, &sig_len, NULL, NULL);
    if (ret != 0)
    {
        Serial.printf("Failed to sign: -0x%04X\n", -ret);
        return "";
    }

    String encodedSignature = base64UrlEncode(sig, sig_len);

    mbedtls_pk_free(&pk);
    mbedtls_md_free(&ctx);

    String jwt = signatureBase + "." + encodedSignature;

    Serial.print("JWT: ");
    Serial.println(jwt);

    return jwt.c_str();
}

const char *createAccessToken(const char *jwt)
{
    HTTPClient http;
    http.begin("https://oauth2.googleapis.com/token");

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "grant_type=urn:ietf:params:oauth:grant-type:jwt-bearer&assertion=" + String(jwt);

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        Serial.println("Response: " + response);

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);
        http.end();
        String aT = doc["access_token"].as<String>();
        return aT.c_str();
    }
    else
    {
        Serial.println("Error on HTTP request: " + String(httpResponseCode));
        Serial.println(http.errorToString(httpResponseCode).c_str());
    }
    http.end();
    String blank = "";
    return blank.c_str();
}

void firebaseInit()
{
#ifndef DEBUG
    if (lastAccessTokenGeneratedAt + 3600 < time(nullptr))
    {
        return;
    }
#endif
    const char *jwt = createJwt();
    if (String(jwt) == "")
    {
        return;
    }
    accessToken = createAccessToken(jwt);
}

const char *getAccessToken()
{
    if (accessToken == "")
    {
        firebaseInit();
    }
    return accessToken;
}

String createGasLeakJsonPayload(uint8_t gasConcentration)
{
    // Create a JsonDocument with automatic memory allocation
    // ArduinoJson will determine whether to use DynamicJsonDocument or StaticJsonDocument
    DynamicJsonDocument doc(1024);

    // Create the JSON structure
    JsonObject message = doc["message"].to<JsonObject>();
    String macAddress = WiFi.macAddress();
    String sanitizedMac = "";
    for (size_t i = 0; i < macAddress.length(); ++i)
    {
        if (macAddress[i] != ':')
        {
            sanitizedMac += macAddress[i];
        }
    }
    message["topic"] = sanitizedMac;

    JsonObject notification = message["notification"].to<JsonObject>();
    notification["title"] = "Gas Leak";
    notification["body"] = gasConcentration > 2 ? "Major gas leak has been detected!" : "Minor gas leak has been detected!";

    JsonObject data = message["data"].to<JsonObject>();
    data["GC"] = String(gasConcentration);

    // Serialize JSON document to a String for printing
    String jsonPayload;
    serializeJson(doc, jsonPayload);
    Serial.println(jsonPayload);
    return jsonPayload;
}

void sendFCMMessage(String jsonPayload)
{
    firebaseInit();
    WiFiClientSecure client;
    // client.setInsecure();
    // Load root CA certificate
    client.setCACert(rootCACertificate);

    if (!client.connect(FCM_SERVER, FCM_PORT))
    {
        Serial.println("Connection to FCM failed");
        return;
    }

    // Construct the HTTP request
    String request = "POST /v1/projects/dnyandases/messages:send HTTP/1.1\r\n";
    request += "Host: fcm.googleapis.com\r\n";
    request += "Authorization: Bearer ";
    request += String(accessToken);
    request += "\r\n";
    request += "Content-Type: application/json; UTF-8\r\n";
    request += "Content-Length: ";
    request += String(jsonPayload.length());
    request += "\r\n\r\n";
    request += jsonPayload;

    // Send the request
    client.print(request);

    // Read response
    while (client.connected())
    {
        String line = client.readStringUntil('\n');
        Serial.println(line);
    }

    client.stop();
}
