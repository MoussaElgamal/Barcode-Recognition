/*****************************************************************************************************
 * Automatische EAN-13-Barcodeerkennung mit OpenCV.
 *****************************************************************************************************
 * Autor: [Moussa Elgamal]
 * Version: 04.02.2024
 * Beschreibung:
                Dieses C++-Programm implementiert die automatische EAN-13-Barcodeerkennung mithilfe der OpenCV-Bibliothek.
                Es bietet vier Modi: Erstellen, Lesen, Aktualisieren und Löschen von Produktinformationen.
                Das System erfasst Bilder, verbessert die Barcodeerkennung und ermöglicht die Interaktion mit der Datenbank
                für verschiedene Anwendungsfälle.
 *****************************************************************************************************/


/* Compiler settings */
#define _CRT_SECURE_NO_WARNINGS		// Enable getenv()

/* Include files */
#include <iostream>
#include <conio.h>
#include <string>
#include <opencv2/opencv.hpp>
#include "imageProcessing.h"
#include "crudOperations.h"
#include <fstream>



/* Defines */
#define IMAGE_DATA_PATH getenv("ImagingData")			// Read environment variable ImagingData
#define INPUT_IMAGE_RELATIVE_PATH "/images/misc/notISBN-13.jpg"	// Image file including relative path
#define GREEN Scalar(0, 255, 0) 
#define WHITE Scalar(255, 255, 255) 
#define BLACK Scalar(0, 0, 0) 
#define BLUE Scalar(255, 0, 0)
#define RED Scalar(0, 0, 255)
#define YELLOW Scalar(0, 255, 255)
#define RandomColor Scalar(rand() % 256, rand() % 256, rand() % 256)

/* Namespaces */
using namespace cv;
using namespace ip;
using namespace std;
using namespace barcode;




/* Prototypes */
tuple<string, bool>  localizeBarcode(Mat& processed);
void onTrackbar(int trackbarPosition, void* imagePtr);
void saveBarcodeInformationCSV(const string& barcodeType, const string& barcodeNumber);
Scalar getRandomColor();
string extractDigitsFromBarcode(const string& barcode);
bool isValidEAN13(const string& barcode);
void saveImageFromCamera(const string imageName); 
tuple<string, string>  getProductInformationFromUser(string barcodeNumber);
void printProductInfo(const ProductInfo* productInfo);
bool askForAnotherBarcode();
void showLoadingAnimation(string word,int numDots, int delay);




