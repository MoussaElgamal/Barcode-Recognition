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


/* Include files */
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include "crudOperations.h"

/* Namespaces */
using namespace std;


namespace ip
{
	vector<ProductInfo> productDatabase;

	void saveBarcodeInformationCSV(const string& barcodeType, const string& barcodeNumber,
		const string& productName, const string& productDescription) {
		const string csvFilePath = "barcode_information.csv";
		bool writeHeader = !ifstream(csvFilePath).good();  // Überprüfe, ob die Datei existiert

		// Öffne die CSV-Datei im Append-Modus
		ofstream outFile(csvFilePath, ios::app);

		if (outFile.is_open()) {
			// Wenn die Datei noch nicht existiert, füge den Header hinzu
			if (writeHeader) {
				outFile << "Barcode Type,Barcode Number,Product Name,Product Description" << endl;
			}

			// Schreibe die Daten in die CSV-Datei
			outFile << barcodeType << "," << barcodeNumber << "," << productName << "," << productDescription << endl;

			// Schließe die Datei
			outFile.close();
		}
		else {
			cerr << "Error: Unable to open CSV file for writing." << endl;
		}
	}

	

	// Function to extract product information from a CSV file based on barcode
	ProductInfo* getProductInfoFromBarcode(const string& barcode) {
		const string csvFilePath = "barcode_information.csv";
		ifstream inFile(csvFilePath);

		// Check if the file could be opened
		if (!inFile.is_open()) {
			cerr << "Error: Unable to open CSV file for reading." << endl;
			return nullptr;
		}

		string line;
		getline(inFile, line); // Skip the header

		while (getline(inFile, line)) {
			istringstream iss(line);
			vector<string> tokens;
			string token;

			// Split the line into tokens
			while (getline(iss, token, ',')) {
				tokens.push_back(token);
			}

			// Check if the number of tokens is correct
			if (tokens.size() == 4) {
				// Compare the barcode with the desired barcode
				if (tokens[1] == barcode) {
					ProductInfo* productInfo = new ProductInfo{
						tokens[0],  // Barcode Type
						tokens[1],  // Barcode Number
						tokens[2],  // Product name
						tokens[3]   // Product description
					};

					inFile.close();  // Close the file
					return productInfo;
				}
			}
		}

		inFile.close();  // Close the file

		// If the barcode is not found, return nullptr
		return nullptr;
	}





	void saveBarcodeInformation(const string& barcodeType, const string& barcodeNumber, const string& productName,
		const string& productDescription) {
		ProductInfo newProduct;
		newProduct.barcodeType = barcodeType;
		newProduct.barcodeNumber = barcodeNumber;
		newProduct.productName = productName;
		newProduct.productDescription = productDescription;

		productDatabase.push_back(newProduct);

		saveBarcodeInformationCSV(newProduct.barcodeType, newProduct.barcodeNumber, newProduct.productName,
			newProduct.productDescription);
		cout << "Product information created successfully." << endl;
	}



	void readBarcodeInformation() {
	};


	void updateBarcodeInformation(const std::string& barcodeToUpdate, const std::string& newProductName, const std::string& newProductDescription) {
		// Read existing information from the CSV file
		std::vector<ProductInfo> allProductInfo;
		std::ifstream inputFile("barcode_information.csv");

		if (!inputFile.is_open()) {
			std::cerr << "Error: Unable to open CSV file for reading." << std::endl;
			return;
		}

		std::string line;
		while (std::getline(inputFile, line)) {
			std::istringstream iss(line);
			ProductInfo product;
			std::getline(iss, product.barcodeType, ',');
			std::getline(iss, product.barcodeNumber, ',');
			std::getline(iss, product.productName, ',');
			std::getline(iss, product.productDescription, ',');

			allProductInfo.push_back(product);
		}

		inputFile.close();

		// Find the barcode to update and modify the information
		for (std::vector<ProductInfo>::iterator it = allProductInfo.begin(); it != allProductInfo.end(); ++it) {
			if (it->barcodeNumber == barcodeToUpdate) {
				it->productName = newProductName;
				it->productDescription = newProductDescription;
				break;  // Since the barcode is unique, no need to continue searching
			}
		}

		// Open the file for writing
		std::ofstream outputFile("barcode_information.csv");

		if (!outputFile.is_open()) {
			std::cerr << "Error: Unable to open CSV file for writing." << std::endl;
			return;
		}

		// Write back all entries to the file
		for (std::vector<ProductInfo>::const_iterator it = allProductInfo.begin(); it != allProductInfo.end(); ++it) {
			outputFile << it->barcodeType << ',' << it->barcodeNumber << ','
				<< it->productName << ',' << it->productDescription << std::endl;
		}

		outputFile.close();

		std::cout << "Product information for barcode " << barcodeToUpdate << " updated successfully." << std::endl;
	}



	void deleteBarcodeInformation(const std::string& barcodeToDelete) {
		// Read existing information from the CSV file
		std::vector<ProductInfo> allProductInfo;
		std::ifstream inputFile("barcode_information.csv");

		if (!inputFile.is_open()) {
			// Error if unable to open CSV file for reading
			cerr << "Error: Unable to open CSV file for reading." << endl;
			return;
		}

		std::string line;
		while (getline(inputFile, line)) {
			// Parse each line in the CSV file and store in vector
			std::istringstream iss(line);
			ProductInfo product;
			getline(iss, product.barcodeType, ',');
			getline(iss, product.barcodeNumber, ',');
			getline(iss, product.productName, ',');
			getline(iss, product.productDescription, ',');

			allProductInfo.push_back(product);
		}

		inputFile.close();

		// Remove the entire line with the specified barcode
		allProductInfo.erase(
			std::remove_if(allProductInfo.begin(), allProductInfo.end(),
				[barcodeToDelete](const ProductInfo& info) {
					return info.barcodeNumber == barcodeToDelete;
				}),
			allProductInfo.end());

		// Open the file for writing
		std::ofstream outputFile("barcode_information.csv");

		if (!outputFile.is_open()) {
			// Error if unable to open CSV file for writing
			cerr << "Error: Unable to open CSV file for writing." << endl;
			return;
		}

		// Write back all remaining entries to the file
		for (const auto& info : allProductInfo) {
			outputFile << info.barcodeType << ',' << info.barcodeNumber << ','
				<< info.productName << ',' << info.productDescription << endl;
		}

		outputFile.close();

		cout << "Product information for barcode " << barcodeToDelete << " deleted successfully." << endl;
	}
}
