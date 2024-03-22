#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "../headers/linked_list.h"

void createDirectory(const char *);
void copyFile(const char *, const char *);

EntryNode* findNodeByName(LinkedList *, char *);
int copyFileIfNewer(EntryNode *, EntryNode *);
int hardLinkCheck(LinkedList * , EntryNode * , char *, char* );
int hardLinkCheckSame(LinkedList * , LinkedList * , LinkedList * , EntryNode * , char *, char *);
void changeDirName(EntryNode *, char *);

void copyFilesToDirectory(LinkedList *, LinkedList *, LinkedList *, char *, LinkedList *, LinkedList *, LinkedList *);

