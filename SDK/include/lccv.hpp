#pragma once


/**
 * @file lccv.hpp
 * @brief Camera helper header (third-party component).
 *
 * This file is derived from the upstream project:
 * - Project: LCCV (libcamera bindings for OpenCV)
 * - Upstream authors: kbarni and contributors
 * - Upstream source: https://github.com/kbarni/LCCV
 *
 * This upstream project is based on Raspberry Pi's libcamera-apps code:
 * - Copyright (C) 2020-2021 Raspberry Pi (Trading) Ltd.
 *
 * License:
 * - BSD 2-Clause ("Simplified BSD License")
 * - Full license text is provided in this repository at: third_party/LCCV/LICENSE
 *
 * @ingroup doly_sdk_common
 */

 // LCCV is a simple C++ wrapper that allows capturing photos or video from a camera
 // handled by the libcamera library. Captured frames are provided as OpenCV cv::Mat
 // objects for easy post-processing.

 /* SPDX-License-Identifier: BSD-2-Clause */
 /*
  * Copyright (C) 2020-2021 Raspberry Pi (Trading) Ltd.
  *
  * libcamera_app.hpp - Base class for libcamera apps.
  * options.hpp       - Common program options.
  */

  /*
   * Based on LCCV (BSD 2-Clause License)
   * Original Copyright (c) 2023 <https://github.com/kbarni/LCCV>
   *
   * Modifications Copyright (c) 2025 Levent
   */


#include <mutex>
#include <atomic>
#include <pthread.h>
#include <opencv2/opencv.hpp>

#include "libcamera_app.hpp"
#include "libcamera_app_options.hpp"

class PiCamera {
public:
	PiCamera();
	~PiCamera();

	Options* options;

	//Photo mode
	bool startPhoto();
	bool capturePhoto(cv::Mat& frame);
	bool stopPhoto();

	//Video mode
	bool startVideo();
	bool getVideoFrame(cv::Mat& frame, unsigned int timeout);
	void stopVideo();

	//Applies new zoom options. Before invoking this func modify options->roi.
	void ApplyZoomOptions();

	// custom settings 
	void SetExposure(const float& value);
	void SetAwbEnable(const bool& enable);

protected:
	LibcameraApp* app;
	void getImage(cv::Mat& frame, CompletedRequestPtr& payload);
	static void* videoThreadFunc(void* p);
	pthread_t videothread;
	unsigned int still_flags;
	unsigned int vw, vh, vstr;
	std::atomic<bool> running, frameready;
	uint8_t* framebuffer;
	std::mutex mtx;
	bool camerastarted;
};

