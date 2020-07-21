/*
// Nerya Aberdam 311416457 LATE-SUBMISSION

#include <stdio.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <wait.h>
#include <sys/time.h>
#include <time.h>

#define SIZE 1024
#define EXCEPTION_EXIT -1
#define MSG_OPEN "Error: could not open file\n"
#define MSG_CLOSE "Error: could not close file\n"
#define MSG_DIR "Error: could not close dir\n"
#define MSG_READ "Error: could not read file\n"
#define MSG_WRITE "Error: could not write\n"
#define MSG_FORK "Error: fork failed\n"
#define MSG_EXE "Error: exec failed\n"
#define MSG_DUP2 "Error: dup2 failed\n"
#define MSG_LSEEK "Error: lseek failed\n"
#define MSG_RUN "Error: running failed\n"
#define MSG_TIMEOUT ",TIMEOUT,20\n"
#define EXCELLENT ",EXCELLENT,100\n"
#define WRONG ",WRONG,50\n"
#define SIMILAR ",SIMILAR,75\n"
#define MSG_DELETE "Error: delete failed\n"
#define MSG_COMPILE "Error: compilation failed\n"
#define MSG_VALID_DIRECTORY "Error: Not a valid directory\n"
#define MSG_COMPILATION_ERROR ",COMPILATION_ERROR,10\n"
#define OUT_FILE_NAME "b.out"
#define RESULT_CSV_FILE_NAME "results.csv"
#define OUTPUT_OF_RUNNING_FILE_NAME "outputOfRunning.txt"
#define SUCCESS 1
#define FAILURE 0
#define COMPILATION_ERROR 2

void WriteMsgWithException(const char* msg)
{
	if (write(2, msg, strlen(msg)) < 0) {}
	exit(EXCEPTION_EXIT);
}

int WriteMsgWithOutException(const char* msg)
{
	if (write(2, msg, strlen(msg)) < 0) {
		return FAILURE;
	}
	return SUCCESS;
}

void CheckOpenFile(int file, const char* msg)
{
	if (file < 0) {
		WriteMsgWithException(msg);
	}
}

void CheckCloseFile(int file)
{
	if (close(file) < 0) {
		WriteMsgWithException(MSG_CLOSE);
	}
}
void CheckCloseDir(DIR* pDir)
{
	if (pDir != NULL) {
		if (closedir(pDir) < 0) {
			WriteMsgWithException(MSG_DIR);
		}
	}
}

int WriteWhenZeroCFiles(DIR* dirPointer)
{
	write(2, MSG_WRITE, strlen(MSG_WRITE) + 1);
	CheckCloseDir(dirPointer);
	return FAILURE;
}

void CheckCloseAllFilesAndFolder(DIR* pDir, int fileInput, int fileOutput, int resultCsvFile)
{
	CheckCloseDir(pDir);
	CheckCloseFile(fileInput);
	CheckCloseFile(fileOutput);
	CheckCloseFile(resultCsvFile);
}

void CreatePathWithD_name(char* path, const char* d_name, const char* all_dirs_path)
{
	strcpy(path, all_dirs_path);
	strcat(path, "/");
	strcat(path, d_name);
}

void SetContentByLine(char* content, int* countChars, const int* sizeRead, const char* buffer,
	int* counterFiles, int* countCharInLine)
{
	while ((*countChars) != (*sizeRead)) {
		if (buffer[(*countChars)] == '\n') {
			(*countChars)++;
			content[(*countCharInLine)] = '\0';
			(*counterFiles)++;
			(*countCharInLine) = 0;
			break;
		}
		content[(*countCharInLine)] = buffer[(*countChars)];
		(*countChars)++;
		(*countCharInLine)++;
	}
}

void SetContentFromConfig(const char* configFileName, char* all_dirs_path, char* input_file,
	char* correct_output_file)
{
	int configFile = open(configFileName, O_RDONLY);
	CheckOpenFile(configFile, MSG_OPEN);
	char buffer[SIZE];
	int sizeRead;
	int countChars = 0;
	int counterFiles = 1;
	int countCharInLine = 0;
	sizeRead = read(configFile, buffer, SIZE);
	CheckOpenFile(sizeRead, MSG_READ);
	SetContentByLine(all_dirs_path, &countChars, &sizeRead, buffer, &counterFiles, &countCharInLine);
	if (counterFiles == 2) {
		SetContentByLine(input_file, &countChars, &sizeRead, buffer, &counterFiles, &countCharInLine);
	}
	if (counterFiles == 3) {
		SetContentByLine(correct_output_file, &countChars, &sizeRead, buffer, &counterFiles, &countCharInLine);
	}
	close(configFile);
}
int CheckStrcmpDName(const char* d_name)
{
	if (strcmp(d_name, "..") == 0) {
		return FAILURE;
	}
	if (strcmp(d_name, ".") == 0) {
		return FAILURE;
	}
	return SUCCESS;
}

int WriteAfterError(int partOne, const char* partTwo, const char* msg)
{
	if (write(partOne, partTwo, strlen(partTwo)) < 0) {
		if (write(2, msg, strlen(msg)) < 0) {
		}
		return FAILURE;
	}
	return SUCCESS;
}

int IsThereCFile(const char* d_name)
{
	if (d_name[strlen(d_name) - 1] != 'c') {
		return FAILURE;
	}
	if (d_name[strlen(d_name) - 2] != '.') {
		return FAILURE;
	}
	return SUCCESS;
}

int CompileCFile(char* path, char* fileToCompile)
{
	int pid = fork();
	//cant open new fork
	if (pid < 0) {
		WriteMsgWithOutException(MSG_FORK);
		return FAILURE;
	}
	//error with pid
	int status;
	//child
	if (pid == 0) {
		unlink(OUT_FILE_NAME);
		char pathOfFile[SIZE];
		CreatePathWithD_name(pathOfFile, fileToCompile, path);
		char* argv[] = { "gcc", "-o", OUT_FILE_NAME, pathOfFile, NULL };
		if (execvp("gcc", argv) <= -1) {
			//if exec fails we run these lines
			if (!WriteMsgWithOutException(MSG_COMPILE)) {
				return FAILURE;
			}
		}
	}
	if (pid > 0) {
		//parent
		int wait_stat;
		wait_stat = waitpid(pid, &status, 0);
	}
}

int RunCFile(int fileInput, int resultCsvFile, char* d_name)
{
	off_t off = 0;
	//cant do lseek
	if (lseek(fileInput, off, SEEK_SET) < 0) {
		WriteMsgWithOutException(MSG_LSEEK);
		return FAILURE;
	}

	int compiledFile;
	//cant open OUT_FILE_NAME
	if ((compiledFile = open(OUT_FILE_NAME, O_WRONLY, 0644)) < 0) {
		if (!WriteAfterError(resultCsvFile, d_name, MSG_WRITE)
			|| !WriteAfterError(resultCsvFile, MSG_COMPILATION_ERROR, MSG_WRITE)) {
			return FAILURE;
		}
		return COMPILATION_ERROR;
	}
	CheckCloseFile(compiledFile);

	pid_t pid = fork();
	//cant open new fork
	if (pid == -1) {
		WriteMsgWithOutException(MSG_FORK);
		return FAILURE;
	}
	if (pid > 1) { // this is the father
		int status;
		struct timeval tv;
		int get_time_stat = gettimeofday(&tv, NULL);
		//if not succeed to get time of day so write exception
		if (get_time_stat == -1) {
			WriteMsgWithOutException("Error: get time of day function failed");
			return FAILURE;
		}
		int start_time = tv.tv_sec;
		waitpid(pid, &status, 0);
		gettimeofday(&tv, NULL);
		int end_time = tv.tv_sec;
		//if it is bigger then 3 so write exception
		if (end_time - start_time > 3) {
			if (!WriteAfterError(resultCsvFile, d_name, MSG_WRITE)
				|| !WriteAfterError(resultCsvFile, MSG_TIMEOUT, MSG_WRITE)) {
				return FAILURE;
			}
			return COMPILATION_ERROR;
		}
		return SUCCESS;
	}
	if (pid == 0) { // this is the son
		// change to output to a file
		int output; */
