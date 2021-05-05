/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#if defined(__ARDUINO_TEENSY_MICROMOD)
#include "arduino_HM01B0_platform.h"
#endif  // defined(ARDUINO)


#ifndef ARDUINO_EXCLUDE_CODE

#include "HM01B0.h"
#include "HM01B0_regs.h"
#include "image_provider.h"
HM01B0 hm01b0(7, 8, 33, 32, 2, 40, 41, 42, 43, 44, 45, 6, 9);  //Sparkfun ML Carrier
uint8_t frameBuffer[(324) * 244] DMAMEM;
  
// These are headers from Ambiq's Apollo3 SDK.
#include "third_party/gemmlowp/internal/detect_platform.h"

// #define DEMO_HM01B0_FRAMEBUFFER_DUMP_ENABLE

// Enabling logging increases power consumption by preventing low power mode
// from being enabled.
#define ENABLE_LOGGING

static constexpr int kFramesToInitialize = 4;

bool g_is_camera_initialized = false;


TfLiteStatus InitCamera(tflite::ErrorReporter* error_reporter) {
  TF_LITE_REPORT_ERROR(error_reporter, "Initializing HM01B0...\n");


  uint8_t status;
  status = hm01b0.loadSettings(LOAD_DEFAULT_REGS);
  status = hm01b0.set_framesize(FRAMESIZE_QVGA);

  if (status != 0) {
    Serial.println("Settings failed to load");
    while (1) {}
  }
  hm01b0.set_framerate(30);  //15, 30, 60, 120

  /* Gain Ceilling
   * GAINCEILING_1X
   * GAINCEILING_4X
   * GAINCEILING_8X
   * GAINCEILING_16X
   */
  hm01b0.set_gainceiling(GAINCEILING_2X);
  /* Brightness
   *  Can be 1, 2, or 3
   */
  hm01b0.set_brightness(3);
  hm01b0.set_autoExposure(true, 1500);  //higher the setting the less saturaturation of whiteness
  hm01b0.cmdUpdate();  //only need after changing auto exposure settings

  hm01b0.set_mode(HIMAX_MODE_STREAMING, 0); // turn on, continuous streaming mode

  uint32_t FRAME_HEIGHT = hm01b0.height();
  uint32_t FRAME_WIDTH  = hm01b0.width();
  Serial.printf("ImageSize (w,h): %d, %d\n", FRAME_WIDTH, FRAME_HEIGHT);

  // Lets setup camera interrupt priorities:
  //hm01b0.setVSyncISRPriority(102); // higher priority than default
  hm01b0.setDMACompleteISRPriority(192); // lower than default


  return kTfLiteOk;
}

// Capture single frame.  Frame pointer passed in to reduce memory usage.  This
// allows the input tensor to be used instead of requiring an extra copy.
TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter, int frame_width,
                      int frame_height, int channels, int8_t* frame) {
  if (!g_is_camera_initialized) {
    TfLiteStatus init_status = InitCamera(error_reporter);

    g_is_camera_initialized = true;
  }

  //hm01b0_blocking_read_oneframe_scaled(&s_HM01B0Cfg, frame, frame_width,
  //                                     frame_height, channels);
									   
  
  hm01b0.readFrame(frameBuffer);
  
  
  // Calculate the number of pixels to crop to get a centered image.
  const int offset_x = (324 - (frame_width)) / 2;
  const int offset_y = (244 - (frame_height )) / 2;
  //Serial.printf("%d, %d\n",offset_x,offset_y);
  
  uint32_t image_idx = 0;
  uint32_t frame_idx = 0;
  for (uint32_t row = 0; row < 96; row++) {
    for (uint32_t col = 0; col < 96; col++) {
      frame_idx = (324 * (row + offset_y)) + col + offset_x;
      uint8_t framePixel = frameBuffer[frame_idx];
      frame[image_idx++] = framePixel;
    }
  }


  return kTfLiteOk;
}

#endif  // ARDUINO_EXCLUDE_CODE
