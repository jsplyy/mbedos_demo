#include "mbed-drivers/mbed.h"
#include "Simpleclient.h"
#include "lwipv4_init.h"
#include <sstream>
#include "minar/minar.h"
#include "security.h"
#include <vector>
#include "mbed-trace/mbed_trace.h"
#include "sockets/TCPListener.h"
#include "sal/socket_api.h"
#include "core-util/FunctionPointer.h"

namespace {
// const char *HTTP_SERVER_NAME = "ss.iotcent.org";
const char *HTTP_SERVER_NAME = "ss.iotcent.org";

const int HTTP_SERVER_PORT = 81;
const int RECV_BUFFER_SIZE = 600;

volatile char HTTP_PATH[] = "/test?temp=34";
const size_t HTTP_PATH_LEN = sizeof(HTTP_PATH) - 1;

/* Test related data */
const char *HTTP_OK_STR = "200 OK";
const char *HTTP_HELLO_STR = "Hello world!";
}

using namespace mbed::util;
using namespace mbed::Sockets::v0;

/* micro define */
#define ADCR_VDD (65535U) /* Maximum value when use 16b resolution */
#define V_BG (1000U)      /* BANDGAP voltage in mV (trim to 1.0V) */
#define VDD 3306
#define V_TEMP25 (716U)   /* Typical VTEMP25 in mV */
#define M (1620U)         /* Typical slope: (mV x 1000)/oC */
#define STANDARD_TEMP (25U)
/* variable */
static uint32_t adcrTemp25 = 0;        /*! Calibrated ADCR_TEMP25 */
static uint32_t adcr100m = 0;
// 
EthernetInterface eth;

Serial &output = get_stdio_serial();

// These are example resource values for the Device Object
struct MbedClientDevice device = {
    "Manufacturer_String",      // Manufacturer
    "Type_String",              // Type
    "ModelNumber_String",       // ModelNumber
    "SerialNumber_String"       // SerialNumber
};

// Instantiate the class which implements LWM2M Client API (from simpleclient.h)
MbedClient mbed_client(device);



void getADC(void)
{
	uint32_t adcValue = 0;
    // uint32_t vdd = 3306;          /*! VDD in mV */
    /* Calibrate ADCR_TEMP25: ADCR_TEMP25 = ADCR_VDD x V_TEMP25 / VDD */
    adcrTemp25 = ADCR_VDD * V_TEMP25 / VDD;
    /* ADCR_100M = ADCR_VDD x M x 100 / VDD */
    adcr100m = (ADCR_VDD * M) / (VDD * 10);

    /* measure mcu temperature. */
	AnalogIn adc(A0);
	adcValue = adc.read_temp();
	adcValue = (uint32_t)(STANDARD_TEMP - ((int32_t)adcValue - (int32_t)adcrTemp25) * 100 / (int32_t)adcr100m);
	printf("adcValue = %ld\n", adcValue);
	// return adcValue;
}

uint32_t getTemp(void)
{
	uint32_t adcValue = 0;
    // uint32_t vdd = 3306;          /*! VDD in mV */
    /* Calibrate ADCR_TEMP25: ADCR_TEMP25 = ADCR_VDD x V_TEMP25 / VDD */
    adcrTemp25 = ADCR_VDD * V_TEMP25 / VDD;
    /* ADCR_100M = ADCR_VDD x M x 100 / VDD */
    adcr100m = (ADCR_VDD * M) / (VDD * 10);

    /* measure mcu temperature. */
	AnalogIn adc(A0);
	adcValue = adc.read_temp();
	adcValue = (uint32_t)(STANDARD_TEMP - ((int32_t)adcValue - (int32_t)adcrTemp25) * 100 / (int32_t)adcr100m);
	printf("adcValue = %ld\n", adcValue);
	return adcValue;
}

class MbedHttp
{
public:
	MbedHttp(const char * domain, const uint16_t port) :
		_stream(SOCKET_STACK_LWIP_IPV4), _domain(domain), _port(port)
    {

        _error = false;
        _gothello = false;
        _got200 = false;
        _bpos = 0;
        _stream.open(SOCKET_AF_INET4);
        _stream.setOnError(TCPStream::ErrorHandler_t(this, &MbedHttp::onError));
    }

