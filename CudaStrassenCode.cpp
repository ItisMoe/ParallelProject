#include <stdio.h>
#include <sys/time.h>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <math.h>


using namespace std;
void releaseResourcesM(int **matrix);
int **commonMM(int size, int **firstMatrix, int **secondMatrix);
int **helper(int size, int **firstMatrix, int **secondMatrix);
int **createM(int size);
int *createMM(int size);
void fillM(int size, int ** &matrix);
int **takePart(int size, int **matrix, int row, int column);
int **addorSub2M(int size, int **firstMatrix, int **secondMatrix, bool add);
int **join(int size_red, int **quadrant2, int **quadrant3, int **quadrant1, int **quadrant4);


int **strassen(int size, int **firstMatrix, int **secondMatrix);

__global__ void multiply(int *firstMatrix, int *secondMatrix, int *product, int size)
{
    int resultMatrix = blockIdx.x * blockDim.x + threadIdx.x;
    int i = resultMatrix / size;
    int j = resultMatrix % size;
    for (int k = 0; k < size; k++)
    {
        product[i * size + j] += firstMatrix[i * size + k] * secondMatrix[k * size + j];
    }
}

int main()
{
    int size=4096;

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
    cout << "Cuda Stressen took " << time << " seconds";

    return 0;
}

int* createMM(int size)
{
    int *num = (int *)malloc(size * size * sizeof(int));
    return num;
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

void fillM(int size, int *&matrix)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i * size + j] = i + j + rand() % 101;
        }
    }
}

void fillM(int size, int **&matrix)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = rand() % 5;
        }
    }
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

int **addorSub2M(int size, int **firstMatrix, int **secondMatrix, bool add)
{
    int **result = createM(size);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (add)
                result[i][j] = firstMatrix[i][j] + secondMatrix[i][j];
            else
                result[i][j] = firstMatrix[i][j] - secondMatrix[i][j];
        }
    }

    return result;
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

void releaseResourcesM(int *matrix)
{
    free(matrix);
}

void releaseResourcesM(int **matrix)
{
    free(matrix);
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
int **helper(int size, int **firstMatrix, int **secondMatrix)
{
    int *matrix1i = createMM(size);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix1i[i * size + j] = firstMatrix[i][j];
        }
    }

    int *matrix2i = createMM(size);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix2i[i * size + j] = secondMatrix[i][j];
        }
    }

    int *producti = createMM(size);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            producti[i * size + j] = 0;
        }
    }

    size_t chunks = size * size * sizeof(int);

    int *matrix1j, *matrix2j, *productj;

    cudaMalloc(&matrix1j, chunks);
    cudaMalloc(&matrix2j, chunks);
    cudaMalloc(&productj, chunks);

    cudaMemcpy(matrix1j, matrix1i, chunks, cudaMemcpyHostToDevice);
    cudaMemcpy(matrix2j, matrix2i, chunks, cudaMemcpyHostToDevice);
    cudaMemcpy(productj, producti, chunks, cudaMemcpyHostToDevice);

    int threads = min(1024, size);
    int blocks = (size * size) / threads;
    dim3 gridSize(blocks, 1, 1);
    dim3 blockSize(threads, 1, 1);

    multiply<<<gridSize, blockSize>>>(matrix1j, matrix2j, productj, size);
    cudaDeviceSynchronize();

    cudaMemcpy(producti, productj, chunks, cudaMemcpyDeviceToHost);

    cudaFree(matrix1j);
    cudaFree(matrix2j);
    cudaFree(productj);

    releaseResourcesM(matrix1i);
    releaseResourcesM(matrix2i);

    int **product = createM(size);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            product[i][j] = producti[i * size + j];
        }
    }
    return product;
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
    int **greyAndwhiteAdd = addorSub2M(size_red, grey, white, true);
    int **result1 = helper(size_red, blueAndredSub, greyAndwhiteAdd);
    releaseResourcesM(blueAndredSub);
    releaseResourcesM(greyAndwhiteAdd);

    int **greenAndredAdd = addorSub2M(size_red, green, red, true);
    int **purpleAndwhiteAdd = addorSub2M(size_red, purple, white, true);
    int **result2 = helper(size_red, greenAndredAdd, purpleAndwhiteAdd);
    releaseResourcesM(greenAndredAdd);
    releaseResourcesM(purpleAndwhiteAdd);

    int **greenAndyellowSub = addorSub2M(size_red, green, yellow, false);
    int **purpleAndblackAdd = addorSub2M(size_red, purple, black, true);
    int **result3 = helper(size_red, greenAndyellowSub, purpleAndblackAdd);
    releaseResourcesM(greenAndyellowSub);
    releaseResourcesM(purpleAndblackAdd);

    int **greenAndblueAdd = addorSub2M(size_red, green, blue, true);
    int **result4 = helper(size_red, greenAndblueAdd, white);
    releaseResourcesM(greenAndblueAdd);
    releaseResourcesM(blue);

    int **blackAndwhiteSub = addorSub2M(size_red, black, white, false);
    int **result5 = helper(size_red, green, blackAndwhiteSub);
    releaseResourcesM(blackAndwhiteSub);
    releaseResourcesM(green);
    releaseResourcesM(black);
    releaseResourcesM(white);

    int **greyAndpurpleSub = addorSub2M(size_red, grey, purple, false);
    int **result6 = helper(size_red, red, greyAndpurpleSub);
    releaseResourcesM(greyAndpurpleSub);
    releaseResourcesM(grey);

    int **yellowAndredAdd = addorSub2M(size_red, yellow, red, true);
    int **result7 = helper(size_red, yellowAndredAdd, purple);
    releaseResourcesM(yellowAndredAdd);
    releaseResourcesM(yellow);
    releaseResourcesM(red);
    releaseResourcesM(purple);

    int **result1result2Add = addorSub2M(size_red, result1, result2, true);
    int **result6result4Sub = addorSub2M(size_red, result6, result4, false);
    int **quadrant2 = addorSub2M(size_red, result1result2Add, result6result4Sub, true);
    releaseResourcesM(result1result2Add);
    releaseResourcesM(result6result4Sub);
    releaseResourcesM(result1);

    int **quadrant3 = addorSub2M(size_red, result4, result5, true);
    releaseResourcesM(result4);

    int **quadrant1 = addorSub2M(size_red, result6, result7, true);
    releaseResourcesM(result6);

    int **result2result3Sub = addorSub2M(size_red, result2, result3, false);
    int **result5result7Sub = addorSub2M(size_red, result5, result7, false);
    int **quadrant4 = addorSub2M(size_red, result2result3Sub, result5result7Sub, true);
    releaseResourcesM(result2result3Sub);
    releaseResourcesM(result5result7Sub);
    releaseResourcesM(result2);
    releaseResourcesM(result3);
    releaseResourcesM(result5);
    releaseResourcesM(result7);

    int **resultMatrix = join(size_red, quadrant2, quadrant3, quadrant1, quadrant4);

    releaseResourcesM(quadrant2);
    releaseResourcesM(quadrant3);
    releaseResourcesM(quadrant1);
    releaseResourcesM(quadrant4);

    return resultMatrix;
}
