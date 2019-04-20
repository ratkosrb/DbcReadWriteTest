// Example how to read and write dbc files
//

#include <stdio.h>
#include <vector>

typedef unsigned long uint32;
typedef unsigned long stringref;

struct dbc_header
{
	char magic[4];            // always 'WDBC'
	uint32 record_count;      // records per file
	uint32 field_count;       // fields per record
	uint32 record_size;       // sum (sizeof (field_type_i)) | 0 <= i < field_count. field_type_i is NOT defined in the files.
	uint32 string_block_size; // combined length of all strings including null bytes
};

template<typename record_type>
struct dbc_file
{
	dbc_header header;
	
	std::vector<record_type> records;
	//record_type records[header.record_count];
	std::vector<char> string_block;
	//char string_block[header.string_block_size];
};

struct TaxiNodesEntry5875
{
	uint32    id;                                           // 0
	uint32    map_id;                                       // 1
	float     x;                                            // 2
	float     y;                                            // 3
	float     z;                                            // 4
	stringref name[8];                                      // 5-12
	uint32    string_flags;									// 13
	uint32    mount_creature_id[2];                         // 14-15
};

dbc_file<TaxiNodesEntry5875> g_dbc;

bool ReadFile()
{
	FILE* pFile = fopen("TaxiNodes.dbc", "rb");
	if (pFile == nullptr)
	{
		printf("Failed to read file.\n");
		return false;
	}

	// read header
	fread(&g_dbc.header, sizeof(dbc_header), 1, pFile);

	// expand records
	g_dbc.records.resize(g_dbc.header.record_count);

	// expand string block
	g_dbc.string_block.resize(g_dbc.header.string_block_size);

	// read records
	fread(g_dbc.records.data(), sizeof(TaxiNodesEntry5875), g_dbc.header.record_count, pFile);

	// read strings
	fread(g_dbc.string_block.data(), sizeof(char), g_dbc.header.string_block_size, pFile);

	printf("File read successfully.\n");
	fclose(pFile);
	return true;
}

void PrintFile()
{
	printf("magic: %c%c%c%c\n", g_dbc.header.magic[0], g_dbc.header.magic[1], g_dbc.header.magic[2], g_dbc.header.magic[3]);
	printf("record count: %u\n", g_dbc.header.record_count);
	printf("field count: %u\n", g_dbc.header.field_count);
	printf("record size: %u\n", g_dbc.header.record_size);
	printf("string block size: %u\n", g_dbc.header.string_block_size);

	for (uint32 i = 0; i < g_dbc.records.size(); i++)
	{
		printf("%u, %u, %g, %g, %g, %s, %s, %s, %s, %s, %s, %s, %s, %u, %u, %u\n",
			g_dbc.records[i].id,
			g_dbc.records[i].map_id,
			g_dbc.records[i].x,
			g_dbc.records[i].y,
			g_dbc.records[i].z,
			&g_dbc.string_block[g_dbc.records[i].name[0]],
			&g_dbc.string_block[g_dbc.records[i].name[1]],
			&g_dbc.string_block[g_dbc.records[i].name[2]],
			&g_dbc.string_block[g_dbc.records[i].name[3]],
			&g_dbc.string_block[g_dbc.records[i].name[4]],
			&g_dbc.string_block[g_dbc.records[i].name[5]],
			&g_dbc.string_block[g_dbc.records[i].name[6]],
			&g_dbc.string_block[g_dbc.records[i].name[7]],
			g_dbc.records[i].string_flags,
			g_dbc.records[i].mount_creature_id[0],
			g_dbc.records[i].mount_creature_id[1]);
	}

	printf("End of file reached.\n");
}

void WriteFile()
{
	FILE* pFile = fopen("TaxiNodes2.dbc", "wb");
	if (pFile == nullptr)
	{
		printf("Failed to write file.\n");
		return;
	}

	// write header
	fwrite(&g_dbc.header, 1, sizeof(dbc_header), pFile);

	// write records
	fwrite(g_dbc.records.data(), sizeof(TaxiNodesEntry5875), g_dbc.records.size(), pFile);

	// write strings
	fwrite(g_dbc.string_block.data(), sizeof(char), g_dbc.string_block.size(), pFile);

	printf("File written successfully.\n");
	fclose(pFile);
}

int main()
{
	if (!ReadFile())
		return 1;

	char option = 0;

	printf("Print to screen? (y/n)\n> ");
	while ((option = getchar()) == '\n'){};
	if (option == 'y')
		PrintFile();

	printf("Write do disk? (y/n)\n> ");
	while ((option = getchar()) == '\n'){};
	if (option == 'y')
		WriteFile();
	
	return 0;
}

