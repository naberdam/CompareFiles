// Nerya Aberdam 311416457 LATE-SUBMISSION

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MSG_CLOSE "After close\n"
#define MSG_OPEN "After open\n"
#define MSG_LSEEK "After lseek\n"
#define MSG_READ "After read\n"
#define MSG_ARGV "dont have enough arguments\n"
#define SIMILAR_EXIT 3
#define IDENTICAL_EXIT 1
#define DIFFERENT_EXIT 2
#define EXCEPTION_EXIT -1

void WriteMsgWithException(const char* msg)
{
	if (write(2, msg, strlen(msg)) < 0) {}
	exit(EXCEPTION_EXIT);
}
void WriteMsgWithOutException(const char* msg)
{
	if (write(2, msg, strlen(msg)) < 0) {}
}
void CloseOneFile(int fd1)
{
	if (close(fd1) < 0) {
		WriteMsgWithException(MSG_CLOSE);
	}
}

void CloseFiles(int fd1, int fd2)
{
	CloseOneFile(fd1);
	CloseOneFile(fd2);
}

int SizeOfText(char* arg1, int* fh)
{
	int size;
	(*fh) = open(arg1, O_RDONLY);
	//check if can open text
	if ((*fh) < 0) {
		WriteMsgWithException(MSG_OPEN);
	}
	size = lseek((*fh), 0, SEEK_END);
	//check if can do lseek
	if (size < 0) {
		WriteMsgWithOutException(MSG_LSEEK);
		CloseOneFile((*fh));
		exit(EXCEPTION_EXIT);
	}
	//close the file
	CloseOneFile((*fh));
	//return the size we got from lseek
	return size;
}
int SizeOfTextWithExistFile(char* arg1, int* fh, int fileExist)
{
	int size;
	(*fh) = open(arg1, O_RDONLY);
	//check if can open text
	if ((*fh) < 0) {
		WriteMsgWithException(MSG_OPEN);
		CloseOneFile(fileExist);
	}
	size = lseek((*fh), 0, SEEK_END);
	//check if can do lseek
	if (size < 0) {
		//write exception and close exist file
		WriteMsgWithOutException(MSG_LSEEK);
		CloseFiles((*fh), fileExist);
		exit(EXCEPTION_EXIT);
	}
	//close the file
	CloseOneFile((*fh));
	//return the size we got from lseek
	return size;
}

void my_read(char* arg1, char* contentOfFile, int sizeOfContent, int* file1)
{
	(*file1) = open(arg1, O_RDONLY);
	if ((*file1) == -1) {
		WriteMsgWithOutException(MSG_OPEN);
		exit(EXCEPTION_EXIT);
	}
	int ret;
	//read data from file
	ret = read((*file1), contentOfFile, sizeOfContent);
	//if cant read from file then write exception and close the file
	if (ret < 0) {
		WriteMsgWithOutException(MSG_READ);
		CloseOneFile((*file1));
		exit(EXCEPTION_EXIT);
	}
	CloseOneFile((*file1));
}

int CheckIfIdentical(const char* oneFile, const char* twoFile, int sizeOne, int sizeTwo)
{
	int i = 0;
	//if the sizes are not equal so they cant be identical
	if (sizeOne != sizeTwo) {
		return 0;
	}
	// the sizes are equal
	for (; i <= sizeOne; ++i) {
		//one letter is not the same
		if (oneFile[i] != twoFile[i]) {
			return 0;
		}
	}
	//the files are the same
	return 1;
}
int MinSize(int sizeOne, int sizeTwo)
{
	if (sizeOne <= sizeTwo) {
		return sizeOne;
	}
	return sizeTwo;
}
int MaxSize(int sizeOne, int sizeTwo)
{
	if (sizeOne > sizeTwo) {
		return sizeOne;
	}
	return sizeTwo;
}
int CountSameLettersFromBegin(int i, const char* fileBig, int sizeBig, const char* fileSmall, int sizeSmall)
{
	int counter = 0;
	int j = 0;
	for (; j < sizeSmall && (i + j < sizeBig) && j < sizeSmall; j++) {
		if (fileBig[i + j] == fileSmall[j]) {
			counter++;
		}
	}
	return counter;
}

void CheckIfSimilar(int counter, int sizeSmall)
{
	if (counter * 2 >= sizeSmall && counter != 0) {
		exit(SIMILAR_EXIT);
	}
}

int CountSameLettersFromEnd(int i, const char* fileBig, int sizeBig, const char* fileSmall, int sizeSmall)
{
	int counter = 0;
	int j = 0;
	for (; j < sizeBig && (i + j < sizeSmall) && j < sizeBig; j++) {
		if (fileBig[j] == fileSmall[i + j]) {
			counter++;
		}
	}
	return counter;
}
void CheckZeroSizes(int sizeOfOneFile, int sizeOfTwoFile) {
	if (sizeOfOneFile == 0 && sizeOfTwoFile == 0) {
		exit(IDENTICAL_EXIT);
	}
	if (sizeOfOneFile == 0 || sizeOfTwoFile == 0) {
		exit(SIMILAR_EXIT);
	}
}
int main(int argc, char* argv[])
{
	//too few arguments
	if (argc <= 2) {
		WriteMsgWithException(MSG_ARGV);
	}
	int minFile, maxFile;
	//get sizes of files
	int sizeOfOneFile = SizeOfText(argv[1], &minFile);
	int sizeOfTwoFile = SizeOfTextWithExistFile(argv[2], &maxFile, minFile);
	CheckZeroSizes(sizeOfOneFile, sizeOfTwoFile);
	int sizeMin = MinSize(sizeOfOneFile, sizeOfTwoFile);
	int sizeMax = MaxSize(sizeOfOneFile, sizeOfTwoFile);
	char minFileContent[sizeMin + 1];
	char maxFileContent[sizeMax + 1];
	minFileContent[sizeMin] = '\0';
	maxFileContent[sizeOfTwoFile] = '\0';
	//set the minFile and maxFile
	if (sizeMin == sizeOfOneFile) {
		my_read(argv[1], minFileContent, sizeMin, &minFile);
		my_read(argv[2], maxFileContent, sizeMax, &maxFile);
	} else {
		my_read(argv[2], minFileContent, sizeMin, &minFile);
		my_read(argv[1], maxFileContent, sizeMax, &maxFile);
	}
	//if they are identical so exit with IDENTICAL_EXIT
	if (CheckIfIdentical(minFileContent, maxFileContent, sizeOfOneFile, sizeOfTwoFile)) {
		exit(IDENTICAL_EXIT);
	} else {
		//can be similar or different
		int i = 0;
		//check the files from begin
		for (; i < sizeMax; ++i) {
			int count = CountSameLettersFromBegin(i, maxFileContent, sizeMax, minFileContent, sizeMin);
			CheckIfSimilar(count, sizeMin);
		}
		//check the files so we begin from start of maxFile and from the end of minFile
		for (; i < sizeMax; ++i) {
			int count = CountSameLettersFromEnd(i, maxFileContent, sizeMax, minFileContent, sizeMin);
			CheckIfSimilar(count, sizeMin);
		}
	}
	//different
	exit(DIFFERENT_EXIT);
}
