#include "../headers/create_new.h"
#include "../headers/linked_list.h"

// Create a new entry in the linked list
void createDirectory(const char *path) {
    if (mkdir(path, 0777) == -1) {
        perror("Error creating directory");
        exit(EXIT_FAILURE);
    }
}

// Copy a file from source to destination
void copyFile(const char *source, const char *destination) {
    int sourceFile = open(source, O_RDONLY); // Open source file for reading
    if (sourceFile == -1) {
        perror("Error opening source file");
        return;
    }

    int destinationFile = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0666); // Open destination file for writing
    if (destinationFile == -1) {
        perror("Error opening destination file");
        close(sourceFile);
        return;
    }

    char buffer[BUFSIZ];
    ssize_t bytesRead, bytesWritten;
    // Read from source file and write to destination file
    while ((bytesRead = read(sourceFile, buffer, BUFSIZ)) > 0) { 
        bytesWritten = write(destinationFile, buffer, bytesRead);
        if (bytesWritten != bytesRead) {
            perror("Error writing to destination file");
            close(sourceFile);
            close(destinationFile);
            return;
        }
    }

    if (bytesRead == -1) {
        perror("Error reading from source file");
    }

    close(sourceFile);
    close(destinationFile);
}

// Find an entry in the linked list by name
EntryNode* findNodeByName(LinkedList *list, char *name) {
    EntryNode *current = list->head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current; 
        }
        current = current->next;
    }
    return NULL; 
}

//Compares modification times of two files
int copyFileIfNewer(EntryNode *src, EntryNode *srch2) {
    time_t srcModTime = src -> fileStat.st_mtime;
    time_t srch2ModTime = srch2 -> fileStat.st_mtime;
    if (srcModTime > srch2ModTime ) {
        return 1;
    }
    else{
        return 2;
    }
}

// Find an entry in the linked list by inode
EntryNode* findNodeByInode(LinkedList *list, EntryNode *file) {
    EntryNode *current = list->head;
    while (current != NULL && current->name != file->name) {
        if (current->fileStat.st_ino == file->fileStat.st_ino) {
            return current; 
        }
        current = current->next;
    }
    return NULL; 
}

