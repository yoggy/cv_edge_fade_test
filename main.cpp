#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <math.h>

float normalized_density(int x0, int y0, int x1, int y1, int offset)
{
	int dx = x1 - x0;
	int dy = y1 - y0;

	int dd = dx * dx + dy * dy;
	float d = (float)sqrt((double)dd);

	float nd = (offset - d) / offset;
	if (nd < 0.0f) nd = 0.0f;

	return nd;
}

int check_type(int x, int y, int x0, int y0, int x1, int y1)
{
	if (y < y0) {
		if (x < x0) {
			return 0;
		}
		else if (x0 <= x && x <= x1) {
			return 1;
		}
		else {
			return 2;
		}
	}
	else if (y0 <= y && y <= y1) {
		if (x < x0) {
			return 3;
		}
		else if (x0 <= x && x <= x1) {
			return 4;
		}
		else {
			return 5;
		}
	}
	else {
		if (x < x0) {
			return 6;
		}
		else if (x0 <= x && x <= x1) {
			return 7;
		}
		else {
			return 8;
		}
	}
}

cv::Mat create_fade_image(const cv::Size &size, const int &offset)
{
	cv::Mat img;
	img.create(size, CV_8UC1);
	img.setTo(0);

	int x0 = offset;
	int x1 = size.width - offset;
	int y0 = offset;
	int y1 = size.height - offset;

	// check
	if (x0 < 0)            x0 = 0;
	if (size.width  <= x0) x0 = size.width - 1;
	if (x1 < 0)            x1 = 0;
	if (size.width  <= x1) x1 = size.width - 1;
	if (x1 < x0)           x1 = x0;

	if (y0 < 0)            y0 = 0;
	if (size.height <= y0) y0 = size.height - 1;
	if (y1 < 0)            y1 = 0;
	if (size.height <= y1) y1 = size.height - 1;
	if (y1 < y0)           y1 = y0;


	for (int y = 0; y < size.height; ++y) {
		uchar *p = img.ptr<uchar>(y);
		for (int x = 0; x < size.width; ++x) {
			int type = check_type(x, y, x0, y0, x1, y1);
			uchar c = 0;
			switch(type) {
				case 0:
					c = (uchar)(normalized_density(x, y, x0, y0, offset) * 255);
					break;
				case 1:
					c = (uchar)(normalized_density(0, y, 0, y0, offset) * 255);
					break;
				case 2:
					c = (uchar)(normalized_density(x, y, x1, y0, offset) * 255);
					break;
				case 3:
					c = (uchar)(normalized_density(x, 0, x0, 0, offset) * 255);
					break;
				case 4:
					c = 255;
					break;
				case 5:
					c = (uchar)(normalized_density(x, 0, x1, 0, offset) * 255);
					break;
				case 6:
					c = (uchar)(normalized_density(x, y, x0, y1, offset) * 255);
					break;
				case 7:
					c = (uchar)(normalized_density(0, y, 0, y1, offset) * 255);
					break;
				case 8:
					c = (uchar)(normalized_density(x, y, x1, y1, offset) * 255);
					break;
			}
			*(p++) = c;
		}
	}

	return img;
}

const char *window_name = "fade_img";
int offset = 100;

int main(int argc, char* argv[])
{
	cv::namedWindow(window_name);
	cv::createTrackbar("offset", window_name, &offset, 400, NULL, NULL);

	while(true) {
		cv::Mat fade_img = create_fade_image(cv::Size(640, 480), offset);

		cv::imshow(window_name, fade_img);
		int c = cv::waitKey(1);

		if (c == 27) break;
	}
	cv::destroyAllWindows();

	return 0;
}

