#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
	FILE* file = tmpfile();
	int fd = fork();
	if (fd == -1)
		return -1;
	if (fd > 0) {
		waitpid(fd, NULL, 0);
		char buffer[1025];
		fseek(file, 0, SEEK_SET);
		fread(buffer, 8, 1, file);
		printf("%s\n", buffer);
		fclose(file);
	}
	else if (fd == 0) {
		const char * buffer = "Coucou";
		fwrite(buffer, 8, 1, file);
		fclose(file);
	}
}