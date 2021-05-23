#include <driver.h>

#include <kernel.h>
#include <memory.h>
#include <mmu_heap.h>
using namespace myos;
using namespace drivers;

Driver::Driver()
{
    
}

Driver::~Driver()
{
}
        
void Driver:: Activate()
{
}

bool Driver::Reset()
{
    return false;
}

void Driver::Deactivate()
{
}



void UsefulDevices::AddDriver(PeripheralComponentInterconnectDeviceDescriptor equipment)
{
    if (numDrivers < MAX_DRIVERS)
    {
        // drivers[numDrivers] = drv;
        /* 得到大致的pci种类 */

        equipments[numDrivers] = equipment;
        numDrivers++;
  
    }
    else
        DEBUG("DriverManaget AddDrivers fail numDrivers >= MAX_DRIVERS\n");
}

uintptr_t UsefulDevices::driverGetBaseAddress(uint16_t vendor_id, uint16_t device_id)
{
    for (int i = 0; i < numDrivers; i++)
        if ((equipments[i].device_id == device_id) && (equipments[i].vendor_id == vendor_id))
            return equipments[i].BaseAddress;

    return 0; //fail
}

uint8_t UsefulDevices::GetInterrupt(uint16_t vendor_id, uint16_t device_id)
{
    for (int i = 0; i < numDrivers; i++)
        if ((equipments[i].device_id) == device_id && equipments[i].vendor_id == vendor_id)
            return equipments[i].InterruptLine;

    return 0; //fail
}

UsefulDevices::UsefulDevices()
{
    numDrivers = 0;
}

UsefulDevices::~UsefulDevices()
{
    numDrivers = 0;
}



int DriverManager::GetnumDrivers()
{
    return numDrivers;
}

Driver* Driver::GetDriver()
{
    return NULL;
}
//DriverManager *DriverManager::drvManager = NULL;
DriverManager::DriverManager()
{
    numDrivers = 0;
    //drvManager = this;
    // for(int i = 0; i < MAX_DRIVERS; i++)
    //   drivers[i] = (Driver*)malloc(sizeof(Driver));
}

uintptr_t DriverManager::driverGetBaseAddress(uint16_t vendor_id, uint16_t device_id)
{
    for (int i = 0; i < numDrivers; i++)
        if ((drivers[i]->equipments.device_id) == device_id && drivers[i]->equipments.vendor_id == vendor_id)
            return drivers[i]->equipments.BaseAddress;

    return 0; //fail
}



int DriverManager::GetDriversTypeOneLocation(int type)
{
     for(int i = 0; i < numDrivers; i++)
        if((drivers[i]->equipments.class_id) << 8 + drivers[i]->equipments.subclass_id  == type)
            return i;

    return -1;
}

void DriverManager::GetDriversTypeAllLocation(int type)
{
    
     for(int i = 0; i < numDrivers; i++)
      //  if(driversType[i] == type)
            DEBUG("%d, ", i);
    
    DEBUG("that all Locations in Drivers\n");
    
}

uint8_t DriverManager::GetInterrupt(uint16_t vendor_id, uint16_t device_id)
{
    for (int i = 0; i < numDrivers; i++)
        if ((drivers[i]->equipments.device_id) == device_id && drivers[i]->equipments.vendor_id == vendor_id)
            return drivers[i]->equipments.InterruptLine;

    return 0; //fail
}

bool DriverManager::EquipmentExist(uint16_t vendor_id, uint16_t device_id)
{
      for(int i = 0; i < numDrivers; i++)
        if((drivers[i]->equipments.vendor_id == vendor_id) && (drivers[i]->equipments.device_id == device_id))
            return true;

        return false;
}




bool DriverManager::AddDriver(Driver* drv, char* name)
{
    if(numDrivers < MAX_DRIVERS)
    {   
        drivers[numDrivers] = drv;
          /* 得到大致的pci种类 */
        //drivers[numDrivers] = (Driver*)malloc(sizeof(Driver));  
        drivers[numDrivers]->name = name;
        numDrivers++;
        return true;
    }
    else DEBUG("DriverManaget AddDrivers fail numDrivers >= MAX_DRIVERS\n");
    return false;
}

void DriverManager::ActivateAll()
{
    DEBUG("\nactivate all \n");
    for (int i = 0; i < numDrivers; i++)
        {
       //     DEBUG("i:%x\t %x \t", i, drivers[i]->equipments.BaseAddress);
           
                drivers[i]->Activate();
        }
   
}

void DriverManager::showAllDevice()
{
    for(int i = 0; i < numDrivers; i++)
    {
    
        DEBUG("0x%x", drivers[i]->equipments.class_id);
    }
}