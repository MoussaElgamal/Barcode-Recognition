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
#ifndef IP_IMAGE_PROCESSING_H
#define IP_IMAGE_PROCESSING_H

/* Include files */
#include <opencv2/opencv.hpp>

/* Namespaces */
using namespace cv;

namespace ip
{
	void unsharpMasking(const Mat& source, Mat& processed, double alpha);
}

#endif /* IP_IMAGE_PROCESSING_H */
