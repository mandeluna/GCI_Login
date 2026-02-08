#include <stdio.h>
#include <stdlib.h>

#include "gci.hf"
#include "gcirtl.hf"

BoolType login_example(void);
void check_error(void);
void load_library(void);

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

void load_library(void) {
  char *gemstone_path = getenv("GEMSTONE");
  char err_buf[1024];

  // Verify Environment Variable exists before calling GciInit
  if (gemstone_path == NULL) {
    fprintf(stderr, "ERROR: GEMSTONE environment variable is not set.\n");
    exit(1);
  }

  BoolType result = GciRtlLoad(true, "$GEMSTONE/lib", err_buf, sizeof(err_buf));
  if (result == false) {
    fprintf(stderr, "Library load failed, %s\n", err_buf);
  }

}

void check_error() {
     GciErrSType errInfo;
     if (GciErr(&errInfo)) {
      printf("error category " FMT_OID " number %d, %s\n",
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

  // You *must* call GciRtLoad or GciInit may simply crash without any diagnostic message
  load_library();

  // GciInit required before first login
  if (!GciInit()) {
    printf("GciInit failed\n");
    return false;
  }

  GciSetNet(stoneName, hostUserId, hostPassword, gemService);
  check_error();
  
  BoolType success = GciLogin(gsUserName, gsPassword);
  if (!success) {
    check_error();
  }
  return success;
}
