#ifndef DARKNET_ORIG_OPENCV_INCLUDES_H
#define DARKNET_ORIG_OPENCV_INCLUDES_H

#pragma GCC system_header
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall -Wextra -Werror -Wunknown-pragmas"
#pragma warning(push, 0)  // MSVC

#include <opencv2/core/version.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/video/video.hpp>

// includes for OpenCV >= 3.x
#ifndef CV_VERSION_EPOCH
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#endif

// OpenCV includes for OpenCV 2.x
#ifdef CV_VERSION_EPOCH
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>

#include <opencv2/core/version.hpp>
#endif

#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#ifndef CV_VERSION_EPOCH
#include <opencv2/videoio/videoio.hpp>
#endif

#pragma warning(pop)
#pragma GCC diagnostic pop

#endif  // DARKNET_ORIG_OPENCV_INCLUDES_H