/* new file descriptor *//*

		if ((output = open(OUTPUT_OF_RUNNING_FILE_NAME, O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0) {
			WriteMsgWithOutException(MSG_OPEN);
			return FAILURE;
		}
		// output is open

		if (dup2(output, 1) < 0) {
			WriteMsgWithOutException(MSG_DUP2);
			return FAILURE;
		}
		if (dup2(fileInput, 0) < 0) {
			WriteMsgWithOutException(MSG_DUP2);
			return FAILURE;
		}
		char outputName[SIZE] = "";
		strcpy(outputName, "./");
		strcat(outputName, OUT_FILE_NAME);
		char* argsForGcc[2] = { outputName, NULL };
		if (execvp(outputName, argsForGcc) == -1) {
			if (write(2, MSG_RUN, strlen(MSG_RUN)) < 0) {
				unlink(OUTPUT_OF_RUNNING_FILE_NAME);
				CheckCloseFile(output);
				return FAILURE;
			}
			CheckCloseFile(output);
			return FAILURE;
		}
		CheckCloseFile(output);

	}
}

int WriteWEXITSTATUS(int resultCsvFile, const char* msg)
{
	if (!WriteAfterError(resultCsvFile, msg, MSG_WRITE)) {
		return FAILURE;
	}
	return SUCCESS;
}

int CompareTheOutputAndWrite(char* correct_output_file, int resultCsvFile, const char* d_name)
{
	pid_t pid = fork();
	int status;
	if (pid < 0) {
		if (unlink(OUTPUT_OF_RUNNING_FILE_NAME) < 0) {
			if (!WriteMsgWithOutException(MSG_DELETE)) {
				return FAILURE;
			}
		}
		WriteMsgWithOutException(MSG_FORK);
		return FAILURE;
	}

	//error with pid
	//child
	if (pid == 0) {
		char* argsForGcc[4] = { "./comp.out", OUTPUT_OF_RUNNING_FILE_NAME, correct_output_file, NULL };
		if (execvp("./comp.out", argsForGcc) == -1) {
			if (unlink(OUTPUT_OF_RUNNING_FILE_NAME) < 0) {
				if (!WriteMsgWithOutException(MSG_DELETE)) {
					return FAILURE;
				}
			}
			if (!WriteMsgWithOutException(MSG_RUN)) {
				return FAILURE;
			}
		}
	} else {
		waitpid(pid, &status, 0);
		if (!WriteAfterError(resultCsvFile, d_name, MSG_WRITE)) {
			return FAILURE;
		}
		int wExitStatus = WEXITSTATUS(status);
		if (wExitStatus == 1) {
			return WriteWEXITSTATUS(resultCsvFile, EXCELLENT);
		}
		if (wExitStatus == 2) {
			return WriteWEXITSTATUS(resultCsvFile, WRONG);
		}
		if (wExitStatus == 3) {
			return WriteWEXITSTATUS(resultCsvFile, SIMILAR);
		}
	}
}

int CheckIfCanIterateDir(DIR* pDir, char* all_dirs_path, int fileInput, char* correct_output_file, int resultCsvFile)
{
	struct dirent* pDirentOut;
	struct dirent* pDirentInside;

	// iterate the sub folders
	while ((pDirentOut = readdir(pDir)) != NULL) {
		if (!CheckStrcmpDName(pDirentOut->d_name)) {
			continue;
		}
		// open the sub folder:
		DIR* dirPointer;
		char path[SIZE];
		CreatePathWithD_name(path, pDirentOut->d_name, all_dirs_path);
		dirPointer = opendir(path);
		if (dirPointer != NULL) {
			// this is a sub folder:
			int numberOfCFiles = 0;
			while ((pDirentInside = readdir(dirPointer)) != NULL) {
				if (!IsThereCFile(pDirentInside->d_name)) {
					continue;
				}
				numberOfCFiles++;
				int comp = CompileCFile(path, pDirentInside->d_name);
				// compilation failed
				if (comp == FAILURE) {
					CheckCloseDir(dirPointer);
					return FAILURE;
				}
				int run = RunCFile(fileInput, resultCsvFile, pDirentOut->d_name);
				// there is now a OUT_FILE_NAME file in the folder
				// running failed
				if (run == FAILURE) {
					CheckCloseDir(dirPointer);
					return FAILURE;
				}
				// if there was compilation error
				if (run == COMPILATION_ERROR) {
					break;
				}
				// run = SUCCESS
				// there is now a OUTPUT_OF_RUNNING_FILE_NAME file in the folder
				if (!CompareTheOutputAndWrite(correct_output_file, resultCsvFile, pDirentOut->d_name)) {
					//something went wrong so close dir and returns FAILURE
					CheckCloseDir(dirPointer);
					return FAILURE;
				}
			}
			// if there is not even one c file
			if (!numberOfCFiles) {
				if (write(resultCsvFile, pDirentOut->d_name, strlen(pDirentOut->d_name)) < 0) {
					return WriteWhenZeroCFiles(dirPointer);
				}
				if (write(resultCsvFile, ",NO_C_FILE,0\n", sizeof(",NO_C_FILE,0\n") - 1) == -1) {
					return WriteWhenZeroCFiles(dirPointer);
				}
			}
		}
	}
	//something went wrong
	return SUCCESS;
}

void UnlinkAndCloseAllFilesAndFolder(DIR* pDir, int fileInput, int fileOutput, int resultCsvFile)
{
	//delete and close all open dir and files.
	unlink(OUT_FILE_NAME);
	unlink(OUTPUT_OF_RUNNING_FILE_NAME);
	CheckCloseAllFilesAndFolder(pDir, fileInput, fileOutput, resultCsvFile);
}

int RunProgram(DIR* pDir,
	char* all_dirs_path,
	int fileInput,
	char* correct_output_file,
	int resultCsvFile,
	int fileOutput)
{
	//start iterate the content we have, and then if something went wrong we will get a number which is not 0
	if (CheckIfCanIterateDir(pDir, all_dirs_path, fileInput, correct_output_file, resultCsvFile) == 0) {
		//close all open dir and files.
		UnlinkAndCloseAllFilesAndFolder(pDir, fileInput, fileOutput, resultCsvFile);
		return EXCEPTION_EXIT;
	}
	//everything was good, so just close all open dir and files.
	UnlinkAndCloseAllFilesAndFolder(pDir, fileInput, fileOutput, resultCsvFile);
	return 0;
}

int main(int argc, char** argv)
{
	char all_dirs_path[SIZE];
	char input_file[SIZE];
	char correct_output_file[SIZE];
	//set the content that is inside argv[1] and put it in those array char
	SetContentFromConfig(argv[1], all_dirs_path, input_file, correct_output_file);

	//create the file RESULT_CSV_FILE_NAME
	char pathOfResult[SIZE] = "";
	strcpy(pathOfResult, "./");
	strcat(pathOfResult, RESULT_CSV_FILE_NAME);
	int resultCsvFile = open(pathOfResult, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	//open the file RESULT_CSV_FILE_NAME
	CheckOpenFile(resultCsvFile, "Error: could not create result.csv file");
	int fileInput = open(input_file, O_RDONLY);
	//check if open fileInput is good
	if (fileInput == -1) {
		//close the open files
		CheckCloseFile(resultCsvFile);
		WriteMsgWithException("Error: could not open the input file\n");
	}
	int fileOutput = open(correct_output_file, O_RDONLY);
	//check if open fileOutput is good
	if (fileOutput == -1) {
		//close the open files
		CheckCloseFile(resultCsvFile);
		CheckCloseFile(fileInput);
		WriteMsgWithException("Error: could not open the output file\n");
	}

	DIR* pDir = opendir(all_dirs_path);
	//if it is null then throws exception
	if (pDir == NULL) {
		WriteMsgWithException(MSG_VALID_DIRECTORY);
	}
	//there is a pDir so we can run our program. this function returns 0 if everything is good or
	//-1 if something went wrong
	return RunProgram(pDir, all_dirs_path, fileInput, correct_output_file, resultCsvFile, fileOutput);
}




*/
