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

#include "imageProcessing.h"

/* Namespaces */
using namespace std;
using namespace cv;

namespace ip
{
    void unsharpMasking(const Mat& source, Mat& processed, double alpha) {
        Mat kernel = (Mat_<double>(9, 1) << 1, 8, 28, 56, 70, 56, 28, 8, 1);
        kernel /= sum(kernel);

        // Apply filter
        Mat filtered;
        sepFilter2D(source, filtered, source.depth(), kernel, kernel);
        processed = ((1 + alpha) * source) - alpha * filtered;

    }
}