/* Main function */
int main(void) {
    showLoadingAnimation("Loading",10, 200);
    cout << "\nThe EAN13 reader is ready for use" << endl;

    // variables
    string imageName ;
    string inputImagePath;
    bool anotherBarcode = true;
    bool isreaded;
    string barcodeNumber = "";
    string productName = "";
    string productDescription = "";
    ProductInfo* foundProduct = nullptr;
    string mode;
    int maxValue = 50;
    int sliderValue = 0;
    /*int targetValue = 0;
    int& sliderValue = targetValue;*/
    Mat processed;
    Mat srcImage;
    pair<Mat*, Mat*> images(&srcImage, &processed); // to avoid global variables


    // create Window
    namedWindow("Window", WINDOW_NORMAL);
    
    // create trackbar
    createTrackbar("SHARPNESS", "Window", NULL, maxValue, onTrackbar,
        reinterpret_cast <void*>(&images));

    //programm Loop
    while (anotherBarcode)
    {
        imageName = "captured_image.jpg";
        cout << "Press any Key to capture a photo" << endl;
        saveImageFromCamera(imageName);
        srcImage = imread(imageName);

        // Load image from file
        inputImagePath = string(IMAGE_DATA_PATH).append(INPUT_IMAGE_RELATIVE_PATH);
        //srcImage = imread(inputImagePath);

        if (srcImage.empty()) {
            cout << "[ERROR] Cannot open image: " << inputImagePath << endl;
            return 0;
        }
        //srcImage = imread("D:/ISBN-13.jpg");


        // get barcodeNumber
        processed = srcImage.clone();
        tuple<string, bool> result = localizeBarcode(processed);
        isreaded = get<1>(result);

        //Sharpen the image if it is not scannable and localize the barcode
        sliderValue = 0;
        while(!isreaded && sliderValue < 10) {
            cout << "Image Sharpness is being edited" << endl;
            showLoadingAnimation("Loading", 3, 200);
            sliderValue += 1;
            setTrackbarPos("SHARPNESS", "Window", sliderValue);
            result = localizeBarcode(processed);
            isreaded = get<1>(result);
        }

        // choose one Mode from the four Modes if the Barcode successfully readed
        if (isreaded) {
            cout << "Barcode readed successfully" << endl;
            barcodeNumber = get<0>(result);
            cout << "Barcode Number: " << barcodeNumber << endl;
            tuple<string, string> prodInformation = make_tuple("", "");

            // Call the different modes
            cout << "Choose a mode (C: CREATE, U: UPDATE, R: READ, D: DELETE): ";
            cin >> mode;
            
            // Process the selected mode
            switch (mode[0]) {
            case 'C':
            case 'c':
                prodInformation = getProductInformationFromUser(barcodeNumber);
                productName = get<0>(prodInformation);
                productDescription = get<1>(prodInformation);
                saveBarcodeInformation("EAN13", barcodeNumber, productName, productDescription);
                break;

            case 'U':
            case 'u':
                //updateMode(barcodeNumber);
                prodInformation = getProductInformationFromUser(barcodeNumber);
                productName = get<0>(prodInformation);
                productDescription = get<1>(prodInformation);
                updateBarcodeInformation(barcodeNumber, productName, productDescription);
                break;

            case 'R':
            case 'r':
                foundProduct = getProductInfoFromBarcode(barcodeNumber);
                // Print the product information
                printProductInfo(foundProduct);
                //free the allocated memory
                delete foundProduct;
                //readMode(barcodeNumber);
                break;

            case 'D':
            case 'd':
                deleteBarcodeInformation(barcodeNumber);
                break;

            default:
                cout << "Invalid mode. Please choose CREATE, UPDATE, READ, or DELETE." << endl;
                break;
            }
         anotherBarcode = askForAnotherBarcode(); //If the user wants to read another barcode => True 
        }
        else { 
            cout << "No barcode detected." << endl;
            anotherBarcode = askForAnotherBarcode(); // End the loop if no barcode is detected and the user end reading
            //anotherBarcode = false;
         }
       imshow("Window", processed);
       
    }
    
    showLoadingAnimation("Bye Bye", 5, 200);
    waitKey(1000);
    return 0;
}

// Function to print product information
void printProductInfo(const ProductInfo* productInfo) {
    if (productInfo != nullptr) {
        cout << "Product Information:" << endl;
        cout << "Barcode Type: " << productInfo->barcodeType << endl;
        cout << "Barcode Number: " << productInfo->barcodeNumber << endl;
        cout << "Product Name: " << productInfo->productName << endl;
        cout << "Product Description: " << productInfo->productDescription << endl;
    }
    else {
        cout << "Product not found." << endl;
    }
}


bool askForAnotherBarcode() {
    cout << "\nDo you want to read another barcode? (y/n):";
    char response;
    cin >> response;
    return (response == 'y' || response == 'Y');
}


tuple<string, string> getProductInformationFromUser(string barcodeNumber) {
    string productName = "";
    string productDescription = "";

    cout << "Enter product name: ";
    // Ignore any leftover characters in the input buffer
    cin.ignore();
    getline(cin, productName);
    cout << "Enter product description: ";
    getline(cin, productDescription);
    
    return make_tuple(productName, productDescription);
}

void saveImageFromCamera(const string imageName) {
    VideoCapture camera(0);
    if (!camera.isOpened()) {
        cout << "ERROR: Cannot open camera" << endl;
        return;
    }

    Mat frame, frameWithoutRectangle;

    while (true) {
        // Get current frame from camera
        camera >> frameWithoutRectangle;
        camera >> frame;

        int rectangleWidth = 300;
        int rectangleHeight = 300;
        int rectangleX = (frame.cols - rectangleWidth) / 2;
        int rectangleY = (frame.rows - rectangleHeight) / 2;

        // Draw a rectangle on the frame
        rectangle(frame, Point(rectangleX, rectangleY),
            Point(rectangleX + rectangleWidth, rectangleY + rectangleHeight), RandomColor, 2);
        putText(frame, "PRESS ANY KEY TO TAKE A PHOTO", Point(rectangleX-20, rectangleY - 20),
            FONT_HERSHEY_SIMPLEX, 0.7, RandomColor, 2);


        // Display frame in named window
        imshow("Camera [press any key to save and quit]", frame);

        // Wait (exit loop on key press)
        int key = waitKey(1);
        if (key >= 0) {

            imwrite(imageName, frameWithoutRectangle);
            break;
        }
    }

    // Free resources
    camera.release();
}


