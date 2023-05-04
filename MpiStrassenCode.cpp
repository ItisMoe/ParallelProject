#include <mpi.h>
#include <bits/stdc++.h>

using namespace std;

void releaseResourcesM(int **matrix);
void fillM(int size, int **&matrix);
int **commonMM(int size, int **firstMatrix, int **secondMatrix);
int **createM(int size);
int **takePart(int size, int **matrix, int row, int column);
int **addorSub2M(int size, int **firstMatrix, int **secondMatrix, bool addition);
int **join(int size_red, int **quadrant2, int **quadrant3, int **quadrant1, int **quadrant4);
int **strassen(int size, int **firstMatrix, int **secondMatrix);
void strassen(int size, int **firstMatrix, int **secondMatrix, int **&resultMatrix, int rank);

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int main_rank;
    int num_process;

    MPI_Comm_rank(MPI_COMM_WORLD, &main_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_process);

    int size;
    if (main_rank == 0)
    {
        cout << endl;
        cout << "Please Enter the Size of The Matrix: ";
        cin >> size;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int **firstMatrix = createM(size);
    int **secondMatrix = createM(size);

    if (main_rank == 0)
    {
        fillM(size, firstMatrix);
        fillM(size, secondMatrix);
    }

    MPI_Bcast(&(firstMatrix[0][0]), size * size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&(secondMatrix[0][0]), size * size, MPI_INT, 0, MPI_COMM_WORLD);

    double startTime = MPI_Wtime();

    int **resultMatrix;
    strassen(size, firstMatrix, secondMatrix, resultMatrix, main_rank);

    double endTime = MPI_Wtime();

    if (main_rank == 0)
    {
        cout << "\nMPI Strassen Code time took:";
        cout << endTime - startTime << endl;
    }

    MPI_Finalize();

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
    releaseResourcesM(blueAndredSub);
    releaseResourcesM(greyAndwhite);

    int **greenAndredAdd = addorSub2M(size_red, green, red, true);
    int **purpleAndwhiteAdd = addorSub2M(size_red, purple, white, true);
    int **result2 = strassen(size_red, greenAndredAdd, purpleAndwhiteAdd);
    releaseResourcesM(greenAndredAdd);
    releaseResourcesM(purpleAndwhiteAdd);

    int **greenAndyellowSub = addorSub2M(size_red, green, yellow, false);
    int **purpleAndblackAdd = addorSub2M(size_red, purple, black, true);
    int **result3 = strassen(size_red, greenAndyellowSub, purpleAndblackAdd);
    releaseResourcesM(greenAndyellowSub);
    releaseResourcesM(purpleAndblackAdd);

    int **greenAndblueAdd = addorSub2M(size_red, green, blue, true);
    int **result4 = strassen(size_red, greenAndblueAdd, white);
    releaseResourcesM(greenAndblueAdd);
    releaseResourcesM(blue);

    int **blackAndwhiteSub = addorSub2M(size_red, black, white, false);
    int **result5 = strassen(size_red, green, blackAndwhiteSub);
    releaseResourcesM(blackAndwhiteSub);
    releaseResourcesM(green);
    releaseResourcesM(black);
    releaseResourcesM(white);

    int **greyAndpurpleSub = addorSub2M(size_red, grey, purple, false);
    int **result6 = strassen(size_red, red, greyAndpurpleSub);
    releaseResourcesM(greyAndpurpleSub);
    releaseResourcesM(grey);

    int **yellowAndredAdd = addorSub2M(size_red, yellow, red, true);
    int **result7 = strassen(size_red, yellowAndredAdd, purple);
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

void strassen(int size, int **firstMatrix, int **secondMatrix, int **&resultMatrix, int rank)
{

    if (size == 1)
    {
        resultMatrix = createM(1);
        resultMatrix[0][0] = firstMatrix[0][0] * secondMatrix[0][0];
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

    int **result1 = createM(size_red);
    int **result2 = createM(size_red);
    int **result3 = createM(size_red);
    int **result4 = createM(size_red);
    int **result5 = createM(size_red);
    int **result6 = createM(size_red);
    int **result7 = createM(size_red);

    if (rank == 0)
    {
        MPI_Recv(&(result1[0][0]), size_red * size_red, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&(result2[0][0]), size_red * size_red, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Recv(&(result3[0][0]), size_red * size_red, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&(result4[0][0]), size_red * size_red, MPI_INT, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Recv(&(result5[0][0]), size_red * size_red, MPI_INT, 5, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Recv(&(result6[0][0]), size_red * size_red, MPI_INT, 6, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Recv(&(result7[0][0]), size_red * size_red, MPI_INT, 7, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if (rank == 7)
    {
        int **blueAndredSub = addorSub2M(size_red, blue, red, false);
        int **greyAndwhiteAdd = addorSub2M(size_red, grey, white, true);
        result1 = strassen(size_red, blueAndredSub, greyAndwhiteAdd);
        releaseResourcesM(blueAndredSub);
        releaseResourcesM(greyAndwhiteAdd);
        MPI_Send(&(result1[0][0]), size_red * size_red, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 6)
    {
        int **greenAndredAdd = addorSub2M(size_red, green, red, true);
        int **purpleAndwhiteAdd = addorSub2M(size_red, purple, white, true);
        result2 = strassen(size_red, greenAndredAdd, purpleAndwhiteAdd);
        releaseResourcesM(greenAndredAdd);
        releaseResourcesM(purpleAndwhiteAdd);
        MPI_Send(&(result2[0][0]), size_red * size_red, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 5)
    {
        int **greenAndyellowSub = addorSub2M(size_red, green, yellow, false);
        int **purpleAndblackAdd = addorSub2M(size_red, purple, black, true);
        result3 = strassen(size_red, greenAndyellowSub, purpleAndblackAdd);
        releaseResourcesM(greenAndyellowSub);
        releaseResourcesM(purpleAndblackAdd);
        MPI_Send(&(result3[0][0]), size_red * size_red, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 4)
    {
        int **greenAndblueAdd = addorSub2M(size_red, green, blue, true);
        result4 = strassen(size_red, greenAndblueAdd, white);
        releaseResourcesM(greenAndblueAdd);
        MPI_Send(&(result4[0][0]), size_red * size_red, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    releaseResourcesM(blue);

    if (rank == 3)
    {
        int **blackAndwhiteSub = addorSub2M(size_red, black, white, false);
        result5 = strassen(size_red, green, blackAndwhiteSub);
        releaseResourcesM(blackAndwhiteSub);
        MPI_Send(&(result5[0][0]), size_red * size_red, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    releaseResourcesM(green);
    releaseResourcesM(black);
    releaseResourcesM(white);

    if (rank == 2)
    {
        int **greyAndpurpleSub = addorSub2M(size_red, grey, purple, false);
        result6 = strassen(size_red, red, greyAndpurpleSub);
        releaseResourcesM(greyAndpurpleSub);
        MPI_Send(&(result6[0][0]), size_red * size_red, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    releaseResourcesM(grey);

    if (rank == 1)
    {
        int **yellowAndredAdd = addorSub2M(size_red, yellow, red, true);
        result1 = strassen(size_red, yellowAndredAdd, purple);
        releaseResourcesM(yellowAndredAdd);
        MPI_Send(&(result7[0][0]), size_red * size_red, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    releaseResourcesM(yellow);
    releaseResourcesM(red);
    releaseResourcesM(purple);

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
    {
        int **result1result2Add = addorSub2M(size_red, result1, result2, true);
        int **result6result4Sub = addorSub2M(size_red, result6, result4, false);
        int **quadrant2 = addorSub2M(size_red, result1result2Add, result6result4Sub, true);
        releaseResourcesM(result1result2Add);
        releaseResourcesM(result6result4Sub);

        int **quadrant3 = addorSub2M(size_red, result4, result5, true);

        int **quadrant1 = addorSub2M(size_red, result6, result7, true);

        int **result2result3Sub = addorSub2M(size_red, result2, result3, false);
        int **result5result7Sub = addorSub2M(size_red, result5, result7, false);
        int **quadrant4 = addorSub2M(size_red, result2result3Sub, result5result7Sub, true);
        releaseResourcesM(result2result3Sub);
        releaseResourcesM(result5result7Sub);

        resultMatrix = join(size_red, quadrant2, quadrant3, quadrant1, quadrant4);

        releaseResourcesM(quadrant2);
        releaseResourcesM(quadrant3);
        releaseResourcesM(quadrant1);
        releaseResourcesM(quadrant4);
    }

    releaseResourcesM(result1);
    releaseResourcesM(result2);
    releaseResourcesM(result3);
    releaseResourcesM(result4);
    releaseResourcesM(result5);
    releaseResourcesM(result6);
    releaseResourcesM(result7);
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
void releaseResourcesM(int **matrix)
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
