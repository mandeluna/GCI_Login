#include <stdio.h>
#include <stdlib.h>

#include "gci.hf"
#include "gcirtl.hf"

BoolType login_example(void);
void check_error(void);
void load_library(void);
void execute_and_fetch(const char *expression);
int asLocalInteger(OopType gsValue);
void reportObjectInfo(OopType oopValue);

int main() {
    printf("Attempting to log in to GemStone\n");

    BoolType success = login_example();
    
    if (success) {
        printf("Successfully logged in\n");
	// simple integer result
	// TODO handle other special types
	execute_and_fetch("4 + 3");
	// simple string result -- one level
	execute_and_fetch("STON toJsonString:(Dictionary new at: #a put: 42; yourself)");
	// extremely complex and large object
	execute_and_fetch("DEVDevelopmentSystemInstaller treasurySymbolDictionary at: #ObjectDictionary");
	// an array of strings
	execute_and_fetch("#('car' 'bat' 'dog')");
        GciLogout();
    } else {
        fprintf(stderr, "Login failed.\n");
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
      printf("error category " FMT_OID " number %d, %s, reason: %s\n",
	     errInfo.category, errInfo.number, errInfo.message, errInfo.reason);
    }
}     

BoolType login_example(void) {
  const char *stoneName = "guava_ops";
  const char *hostUserId = "";
  const char *hostPassword = "";
  const char *gemService = "!tcp@localhost#netldi:8080#task!gemnetobject";
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

  char outbuf[1024];
  char *gsEncryptedPw = GciEncrypt(gsPassword, outbuf, sizeof(outbuf));
  int login_flags = GCI_LOGIN_PW_ENCRYPTED | GCI_LOGIN_QUIET;
  BoolType success = GciLoginEx(gsUserName, gsEncryptedPw, login_flags, 0);

  if (!success) {
    check_error();
  }
  return success;
}

void execute_and_fetch(const char *expression) {

  printf("executing expression: %s\n", expression);
  
  OopType oString = GciNewString(expression);
  OopType result = GciExecute(oString, OOP_NIL);
  if (result == OOP_NIL) {
    printf("execution error\n");
    return;
  }

  int impl_type = GciFetchObjImpl(result);
  reportObjectInfo(result);

  switch (impl_type) {
  case 0: // pointer GC_FORMAT_OOP
    // object can have both named and unnamed instance variables
    printf("Result is an object with named or unnamed instance variables\n");
    break;
  case 1: // byte GC_FORMAT_BYTE
    // object can have only indexed variables, namedSize is always zero
    printf("Result is a byte indexed object\n");
    break;
  case 2: // NSC
      // object can have both named and unnamed instance variables
      printf("Result is an NSC with named or unnamed instance variables\n");
      // object can have both named and unnamed instance variables
    break;
  case 3: // special
    // object cannot have any instance variables, both named and unnamed always zero
    {
      int localInt = asLocalInteger(result);
      printf("result = %d\n", localInt);
    }
    break;
  default:
    fprintf(stderr, "unexpected implementation type: %d\n", impl_type);
    break;
  }

  return;
}

int asLocalInteger(OopType gsValue) {
  BoolType conversionError = false;
  int value = GciOopToI32_(gsValue, &conversionError);
  if (conversionError) {
    fprintf(stderr, "Error converting result to C int\n");
    exit(-1);
  }
  return value;
}

void reportObjectInfo(OopType oopValue) {
  GciObjInfoSType info = GciObjInfoSType();
  GciFetchObjInfoArgsSType args = GciFetchObjInfoArgsSType();
  ByteType buffer[1024];
    
  args.info = &info;
  args.bufSize = 1024;
  args.buffer = buffer;
  args.startIndex = 1;
    
  if (!GciFetchObjectInfo(oopValue, &args)) {
    fprintf(stderr, "GciFetchObjectInfo failed\n");
    check_error();
  }

  printf("Object info:\n\tobjectId=" FMT_OID "\n\tclassId=" FMT_OID "\n\tsize=%ld\n\tnamedSize=%d\n",
	 info.objId, info.objClass, info.objSize, info.namedSize);

  printf("\tNumber of elements returned: %ld\n", args.numReturned);
  printf("\tObject buffer: ");
  for (int i = 0; i < args.numReturned; i++) {
    printf("%c", (char)buffer[i]);
  }
  printf("\n");
}

