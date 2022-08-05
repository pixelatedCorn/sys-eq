#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    uint32_t cols;
    uint32_t rows;
    float* data;
} Matrix;

Matrix CreateMatrix(uint32_t cols, uint32_t rows)
{
    Matrix mat;
    mat.cols = cols;
    mat.rows = rows;
    mat.data = malloc(mat.cols * mat.rows * sizeof(float));
    return mat;
}

void FreeMatrix(Matrix* mat)
{
    free(mat->data);
}

float* At(Matrix* mat, uint32_t col, uint32_t row)
{
    return &mat->data[row * mat->cols + col];
}

void ScaleMatrix(Matrix* mat, float val)
{
    for (int i = 0; i < mat->rows; i++)
    {
        for (int j = 0; j < mat->cols; j++)
        {
            *At(mat, j, i) *= val;
        }
    }
}

float DotProduct(Matrix* a, Matrix* b, uint32_t col, uint32_t row)
{
    uint32_t elems = a->cols;
    float result = 0;
    for (int i = 0; i < elems; i++)
    {
        result += *At(a, i, row) * *At(b, col, i);
    }
    return result;
}

Matrix MultiplyMatrices(Matrix* a, Matrix* b)
{
    if (a->cols != b->rows)
    {
        printf("cannot multiply matrices...");
        exit(1);
    }
    Matrix result = CreateMatrix(b->cols, a->rows);
    for (int i = 0; i < result.rows; i++)
    {
        for (int j = 0; j < result.cols; j++)
        {
            *At(&result, j, i) = DotProduct(a, b, j, i);
        }
    }
    return result;
}

Matrix Minor(Matrix* mat, uint32_t col, uint32_t row)
{
    Matrix minor = CreateMatrix(mat->cols - 1, mat->rows - 1);
    uint32_t curRow = 0;
    for (int i = 0; i < mat->rows; i++)
    {
        if (i == row) continue;
        uint32_t curCol = 0;
        for (int j = 0; j < mat->cols; j++)
        {
            if (j == col) continue;
            *At(&minor, curCol, curRow) = *At(mat, j, i);
            curCol++;
        }
        curRow++;
    }

    return minor;
}

float Determinant(Matrix* mat)
{
    if (mat->cols == 2)
    {
        return mat->data[0] * mat->data[3] - mat->data[1] * mat->data[2];
    }

    uint8_t op = 0;
    int32_t det = 0;
    for (int i = 0; i < mat->cols; i++)
    {
        Matrix minor = Minor(mat, i, 0);
        float cur = *At(mat, i, 0);
        if (op == 0)
        {
            op = 1;
            det += cur * Determinant(&minor);
        }
        else
        {
            op = 0;
            det -= cur * Determinant(&minor);
        }
        FreeMatrix(&minor);
    }

    return det;
}

Matrix Cofactor(Matrix* mat)
{
    Matrix cof = CreateMatrix(mat->cols, mat->rows);
    uint8_t s = 0;
    for (int i = 0; i < mat->rows; i++)
    {
        for (int j = 0; j < mat->cols; j++)
        {
            Matrix minor = Minor(mat, j, i);
            float* val = At(&cof, j, i);
            *val = Determinant(&minor);
            if (s == 0)
            {
                s = 1;
            }
            else
            {
                s = 0;
                *val *= -1;
            }
            FreeMatrix(&minor);
        }
    }
    return cof;
}

Matrix Transpose(Matrix* mat)
{
    Matrix trans = CreateMatrix(mat->cols, mat->rows);
    for (int i = 0; i < mat->rows; i++)
    {
        for (int j = 0; j < mat->cols; j++)
        {
            *At(&trans, j, i) = *At(mat, i, j);
        }
    }
    return trans;
}

Matrix Adjunct(Matrix* mat)
{
    Matrix cof = Cofactor(mat);
    Matrix adjunct = Transpose(&cof);
    FreeMatrix(&cof);
    return adjunct;
}

Matrix Inverse(Matrix* mat)
{
    float det = Determinant(mat);
    if (det == 0)
    {
        printf("matrix has no inverse...");
        exit(1);
    }
    det = 1 / det;
    Matrix inverse = Adjunct(mat);
    ScaleMatrix(&inverse, det);
    return inverse;
}

void PrintMatrix(Matrix* mat)
{
    float* data = mat->data;
    for (uint32_t i = 0; i < mat->rows; i++)
    {
        for (uint32_t j = 0; j < mat->cols; j++)
        {
            printf("%.2f ", *At(mat, j, i));
        }
        printf("\n");
    }
}

int main()
{

    int32_t matSize = 0;
    printf("enter number of variables: ");
    scanf("%d", &matSize);

    if (matSize < 2)
    {
        printf("invalid number of variables...");
        exit(-1);
    }

    Matrix coeffs = CreateMatrix(matSize, matSize);
    uint32_t numElems = coeffs.rows * coeffs.cols;
    float* matData = coeffs.data;

    for (uint32_t i = 0; i < numElems; i++)
    {
        float coeff;
        printf("enter coefficient: ");
        scanf("%f", &coeff);
        *matData++ = coeff;
    }
    
    Matrix rightSide = CreateMatrix(1, matSize);
    numElems = rightSide.rows;
    matData = rightSide.data;

    for (uint32_t i = 0; i < numElems; i++)
    {
        float val;
        printf("enter number on right side of equal sign: ");
        scanf("%f", &val);
        *matData++ = val;
    }

    Matrix coeffsInv = Inverse(&coeffs);
    Matrix result = MultiplyMatrices(&coeffsInv, &rightSide);
    PrintMatrix(&result);
    
    FreeMatrix(&coeffsInv);
    FreeMatrix(&result);
    FreeMatrix(&rightSide);
    FreeMatrix(&coeffs);
    exit(0);
}