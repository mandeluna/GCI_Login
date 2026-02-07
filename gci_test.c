#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include "gci.hf"

BoolType login_example(void);
void check_error(void);
void check_libraries(void);

int main() {
    GciErrSType err;

    printf("Attempting to log in to GemStone\n");

    BoolType success = login_example();
    
    if (success) {
        printf("Successfully logged in\n");
        GciLogout();
    } else {
        fprintf(stderr, "Login failed: %s\n", err.message);
        return false;
    }

    return true;
}

void check_libraries(void) {
  struct stat stat_buf;
  char library_path[256];
  char *gemstone_path = getenv("GEMSTONE");

  // Verify Environment Variable exists before calling GciInit
  if (gemstone_path == NULL) {
    fprintf(stderr, "ERROR: GEMSTONE environment variable is not set.\n");
    exit(1);
  }

  sprintf(library_path, "%s/%s", gemstone_path, "lib/libgcirpc-3.7.4.3-64.so");
  printf("Looking for libraries in %s\n", library_path);
    
  int result = stat(library_path, &stat_buf);
  if (result != 0) {
    char *error_message = strerror(result);
    fprintf(stderr, "Unable to stat library: %s\n", error_message);
    exit(result);
  }

  void *handle = dlopen(library_path, RTLD_NOW);
  if (result != 0) {
    char *error_message = strerror(result);
    fprintf(stderr, "Unable to load library: %s\n", error_message);
    exit(result);
  }
  printf("Successfully loaded %s, now calling dlclose()\n", library_path);
  dlclose(handle);
}

void check_error() {
     GciErrSType errInfo;
     memset(&errInfo, 0, sizeof(GciErrSType));
     if (GciErr(&errInfo)) {
      printf("error category %ld number %d, %s\n",
	     errInfo.category, errInfo.number, errInfo.message);
    }
}     

BoolType login_example(void) {
  const char *stoneName = "guava_ops";
  const char *hostUserId = "";
  const char *hostPassword = "";
  const char *gemService = "gemnetobject!tcp@localhost#netldi:8080#task!gemnetobject";
  const char *gsUserName = "SystemUser";
  const char *gsPassword = "swordfish";

  check_libraries();

  printf("Initializing Gci...\n");
  // GciInit required before first login
  if (!GciInit()) {
    printf("GciInit failed\n");
    return false;
  }

  printf("GciInit succeeded. Calling GciSetNet...\n");
  GciSetNet(stoneName, hostUserId, hostPassword, gemService);
  check_error();
  
  BoolType success = GciLogin(gsUserName, gsPassword);
  check_error();
  return success;
}
