#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat K_Means(cv::Mat Input, int K);

int main() {
	//Open picture from the terminal, create color palette from it (possibly with different palette options. Ie: NES or Gameboy etc), scan through image with Bitmap
	//Then turn that image into a pixel rendition of it by averaging width x height squares of variable length depending on resolution desired of pixel image.
	//Possibly use other algorithms to do this if any can be found.

	//Read the image
	std::cout << "Input file name";
	std::string filepath = "L:\\Code\\Visual Studio Workspaces\\PixelArtGenerator\\PixelArtGenerator\\image2.jpg";
	//std::cin >> filepath;
	cv::Mat image = cv::imread(filepath);
	cv::Mat newImage = cv::imread(filepath);
	cv::Mat temp = cv::imread(filepath);
	int pixelSize = 6;
	//std::cout << "Choose a pixel size";
	//std::cin >> pixelSize;
	//std::cout << "Choose a palette";
	//Have an enum with possible palettes. Ie: NES with it's defined colors, etc;
	//std::cin >> Palette;


	//Check for failure
	if (image.empty()) {
		std::cout << "Image cannot be found";
		std::cin.get();
		return -1;
	}
	std::cout << image.rows;
	//Doesn't work well right now, but theoretically resize can be used to pixelize an image as well.
	cv::resize(temp, temp, cv::Size(), 1, 1, cv::INTER_LINEAR);

	//cv::Mat newImage;
	//newImage = image;
	
	std::cout << "Height: " << image.rows << " Width: " << image.cols << " Channels: " << image.channels() << std::endl;
	int clusters = 8;
	cv::Mat ClusteredImage = K_Means(image, clusters);

	//Temperary loop. Loop through image, get color at that pixel.
	//Will change so y++ is variable size of pixel, and then average pixel color there, find closest color in palette.
	//for (int y = 0; y < image.rows; y++) {
		//for (int x = 0; x < image.cols; x++) {
			//cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(x, y));
			//if (color[0] > 150 && color[1] < 150 && color[2] > 150) {
				//color[0] = color[1] = color[2] = 0;
			//}
			//if (color[2] > 200) {
				//color[0] = color[1] = 0;
			//}
			//else if(color[1] > 100) {
				//color[0] = color[2] = 0;
			//}
			//else if (color[2] > 100) {
				//color[0] = color[1] = 0;
			//}
			//else {
				//color[0] = color[1] = color[2] = 75;
			//}
			//newImage.at<cv::Vec3b>(cv::Point(x, y)) = color;
		//}
	//}

	//Right now my inner loop is broken. Only does the first square and then stops
	//Crashes when innerY > 1080, which is the height of the image I have loaded
	for (int y = 0; y <= image.rows - 10; y += pixelSize) {
		for (int x = 0; x <= image.cols; x += pixelSize) {
			int avgRed = 0, avgBlue = 0, avgGreen = 0, pixelCount = 0;
			int innerY;
			int innerX;
			for (innerY = 0; innerY < pixelSize; innerY++) {
				for (innerX = 0; innerX < pixelSize; innerX++) {
					int pixelY = innerY + y;
					int pixelX = innerX + x;
					if (innerY + y > image.rows)
						pixelY = image.rows;
					if (innerX + x > image.cols)
						pixelX = image.cols;
					cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(pixelX, pixelY));
					avgRed += color[1];
					avgBlue += color[2];
					avgGreen += color[0];
					pixelCount++;
				}
			}
			int newRed = 0, newBlue = 0, newGreen = 0;
			newRed = avgRed / pixelCount;
			newBlue = avgBlue / pixelCount;
			newGreen = avgGreen / pixelCount;
			cv::Vec3b newColor;
			newColor[0] = newGreen;
			newColor[1] = newRed;
			newColor[2] = newBlue;
			// Now find closest color in palette to these new numbers
			for (innerY = 0; innerY < pixelSize; innerY++) {
				for (innerX = 0; innerX < pixelSize; innerX++) {
					int pixelY = innerY + y;
					int pixelX = innerX + x;
					if (innerY + y > image.rows)
						pixelY = image.rows;
					if (innerX + x > image.cols)
						pixelX = image.cols;
					newImage.at<cv::Vec3b>(cv::Point(pixelX, pixelY)) = newColor;
				}
			}
		}
	}

	//cv::GaussianBlur(newImage, newImage, cv::Size(3, 3), 0);
	cv::Mat ClusteredPixelImage = K_Means(newImage, clusters);

	std::string windowName = "Pixel Art Generator"; //Name of the window
	std::string modifiedImage = "Modified Image";
	std::string resizeImage = "Interpolated Size";
	std::string kmeans = "K Means Image";
	std::string kmeanspixel = "K Means Pixel Image";

	cv::namedWindow(windowName); //Create window
	cv::namedWindow(modifiedImage);
	//cv::namedWindow(resizeImage);
	cv::namedWindow(kmeans);
	cv::namedWindow(kmeanspixel);

	cv::imshow(windowName, image); //show image in the window
	cv::imshow(modifiedImage, newImage);
	//cv::imshow(resizeImage, temp);
	cv::imshow(kmeans, ClusteredImage);
	cv::imshow(kmeanspixel, ClusteredPixelImage);

	cv::waitKey(0); // wait for keystroke

	//cv::destroyWindow(windowName); //close the window
	cv::destroyAllWindows();

	return 0;
}

cv::Mat K_Means(cv::Mat Input, int K) {
	cv::Mat samples(Input.rows * Input.cols, Input.channels(), CV_32F); //change image data to float
	for (int y = 0; y < Input.rows; y++) {
		for (int x = 0; x < Input.cols; x++) {
			for (int z = 0; z < Input.channels(); z++) {
				if (Input.channels() == 3) {
					samples.at<float>(y + x * Input.rows, z) = Input.at<cv::Vec3b>(y, x)[z]; //For color images
				}
				else {
					samples.at<float>(y + x * Input.rows, z) = Input.at<uchar>(y, x); //For gray scale images
				}
			}
		}
	}

	cv::Mat labels;
	int attempts = 5;
	cv::Mat centers;
	cv::kmeans(samples, K, labels, cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 1.0), attempts, cv::KMEANS_PP_CENTERS, centers);
	cv::Mat New_Image(Input.size(), Input.type());
	for (int y = 0; y < Input.rows; y++) {
		for (int x = 0; x < Input.cols; x++) {
			int cluster_idx = labels.at<int>(y + x * Input.rows, 0);
			if (Input.channels() == 3) {
				for (int z = 0; z < Input.channels(); z++) {
					New_Image.at<cv::Vec3b>(y, x)[z] = centers.at<float>(cluster_idx, z);
				}
			}
			else {
				New_Image.at<uchar>(y, x) = centers.at<float>(cluster_idx, 0);
			}
		}
	}
	return New_Image;
}