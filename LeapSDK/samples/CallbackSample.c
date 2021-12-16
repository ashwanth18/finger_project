/* Copyright (C) 2012-2017 Ultraleap Limited. All rights reserved.
 *
 * Use of this code is subject to the terms of the Ultraleap SDK agreement
 * available at https://central.leapmotion.com/agreements/SdkAgreement unless
 * Ultraleap has signed a separate license agreement with you or your
 * organisation.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "LeapC.h"
#include "ExampleConnection.h"
#include <windows.h>
#include <conio.h>
#include<string.h>



static LEAP_CONNECTION* connectionHandle;

int Send_data(char *val)
{
    printf(val);
    HANDLE hComm;                       // Handle to the Serial port
    char ComPortName[] = "\\\\.\\COM5"; // Name of the Serial port(May Change) to be opened,
    BOOL Status;

    printf("\n\n +==========================================+");
    printf("\n |  Serial Transmission (Win32 API)         |");
    printf("\n +==========================================+\n");
    /*----------------------------------- Opening the Serial Port --------------------------------------------*/

    hComm = CreateFile(ComPortName,                  // Name of the Port to be Opened
        GENERIC_READ | GENERIC_WRITE, // Read/Write Access
        0,                            // No Sharing, ports cant be shared
        NULL,                         // No Security
        OPEN_EXISTING,                // Open existing port only
        0,                            // Non Overlapped I/O
        NULL);                        // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE)
        printf("\n   Error! - Port %s can't be opened", ComPortName);
    else
        printf("\n   Port %s Opened\n ", ComPortName);

    /*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/

    DCB dcbSerialParams = { 0 }; // Initializing DCB structure
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    Status = GetCommState(hComm, &dcbSerialParams); //retreives  the current settings

    if (Status == FALSE)
        printf("\n   Error! in GetCommState()");

    dcbSerialParams.BaudRate = CBR_9600;   // Setting BaudRate = 9600
    dcbSerialParams.ByteSize = 8;          // Setting ByteSize = 8
    dcbSerialParams.StopBits = ONESTOPBIT; // Setting StopBits = 1
    dcbSerialParams.Parity = NOPARITY;     // Setting Parity = None

    Status = SetCommState(hComm, &dcbSerialParams); //Configuring the port according to settings in DCB

    if (Status == FALSE)
    {
        printf("\n   Error! in Setting DCB Structure");
    }
    else
    {
        printf("\n   Setting DCB Structure Successfull\n");
        printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
        printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
        printf("\n       StopBits = %d", dcbSerialParams.StopBits);
        printf("\n       Parity   = %d", dcbSerialParams.Parity);
    }

    /*------------------------------------ Setting Timeouts --------------------------------------------------*/

    COMMTIMEOUTS timeouts = { 0 };

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (SetCommTimeouts(hComm, &timeouts) == FALSE)
        printf("\n   Error! in Setting Time Outs");
    else
        printf("\n\n   Setting Serial Port Timeouts Successfull");
    
    /*----------------------------- Writing a Character to Serial Port----------------------------------------*/
    //char* lpBuffer = "0"; // lpBuffer should be  char or byte array, otherwise write wil fail
    char rBuffer[64];
    char size[] = "received";

    DWORD dNoOFBytestoRead;
    DWORD dNoOFBytestoWrite;     // No of bytes to write into the port
    DWORD dNoOfBytesWritten = 0; // No of bytes written to the port

    dNoOFBytestoWrite = sizeof(val); // Calculating the no of bytes to write into the port
    dNoOFBytestoRead = sizeof(size);

    
    Status = WriteFile(hComm,              // Handle to the Serialport
        val,           // Data to be written to the port
        dNoOFBytestoWrite,  // No of bytes to write into the port
        &dNoOfBytesWritten, // No of bytes written to the port
        NULL
        );  

   
    int x = 0;
    if (Status == TRUE)
        printf("\n\n    %s - Written to %s", val, ComPortName);
    else
        printf("\n\n   Error %d in Writing to Serial Port", GetLastError());

    Status = ReadFile(hComm,              // Handle to the Serialport
        rBuffer,           // Data to be written to the port
        dNoOFBytestoRead,  // No of bytes to write into the port
        &dNoOfBytesWritten, // No of bytes written to the port
        NULL
    );

    if (Status == TRUE)
        printf("\n\n    %s - reading  %s",rBuffer, ComPortName);
    else
        printf("\n\n   Error %d in reading to Serial Port", GetLastError());

    CloseHandle(hComm); //Closing the Serial Port
    printf("\n ==========================================\n");

    return 0;
}

/** Callback for when the connection opens. */
static void OnConnect(){
  printf("Connected.\n");
}

/** Callback for when a device is found. */
static void OnDevice(const LEAP_DEVICE_INFO *props){
  printf("Found device %s.\n", props->serial);
}

