#include "mbed-drivers/mbed.h"
#include "Simpleclient.h"
#include "lwipv4_init.h"
#include <sstream>
#include "minar/minar.h"
#include "security.h"
#include <vector>
#include "mbed-trace/mbed_trace.h"
#include "core-util/FunctionPointer.h"
#include "FXOS8700Q.h"

using namespace mbed::util;

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
// declare
void ble_RxIrq();

I2C i2c(PTE25, PTE24);
FXOS8700QAccelerometer accel(i2c,FXOS8700CQ_SLAVE_ADDR1);

motion_data_counts_t acc_raw;
motion_data_units_t acc_data;

EthernetInterface eth;

//get stdio
Serial &output = get_stdio_serial();

//ble uses uart3


Serial& get_ble_serial()
{
    static bool ble_uart_inited = false;
    static Serial stdio_serial(PTC17, PTC16);
    if (!ble_uart_inited) {
        stdio_serial.baud(115200);
        stdio_serial.attach(&ble_RxIrq, Serial::RxIrq);
        ble_uart_inited = true;
    }
    return stdio_serial;
} 
Serial &bleUart = get_ble_serial();
void ble_RxIrq()
{
    static unsigned char buf[100]={0};
    static unsigned char i = 0;

    while(bleUart.readable())
    {

        buf[i]=bleUart.getc();

        if(buf[i]=='.')
        {
            buf[i]='\0';
            i = 0;
            printf("%s\n", buf);
            memset(buf,'\0', sizeof(char)*100);                        
            break;
        } 
        i++;               
    }
}

// These are example resource values for the Device Object
struct MbedClientDevice device = {
    "Manufacturer_String",      // Manufacturer
    "Type_String",              // Type
    "ModelNumber_String",       // ModelNumber
    "SerialNumber_String"       // SerialNumber
};

// Instantiate the class which implements LWM2M Client API (from simpleclient.h)
MbedClient mbed_client(device);

// Set up Hardware interrupt button.
InterruptIn obs_button(SW2);
InterruptIn unreg_button(SW3);

// LED Output
DigitalOut led1(LED1);


// void getADC(void)
// {
// 	uint32_t adcValue = 0;
//     // uint32_t vdd = 3306;          /*! VDD in mV */
//     /* Calibrate ADCR_TEMP25: ADCR_TEMP25 = ADCR_VDD x V_TEMP25 / VDD */
//     adcrTemp25 = ADCR_VDD * V_TEMP25 / VDD;
//     /* ADCR_100M = ADCR_VDD x M x 100 / VDD */
//     adcr100m = (ADCR_VDD * M) / (VDD * 10);

//     /* measure mcu temperature. */
// 	AnalogIn adc(A0);
// 	adcValue = adc.read_temp();
// 	adcValue = (uint32_t)(STANDARD_TEMP - ((int32_t)adcValue - (int32_t)adcrTemp25) * 100 / (int32_t)adcr100m);
// 	printf("adcValue = %ld\n", adcValue);
// 	// return adcValue;
// }

// uint32_t getTemp(void)
// {
// 	uint32_t adcValue = 0;
//     // uint32_t vdd = 3306;          /*! VDD in mV */
//     /* Calibrate ADCR_TEMP25: ADCR_TEMP25 = ADCR_VDD x V_TEMP25 / VDD */
//     adcrTemp25 = ADCR_VDD * V_TEMP25 / VDD;
//     /* ADCR_100M = ADCR_VDD x M x 100 / VDD */
//     adcr100m = (ADCR_VDD * M) / (VDD * 10);

//     /* measure mcu temperature. */
// 	AnalogIn adc(A0);
// 	adcValue = adc.read_temp();
// 	adcValue = (uint32_t)(STANDARD_TEMP - ((int32_t)adcValue - (int32_t)adcrTemp25) * 100 / (int32_t)adcr100m);
// 	printf("adcValue = %ld\n", adcValue);
// 	return adcValue;
// }

/*
 * The Led contains one property (pattern) and a function (blink).
 * When the function blink is executed, the pattern is read, and the LED
 * will blink based on the pattern.
 */