    void startTest(const char *path) {
        /* Initialize the flags */
        _got200 = false;
        _gothello = false;
        _error = false;
        _disconnected = false;
        /* Fill the request buffer */
        _bpos = snprintf(_buffer, sizeof(_buffer) - 1, "GET %s HTTP/1.1\nHost: %s\n\n", path, HTTP_SERVER_NAME);
        // _bpos = snprintf(_buffer, sizeof(_buffer) - 1, "GET %s HTTP/1.1\n", path);
        /* Connect to the server */
        printf("Starting DNS lookup for %s\r\n", _domain);
        /* Resolve the domain name: */
        socket_error_t err = _stream.resolve(_domain, TCPStream::DNSHandler_t(this, &MbedHttp::onDNS));
        _stream.error_check(err);
    }
    /**
     * Check if the test has completed.
     * @return Returns true if done, false otherwise.
     */
    bool done() {
        return _error || (_got200 && _gothello);
    }
    /**
     * Check if there was an error
     * @return Returns true if there was an error, false otherwise.
     */
    bool error() {
        return _error;
    }
protected:
    void onError(Socket *s, socket_error_t err) {
        (void) s;
        printf("MBED: Socket Error: %s (%d)\r\n", socket_strerror(err), err);
        _stream.close();
        _error = true;
        printf("{{%s}}\r\n",(error()?"failure":"success"));
        printf("{{end}}\r\n");
    }
    /**
     * On Connect handler
     * Sends the request which was generated in startTest
     */
    void onConnect(TCPStream *s) {
        char buf[16];
        _remoteAddr.fmtIPv4(buf,sizeof(buf));
        printf("Connected to %s:%d\r\n", buf, _port);
        /* Send the request */
        s->setOnReadable(TCPStream::ReadableHandler_t(this, &MbedHttp::onReceive));
        s->setOnDisconnect(TCPStream::DisconnectHandler_t(this, &MbedHttp::onDisconnect));
        printf("Sending HTTP Get Request...\r\n");
        socket_error_t err = _stream.send(_buffer, _bpos);
        s->error_check(err);
    }
    /**
     * On Receive handler
     * Parses the response from the server, to check for the HTTP 200 status code and the expected response ("Hello World!")
     */
    void onReceive(Socket *s) {
        printf("HTTP Response received.\r\n");
        _bpos = sizeof(_buffer);
        /* Read data out of the socket */
        socket_error_t err = s->recv(_buffer, &_bpos);
        if (err != SOCKET_ERROR_NONE) {
            onError(s, err);
            return;
        }
        _buffer[_bpos] = 0;
        /* Check each of the flags */
        _got200 = _got200 || strstr(_buffer, HTTP_OK_STR) != NULL;
        _gothello = _gothello || strstr(_buffer, HTTP_HELLO_STR) != NULL;
        /* Print status messages */
        printf("HTTP: Received %d chars from server\r\n", _bpos);
        printf("HTTP: Received 200 OK status ... %s\r\n", _got200 ? "[OK]" : "[FAIL]");
        printf("HTTP: Received '%s' status ... %s\r\n", HTTP_HELLO_STR, _gothello ? "[OK]" : "[FAIL]");
        printf("HTTP: Received message:\r\n\r\n");
        printf("%s", _buffer);
        _error = !(_got200 && _gothello);

        s->close();
    }
    /**
     * On DNS Handler
     * Reads the address returned by DNS, then starts the connect process.
     */
    void onDNS(Socket *s, struct socket_addr addr, const char *domain) {
        /* Check that the result is a valid DNS response */
        if (socket_addr_is_any(&addr)) {
            /* Could not find DNS entry */
            printf("Could not find DNS entry for %s", HTTP_SERVER_NAME);
            onError(s, SOCKET_ERROR_DNS_FAILED);
        } else {
            /* Start connecting to the remote host */
            char buf[16];
            _remoteAddr.setAddr(&addr);
            _remoteAddr.fmtIPv4(buf,sizeof(buf));
            printf("DNS Response Received:\r\n%s: %s\r\n", domain, buf);
            printf("Connecting to %s:%d\r\n", buf, _port);
            socket_error_t err = _stream.connect(_remoteAddr, _port, TCPStream::ConnectHandler_t(this, &MbedHttp::onConnect));

            if (err != SOCKET_ERROR_NONE) {
                onError(s, err);
            }
        }
    }
    void onDisconnect(TCPStream *s) {
        s->close();
        printf("{{%s}}\r\n",(error()?"failure":"success"));
        printf("{{end}}\r\n");
    }

protected:
    TCPStream _stream;              /**< The TCP Socket */
    const char *_domain;            /**< The domain name of the HTTP server */
    const uint16_t _port;           /**< The HTTP server port */
    char _buffer[RECV_BUFFER_SIZE]; /**< The response buffer */
    size_t _bpos;                   /**< The current offset in the response buffer */
    SocketAddr _remoteAddr;         /**< The remote address */
    volatile bool _got200;          /**< Status flag for HTTP 200 */
    volatile bool _gothello;        /**< Status flag for finding the test string */
    volatile bool _error;           /**< Status flag for an error */
    volatile bool _disconnected;

	~MbedHttp();

	/* data */
};

class TempResource
{
public:
	TempResource(){
		tempObject = M2MInterfaceFactory::create_object("3205");
		M2MObjectInstance* tempInstance = tempObject->create_object_instance();
		M2MResource* TempRes = tempInstance->create_dynamic_resource("3206","temp",M2MResourceInstance::INTEGER,true);
		TempRes->set_operation(M2MBase::GET_POST_ALLOWED);
		TempRes->set_value((uint8_t*)"42",2);
		TempRes->set_execute_function(execute_callback(this,&TempResource::updateTemperature));
	}

