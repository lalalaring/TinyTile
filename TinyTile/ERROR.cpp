#include "ERROR.h"
#include <stdio.h>
#include <stdarg.h>
#include <process.h>

void ReportError(int line, char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	FILE* fp = fopen("Error.txt", "a+");
	fprintf(fp, "Error at line (%d): ", line);
	fprintf(fp, fmt, ap);
	fclose(fp);
	va_end(ap);

	exit(0);
}
