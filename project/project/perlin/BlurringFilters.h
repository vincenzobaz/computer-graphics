
namespace BlurringFilters {
void blur(GLfloat* tab, int width, int height) {
    float filter[7][7] = {
        {0.00000067,0.00002292,0.00019117,0.00038771,0.00019117,0.00002292,0.00000067},
        {0.00002292,0.00078634,0.00655965,0.01330373,0.00655965,0.00078633,0.00002292},
        {0.00019117,0.00655965,0.05472157,0.11098164,0.05472157,0.00655965,0.00019117},
        {0.00038771,0.01330373,0.11098164,0.22508352,0.11098164,0.01330373,0.00038771},
        {0.00019117,0.00655965,0.05472157,0.11098164,0.05472157,0.00655965,0.00019117},
        {0.00002292,0.00078633,0.00655965,0.01330373,0.00655965,0.00078633,0.00002292},
        {0.00000067,0.00002292,0.00019117,0.00038771,0.00019117,0.00002292,0.00000067}
    };
    /*
    float filter[5][5] = {
        {1,4,7,4,1},
        {4,16,26,16,4},
        {7,26,41,26,7},
        {4,16,26,16,4},
        {1,4,7,4,1}
    };

    for (size_t x = 0; x < 5; ++x) {
        for (size_t y = 0; y < 5; ++y) {
            filter[x][y] /= 273.0f
        }
    }
    */

    for (int row = 6; row < height - 6; ++row) {
        for (int col = 6; col < width - 6; ++col) {
            float tmp = 0;
            for (int i = -3; i <= 3; ++i) {
                for (int j = -3; j <= 3; ++j) {
                    tmp += filter[i + 3][j + 3] * tab[(row + i) * width + (col + j)];
                }
            }
            tab[row * height + col] = tmp;
        }
    }
}
}