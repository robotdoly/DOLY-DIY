/**
 * @example Camera/main.cpp
 * @brief Camera capture and streaming example.
 *
 * Demonstrates:
 * - Initializing camera subsystem
 * - Capturing frames using libcamera
 * - Basic image/video handling
 */

#include <spdlog/spdlog.h>
#include "lccv.hpp"

#include <opencv2/imgcodecs.hpp>

int main()
{

	// First find active camera id	
	spdlog::info("Initializing camera");

	PiCamera cam;
	cam.options->photo_width = 3280;
	cam.options->photo_height = 2464;
	cam.options->video_width = 1280;
	cam.options->video_height = 960;
	cam.options->framerate = 15;
	cam.options->verbose = true;

	// Single frame capture example
	if (cam.startPhoto())
	{
		cv::Mat frame_rgb;
		if (!cam.capturePhoto(frame_rgb))
		{
			spdlog::error("Camera read buffer failed at capture!");
			return -3;
		}

		cv::imwrite("capture.jpg", frame_rgb);
		spdlog::info("Frame saved!");
		cam.stopPhoto();
	}

	// Video frame capture example
	// 
	// This example uses an OpenCV imshow() window to display captured frames. (X11 forwarding over SSH)
	// However, you have several options to view the camera output.
	// Here are a few examples:
	// 1) Stream frames over the network (probably fast and best option, requires addditional coding)
	//		inside your capture loop:
	//		std::vector<uchar> jpeg;
	//		cv::imencode(".jpg", frame, jpeg); (optional: reduce JPEG quality for small data)
	//		-> send jpeg over socket / HTTP
	// 2) X11 forwarding over SSH (simple, but slow)
	// 
	// Note:
	// - The image window may NOT be displayed when running with root privileges
	//   (this can cause GTK/X11 permission errors).
	// - X11 forwarding is required to display the window over SSH.
	// - Make sure your SSH client supports X11 forwarding.
	//
	// To test whether X11 forwarding is working correctly:
	//   sudo apt install -y x11-apps
	//   xclock
	// If X11 is working, a clock window should appear.
	//
	// Also verify that DISPLAY is set:
	//   echo $DISPLAY
	// It should show something like "localhost:11.0"
	if (cam.startVideo())
	{
		spdlog::info("Video window is live! Press ESC to quit!");

		// make window size auto size
		cv::namedWindow("Camera", cv::WINDOW_NORMAL);
		cv::resizeWindow("Camera", 640, 480);

		int time_out_ms = 1500;
		int total_frame = 300;

		cv::Mat frame;
		cv::Mat frame_640x480;		
		while (total_frame-- > 0)
		{
			if (cam.getVideoFrame(frame, time_out_ms))
			{
				// resize frame for fast X11
				cv::resize(frame, frame_640x480, cv::Size(640, 480));
				cv::imshow("Camera", frame);

				int k = cv::waitKey(1);
				if (k == 27) break; // ESC
			}
			else
				spdlog::warn("getVideoFrame() timeout/fail");
		}

		cam.stopVideo();
	}

	return 0;
}