int hardLinkCheck(LinkedList* List, EntryNode* file, char *destinationPath, char* destinationDir) {
    EntryNode *current = List->head;
    while (current != NULL) {
        if (current->fileStat.st_ino == file->fileStat.st_ino) { //Check if there are 2 files with the same inode
            // Construct the final destination path
            char finalDestinationPath[PATH_MAX];
            strcpy(finalDestinationPath, destinationDir);
            strcat(finalDestinationPath, "/");
            strcat(finalDestinationPath, file->relativePath);

            // Construct the final copying path
            char finalCopyingPath[PATH_MAX];
            strcpy(finalCopyingPath, destinationDir);
            strcat(finalCopyingPath, "/");
            strcat(finalCopyingPath, current->relativePath);
            
            // Check if destination path already exists
            struct stat destStat;
            if (stat(destinationPath, &destStat) == 0) {
                // Destination already exists, it might be a hard link, skip copying
                printf("Destination path already exists, skipping...\n");
                return 1;
            }

            // Check if source file is a regular file (not a symbolic link)
            if (!S_ISREG(file->fileStat.st_mode)) {
                printf("Source file is not a regular file, skipping hard link creation.\n");
                return 0;
            }

            // Attempt to create hard link
            if (link(finalCopyingPath,finalDestinationPath) == -1) {
                perror("Error creating hard link");
            } else {
                //after linking, we add the new entry to the list
                insertEntry(List, file->name, finalCopyingPath, file->fileStat, destinationDir);
            }
            return 1;
        }
        current = current->next;
    }
    
    return 0;
}


    
int hardLinkCheckSame(LinkedList* List,LinkedList* originalDic, LinkedList* otherDic, EntryNode* file, char *destinationPath, char* destinationDir) {
    EntryNode* newerFile = findNodeByName(otherDic, file->name);
    //If we find a file with the same name in the other directory, we check which one is newer
    if (newerFile != NULL && copyFileIfNewer(newerFile, file) == 1) {
        //If the file in the other directory is newer, we hardlink it
        return (hardLinkCheck(List, newerFile, destinationPath, destinationDir));
    } else {
        EntryNode* sameInode = findNodeByInode(originalDic, file);
        if (sameInode != NULL) {
            EntryNode* current = List->head;
            while(current!=NULL){
                if (strcmp(current->name, sameInode->name) == 0) {
                    // Construct the final destination path
                    char finalDestinationPath[PATH_MAX];
                    strcpy(finalDestinationPath, destinationDir);
                    strcat(finalDestinationPath, "/");
                    strcat(finalDestinationPath, file->relativePath);

                    // Construct the final copying path
                    char finalCopyingPath[PATH_MAX];
                    strcpy(finalCopyingPath, destinationDir);
                    strcat(finalCopyingPath, "/");
                    strcat(finalCopyingPath, current->relativePath);
                    
                    // Check if destination path already exists
                    struct stat destStat;
                    if (stat(destinationPath, &destStat) == 0) {
                        // Destination already exists, it might be a hard link, skip copying
                        printf("Destination path already exists, skipping...\n");
                        return 1;
                    }

                    // Check if source file is a regular file (not a symbolic link)
                    if (!S_ISREG(file->fileStat.st_mode)) {
                        printf("Source file is not a regular file, skipping hard link creation.\n");
                        return 0;
                    }

                    // Attempt to create hard link
                    if (link(finalCopyingPath,finalDestinationPath) == -1) {
                        perror("Error creating hard link");
                    } else {
                        //after linking, we add the new entry to the list
                        insertEntry(List, file->name, finalCopyingPath, file->fileStat, destinationDir);
                    }
                    return 1;
                }
                current = current->next;
            }
        }
        return (hardLinkCheck(List, file, destinationPath, destinationDir));
    }
    return 0;
}


