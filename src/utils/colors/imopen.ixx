export module imopen;

import std;

// Define a binary image as a flat array (1D vector)
using Image = std::vector<float>;

// Helper function to check if a point is within bounds
bool isInBounds(int x, int y, int rows, int cols)
{
    return x >= 0 && x < rows && y >= 0 && y < cols;
}

// Get the linear index in the flat array
int getIndex(int x, int y, int cols)
{
    return y * cols + x;
}

/// <summary>
/// Performs morphological erosion on an image using a given structuring element.
/// Erosion shrinks bright regions by applying the minimum value from the structuring element.
/// </summary>
/// <param name="image">The input image to be processed.</param>
/// <param name="structElem">The structuring element used for erosion.</param>
/// <param name="rows">The number of rows in the image.</param>
/// <param name="cols">The number of columns in the image.</param>
/// <param name="elemCenterX">The x-coordinate of the structuring element's center.</param>
/// <param name="elemCenterY">The y-coordinate of the structuring element's center.</param>
/// <returns>Returns the image after morphological erosion.</returns>
Image erode(const Image& image, const std::vector<std::vector<int>>& structElem, int rows, int cols, int elemCenterX, int elemCenterY)
{
    int elemRows = structElem.size();
    int elemCols = structElem[0].size();

    Image result(rows * cols, 0.0f); // Default to black pixels

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            bool fits = true;

            // Check if the structuring element fits
            for (int m = 0; m < elemRows; ++m)
            {
                for (int n = 0; n < elemCols; ++n)
                {
                    int nx = x + n - elemCenterX;
                    int ny = y + m - elemCenterY;

                    if (structElem[m][n] == 1)
                    {
                        if (!isInBounds(nx, ny, rows, cols) || image[getIndex(nx, ny, cols)] == 0.0f)
                        {
                            fits = false;
                            break;
                        }
                    }
                }
                if (!fits) break;
            }

            result[getIndex(x, y, cols)] = fits ? 1.0f : 0.0f;
        }
    }

    return result;
}

/// <summary>
/// Performs morphological dilation on an image using a given structuring element.
/// Dilation expands bright regions by applying the maximum value from the structuring element.
/// </summary>
/// <param name="image">The input image to be processed.</param>
/// <param name="structElem">The structuring element used for dilation.</param>
/// <param name="rows">The number of rows in the image.</param>
/// <param name="cols">The number of columns in the image.</param>
/// <param name="elemCenterX">The x-coordinate of the structuring element's center.</param>
/// <param name="elemCenterY">The y-coordinate of the structuring element's center.</param>
/// <returns>Returns the image after morphological dilation.</returns>
Image dilate(const Image& image, const std::vector<std::vector<int>>& structElem, int rows, int cols, int elemCenterX, int elemCenterY)
{
    int elemRows = structElem.size();
    int elemCols = structElem[0].size();

    Image result(rows * cols, 0.0f); // Default to black pixels

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            if (image[getIndex(x, y, cols)] == 1.0f)
            {
                // Apply structuring element
                for (int m = 0; m < elemRows; ++m)
                {
                    for (int n = 0; n < elemCols; ++n)
                    {
                        int nx = x + n - elemCenterX;
                        int ny = y + m - elemCenterY;

                        if (isInBounds(nx, ny, rows, cols) && structElem[m][n] == 1)
                        {
                            result[getIndex(nx, ny, cols)] = 1.0f;
                        }
                    }
                }
            }
        }
    }

    return result;
}

export namespace colors
{
    /// <summary>
    /// Performs morphological opening on an image using a given structuring element.
    /// Opening is defined as erosion followed by dilation.
    /// </summary>
    /// <param name="image">The input image to be processed.</param>
    /// <param name="structElem">The structuring element used for morphological operations.</param>
    /// <param name="rows">The number of rows in the image.</param>
    /// <param name="cols">The number of columns in the image.</param>
    /// <returns>Returns the image after morphological opening.</returns>
    Image imopen(const Image& image, const std::vector<std::vector<int>>& structElem, int rows, int cols)
    {
        int elemCenterX = structElem[0].size() / 2;
        int elemCenterY = structElem.size() / 2;

        Image eroded = erode(image, structElem, rows, cols, elemCenterX, elemCenterY);
        Image opened = dilate(eroded, structElem, rows, cols, elemCenterX, elemCenterY);

        return opened;
    }
}