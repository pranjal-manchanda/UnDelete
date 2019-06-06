/*
    Program recovers deleted files from a memory card file (card.raw)
    by accessing the "deleted" bits of the memory card and "stitching"
    them together to reproduce the original JPEG images.

    Program requires the name of the input raw file as a command line argument
*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t BYTE;
#define BLOCK_SIZE 512

int main(int argc, char *argv[])
//Program expects to receive the name of the raw file as a command line argument
{
    //Ensure correct number of command line arguments passed
    if (argc != 2)
    {
        fprintf(stderr, "Invalid number of inputs.");
        return 1;
    }

    //Open input file
    FILE *inptr = fopen(argv[1], "r");

    //Error if file is unable to open
    if (inptr == NULL)
    {
        fprintf(stderr, "Unable to open file.");
        return 2;
    }

    //Dynamically allocate space for 512 bytes
    BYTE *block = malloc(BLOCK_SIZE);

    //If space unable to be allocated
    if (block == NULL)
    {
        fprintf(stderr, "Dynamic Memory Allocation Problem");
        return 3;
    }

    FILE *jpeg = NULL; //Output file
    bool found = false; //Keep track of status of jpeg search
    int count = 0; //File Count
    char filename[8]; //File name format file_count###.jpg

    //Keep reading until end of file
    while (fread(block, BLOCK_SIZE, 1, inptr))
    {
        //Start of a new JPEG?
        if (block[0] == 0xff && block[1] == 0xd8 && block[2] == 0xff && (block[3] & 0xf0) == 0xe0)
        {
            //Close previous JPEG if one exists
            if (found == true)
            {
                fclose(jpeg);
            }

            //If one does not exist, the note that one has been found
            else
            {
                found = true;
            }

            //Create output file
            sprintf(filename, "%03d.jpg", count);
            jpeg = fopen(filename, "w");
            count++;
        }

        if (found == true)
        {
            //Write to output file
            fwrite(block, BLOCK_SIZE, 1, jpeg);
        }
    }

    //Close files
    fclose(inptr);
    fclose(jpeg);

    //Free up space
    free(block);
}