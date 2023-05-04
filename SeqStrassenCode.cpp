#include <bits/stdc++.h>

using namespace std;

void releaseResourcesM(int size, int **matrix);
void fillM(int size, int **&matrix);
int **commonMM(int size, int **firstMatrix, int **secondMatrix);
int **createM(int size);
int **takePart(int size, int **matrix, int row, int column);
int **addorSub2M(int size, int **firstMatrix, int **secondMatrix, bool addition);
int **join(int size_red, int **quadrant2, int **quadrant3, int **quadrant1, int **quadrant4);
int **strassen(int size, int **firstMatrix, int **secondMatrix);

int main()
{
    int size;
    cout << "Please Enter the Size of The Matrix:";
    cin >> size;

    int **firstMatrix = createM(size);
    fillM(size, firstMatrix);

    int **secondMatrix = createM(size);
    fillM(size, secondMatrix);

    clock_t start;
    clock_t end;
    start = clock();
    int **resultMatrix = strassen(size, firstMatrix, secondMatrix);
    end = clock();
    double time = double(end - start) / double(CLOCKS_PER_SEC);
    cout << "\nSeq Strassen Code : " << time << " seconds";

    cout << endl;

    cout << endl
         << endl;

    return 0;
}
int **createM(int size)
{
    int *num = (int *)malloc(size * size * sizeof(int));
    int **arr = (int **)malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++)
    {
        arr[i] = &(num[size * i]);
    }
    return arr;
}

void fillM(int size, int **&matrix)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = i + j + rand() % 101;
        }
    }
}

int **strassen(int size, int **firstMatrix, int **secondMatrix)
{

    if (size <= 32)
    {
        return commonMM(size, firstMatrix, secondMatrix);
    }

    int size_red = size / 2;

    int **green = takePart(size, firstMatrix, 0, 0);
    int **blue = takePart(size, firstMatrix, 0, size_red);
    int **yellow = takePart(size, firstMatrix, size_red, 0);
    int **red = takePart(size, firstMatrix, size_red, size_red);
    int **purple = takePart(size, secondMatrix, 0, 0);
    int **black = takePart(size, secondMatrix, 0, size_red);
    int **grey = takePart(size, secondMatrix, size_red, 0);
    int **white = takePart(size, secondMatrix, size_red, size_red);

    int **blueAndredSub = addorSub2M(size_red, blue, red, false);
    int **greyAndwhite = addorSub2M(size_red, grey, white, true);
    int **result1 = strassen(size_red, blueAndredSub, greyAndwhite);
    releaseResourcesM(size_red, blueAndredSub);
    releaseResourcesM(size_red, greyAndwhite);

    int **greenAndredAdd = addorSub2M(size_red, green, red, true);
    int **purpleAndwhiteAdd = addorSub2M(size_red, purple, white, true);
    int **result2 = strassen(size_red, greenAndredAdd, purpleAndwhiteAdd);
    releaseResourcesM(size_red, greenAndredAdd);
    releaseResourcesM(size_red, purpleAndwhiteAdd);

    int **greenAndyellowSub = addorSub2M(size_red, green, yellow, false);
    int **purpleAndblackAdd = addorSub2M(size_red, purple, black, true);
    int **result3 = strassen(size_red, greenAndyellowSub, purpleAndblackAdd);
    releaseResourcesM(size_red, greenAndyellowSub);
    releaseResourcesM(size_red, purpleAndblackAdd);

    int **greenAndblueAdd = addorSub2M(size_red, green, blue, true);
    int **result4 = strassen(size_red, greenAndblueAdd, white);
    releaseResourcesM(size_red, greenAndblueAdd);
    releaseResourcesM(size_red, blue);

    int **blackAndwhiteSub = addorSub2M(size_red, black, white, false);
    int **result5 = strassen(size_red, green, blackAndwhiteSub);
    releaseResourcesM(size_red, blackAndwhiteSub);
    releaseResourcesM(size_red, green);
    releaseResourcesM(size_red, black);
    releaseResourcesM(size_red, white);

    int **greyAndpurpleSub = addorSub2M(size_red, grey, purple, false);
    int **result6 = strassen(size_red, red, greyAndpurpleSub);
    releaseResourcesM(size_red, greyAndpurpleSub);
    releaseResourcesM(size_red, grey);

    int **yellowAndredAdd = addorSub2M(size_red, yellow, red, true);
    int **result7 = strassen(size_red, yellowAndredAdd, purple);
    releaseResourcesM(size_red, yellowAndredAdd);
    releaseResourcesM(size_red, yellow);
    releaseResourcesM(size_red, red);
    releaseResourcesM(size_red, purple);

    int **result1result2Add = addorSub2M(size_red, result1, result2, true);
    int **result6result4Sub = addorSub2M(size_red, result6, result4, false);
    int **quadrant2 = addorSub2M(size_red, result1result2Add, result6result4Sub, true);
    releaseResourcesM(size_red, result1result2Add);
    releaseResourcesM(size_red, result6result4Sub);
    releaseResourcesM(size_red, result1);

    int **quadrant3 = addorSub2M(size_red, result4, result5, true);
    releaseResourcesM(size_red, result4);

    int **quadrant1 = addorSub2M(size_red, result6, result7, true);
    releaseResourcesM(size_red, result6);

    int **result2result3Sub = addorSub2M(size_red, result2, result3, false);
    int **result5result7Sub = addorSub2M(size_red, result5, result7, false);
    int **quadrant4 = addorSub2M(size_red, result2result3Sub, result5result7Sub, true);
    releaseResourcesM(size_red, result2result3Sub);
    releaseResourcesM(size_red, result5result7Sub);
    releaseResourcesM(size_red, result2);
    releaseResourcesM(size_red, result3);
    releaseResourcesM(size_red, result5);
    releaseResourcesM(size_red, result7);

    int **resultMatrix = join(size_red, quadrant2, quadrant3, quadrant1, quadrant4);

    releaseResourcesM(size_red, quadrant2);
    releaseResourcesM(size_red, quadrant3);
    releaseResourcesM(size_red, quadrant1);
    releaseResourcesM(size_red, quadrant4);

    return resultMatrix;
}

