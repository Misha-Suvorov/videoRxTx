#include <opencv2/opencv.hpp>
#include <QString>
#include <QThread>

using namespace cv;

int main() {
    // Define the GStreamer pipeline with the target resolution
    cv::VideoWriter writer;
    QString ipRTSPout = "appsrc ! videoconvert ! video/x-raw, format=NV12, width=960, height=540 ! mpph265enc bps=2000000 bps-max=2000000 ! rtph265pay config-interval=5 ! udpsink host=127.0.0.1 port=5620"; // ALPHA

    // Initialize VideoWriter with the updated pipeline and check if it opens
    writer.open(ipRTSPout.toStdString(), 0, 25, Size(960, 540));
    if (!writer.isOpened()) {
        printf("=ERR1= can't create video writer\n");
        return -1;
    }

    // Initialize VideoCapture to get frames from the camera
    cv::VideoCapture cap(0, cv::CAP_V4L2); // 0 for default camera
    if (!cap.isOpened()) {
        printf("=ERR2= can't open camera\n");
        return -1;
    }

    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cap.set(CAP_PROP_FRAME_WIDTH, 1280);   // Set frame width
    cap.set(CAP_PROP_FRAME_HEIGHT, 720);   // Set frame height
    cap.set(CAP_PROP_FPS, 25);             // Set frame rate

    bool isThreadRunning = true;
    cv::Mat frame;
    cv::Mat resizedFrame; // Create a new Mat to hold the resized frame

    while (isThreadRunning) {
        // Capture a frame from the camera
        cap >> frame;

        // Check if frame is valid before writing
        if (frame.empty()) {
            printf("=ERR3= captured empty frame\n");
            break;
        }

        // Resize the frame to 960x540
        cv::resize(frame, resizedFrame, Size(960, 540));

        // Write the resized frame to the pipeline
        writer.write(resizedFrame);
        QThread::msleep(33);  // Adjust delay as necessary for smooth streaming
    }

    // Release resources when done
    cap.release();
    writer.release();

    return 0;
}
