#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <thread>

int main() {
    // GStreamer pipeline
    std::string gst = "udpsrc port=5620 ! application/x-rtp, encoding-name=H265,payload=96 ! rtph265depay ! h265parse ! avdec_h265 ! videoconvert ! appsink";

    // Open the video capture
    cv::VideoCapture cap(gst, cv::CAP_GSTREAMER);

    // Check if the video capture is open
    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to open video stream." << std::endl;
        return -1;
    }

    // Get the frame width and height
    int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

    // Output frame dimensions for debugging
    std::cout << "Frame Width: " << frame_width << ", Frame Height: " << frame_height << std::endl;

    // Generate a filename with the current date and time
    std::time_t now = std::time(nullptr);
    std::tm local_time = *std::localtime(&now);
    std::ostringstream filename;
    filename << "/home/orangepi/capturing_q/" << std::put_time(&local_time, "%Y-%m-%d_%H-%M-%S") << ".avi";

    // Define the codec and create a VideoWriter object
    int fourcc = cv::VideoWriter::fourcc('M', 'J', 'P', 'G'); // Use MJPG codec
    cv::VideoWriter out(filename.str(), fourcc, 25, cv::Size(frame_width, frame_height));

    // Check if VideoWriter is opened
    if (!out.isOpened()) {
        std::cerr << "Error: Could not open the output video file for write." << std::endl;
        std::cerr << "Filename: " << filename.str() << std::endl; // Output the filename for reference
        return -1;
    }

    cv::Mat frame;
    while (true) {
        // Read a frame from the capture
        if (!cap.read(frame)) {
            std::cerr << "Failed to grab frame" << std::endl;
            break;
        }

        // Write the frame to the output file
        out.write(frame);

        // Display the frame
        cv::imshow("Video Stream", frame);

        // Introduce a small delay to control frame rate
        if (cv::waitKey(1) == 'q') {
            break; // Exit on 'q' key press
        }
    }

    // Release the capture and close any OpenCV windows
    cap.release();
    out.release();
    std::cout << "Video saved as: " << filename.str() << std::endl; // Confirm file saving
    cv::destroyAllWindows(); // Close any OpenCV windows

    return 0;
}
