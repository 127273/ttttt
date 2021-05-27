 
#ifndef __MYOS__DRIVERS__DRIVER_H
#define __MYOS__DRIVERS__DRIVER_H

#include <types.h>
//#include <pci.h>
/*  这个把后面进程的内存空间抢了*/
#define MAX_DRIVERS  0x00ff  //256

namespace myos
{
    namespace drivers
    {

        
        struct PeripheralComponentInterconnectDeviceDescriptor
        {
     
            uint32_t portBase;
			uint32_t mmap;
            uint8_t InterruptLine;

            uint16_t bus;
            uint16_t device;
            uint16_t function;

            uint16_t vendor_id;
            uint16_t device_id;
            
			uint16_t Status;
			uint16_t Command;
		
            uint8_t class_id;
            uint8_t subclass_id;
            //uint8_t interface_id;
			uint8_t ProgIF;
            uint8_t revision_id;

			uint8_t BIST;
			uint8_t Headertype;/* 影响了下面一部分 */
			uint8_t LatencyTimer;
			uint8_t CacheLineSize;

			uint16_t BridgeControl;	
			uint8_t InterruptPIN;
			

            uintptr_t BaseAddress;

        } __attribute__((packed));

        class Driver
        {
        public:
            Driver();
            ~Driver();
            PeripheralComponentInterconnectDeviceDescriptor equipments;
      
            const char* name;
           
            virtual void Activate();
            virtual bool Reset();
            virtual void Deactivate();
            virtual Driver *GetDriver();
        };

        class UsefulDevices
        {
        public:
            PeripheralComponentInterconnectDeviceDescriptor equipments[MAX_DRIVERS];
            int numDrivers;

            uintptr_t driverGetBaseAddress(uint16_t vendor_id, uint16_t device_id);
            void AddDriver(PeripheralComponentInterconnectDeviceDescriptor equipment);
            uint8_t GetInterrupt(uint16_t vendor_id, uint16_t device_id);
            
            UsefulDevices();
            ~UsefulDevices();
        };

        class DriverManager
        {
        public:
            Driver *drivers[MAX_DRIVERS];
            int numDrivers;
            //  static DriverManager *drvManager;
            

        public:
            DriverManager();
    
            bool AddDriver(Driver *drv, char *name);
            bool EquipmentExist(uint16_t vendor_id, uint16_t device_id);
            int GetnumDrivers();
            int GetDriversTypeOneLocation(int type);
            void GetDriversTypeAllLocation(int type);
            uint8_t GetInterrupt(uint16_t vendor_id, uint16_t device_id);
            void ActivateAll();
            void showAllDevice();

            uintptr_t driverGetBaseAddress(uint16_t vendor_id, uint16_t device_id);

                
            };
    }
}
    
    
#endif