	M2MObject* getObject(){
		return tempObject;
	}

	uint32_t getADC(void)
	{
		uint32_t adcValue = 0;
	    // uint32_t vdd = 3306;          /*! VDD in mV */
	    /* Calibrate ADCR_TEMP25: ADCR_TEMP25 = ADCR_VDD x V_TEMP25 / VDD */
	    adcrTemp25 = ADCR_VDD * V_TEMP25 / VDD;
	    /* ADCR_100M = ADCR_VDD x M x 100 / VDD */
	    adcr100m = (ADCR_VDD * M) / (VDD * 10);

	    /* measure mcu temperature. */
		AnalogIn adc(A0);
		adcValue = adc.read_temp();
		adcValue = (uint32_t)(STANDARD_TEMP - ((int32_t)adcValue - (int32_t)adcrTemp25) * 100 / (int32_t)adcr100m);
		printf("adcValue = %ld\n", adcValue);
		return adcValue;
	}

	void updateTemperature(void *){

		M2MObjectInstance* inst = tempObject->object_instance();
		M2MResource* res = inst->resource("3206");
		currentTemp = getADC();
		printf("current temp is:%ld",currentTemp);
		stringstream ss;
		ss << currentTemp;
		std::string stringField = ss.str();
		// std::string stringField = "36";
		res->set_value((uint8_t*)stringField.c_str(), stringField.length());
	}

	~TempResource();
private:
	M2MObject* tempObject;
	uint32_t currentTemp;
	/* data */
};

MbedHttp* mbedHttp;
void push()
{
	static uint32_t count=0;
	uint32_t wendu = getTemp();
	count++;
	HTTP_PATH[HTTP_PATH_LEN-1] = wendu%10+48;
	HTTP_PATH[HTTP_PATH_LEN-2] = wendu/10%10+48;

	printf("count=%ld HTTP_PATH:%s\n", count, HTTP_PATH);

	mbedHttp = new MbedHttp(HTTP_SERVER_NAME, HTTP_SERVER_PORT);

	mbed::util::FunctionPointer1<void, const char*> fp1(mbedHttp, &MbedHttp::startTest);

	minar::Scheduler::postCallback(fp1.bind((const char*)HTTP_PATH));

}
void app_start(int, char**)
{
	output.baud(115200);
	printf("App init success!\r\n");

	eth.init();
	if(eth.connect() != 0)
	{
		printf("fail to init EthernetInterface connect\n");		
	}
	if(lwipv4_socket_init() != 0)
	{
		printf("fail to init lwipv4 socket\n");
	}
	output.printf("IPv4 address is:%s\r\n", eth.getIPAddress());
	output.printf("endpoint device name is:%s\r\n", MBED_ENDPOINT_NAME);
    
//section  mbed http
 //    /* mbed http desperate from mbed connector server */
	// mbedHttp = new MbedHttp(HTTP_SERVER_NAME, HTTP_SERVER_PORT);
 //    minar::Scheduler::postCallback(push).period(minar::milliseconds(5000));
//section mbed http

    auto tempRes = new TempResource();
    mbed_client.create_interface();
    // server = new TcpSocketServer;


	M2MSecurity* registerObject = mbed_client.create_register_object();
	M2MDevice* deviceObject = mbed_client.create_device_object();

	M2MObjectList objectList;
	objectList.push_back(deviceObject);
	objectList.push_back(tempRes->getObject());
	// objectList.push_back(tempRes->getADC());

	mbed_client.set_register_object(registerObject);

	FunctionPointer2<void, M2MSecurity*, M2MObjectList> fp(&mbed_client, &MbedClient::test_register);
	// mbed::util::FunctionPointer1<void, uint16_t> fp1(server, &TcpSocketServer::start);
	// minar::Scheduler::postCallback(fp1.bind(TCP_SERVER_PORT));


	minar::Scheduler::postCallback(fp.bind(registerObject,objectList));
	minar::Scheduler::postCallback(&mbed_client,&MbedClient::test_update_register).period(minar::milliseconds(25000));
	// // minar::Scheduler::postCallback(adc).period(minar::milliseconds(500));
	// // minar::Scheduler::postCallback().period(minar::milliseconds(500));
	// // minar::Scheduler::postCallback(getADC).period(minar::milliseconds(500));
	// mbed::util::FunctionPointer1<void, const char*> fp1(mbedHttp, &MbedHttp::startTest);
	// mbed::util::FunctionPointer1<void, const char*> fp1(mbedHttp, &MbedHttp::startTest);

	// minar::Scheduler::postCallback(fp1.bind(HTTP_PATH));

	// minar::Scheduler::postCallback(fp1.bind(HTTP_PATH)).period(minar::milliseconds(5000));


}