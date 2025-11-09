#include <stdio.h>
#include <stdlib.h>
#include "shapefil.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s input_shapefile_without_extension\n", argv[0]);
        return 1;
    }

    SHPHandle hSHP = SHPOpen(argv[1], "rb");
    if (!hSHP)
    {
        printf("Error: Could not open SHP file.\n");
        return 1;
    }

    DBFHandle hDBF = DBFOpen(argv[1], "rb");
    if (!hDBF)
    {
        printf("Warning: Could not open DBF file. Continuing without attributes.\n");
    }

    int nEntities, shapeType;
    double minBound[4], maxBound[4];
    SHPGetInfo(hSHP, &nEntities, &shapeType, minBound, maxBound);

    printf("===== SHAPEFILE HEADER INFORMATION =====\n");
    printf("Shapefile Base Name: %s\n", argv[1]);
    printf("Shape Type (numeric): %d\n", shapeType);
    printf("Number of Shapes: %d\n", nEntities);
    printf("Bounding Box:\n");
    printf("  Xmin: %.10g\n  Ymin: %.10g\n  Xmax: %.10g\n  Ymax: %.10g\n",
           minBound[0], minBound[1], maxBound[0], maxBound[1]);
    printf("  Zmin: %.10g\n  Zmax: %.10g\n", minBound[2], maxBound[2]);
    printf("  Mmin: %.10g\n  Mmax: %.10g\n\n", minBound[3], maxBound[3]);

    if (hDBF)
    {
        int numRecords = DBFGetRecordCount(hDBF);
        int numFields  = DBFGetFieldCount(hDBF);

        printf("===== DBF ATTRIBUTE TABLE HEADER =====\n");
        printf("Number of Attribute Records: %d\n", numRecords);
        printf("Number of Fields: %d\n\n", numFields);

        printf("Fields:\n");
        for (int f = 0; f < numFields; f++)
        {
            char fieldName[12];
            int fieldWidth, fieldDecimals;
            DBFFieldType ftype = DBFGetFieldInfo(hDBF, f, fieldName,
                                                &fieldWidth, &fieldDecimals);

            printf("  %-11s  Type=%d  Width=%d  Decimals=%d\n",
                   fieldName, ftype, fieldWidth, fieldDecimals);
        }
        printf("\n");
    }

    printf("===== SHAPE RECORDS =====\n\n");

    for (int i = 0; i < nEntities; i++)
    {
        SHPObject *obj = SHPReadObject(hSHP, i);
        if (!obj) continue;

        printf("Record %d:\n", i);

        if (hDBF)
        {
            int numFields = DBFGetFieldCount(hDBF);
            for (int f = 0; f < numFields; f++)
            {
                char fieldName[12];
                int fieldWidth, fieldDecimals;
                DBFGetFieldInfo(hDBF, f, fieldName, &fieldWidth, &fieldDecimals);

                const char *value = DBFReadStringAttribute(hDBF, i, f);
                if (!value) value = "(null)";

                printf("  %s: %s\n", fieldName, value);
            }
        }
        printf("\n");

        for (int j = 0; j < obj->nVertices; j++)
        {
            printf("  Vertex %d: (%.6f, %.6f)\n",
                   j, obj->padfX[j], obj->padfY[j]);
        }
        printf("\n");


        SHPDestroyObject(obj);
    }

    SHPClose(hSHP);
    if (hDBF) DBFClose(hDBF);
    return 0;
}