int **takePart(int size, int **matrix, int row, int column)
{
    int size_red = size / 2;
    int **part = createM(size_red);
    for (int i = 0; i < size_red; i++)
    {
        for (int j = 0; j < size_red; j++)
        {
            part[i][j] = matrix[row + i][column + j];
        }
    }
    return part;
}
void releaseResourcesM(int size, int **matrix)
{
    free(matrix);
}

int **addorSub2M(int size, int **firstMatrix, int **secondMatrix, bool addition)
{
    int **result = createM(size);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (addition)
                result[i][j] = firstMatrix[i][j] + secondMatrix[i][j];
            else
                result[i][j] = firstMatrix[i][j] - secondMatrix[i][j];
        }
    }

    return result;
}
int **commonMM(int size, int **firstMatrix, int **secondMatrix)
{
    int **resultMatrix = createM(size);

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            resultMatrix[i][j] = 0;
            for (int k = 0; k < size; k++)
            {
                resultMatrix[i][j] += firstMatrix[i][k] * secondMatrix[k][j];
            }
        }
    }

    return resultMatrix;
}

int **join(int size_red, int **quadrant2, int **quadrant3, int **quadrant1, int **quadrant4)
{
    int size = 2 * size_red;
    int **result = createM(size);

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (i < size_red && j < size_red)
                result[i][j] = quadrant2[i][j];
            else if (i < size_red)
                result[i][j] = quadrant3[i][j - size_red];
            else if (j < size_red)
                result[i][j] = quadrant1[i - size_red][j];
            else
                result[i][j] = quadrant4[i - size_red][j - size_red];
        }
    }

    return result;
}