void copyFilesToDirectory(LinkedList* list, LinkedList* list2, LinkedList* list3, char *destinationDir, LinkedList* list4, LinkedList* entriesA, LinkedList* entriesB) {
    // Create the destination directory if it doesn't exist
    struct stat dirStat;
    if (stat(destinationDir, &dirStat) == -1) {
        createDirectory(destinationDir);
    }

    EntryNode *current = list->head;
    while (current != NULL) {
        char destinationPath[PATH_MAX];
        strcpy(destinationPath, destinationDir);
        strcat(destinationPath, "/");
        strcat(destinationPath, current->relativePath);
        printf("Copying %s to %s\n", current->relativePath, destinationPath);

        if (current->fileStat.st_nlink > 1){ //If the file has more than one link, it might be a hard link
            if (hardLinkCheck(list4, current, destinationPath, destinationDir) == 1){
                current = current->next;    //If it was a hard link, we skip the file
                continue;
            }
        }
        if (S_ISLNK(current->fileStat.st_mode)) {
            char linkTarget[PATH_MAX];
            ssize_t len = readlink(current->path, linkTarget, PATH_MAX - 1);
            if (len != -1) {
                linkTarget[len] = '\0';
                // Create a copy of the symbolic link in the destination directory
                if (symlink(linkTarget, destinationPath) == -1) {
                    perror("Error creating symbolic link");
                }
                insertEntry(list4, current->name, destinationPath, current->fileStat, destinationDir);
            }
        } else if (S_ISDIR(current->fileStat.st_mode)) {
            createDirectory(destinationPath);   //If it's a directory, we create a new directory
            insertEntry(list4, current->name, current->path, current->fileStat, destinationDir);  
        } else {
            copyFile(current->path, destinationPath); //If it's a file, we copy it to the destination directory
            insertEntry(list4, current->name, current->path, current->fileStat, destinationDir);      
        }
        current = current->next;
    }

    EntryNode *current2 = list2->head;
    EntryNode *current3 = list3->head;
    while (current2 != NULL) {
        char destinationPath[PATH_MAX];
        strcpy(destinationPath, destinationDir);
        strcat(destinationPath, "/");
        strcat(destinationPath, current2->relativePath);
        

        if (current2->fileStat.st_nlink > 1){
            //Here we check files we same name that might be hard links
            //We pass entriesA because we copy all our same files from directory B
            //So then we can check entriesA in the case a hardlink exists which is linked to a file in directory A that we copied from directory B
            printf("Copying %s to %s\n", current2->relativePath, destinationPath);
            if (hardLinkCheckSame(list4,entriesA, list3, current2, destinationPath, destinationDir) == 1){ 
                current2 = current2->next;
                continue;
            }            
        }

         if (S_ISLNK(current2->fileStat.st_mode)) {
            char linkTarget[PATH_MAX];
            ssize_t len = readlink(current2->path, linkTarget, PATH_MAX - 1);
            if (len != -1) {
                linkTarget[len] = '\0';
                // Create a copy of the symbolic link in the destination directory
                if (symlink(linkTarget, destinationPath) == -1) {
                    perror("Error creating symbolic link");
                }
                insertEntry(list4, current->name, destinationPath, current->fileStat, destinationDir);
            }
        }

        if (S_ISDIR(current2->fileStat.st_mode)) {
            printf("Copying %s to %s\n", current2->relativePath, destinationPath);
            createDirectory(destinationPath);
            insertEntry(list4, current2->name, current2->path, current2->fileStat, destinationDir);          
        } else {
            EntryNode *sameNamed = findNodeByName(list3, current2->name); //Check if the file has the same name in the other directory
            if (sameNamed != NULL) {
                //If there is same name file in the other directory, we check which one is newer and copy it
                printf("Copying %s to %s\n", current2->relativePath, destinationPath);
                if (copyFileIfNewer(current2, sameNamed) == 1) {            
                    copyFile(current2->path, destinationPath);
                    insertEntry(list4, current2->name, current2->path, current2->fileStat, destinationDir);    
                }
                else {
                    copyFile(sameNamed->path, destinationPath);
                    insertEntry(list4, sameNamed->name, sameNamed->path, sameNamed->fileStat, destinationDir);               
                }
            } else {
                copyFile(current2->path, destinationPath);
                insertEntry(list4, current2->name, current2->path, current2->fileStat, destinationDir);
            }
        }
        current2 = current2->next;
    }

    // Copy files from list3, the same name files dont need to be checked again since we already checked them in list2
    while (current3 != NULL) {
        char destinationPath[PATH_MAX];
        strcpy(destinationPath, destinationDir);
        strcat(destinationPath, "/");
        strcat(destinationPath, current3->relativePath);

         if (S_ISLNK(current3->fileStat.st_mode)) {
            char linkTarget[PATH_MAX];
            ssize_t len = readlink(current3->path, linkTarget, PATH_MAX - 1);
            if (len != -1) {
                linkTarget[len] = '\0';
                // Create a copy of the symbolic link in the destination directory
                if (symlink(linkTarget, destinationPath) == -1) {
                    perror("Error creating symbolic link");
                }
                insertEntry(list4, current->name, destinationPath, current->fileStat, destinationDir);
            }
        }
        
        if (S_ISDIR(current3->fileStat.st_mode)) {
            printf("Copying %s to %s\n", current3->relativePath, destinationPath);
            createDirectory(destinationPath);
            insertEntry(list4, current3->name, current3->path, current3->fileStat, destinationDir);
        } else {
            if (!findNodeByName(list4, current3->name)) {
                printf("Copying %s to %s\n", current3->relativePath, destinationPath);
                copyFile(current3->path, destinationPath);
                insertEntry(list4, current3->name, current3->path, current3->fileStat, destinationDir);
            } 
        }
        current3 = current3->next;
    }
}