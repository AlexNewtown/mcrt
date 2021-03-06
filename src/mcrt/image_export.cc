#include "mcrt/image_export.hh"

#include <fstream>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include "lodepng.hh"

// Parses the extension and try to save with the formats we have implemented.
void mcrt::ImageExporter::save(const Image& image, const std::string& file) {
    size_t extensionPosition { file.find_last_of(".") + 1 };
    std::string extension { file.substr(extensionPosition) };
    if (extension == "png") mcrt::PngImageExporter::save(image, file);
    else if (extension == "ppm") mcrt::NetpbmImageExporter::save(image, file);
    else if (extension == "ff") mcrt::FarbfeldImageExporter::save(image, file);
    else throw std::runtime_error { "Invalid format for '" + file + "'!" };
}

// This is the binary 8-bit per channel variant of PPM, which hasn't alpha-channel.
void mcrt::NetpbmImageExporter::save(const Image& image, const std::string& file) {
    std::ofstream fileStream { file, std::ios::binary };
    if (!fileStream) throw std::runtime_error { "Could not create '" + file + "'!"};
    // Write the format magic data, as: https://en.wikipedia.org/wiki/Netpbm_format.
    fileStream << "P6 " <<  image.getWidth() << " " << image.getHeight() << " 255 ";
    std::vector<mcrt::Color<unsigned char>> normalizedPixelData = image.getNormalizedPixelData();
    for (const auto& pixelData : normalizedPixelData )
        fileStream.write((const char*) pixelData, 3);
}

// Each channel is 16-bits long in big-endian and the image has an alpha-channel too.
void mcrt::FarbfeldImageExporter::save(const Image& image, const std::string& file) {
    std::ofstream fileStream { file, std::ios::binary };
    if (!fileStream) throw std::runtime_error { "Couldn't create '" + file + "'!"};
    // See 'man farbfeld' or just go over to www.suckless.org/tools/farbfeld.
    fileStream << "farbfeld"; // Farbfeld's magic number for the file format.
    std::uint32_t imageWidth  { static_cast<std::uint32_t>(image.getWidth()) },
                  imageHeight { static_cast<std::uint32_t>(image.getHeight()) };
    char* imageWidthBytes  = (char*) &imageWidth,
        * imageHeightBytes = (char*) &imageHeight;

    // Since Farbfeld works with big-endian, we need to write data in the rev. order it is laid in memory.
    fileStream << imageWidthBytes[3]  << imageWidthBytes[2]  << imageWidthBytes[1]  << imageWidthBytes[0];
    fileStream << imageHeightBytes[3] << imageHeightBytes[2] << imageHeightBytes[1] << imageHeightBytes[0];

    std::vector<mcrt::Color<unsigned char>> normalizedPixelData = image.getNormalizedPixelData();
    for (const auto& pixelData : normalizedPixelData) {
        std::uint16_t scaledColors[4];
        // We scale up the colors fr. 8-bit to 16-bit RGBA-color range.
        scaledColors[0] = static_cast<std::uint16_t>(pixelData.r) << 8;
        scaledColors[1] = static_cast<std::uint16_t>(pixelData.g) << 8;
        scaledColors[2] = static_cast<std::uint16_t>(pixelData.b) << 8;
        scaledColors[3] = static_cast<std::uint16_t>(pixelData.a) << 8;

        // We prepare the 16-bit pixel values to be encoded using big-endian.
        char* colorBytes[] { (char*) &scaledColors[0], (char*) &scaledColors[1],
                             (char*) &scaledColors[2], (char*) &scaledColors[3] };

        // Write the pixel colors using big-endian format.
        fileStream << colorBytes[0][1] << colorBytes[0][0]
                   << colorBytes[1][1] << colorBytes[1][0]
                   << colorBytes[2][1] << colorBytes[2][0]
                   << colorBytes[3][1] << colorBytes[3][0];
    }
}

void mcrt::PngImageExporter::save(const Image& image, const std::string& file) {
    auto data = image.getNormalizedPixelData();
    // Converts the double vector data to some sort of byte representation for saving to a file.
    std::vector<unsigned char>& imageData = reinterpret_cast<std::vector<unsigned char>&>(data);
    unsigned errorCode { lodepng::encode(file, imageData, image.getWidth(),
                                                          image.getHeight()) };
    if (errorCode) throw std::runtime_error { lodepng_error_text(errorCode) };
}
