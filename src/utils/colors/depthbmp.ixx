export module depthbmp;

import std;

export void saveDepthBmp(const std::vector<float>& depthData, int width, int height, const char* filename)
{
	if (depthData.size() == 0)
		return;

	// Step 1: Normalize the height values to the range [0, 255]
	float minVal = *std::min_element(depthData.begin(), depthData.end());
	float maxVal = *std::max_element(depthData.begin(), depthData.end());

	if (minVal == maxVal)
	{
		minVal = 0.0f;
		maxVal = 1.0f;
	}

	// Create a vector to store the grayscale pixel data
	std::vector<unsigned char> pixelData(width * height);
	for (size_t i = 0; i < depthData.size(); ++i)
	{
		// Normalize the value and convert to [0, 255]
		float normalized = (depthData[i] - minVal) / (maxVal - minVal);
		pixelData[i] = static_cast<unsigned char>(normalized * 255);
	}

	// Step 2: Create the BMP file headers

	// Bitmap File Header (14 bytes)
	unsigned char bmpFileHeader[14] = {
		'B', 'M', // Signature
		0, 0, 0, 0, // File size in bytes (will fill later)
		0, 0, 0, 0, // Reserved
		54, 0, 0, 0 // Pixel data offset (54 bytes)
	};

	// DIB Header (40 bytes)
	unsigned char dibHeader[40] = {
		40, 0, 0, 0,  // DIB header size
		0, 0, 0, 0,   // Width (will fill later)
		0, 0, 0, 0,   // Height (will fill later)
		1, 0,         // Color planes (must be 1)
		8, 0,         // Bits per pixel (8-bit grayscale)
		0, 0, 0, 0,   // Compression (none)
		0, 0, 0, 0,   // Image size (can be 0 for uncompressed)
		0, 0, 0, 0,   // X pixels per meter (unused)
		0, 0, 0, 0,   // Y pixels per meter (unused)
		256, 0, 0, 0, // Number of colors in the palette (256 for 8-bit)
		0, 0, 0, 0    // Important colors (0 means all)
	};

	// Fill in the width and height in the DIB header
	dibHeader[4] = static_cast<unsigned char>(width);
	dibHeader[5] = static_cast<unsigned char>(width >> 8);
	dibHeader[6] = static_cast<unsigned char>(width >> 16);
	dibHeader[7] = static_cast<unsigned char>(width >> 24);

	dibHeader[8] = static_cast<unsigned char>(height);
	dibHeader[9] = static_cast<unsigned char>(height >> 8);
	dibHeader[10] = static_cast<unsigned char>(height >> 16);
	dibHeader[11] = static_cast<unsigned char>(height >> 24);

	// Calculate file size and fill it in the file header
	int fileSize = 54 + 256 * 4 + (width * height); // Headers + palette + pixel data
	bmpFileHeader[2] = static_cast<unsigned char>(fileSize);
	bmpFileHeader[3] = static_cast<unsigned char>(fileSize >> 8);
	bmpFileHeader[4] = static_cast<unsigned char>(fileSize >> 16);
	bmpFileHeader[5] = static_cast<unsigned char>(fileSize >> 24);

	// Step 3: Create the grayscale color palette (256 entries)
	unsigned char colorPalette[1024]; // 256 colors * 4 bytes (BGRA)
	for (int i = 0; i < 256; ++i)
	{
		colorPalette[i * 4] = static_cast<unsigned char>(i);     // Blue
		colorPalette[i * 4 + 1] = static_cast<unsigned char>(i); // Green
		colorPalette[i * 4 + 2] = static_cast<unsigned char>(i); // Red
		colorPalette[i * 4 + 3] = 0;                             // Reserved
	}

	// Step 4: Write the data to the BMP file
	std::ofstream file(filename, std::ios::out | std::ios::binary);
	if (!file)
	{
		std::cerr << "Error: Could not open the file for writing!" << std::endl;
	}

	// Write headers, palette, and pixel data
	file.write(reinterpret_cast<char*>(bmpFileHeader), sizeof(bmpFileHeader));
	file.write(reinterpret_cast<char*>(dibHeader), sizeof(dibHeader));
	file.write(reinterpret_cast<char*>(colorPalette), sizeof(colorPalette));

	// BMP stores rows bottom to top, so we need to flip the rows
	for (int row = height - 1; row >= 0; --row)
	{
		file.write(reinterpret_cast<char*>(&pixelData[row * width]), width);
	}

	file.close();

	std::cout << "BMP file saved successfully: " << filename << std::endl;
}