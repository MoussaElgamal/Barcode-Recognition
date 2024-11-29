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

#pragma once
#ifndef IP_CRUD_OPERATIONS_H
#define IP_CRUD_OPERATIONS_H

/* Include files */
#include <opencv2/opencv.hpp>
#include <string>

/* Namespaces */
using namespace std;

namespace ip
{
	struct ProductInfo {
		string barcodeType;
		string barcodeNumber;
		string productName;
		string productDescription;
		// 
	};

	ProductInfo* getProductInfoFromBarcode(const string& barcode);
	void saveBarcodeInformation(const string& barcodeType, const string& barcodeNumber, const string& productName, const string& productDescription);
	void readBarcodeInformation();
	void updateBarcodeInformation(const std::string& barcodeToUpdate, const std::string& newProductName, const std::string& newProductDescription);
	void deleteBarcodeInformation(const string& barcodeToDelete);
	void saveBarcodeInformationCSV(const string& barcodeType, const string& barcodeNumber);

	
}

#endif /* IP_CRUD_OPERATIONS_H */