class LedResource {
public:
    LedResource() {
        // create ObjectID with metadata tag of '3201', which is 'digital output'
        led_object = M2MInterfaceFactory::create_object("3201");
        M2MObjectInstance* led_inst = led_object->create_object_instance();

        // 5853 = Multi-state output
        M2MResource* pattern_res = led_inst->create_dynamic_resource("5853", "Pattern",
            M2MResourceInstance::STRING, false);
        // read and write
        pattern_res->set_operation(M2MBase::GET_PUT_ALLOWED);
        // set initial pattern (toggle every 200ms. 7 toggles in total)
        pattern_res->set_value((const uint8_t*)"500:500:500:500:500:500:500", 27);

        // there's not really an execute LWM2M ID that matches... hmm...
        M2MResource* led_res = led_inst->create_dynamic_resource("5850", "Blink",
            M2MResourceInstance::OPAQUE, false);
        // we allow executing a function here...
        led_res->set_operation(M2MBase::POST_ALLOWED);
        // when a POST comes in, we want to execute the led_execute_callback
        led_res->set_execute_function(execute_callback(this, &LedResource::blink));
    }

    M2MObject* get_object() {
        return led_object;
    }

    void blink(void *) {
        // read the value of 'Pattern'
        M2MObjectInstance* inst = led_object->object_instance();
        M2MResource* res = inst->resource("5853");

        // values in mbed Client are all buffers, and we need a vector of int's
        uint8_t* buffIn = NULL;
        uint32_t sizeIn;
        res->get_value(buffIn, sizeIn);

        // turn the buffer into a string, and initialize a vector<int> on the heap
        std::string s((char*)buffIn, sizeIn);
        std::vector<uint32_t>* v = new std::vector<uint32_t>;
        std::stringstream ss(s);
        std::string item;
        // our pattern is something like 500:200:500, so parse that
        while (std::getline(ss, item, ':')) {
            // then convert to integer, and push to the vector
            v->push_back(atoi((const char*)item.c_str()));
        }

        output.printf("led_execute_callback pattern=%s\r\n", s.c_str());

        // do_blink is called with the vector, and starting at -1
        do_blink(v, 0);
    }

private:
    M2MObject* led_object;

    void do_blink(std::vector<uint32_t>* pattern, uint16_t position) {
        // blink the LED
        led1 = !led1;

        // up the position, if we reached the end of the vector
        if (position >= pattern->size()) {
            // free memory, and exit this function
            delete pattern;
            return;
        }

        // how long do we need to wait before the next blink?
        uint32_t delay_ms = pattern->at(position);

        // we create a FunctionPointer to this same function
        FunctionPointer2<void, std::vector<uint32_t>*, uint16_t> fp(this, &LedResource::do_blink);
        // and invoke it after `delay_ms` (upping position)
        minar::Scheduler::postCallback(fp.bind(pattern, ++position)).delay(minar::milliseconds(delay_ms));
    }
};

/*
 * The button contains one property (click count).
 * When `handle_button_click` is executed, the counter updates.
 */
class ButtonResource {
public:
    ButtonResource() {
        // create ObjectID with metadata tag of '3200', which is 'digital input'
        btn_object = M2MInterfaceFactory::create_object("3200");
        M2MObjectInstance* btn_inst = btn_object->create_object_instance();
        // create resource with ID '5501', which is digital input counter
        M2MResource* btn_res = btn_inst->create_dynamic_resource("5501", "Button",
            M2MResourceInstance::INTEGER, true /* observable */);
        // we can read this value
        btn_res->set_operation(M2MBase::GET_ALLOWED);
        // set initial value (all values in mbed Client are buffers)
        // to be able to read this data easily in the Connector console, we'll use a string
        btn_res->set_value((uint8_t*)"0", 1);
    }

    M2MObject* get_object() {
        return btn_object;
    }

    /*
     * When you press the button, we read the current value of the click counter
     * from mbed Device Connector, then up the value with one.
     */
    void handle_button_click() {
        M2MObjectInstance* inst = btn_object->object_instance();
        M2MResource* res = inst->resource("5501");

        // up counter
        counter++;

        printf("handle_button_click, new value of counter is %d\r\n", counter);

        // serialize the value of counter as a string, and tell connector
        stringstream ss;
        ss << counter;
        std::string stringified = ss.str();
        res->set_value((uint8_t*)stringified.c_str(), stringified.length());
    }

private:
    M2MObject* btn_object;
    uint16_t counter = 0;
};  

