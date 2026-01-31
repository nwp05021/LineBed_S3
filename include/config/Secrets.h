#pragma once
#include <Arduino.h>

static const char* AWS_IOT_ENDPOINT = "CHANGE_ME-ats.iot.ap-northeast-2.amazonaws.com";
static const uint16_t AWS_IOT_PORT = 8883;

static const char* AWS_ROOT_CA_PEM = R"EOF(
-----BEGIN CERTIFICATE-----
CHANGE_ME
-----END CERTIFICATE-----
)EOF";

static const char* AWS_DEVICE_CERT_PEM = R"EOF(
-----BEGIN CERTIFICATE-----
CHANGE_ME
-----END CERTIFICATE-----
)EOF";

static const char* AWS_PRIVATE_KEY_PEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
CHANGE_ME
-----END RSA PRIVATE KEY-----
)EOF";
