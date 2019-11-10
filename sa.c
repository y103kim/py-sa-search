#include "gsaca.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define STR(x) #x
#define ASSERT(x, msg) \
  do { \
    if (!(x)) { \
      printf("ERROR: (%s), function %s, file %s, line %d.\n", STR(x), __PRETTY_FUNCTION__, __FILE__, __LINE__); \
      printf("        %s\n", (msg)); \
      abort(); \
    } \
  } while(0)

int getFileSize(FILE* fp) {
  fseek(fp, 0, SEEK_END);
  int seekSize = ftell(fp);
  rewind(fp);
  return seekSize;
}

FILE* openOutputFile(const char* path, const char* mode) {
  int len = strlen(path);
  char* buf = (char*)malloc(sizeof(char) * (len+4));
  ASSERT(buf, "Cannot allocate memory");

  sprintf(buf, "%s.sa", path);
  FILE* fp = fopen(buf, mode);
  ASSERT(fp, "Cannot open output file");

  free(buf);
  return fp;
}

char* readInputFile(const char* path, int* size) {
  FILE* fp = fopen(path, "r");
  ASSERT(fp, "No such file or directory");

  *size = getFileSize(fp);
  char* text = (char*)malloc(sizeof(char) * (*size+1));
  ASSERT(text, "Cannot allocate memory");

  int readSize = fread(text, 1, *size, fp);
  ASSERT(readSize == *size, "Error while reading file");
  text[*size] = 0;
  fclose(fp);
  return text;
}

void genSA(const char* path) {
  int size;
  char* text = readInputFile(path, &size);
  int* sa = (int*)malloc(sizeof(int) * (size+1));
  ASSERT(sa, "Cannot allocate memory");

  int ret = gsaca((const unsigned char*) text, sa, size+1);
  ASSERT(ret == 0, "Error while generating SA");
  FILE* fp = openOutputFile(path, "w");
  fwrite(sa, sizeof(int), size+1, fp);

  free(text);
  free(sa);
  fclose(fp);
}

int* getSA(const char* path, int* size) {
  FILE* fp = openOutputFile(path, "r");
  int fileSize = getFileSize(fp);
  *size = fileSize / 4;

  int* sa = (int*)malloc(fileSize);
  ASSERT(sa, "Cannot allocate memory");

  int readSize = fread(sa, 1, fileSize, fp);
  ASSERT(readSize == fileSize, "Error while reading file");

  fclose(fp);
  return sa;
}

void saveResult(int* result, int resultCnt, int m, int* sa, const char* match, char* input, int size, int matchSize) {
  int cnt = 0;
  int um = m, lm = m-1;;

  while(cnt < resultCnt) {
    result[cnt++] = sa[um];
    int s = sa[++um], k = 0;
    while(k < matchSize && s + k < size) {
      if(match[k] != input[s + k]) break;
      k++;
    }
    if(k != matchSize) break;
  }

  while(cnt < resultCnt) {
    result[cnt++] = sa[lm];
    int s = sa[--lm], k = 0;
    while(k < matchSize && s + k < size) {
      if(match[k] != input[s + k]) break;
      k++;
    }
    if(k != matchSize) break;
  }
}

void search(const char* path, const char* match, int* result, int resultCnt) {
  int saSize, size, matchSize = strlen(match);
  int* sa = getSA(path, &saSize);
  char* input = readInputFile(path, &size);
  ASSERT(saSize == size+1, "Size umatched between SA and input");

  for (int i = 0; i < resultCnt; i++)
    result[i] = -1;

  int l = 0, r = size;
  while(l < r - 1) {
    int m = (l + r) / 2;
    int s = sa[m];

    int k = 0;
    while(k < matchSize && s + k < size) {
      if(match[k] != input[s + k]) break;
      k++;
    }

    if(k == matchSize) {
      saveResult(result, resultCnt, m, sa, match, input, size, matchSize);
      return;
    }

    if(k == matchSize || s + k == size) k--;
    if(match[k] < input[s + k]) {
      r = m;
    } else {
      l = m;
    }
  }

  free(sa);
  free(input);
}

/*
// Test Code of the sa.c
int main() {
  const char* path = "./kernel.log";
  genSA(path);
  int res[30];
  search("kernel.log", "microcode", res, 30);
  for(int i = 0; i < 30; i++) printf("%d ", res[i]);
  return 0;
}
*/