/*
 * The acce contains one property (click count).
 * When `handle_button_click` is executed, the counter updates.
 */
// class AcceResource {
// public:
//     AcceResource() {
//         // create ObjectID with metadata tag of 'location', which is 'digital input'
//         acce_object = M2MInterfaceFactory::create_object("loc");
//         M2MObjectInstance* acce_inst = acce_object->create_object_instance();
//         // create resource with ID 'x', which is digital input counter
//         M2MResource* resx = acce_inst->create_dynamic_resource("x", "8700",
//             M2MResourceInstance::INTEGER, true /* observable */);
//         // we can read this value
//         resx->set_operation(M2MBase::GET_POST_ALLOWED);
//         // set initial value (all values in mbed Client are buffers)
//         // to be able to read this data easily in the Connector console, we'll use a string
//         resx->set_value((uint8_t*)"0", 1);
//         M2MResource* resy = acce_inst->create_dynamic_resource("y", "8700",
//             M2MResourceInstance::INTEGER, true /* observable */);
//         // we can read this value
//         resy->set_operation(M2MBase::GET_POST_ALLOWED);
//         // set initial value (all values in mbed Client are buffers)
//         // to be able to read this data easily in the Connector console, we'll use a string
//         resy->set_value((uint8_t*)"0", 1);        
//     }

//     M2MObject* get_object() {
//         return acce_object;
//     }

//     /*
//      * When you press the button, we read the current value of the click counter
//      * from mbed Device Connector, then up the value with one.
//      */
//     void handle_button_click() {
//         M2MObjectInstance* inst = acce_object->object_instance();
//         M2MResource* res = inst->resource("5501");

//         // up counter
//         counter++;

//         printf("handle_button_click, new value of counter is %d\r\n", counter);

//         // serialize the value of counter as a string, and tell connector
//         stringstream ss;
//         ss << counter;
//         std::string stringified = ss.str();
//         res->set_value((uint8_t*)stringified.c_str(), stringified.length());
//     }

// private:
//     M2MObject* acce_object;
//     uint16_t counter = 0;
// };


// sample the accelerometer every second
void acceSample(void) {
    char buffer[24];
    accel.getAxis(acc_raw);
    int len = 0;

    len = snprintf(buffer,sizeof(buffer), "%d",acc_raw.x);
    printf("x: %s\r\n",buffer);

    // Y
    len = snprintf(buffer,sizeof(buffer), "%d",acc_raw.y);
    printf("y: %s\r\n",buffer);

}


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

void app_start(int, char**)
{

    bleUart.printf("uart3 printf\n");
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

    auto tempRes = new TempResource();
    // we create our button and LED resources
    auto button_resource = new ButtonResource();
    auto led_resource = new LedResource();

    // Unregister button (SW3) press will unregister endpoint from connector.mbed.com
    unreg_button.fall(&mbed_client, &MbedClient::test_unregister);

    // Observation Button (SW2) press will send update of endpoint resource values to connector
    obs_button.fall(button_resource, &ButtonResource::handle_button_click);

    mbed_client.create_interface();

	M2MSecurity* registerObject = mbed_client.create_register_object();
	M2MDevice* deviceObject = mbed_client.create_device_object();

	M2MObjectList objectList;
	objectList.push_back(deviceObject);
    objectList.push_back(button_resource->get_object());
    objectList.push_back(led_resource->get_object());
	objectList.push_back(tempRes->getObject());

	mbed_client.set_register_object(registerObject);

	FunctionPointer2<void, M2MSecurity*, M2MObjectList> fp(&mbed_client, &MbedClient::test_register);

	minar::Scheduler::postCallback(fp.bind(registerObject,objectList));
	minar::Scheduler::postCallback(&mbed_client,&MbedClient::test_update_register).period(minar::milliseconds(25000));
    // minar::Scheduler::postCallback(acceSample).period(minar::milliseconds(500));
}