/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __SECURITY_H__
#define __SECURITY_H__
 
#include <inttypes.h>
 
#define MBED_DOMAIN "9a3572f4-a2af-4ec2-a232-cf75fe6bf7a1"
#define MBED_ENDPOINT_NAME "32c68efc-0171-414a-b9ed-39ab0c38cc8e"
 
const uint8_t SERVER_CERT[] = "-----BEGIN CERTIFICATE-----\r\n"
"MIIBmDCCAT6gAwIBAgIEVUCA0jAKBggqhkjOPQQDAjBLMQswCQYDVQQGEwJGSTEN\r\n"
"MAsGA1UEBwwET3VsdTEMMAoGA1UECgwDQVJNMQwwCgYDVQQLDANJb1QxETAPBgNV\r\n"
"BAMMCEFSTSBtYmVkMB4XDTE1MDQyOTA2NTc0OFoXDTE4MDQyOTA2NTc0OFowSzEL\r\n"
"MAkGA1UEBhMCRkkxDTALBgNVBAcMBE91bHUxDDAKBgNVBAoMA0FSTTEMMAoGA1UE\r\n"
"CwwDSW9UMREwDwYDVQQDDAhBUk0gbWJlZDBZMBMGByqGSM49AgEGCCqGSM49AwEH\r\n"
"A0IABLuAyLSk0mA3awgFR5mw2RHth47tRUO44q/RdzFZnLsAsd18Esxd5LCpcT9w\r\n"
"0tvNfBv4xJxGw0wcYrPDDb8/rjujEDAOMAwGA1UdEwQFMAMBAf8wCgYIKoZIzj0E\r\n"
"AwIDSAAwRQIhAPAonEAkwixlJiyYRQQWpXtkMZax+VlEiS201BG0PpAzAiBh2RsD\r\n"
"NxLKWwf4O7D6JasGBYf9+ZLwl0iaRjTjytO+Kw==\r\n"
"-----END CERTIFICATE-----\r\n";
 
const uint8_t CERT[] = "-----BEGIN CERTIFICATE-----\r\n"
"MIIBzzCCAXOgAwIBAgIEXPjyCjAMBggqhkjOPQQDAgUAMDkxCzAJBgNVBAYTAkZ\r\n"
"JMQwwCgYDVQQKDANBUk0xHDAaBgNVBAMME21iZWQtY29ubmVjdG9yLTIwMTYwHh\r\n"
"cNMTYwODMwMDEzMjM5WhcNMTYxMjMxMDYwMDAwWjCBoTFSMFAGA1UEAxNJOWEzN\r\n"
"TcyZjQtYTJhZi00ZWMyLWEyMzItY2Y3NWZlNmJmN2ExLzMyYzY4ZWZjLTAxNzEt\r\n"
"NDE0YS1iOWVkLTM5YWIwYzM4Y2M4ZTEMMAoGA1UECxMDQVJNMRIwEAYDVQQKEwl\r\n"
"tYmVkIHVzZXIxDTALBgNVBAcTBE91bHUxDTALBgNVBAgTBE91bHUxCzAJBgNVBA\r\n"
"YTAkZJMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEuvcEJgdUiQNEFpB6hGQE2\r\n"
"IE2E497TQdO8ZCGUMrgYWdX1UzWsze9Hj7gi5x4ayJN/uyN17xFz1YwbM/p0zCa\r\n"
"PzAMBggqhkjOPQQDAgUAA0gAMEUCIAVQhCcKTjaNzJjIcPyIJQq9k21c9kn4pVT\r\n"
"b00rPymavAiEA0Yrbg3taPGIIViGe2lOizeJCpuK7US7E1OXCRwYW0aQ=\r\n"
"-----END CERTIFICATE-----\r\n";
 
const uint8_t KEY[] = "-----BEGIN PRIVATE KEY-----\r\n"
"MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgKHGf4K5Rnn681kZU\r\n"
"AaUwxy2FS6yyMfkwkhoR4A7zM3KhRANCAAS69wQmB1SJA0QWkHqEZATYgTYTj3tN\r\n"
"B07xkIZQyuBhZ1fVTNazN70ePuCLnHhrIk3+7I3XvEXPVjBsz+nTMJo/\r\n"
"-----END PRIVATE KEY-----\r\n";
 
#endif //__SECURITY_H__