tuple<string, bool> localizeBarcode(Mat& processed) {
    vector<Point> corners;
    vector<string> decode_info;
    vector<string> decode_type;
    BarcodeDetector barcodeDetector;
    bool isreaded = false;
    string barcodeNumber = "";
    barcodeDetector.detectAndDecodeWithType(processed, decode_info, decode_type, corners);

    if (decode_info.empty() || decode_type.empty() || corners.empty()) {
        cout << "NO Barcode" << endl;
        return make_tuple(barcodeNumber, isreaded);
    }
    
    for (size_t i = 0; i < corners.size(); i += 4)
    {
        const size_t idx = i / 4;
        const bool isDecodable = idx < decode_info.size()
            && idx < decode_type.size()
            && !decode_type[idx].empty();
        const Scalar lineColor = isDecodable ? BLUE : RED;
        // draw barcode rectangle
        vector<Point> contour(corners.begin() + i, corners.begin() + i + 4);
        const vector< vector<Point> > contours{ contour };
        drawContours(processed, contours, 0, lineColor, 2);
        // draw vertices
        for (size_t j = 0; j < 4; j++)
            circle(processed, contour[j], 2, getRandomColor(), -1);
        // write decoded text
        
        if (isDecodable)
        {
            ostringstream buf;
            buf << "[" << decode_type[idx] << "] " << decode_info[idx];
            barcodeNumber = extractDigitsFromBarcode(buf.str()).substr(2);
            //string barcodeNumber = "1234567890122"; invalid Barcode
            if (isValidEAN13(barcodeNumber)){
                isreaded = true;
                
                /*saveBarcodeInformationCSV("EAN13", barcodeNumber);*/
                putText(processed, barcodeNumber, contour[1], FONT_ITALIC, 1, YELLOW, 2);
      
            }
            else {
                putText(processed, string("EAN13 IS INVALID").append(barcodeNumber), contour[1], FONT_ITALIC, 1, RED, 2);
            }
            
        }
     }
    return make_tuple(barcodeNumber, isreaded);
}

void onTrackbar(int trackbarPosition, void* imagePtr) {
    pair<Mat*, Mat*>* images = static_cast<pair<Mat*, Mat*>*>(imagePtr);
    Mat processed = *(static_cast <Mat*>(images->second));
    double alpha = 0;
    Mat srcImage = *(images->first);
    
    alpha = trackbarPosition / 2;
    unsharpMasking(srcImage, processed, alpha);
    localizeBarcode(processed);
    imshow("Window", processed);
};


Scalar getRandomColor() {
    return Scalar(rand() % 256, rand() % 256, rand() % 256);
}

string extractDigitsFromBarcode(const string& barcode) {
    string digitsOnly;

    for (char ch : barcode) {
        if (isdigit(ch)) {
            digitsOnly += ch;
        }
    }

    return digitsOnly;
}


// Function to check the validity of an EAN-13 barcode using a checksum
bool isValidEAN13(const string& barcode) {
    // Check if the barcode has the correct length
    if (barcode.length() != 13) {
        return false;
    }

    // Extract the first 12 digits to calculate the checksum
    string digits = barcode.substr(0, 12);

    // Calculate the checksum
    int calculatedChecksum = 0;
    for (int i = 0; i < 12; ++i) {
        int digit = digits[i] - '0';
        calculatedChecksum += (i % 2 == 0) ? digit : digit * 3;
    }

    int mod = calculatedChecksum % 10;
    int checksum = (mod == 0) ? 0 : 10 - mod;

    // Check if the calculated checksum matches the last digit of the barcode
    return checksum == (barcode.back() - '0');
}


// Function to show loading animation with specified number of dots and delay between dots
void showLoadingAnimation(string word,int numDots, int delay) {
    cout << word; // Display "Loading" message
    for (int i = 0; i < numDots; ++i) {
        cout << "."; 
        cout.flush(); // Flush the output buffer to immediately display the dots
        std::this_thread::sleep_for(std::chrono::milliseconds(delay)); // short delay between dots
    }
    cout << endl;
}
