#include <firebaseLib.h>
#include <ESP_Signer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

bool firebaseInitialized = false;

SignerConfig config;

void tokenStatusCallback(TokenInfo info);

const char *client_email = "firebase-adminsdk-y9kzk@dnyandases.iam.gserviceaccount.com";
const char *project_id = "dnyandases";
const char private_key[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCro9dsaTZhuDCc\nyAphqns58wSKiN9oXCKl7OrYBbQNwm5IhD/gq2TjiWPhxPeO4kthPcx0LnUIVy6y\neisPlWRvXE6JyO2CEdAvly1Oj6AFwqddbPXeAm/Ba5VLXSClPoa0NzYMNmj0LEEc\nTezIdfdDnHCo+O4+rSj6GmSBjGdBLi9pVo48x87yQuJZ9Fy0x1l3xHuzErMQ+P4P\n7HJPxPDO5dCFNMMqCeoole00jtuVOqzHw49yQvSQx60yY0zsvP5tdWzzKfsVa37C\njrlpLYqRz8OJkYCpSEl8bexDOOFbMmOxxN0PWzmxlxg3atRprRjpTYCZv3sUbcbw\njDZGrmxvAgMBAAECggEAA1iCwwC8CCScgfneRfAaTdNgY8juPe0J2d0n6MUXsOr4\n1l4JlroQkeTkNwrtXnIn0QHw4dTkACEe7mi2PSwG1KridwodIxtwyPSz8+OV1Xbp\nZ0dP9Z9riUQtGVZ74+y3mHy5B3zqB9RzAne8ZV3zT3SVt51+m2cptNJd4i6X9qvF\nHrU6z/YGqJ/xPIfAVnJGaMhl6xQPWRP9yqF5LKJjIzoTUm4YPP1702IlbW9WvnOi\nRpOH6wzwbmV/hq4+fvW5uVtuON8sbfP0tYQQZMSjSS3O5BDmoTGlfVTpfuq0drz+\nYIaGLmqc9N8vNToVRe/F9u3mT4Aa0Ax9lu1hTl6buQKBgQDGpAznsTBkspqNCxR6\n/2b99DHHXjaHVfvnpiIz3W/5htQ/10cgX00MfcEmqas2b92vud4BDF+JDMib/Frj\nEClkUDEzO3RxvGmz/uK+B9AiliYo6MK4d8mSaRFAxKFxsOnidGFNaPrs30vIU20H\nNLpLOLHE2HatjSRHqDCFqZfhZwKBgQDdM9VrZD84BTkk0aHjElXdqBU4F/IHO0V0\nePd4o4QA4J+7qYlOaQCilBek3EuDHxNMkUrBR/Vb0VyQuW96wPp3VHF/flFhccc+\ny+7uSdAE7R2ENaPb6bMUqazcbJ3FvqGn6VgylHt6g4Ma56PtEKsVSgj/tS18o8g3\njxo3IFyPuQKBgQCo2v9VXNwEXYbfapHIVJ+aNSCzlzTHKqJuMl1UtWHgPWrjsf52\nWEzI14V/di9ioQlTva4oOfpkKAfIAWbJKg3lhf2szVeY707zgTIB0+Zr4DMkB2iv\nIrSozsOpyPxsBrVqmP+1rA3Rmadz7Ob7fyj1lIYZDOEleptihmho1teS9QKBgGWZ\nChxCNfswAvgTROy7ni6ppd1WujZyFcuSR+/kApw4j4UtMECQX9dMd7Oj7xbxbfbM\nAO7Xcy1R8tcSJM7X1fJ7GYo6ZXyBOsdv+XT++AlbMyRnUrSOGU+TQy8WwDxz+2HY\n/kZ1BmXWA6CW5ibbcgjcMVfpSlQmEa7bDmpldb4ZAoGBAKA8YDb0IfZec+jmwkM5\n5RFwbVEsqQ6acMByNRMl+geO9Pn2Lr4MZGFC8Ag4mO9ieoT9wtt3KABv0+7589Pk\n4QQNgRpcxgUE+faKK4IWK316xlT/RGCBt9pIg7uMuzH+8XpoedaBkoRnvy1Hvjws\nvhybGi3nXnuRhOXbmhr6Shhe\n-----END PRIVATE KEY-----\n";

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

void firebaseInit()
{
    /* Assign the sevice account credentials and private key (required) */
    config.service_account.data.client_email = client_email;
    config.service_account.data.project_id = project_id;
    config.service_account.data.private_key = private_key;
    /** Expired period in seconds (optional).
     * Default is 3600 sec.
     * This may not afftect the expiry time of generated access token.
     */
    config.signer.expiredSeconds = 3600;

    /* Seconds to refresh the token before expiry time (optional). Default is 60 sec.*/
    config.signer.preRefreshSeconds = 60;

    /** Assign the API scopes (required)
     * Use space or comma to separate the scope.
     */
    config.signer.tokens.scope = "https://www.googleapis.com/auth/cloud-platform, https://www.googleapis.com/auth/userinfo.email, https://www.googleapis.com/auth/firebase.messaging";

    /** Assign the callback function for token ggeneration status (optional) */
    config.token_status_callback = tokenStatusCallback;
    // To set the device time without NTP time acquisition.
    // Signer.setSystemTime(<timestamp>);

    /* Create token */
    Signer.begin(&config);
}

void tokenStatusCallback(TokenInfo info)
{
    if (info.status == esp_signer_token_status_error)
    {
        Signer.printf("Token info: type = %s, status = %s\n", Signer.getTokenType(info).c_str(), Signer.getTokenStatus(info).c_str());
        Signer.printf("Token error: %s\n", Signer.getTokenError(info).c_str());
    }
    else
    {
        Signer.printf("Token info: type = %s, status = %s\n", Signer.getTokenType(info).c_str(), Signer.getTokenStatus(info).c_str());
        if (info.status == esp_signer_token_status_ready)
        {
            Signer.printf("Token: %s\n", Signer.accessToken().c_str());
        }
    }
}

String createGasLeakJsonPayload(uint8_t gasConcentration)
{
    // Create a JsonDocument with automatic memory allocation
    // ArduinoJson will determine whether to use DynamicJsonDocument or StaticJsonDocument
    DynamicJsonDocument doc(4096);

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
    if (!firebaseInitialized)
    {
        firebaseInitialized = true;
        firebaseInit();
    }
    if (!Signer.tokenReady())
    {
        delay(100);
        sendFCMMessage(jsonPayload);
        return;
    }
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
    request += String(Signer.accessToken());
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