/** Callback for when a frame of tracking data is available. */
static void OnFrame(const LEAP_TRACKING_EVENT *frame){
   if (frame->info.frame_id % 60 == 0)
    printf("Frame %lli with %i hands.\n", (long long int)frame->info.frame_id, frame->nHands);

  for(uint32_t h = 0; h < frame->nHands; h++){
    LEAP_HAND* hand = &frame->pHands[h];
    
    if (hand->index.is_extended == 1) {
            char *val = "0";
            printf(val);
            Send_data(val);
    }
    else {
        char *val = "90";
        printf(val);
        Send_data(val);
    }
    printf("Hand id %s finger_id %u.\n",
                (hand->type == eLeapHandType_Left ? "left" : "right"),
                hand->index.is_extended);
  }
}

static void OnImage(const LEAP_IMAGE_EVENT *image){
  printf("Image %lli  => Left: %d x %d (bpp=%d), Right: %d x %d (bpp=%d)\n",
      (long long int)image->info.frame_id,
      image->image[0].properties.width,image->image[0].properties.height,image->image[0].properties.bpp*8,
      image->image[1].properties.width,image->image[1].properties.height,image->image[1].properties.bpp*8);
}

static void OnLogMessage(const eLeapLogSeverity severity, const int64_t timestamp,
                         const char* message) {
  const char* severity_str;
  switch(severity) {
    case eLeapLogSeverity_Critical:
      severity_str = "Critical";
      break;
    case eLeapLogSeverity_Warning:
      severity_str = "Warning";
      break;
    case eLeapLogSeverity_Information:
      severity_str = "Info";
      break;
    default:
      severity_str = "";
      break;
  }
  printf("[%s][%lli] %s\n", severity_str, (long long int)timestamp, message);
}

static void* allocate(uint32_t size, eLeapAllocatorType typeHint, void* state) {
  void* ptr = malloc(size);
  return ptr;
}

static void deallocate(void* ptr, void* state) {
  if (!ptr)
    return;
  free(ptr);
}

void OnPointMappingChange(const LEAP_POINT_MAPPING_CHANGE_EVENT *change){
  if (!connectionHandle)
    return;

  uint64_t size = 0;
  if (LeapGetPointMappingSize(*connectionHandle, &size) != eLeapRS_Success || !size)
    return;

  LEAP_POINT_MAPPING* pointMapping = (LEAP_POINT_MAPPING*)malloc((size_t)size);
  if (!pointMapping)
    return;

  if (LeapGetPointMapping(*connectionHandle, pointMapping, &size) == eLeapRS_Success &&
      pointMapping->nPoints > 0) {
    printf("Managing %u points as of frame %lld at %lld\n", pointMapping->nPoints, (long long int)pointMapping->frame_id, (long long int)pointMapping->timestamp);
  }
  free(pointMapping);
}

void OnHeadPose(const LEAP_HEAD_POSE_EVENT *event) {
  printf("Head pose:\n");
  printf("    Head position (%f, %f, %f).\n",
    event->head_position.x,
    event->head_position.y,
    event->head_position.z);
  printf("    Head orientation (%f, %f, %f, %f).\n",
    event->head_orientation.w,
    event->head_orientation.x,
    event->head_orientation.y,
    event->head_orientation.z);
  printf("    Head linear velocity (%f, %f, %f).\n",
    event->head_linear_velocity.x,
    event->head_linear_velocity.y,
    event->head_linear_velocity.z);
  printf("    Head angular velocity (%f, %f, %f).\n",
    event->head_angular_velocity.x,
    event->head_angular_velocity.y,
    event->head_angular_velocity.z);
 }

#include<windows.h>
#include<stdio.h>




int main(int argc, char** argv) {
    //Set callback function pointers
    ConnectionCallbacks.on_connection = &OnConnect;
    ConnectionCallbacks.on_device_found = &OnDevice;
    ConnectionCallbacks.on_frame               = &OnFrame;

    //char *val = {"55"};
  //Send_data(val);


  //ConnectionCallbacks.on_image               = &OnImage;
  ConnectionCallbacks.on_point_mapping_change = &OnPointMappingChange;
  ConnectionCallbacks.on_log_message         = &OnLogMessage;
  ConnectionCallbacks.on_head_pose           = &OnHeadPose;



  connectionHandle = OpenConnection();


  {
    LEAP_ALLOCATOR allocator = { allocate, deallocate, NULL };
    LeapSetAllocator(*connectionHandle, &allocator);
  }
  LeapSetPolicyFlags(*connectionHandle, eLeapPolicyFlag_Images | eLeapPolicyFlag_MapPoints, 0);

  printf("Press Enter to exit program.\n");
  getchar();
  
  CloseConnection();
  DestroyConnection();

  return 0;
}
//End-of-Sample
