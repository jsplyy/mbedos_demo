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
#define MBED_ENDPOINT_NAME "67e70b71-9460-4d1b-9035-c5eee0256f86"
 
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
"MIIB0DCCAXOgAwIBAgIEXAxN1zAMBggqhkjOPQQDAgUAMDkxCzAJBgNVBAYTAkZ\r\n"
"JMQwwCgYDVQQKDANBUk0xHDAaBgNVBAMME21iZWQtY29ubmVjdG9yLTIwMTYwHh\r\n"
"cNMTYwODE0MTAyNzUyWhcNMTYxMjMxMDYwMDAwWjCBoTFSMFAGA1UEAxNJOWEzN\r\n"
"TcyZjQtYTJhZi00ZWMyLWEyMzItY2Y3NWZlNmJmN2ExLzY3ZTcwYjcxLTk0NjAt\r\n"
"NGQxYi05MDM1LWM1ZWVlMDI1NmY4NjEMMAoGA1UECxMDQVJNMRIwEAYDVQQKEwl\r\n"
"tYmVkIHVzZXIxDTALBgNVBAcTBE91bHUxDTALBgNVBAgTBE91bHUxCzAJBgNVBA\r\n"
"YTAkZJMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE0npjkOvmvkz3GkPbNF2iT\r\n"
"ae54EhAAOJSVC7/ePKIUB5N7EH0KZp9/sI0sZQthlCDIiBLq9zlPmehgJI/VNlS\r\n"
"SjAMBggqhkjOPQQDAgUAA0kAMEYCIQC6nI7rqvqa+flig9kDTOkyjYdxh6PRdJL\r\n"
"QXgqf+iLBkgIhALFPlWoKo3isFZ7CCl2WTFRafpQegT+Dkq8vm1W0Wm1N\r\n"
"-----END CERTIFICATE-----\r\n";
 
const uint8_t KEY[] = "-----BEGIN PRIVATE KEY-----\r\n"
"MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgjID/jqFts1EgSgnv\r\n"
"RLNAPKknWoVT9pcRQvJUV4qtKwKhRANCAATSemOQ6+a+TPcaQ9s0XaJNp7ngSEAA\r\n"
"4lJULv948ohQHk3sQfQpmn3+wjSxlC2GUIMiIEur3OU+Z6GAkj9U2VJK\r\n"
"-----END PRIVATE KEY-----\r\n";
 
#endif //__SECURITY_H__