#include <omp.h>
#include <bits/stdc++.h>

using namespace std;

void releaseResourcesM( int **matrix);
void fillM(int size, int **&matrix);
int **commonMM(int size, int **firstMatrix, int **secondMatrix);
int **createM(int size);
int **takePart(int size, int **matrix, int row, int column);
int **addorSub2M(int size, int **firstMatrix, int **secondMatrix, bool addition);
int **strassen(int size, int **firstMatrix, int **secondMatrix);



int main()
{
    int size;
    cout << "Please Enter the Size of The Matrix: ";
    cin >> size;

    int** firstMatrix = createM(size);
    fillM(size, firstMatrix);

    int** secondMatrix = createM(size);
    fillM(size, secondMatrix);

    double startParStrassen = omp_get_wtime();
    int** resultMatrix;

    omp_set_num_threads(8);

    #pragma omp parallel
    {
    #pragma omp single
        {
            resultMatrix = strassen(size, firstMatrix, secondMatrix);
        }
    }
    double endParStrassen = omp_get_wtime();
    cout << "\n OMP Strassen Code took time: ";
    cout << endParStrassen - startParStrassen << endl;

    return 0;
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
void releaseResourcesM( int **matrix)
{
    free(matrix);
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

int** commonMM(int size, int** firstMatrix, int** secondMatrix)
{
    int** resultMatrix = createM(size);

    int i, j;

    #pragma omp parallel for collapse(2)
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
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

int** strassen(int size, int** firstMatrix, int** secondMatrix)
{

    if (size <= 32)
    {
        return commonMM(size, firstMatrix, secondMatrix);
    }

    int size_red = size / 2;

    int** green = takePart(size, firstMatrix, 0, 0);
    int** blue = takePart(size, firstMatrix, 0, size_red);
    int** yellow = takePart(size, firstMatrix, size_red, 0);
    int** red = takePart(size, firstMatrix, size_red, size_red);
    int** purple = takePart(size, secondMatrix, 0, 0);
    int** black = takePart(size, secondMatrix, 0, size_red);
    int** grey = takePart(size, secondMatrix, size_red, 0);
    int** white = takePart(size, secondMatrix, size_red, size_red);

    int** result1;
    #pragma omp task shared(result1)
    {
        int** blueAndredSub = addorSub2M(size_red, blue, red, false);
        int** greyAndwhiteAdd = addorSub2M(size_red, grey, white, true);
        result1 = strassen(size_red, blueAndredSub, greyAndwhiteAdd);
        releaseResourcesM( blueAndredSub);
        releaseResourcesM( greyAndwhiteAdd);
    }

    int** result2;
    #pragma omp task shared(result2)
    {
        int** greenAndredAdd = addorSub2M(size_red, green, red, true);
        int** purpleAndwhiteAdd = addorSub2M(size_red, purple, white, true);
        result2 = strassen(size_red, greenAndredAdd, purpleAndwhiteAdd);
        releaseResourcesM( greenAndredAdd);
        releaseResourcesM( purpleAndwhiteAdd);
    }

    int** result3;
    #pragma omp task shared(result3)
    {
        int** greenAndyellowSub = addorSub2M(size_red, green, yellow, false);
        int** purpleAndblackAdd = addorSub2M(size_red, purple, black, true);
        result3 = strassen(size_red, greenAndyellowSub, purpleAndblackAdd);
        releaseResourcesM( greenAndyellowSub);
        releaseResourcesM( purpleAndblackAdd);
    }

    int** result4;
    #pragma omp task shared(result4)
    {
        int** greenAndblueAdd = addorSub2M(size_red, green, blue, true);
        result4 = strassen(size_red, greenAndblueAdd, white);
        releaseResourcesM( greenAndblueAdd);
    }

    int** result5;
    #pragma omp task shared(result5)
    {
        int** blackAndwhiteSub = addorSub2M(size_red, black, white, false);
        result5 = strassen(size_red, green, blackAndwhiteSub);
        releaseResourcesM( blackAndwhiteSub);
    }

    int** result6;
    #pragma omp task shared(result6)
    {
        int** greyAndpurpleSub = addorSub2M(size_red, grey, purple, false);
        result6 = strassen(size_red, red, greyAndpurpleSub);
        releaseResourcesM( greyAndpurpleSub);
    }

    int** result7;
    #pragma omp task shared(result7)
    {
        int** yellowAndredAdd = addorSub2M(size_red, yellow, red, true);
        result7 = strassen(size_red, yellowAndredAdd, purple);
        releaseResourcesM( yellowAndredAdd);
    }

    #pragma omp taskwait

    releaseResourcesM( green);
    releaseResourcesM( blue);
    releaseResourcesM( yellow);
    releaseResourcesM( red);
    releaseResourcesM( purple);
    releaseResourcesM( black);
    releaseResourcesM( grey);
    releaseResourcesM( white);

    int** quadrant2;
    #pragma omp task shared(quadrant2)
    {
        int** result1result2Add = addorSub2M(size_red, result1, result2, true);
        int** result6result4Sub = addorSub2M(size_red, result6, result4, false);
        quadrant2 = addorSub2M(size_red, result1result2Add, result6result4Sub, true);
        releaseResourcesM( result1result2Add);
        releaseResourcesM( result6result4Sub);
    }

    int** quadrant3;
    #pragma omp task shared(quadrant3)
    {
        quadrant3 = addorSub2M(size_red, result4, result5, true);
    }

    int** quadrant1;
    #pragma omp task shared(quadrant1)
    {
        quadrant1 = addorSub2M(size_red, result6, result7, true);
    }

    int** quadrant4;
    #pragma omp task shared(quadrant4)
    {
        int** result2result3Sub = addorSub2M(size_red, result2, result3, false);
        int** result5result7Sub = addorSub2M(size_red, result5, result7, false);
        quadrant4 = addorSub2M(size_red, result2result3Sub, result5result7Sub, true);
        releaseResourcesM( result2result3Sub);
        releaseResourcesM( result5result7Sub);
    }

    #pragma omp taskwait

    releaseResourcesM( result1);
    releaseResourcesM( result2);
    releaseResourcesM( result3);
    releaseResourcesM( result4);
    releaseResourcesM( result5);
    releaseResourcesM( result6);
    releaseResourcesM( result7);

    int** resultMatrix = join(size_red, quadrant2, quadrant3, quadrant1, quadrant4);

    releaseResourcesM( quadrant2);
    releaseResourcesM( quadrant3);
    releaseResourcesM( quadrant1);
    releaseResourcesM( quadrant4);

    return resultMatrix;
}
