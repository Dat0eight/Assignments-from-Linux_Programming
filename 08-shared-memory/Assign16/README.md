This is README file for assignment 16.

Make usage:
- Make all: build executable file include: reader and writer
- Make clean: delete all executable file

Explain:
- write.c and read.c are using MAP_SHARED flag (certainly Shared File Mapping). So, reader executable file could read changed data from key board
- If those programming use MAP_PRIVATE flag (certainly Private File Mapping), reader executable file just reads first data from keyboard only, then it could not read changed data.