#define _CRT_SECURE_NO_WARNINGS
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _pak_record
{
	unsigned long type;
	unsigned long offset;
	unsigned long length;
	unsigned long checksum;
	unsigned long reserved;
	unsigned long reserved2;
	unsigned long reserved3;
	unsigned long flags;
	char * filename;
} pak_record;

char * ftypes[] =
{
	".ske", ".ska", ".fam", ".shd", ".mqb", ".stex", ".skin", ".mdl",
	".mcol", ".img", ".pimg", ".tex", ".geom", ".col", ".sqb", ".pfx",
	".nqb", ".rnb", ".nav", ".fnt", ".gap", ".dbg", ".qb"
};

unsigned long fhash[] =
{
	0x7330095C, 0x745DCD45, 0x9DE9087F, 0x2F1A6A09, 0x4BC1E85E, 0x2B0A3095,
	0x64112E85, 0x9BCC234D, 0x6290993B, 0xDAD5E950, 0x6890993B, 0x8BFA5E8E,
	0x7EA7357B, 0x72A6D78C, 0x5D796624, 0xA7DEA591, 0x49875607, 0x91E1028D,
	0x199F902B, 0x7E1ABC70, 0xFF2D0E91, 0x559566CC, 0xA7F505C4
};

int num_ftype = 22;

void mkdirs(char * filepath)
{
	char * fpcp = filepath;
	char * fetch = fpcp;
	char * cwd = _getcwd(0, 0);

	char * temp = 0;
	long tlen = 0;

	while (*fpcp && *fetch)
	{
		while (*fetch && *fetch != '\\')
		{
			fetch++;
			if (*fetch == '/') *fetch = '\\';
		}

		if (!*fetch) break;

		tlen = (fetch - fpcp) + 2;
		temp = (char *)malloc(tlen);
		memset(temp, 0, tlen--);
		strncpy(temp, (const char *)fpcp, tlen);

		if (_chdir(temp))
		{
			_mkdir(temp);
			_chdir(temp);
		}

		free(temp);

		if (*(fetch++))
		{
			fpcp = fetch;
		}
		else break;     /* Break if its the end of the string */
	}

	_chdir(cwd);     /* Go back to previous working directory */
	free(cwd);      /* Free cwd buffer */
}

char * getFileName(char * filePath)
{
	char * startchar = filePath;

	while (*filePath)
		if (*(filePath++) == '\\') startchar = filePath;

	return startchar;
}

void printFileType(unsigned long type)
{
	int f;

	for (f = 0; f < num_ftype; f++)
	{
		if (type == fhash[f])
		{
			printf("\t%s file\n", ftypes[f]);
			return;
		}
	}

	printf("\tUnknown file type\n");
}

int i;
int f;
int u;
int o;
char * buffer;
char * bufpnt;
char * bufpnt2;
unsigned long flen;
pak_record * record;
char outfname[512];

void openPak(char * pak)
{
	int z;
	FILE * pFile;
	FILE * outFile;

	pFile = fopen(pak, "rb");

	if (pFile != 0)
	{
		z = 0;
		u = 0;
		fseek(pFile, 0, SEEK_END);
		flen = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		buffer = (char *)malloc(flen);
		bufpnt = buffer;

		fread(buffer, flen, 1, pFile);
		fclose(pFile);

		while ((unsigned char)(*bufpnt) != 0xCD)
		{
			z++;

			bufpnt2 = bufpnt;
			memset(record, 0, sizeof(pak_record));
			memcpy(record, bufpnt, 32);
			bufpnt += 32;

			switch (record->flags & 0x00000020)
			{
			case 0x00000000:
			{
				u++;
				printf("\tWarning: Unnamed file detected, extraction skipped\n");
			}
			break;
			default:
			{
				record->filename = bufpnt;
				mkdirs(record->filename);

				bufpnt += 160;

				outFile = fopen(record->filename, "wb");

				if (outFile != 0)
				{
					fwrite((bufpnt2 + record->offset), 1, record->length, outFile);
					fclose(outFile);
					printf("\tFile %s extracted!\n", record->filename);
				}
				else printf("\tFile %s extraction failed!\n", record->filename);
			}
			break;
			}
		}

		printf("%u records extracted!\n", z);
		free(buffer);
	}
}

void displayHelp()
{
	printf("To use this program, either:\n\n");
	printf("1. Call it on the command line with the arguments of the files you\n");
	printf("\twish to extract\n");
	printf("2. On Windows you can drag as many files as needed on to the program\n");
	printf("\ticon and they will be extracted\n\n");
	printf("Have fun! -reaper\n\n");
}

int main(int nArgs, char * pszArgs[])
{
	_chdir("C:\\");
	int j;

	printf("UnPAKer, v0.3 by reaper\n");
	printf("Warning: Keep out of reach of children!\n\n");
	char * cwd = _getcwd(0, 0);

	record = (pak_record *)malloc(sizeof(pak_record));

	switch (nArgs)
	{
	case 0:
	case 1:
	{
		displayHelp();
	}
	break;
	default:
	{
		for (j = 1; j < nArgs; j++)
		{
			openPak(pszArgs[j]);
		}
	}
	break;
	}

	free(record);

	printf("\n");
	printf("Files extracted to: %s\n", cwd);

	system("PAUSE");
	return 0;
}