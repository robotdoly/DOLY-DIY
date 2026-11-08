# Camera (Python)

## Import

```python
import doly_camera
```

## Minimal working example

Below is the module’s `examples/python/Camera/example.py` as a starting point.

```python
import cv2
import doly_camera

def main():
    print("Initializing camera")

    cam = doly_camera.PiCamera()

    cam.options.photo_width = 3280
    cam.options.photo_height = 2464
    cam.options.video_width = 1280
    cam.options.video_height = 960
    cam.options.framerate = 15
    cam.options.verbose = False

    result = 0
    try:
        # -----------------------------
        # Single frame capture example
        # -----------------------------
        if not cam.start_photo():
            print("[error] Camera start_photo failed")
            return -2
        try:
            frame = cam.capture_photo()  # numpy array or None
            if frame is None:
                print("[error] Camera read buffer failed at capture!")
                return -3
            if not cv2.imwrite("capture.jpg", frame):
                print("[error] Saving capture.jpg failed")
                return -3
            print("Frame saved!")
        finally:
            try:
                if not cam.stop_photo():
                    print("[error] Camera stop_photo failed")
                    result = 1
            except Exception as exc:
                print(f"[error] Camera stop_photo raised: {exc}")
                result = 1
        if result:
            return result

        # -----------------------------
        # Video frame capture example
        #
        # This example uses an OpenCV imshow() window to display captured frames. (X11 forwarding over SSH)
        # However, you have several options to view the camera output.
        # Here are a few examples:
        # 1) Stream frames over the network (probably fast and best option, requires addditional coding)
        #        inside your capture loop:
        #        std::vector<uchar> jpeg;
        #        cv::imencode(".jpg", frame, jpeg); (optional: reduce JPEG quality for small data)
        #        -> send jpeg over socket / HTTP
        # 2) X11 forwarding over SSH (simple, but slow)
        #
        #
        # This example uses an OpenCV imshow() window to display captured frames (X11 forwarding over SSH).
        #
        # Note:
        # - The image window may NOT be displayed when running with root privileges
        #   (this can cause GTK/X11 permission errors).
        # - X11 forwarding is required to display the window over SSH.
        # - Make sure your SSH client supports X11 forwarding.
        #
        # To test whether X11 forwarding is working correctly:
        #   sudo apt install -y x11-apps
        #   xclock
        # If X11 is working, a clock window should appear.
        # These historical setup suggestions require separate authorization;
        # this example does not install dependencies or configure X11.
        #
        # Also verify that DISPLAY is set:
        #   echo $DISPLAY
        # It should show something like "localhost:11.0"
        # -----------------------------
        if not cam.start_video():
            print("[error] Camera start_video failed")
            return -4
        try:
            print("Video window is live! Press ESC to quit!")

            cv2.namedWindow("Camera", cv2.WINDOW_NORMAL)
            cv2.resizeWindow("Camera", 640, 480)

            time_out_ms = 1500
            total_frame = 300 #test frame count

            while total_frame > 0:
                total_frame -= 1

                frame = cam.get_video_frame(time_out_ms)  # numpy array or None
                if frame is not None:
                    # resize frame for fast X11 transfer
                    resized = cv2.resize(frame, (640, 480), interpolation=cv2.INTER_AREA)
                    cv2.imshow("Camera", resized)
                    k = cv2.waitKey(1) & 0xFF
                    if k == 27:  # ESC
                        break
                else:
                    print("[warn] getVideoFrame() timeout/fail")

        finally:
            try:
                cam.stop_video()  # Returns None.
            except Exception as exc:
                print(f"[error] Camera stop_video raised: {exc}")
                result = 1
    finally:
        try:
            cv2.destroyAllWindows()
        except Exception as exc:
            print(f"[error] Window cleanup raised: {exc}")
            result = 1

    return result


if __name__ == "__main__":
    raise SystemExit(main())
```

## Common tasks

- **Initialize**
- **Capture frame(s)**
- **Process frame**
- **Dispose / cleanup**

## API reference

See: [API reference](../api/camera.md)

