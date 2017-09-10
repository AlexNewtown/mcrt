#include "mcrt/image_format.hh"

#include <fstream>
#include <cstdint>

void mcrt::NetpbmImage::save(const std::string& file) const  {
    std::ofstream fileStream { file, std::ios::binary };
    // Write the format magic data, as: https://en.wikipedia.org/wiki/Netpbm_format.
    fileStream << "P6 " <<  this->getWidth() << " " << this->getHeight() << " 255 ";
    for (const auto& pixelData : this->getPixelData())
        fileStream.write((const char*) pixelData, 3);
}

void mcrt::FarbfeldImage::save(const std::string& file) const  {
    std::ofstream fileStream { file, std::ios::binary };
    // See 'man farbfeld' or just go over to www.suckless.org/tools/farbfeld.
    fileStream << "farbfeld"; // Farbfeld's magic number for the file format.
    std::uint32_t imageWidth  { static_cast<std::uint32_t>(this->getWidth()) },
                  imageHeight { static_cast<std::uint32_t>(this->getHeight()) };
    char* imageWidthBytes  = (char*) &imageWidth,
        * imageHeightBytes = (char*) &imageHeight;

    // Since Farbfeld works with big-endian, we need to write data in the rev. order it is laid in memory.
    fileStream << imageWidthBytes[3]  << imageWidthBytes[2]  << imageWidthBytes[1]  << imageWidthBytes[0];
    fileStream << imageHeightBytes[3] << imageHeightBytes[2] << imageHeightBytes[1] << imageHeightBytes[0];

    for (const auto& pixelData : this->getPixelData()) {
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
