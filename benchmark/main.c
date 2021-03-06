// 30 november 2016
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "common.h"

// TODO make this just a list of strings
struct item {
	const char *name;
	benchFunc portable;
	benchFunc system;
};

static struct item items[] = {
	{ "BenchmarkUTF8RuneCountASCIIBuf", NULL, NULL },
	{ "BenchmarkUTF8UTF16CountASCIIBuf", NULL, NULL },
	{ "BenchmarkUTF8RuneCountJPBuf", NULL, NULL },
	{ "BenchmarkUTF8UTF16CountJPBuf", NULL, NULL },
	{ "BenchmarkUTF8RuneCountEmojiBuf", NULL, NULL },
	{ "BenchmarkUTF8UTF16CountEmojiBuf", NULL, NULL },
	{ "BenchmarkUTF16RuneCountASCIIBuf", NULL, NULL },
	{ "BenchmarkUTF16UTF8CountASCIIBuf", NULL, NULL },
	{ "BenchmarkUTF16RuneCountJPBuf", NULL, NULL },
	{ "BenchmarkUTF16UTF8CountJPBuf", NULL, NULL },
	{ "BenchmarkUTF16RuneCountEmojiBuf", NULL, NULL },
	{ "BenchmarkUTF16UTF8CountEmojiBuf", NULL, NULL },
	{ "BenchmarkUTF8EncodeRuneASCII", NULL, NULL },
	{ "BenchmarkUTF8EncodeRuneJP", NULL, NULL },
	{ "BenchmarkUTF8EncodeRuneEmoji", NULL, NULL },
	{ "BenchmarkUTF16EncodeRuneASCII", NULL, NULL },
	{ "BenchmarkUTF16EncodeRuneJP", NULL, NULL },
	{ "BenchmarkUTF16EncodeRuneEmoji", NULL, NULL },
	{ "BenchmarkUTF8DecodeRuneASCII", NULL, NULL },
	{ "BenchmarkUTF8DecodeRuneJP", NULL, NULL },
	{ "BenchmarkUTF8DecodeRuneEmoji", NULL, NULL },
	{ "BenchmarkUTF16DecodeRuneASCII", NULL, NULL },
	{ "BenchmarkUTF16DecodeRuneJP", NULL, NULL },
	{ "BenchmarkUTF16DecodeRuneEmoji", NULL, NULL },
	{ NULL, NULL, NULL },
};

void printTime(int64_t nsec)
{
	double d;
	int64_t hours, minutes;

	if (nsec == 0) {
		printf("0s");
		return;
	}
	if (nsec < 1000) {
		printf("%" PRIu64 "ns", nsec);
		return;
	}
	if (nsec < 1000000) {
		d = ((double) nsec) / 1000;
		printf("%.3gus", d);
		return;
	}
	if (nsec < 1000000000) {
		d = ((double) nsec) / 1000000;
		printf("%.6gms", d);
		return;
	}
	hours = nsec / 3600000000000;
	nsec %= 3600000000000;
	if (hours != 0)
		printf("%" PRIu64 "h", hours);
	minutes = nsec / 60000000000;
	nsec %= 60000000000;
	if (minutes != 0)
		printf("%" PRIu64 "m", minutes);
	d = ((double) nsec) / 1000000000;
	printf("%.9gs", d);
}

int main(int argc, char *argv[])
{
	int i;
	int skipUndefined = 0;

	if (argc > 1)
		skipUndefined = strcmp(argv[1], "-skip") == 0;
	init();
	for (i = 0; items[i].name != NULL; i++) {
		uint16_t nsec, pnsec;

		if (skipUndefined && systemBenchFuncs[i] == NULL) {
			printf("Skipping %-50s\n", items[i].name);
			continue;
		}
		printf("Portable %-50s ", items[i].name);
		nsec = bench(portableBenchFuncs[i]);
		printTime(nsec);
		printf("/op\n");
		pnsec = nsec;

		printf("System   %-50s ", items[i].name);
		if (systemBenchFuncs[i] != NULL) {
			nsec = bench(systemBenchFuncs[i]);
			printTime(nsec);
			printf("/op (");
			if (nsec == pnsec)
				printf("the same");
			else if (nsec < pnsec)
				printf("faster");
			else
				printf("slower");
			printf(")\n");
		} else
			printf("undefined\n");
	}

	return 0;
}

// TODO make all of this unsigned char -> char explicitly
const char asciiBufUTF8[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0 };
const char jpBufUTF8[] = { 0xe6, 0x97, 0xa5, 0xe6, 0x9c, 0xac, 0xe8, 0xaa, 0x9e, 0xe6, 0x97, 0xa5, 0xe6, 0x9c, 0xac, 0xe8, 0xaa, 0x9e, 0xe6, 0x97, 0xa5, 0xe6, 0x9c, 0xac, 0xe8, 0xaa, 0x9e, 0xe6, 0x97, 0xa5, 0 };
const char emojiBufUTF8[] = { 0xf0, 0x9f, 0x93, 0x93, 0xf0, 0x9f, 0x93, 0x95, 0xf0, 0x9f, 0x93, 0x97, 0xf0, 0x9f, 0x93, 0x98, 0xf0, 0x9f, 0x93, 0x99, 0xf0, 0x9f, 0x93, 0x94, 0xf0, 0x9f, 0x93, 0x92, 0xf0, 0x9f, 0x92, 0x96, 0xf0, 0x9f, 0x92, 0x98, 0xf0, 0x9f, 0x92, 0x9d, 0xf0, 0x9f, 0x92, 0x9b, 0xf0, 0x9f, 0x92, 0x99, 0xf0, 0x9f, 0x92, 0x9c, 0 };
const uint16_t asciiBufUTF16[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0 };
const uint16_t jpBufUTF16[] = { 0x65e5, 0x672c, 0x8a9e, 0x65e5, 0x672c, 0x8a9e, 0x65e5, 0x672c, 0x8a9e, 0x65e5, 0 };
const uint16_t emojiBufUTF16[] = { 0xd83d, 0xdcd3, 0xd83d, 0xdcd5, 0xd83d, 0xdcd7, 0xd83d, 0xdcd8, 0xd83d, 0xdcd9, 0xd83d, 0xdcd4, 0xd83d, 0xdcd2, 0xd83d, 0xdc96, 0xd83d, 0xdc98, 0xd83d, 0xdc9d, 0xd83d, 0xdc9b, 0xd83d, 0xdc99, 0xd83d, 0xdc9c, 0 };
const char asciiDecodeUTF8[] = { 0x61 };
const char jpDecodeUTF8[] = { 0xe6, 0x9c, 0xac };
const char emojiDecodeUTF8[] = { 0xf0, 0x9f, 0x93, 0x9a };
const uint16_t asciiDecodeUTF16[] = { 0x61 };
const uint16_t jpDecodeUTF16[] = { 0x672C };
const uint16_t emojiDecodeUTF16[] = { 0xd83d, 0xdcda };
