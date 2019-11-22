/* SLAC common platform  - yaml loader epics driver */

#include <fstream>
#include <sstream>

#include <cpsw_api_user.h>
#include <yamlLoader.h>

#include <cantProceed.h>
#include <epicsString.h>
#include <ellLib.h>
#include <iocsh.h>
#include <drvSup.h>
#include <epicsExport.h>

#include <yaml-cpp/yaml.h>

class IYamlSetIP : public IYamlFixup {
public:
        IYamlSetIP( const char* ip_addr ) : ip_addr_(ip_addr) {}
        virtual void operator()(YAML::Node &node, YAML::Node &dummy)
        {
          node["ipAddr"] = ip_addr_;
        }

        virtual ~IYamlSetIP() {}
private:
        std::string ip_addr_;
};


extern "C" {
// need to think more about the ugly global one

static ELLLIST *pRootList = NULL;

typedef struct {
    ELLNODE      node; 
    Path         root;
    char         *description;
    char         *name;
} rootList_t;

static void init_rootList(void)
{

    if(!pRootList) {
        pRootList = (ELLLIST*) mallocMustSucceed(sizeof(ELLLIST), "linked list for root list in yamlLoader driver");
        ellInit(pRootList);
    }
}

int cpswIsThereRoot(void)
{
    init_rootList();
    return ellCount(pRootList);
}

Path cpswGetRoot(void)
{
    init_rootList();
    rootList_t *p = NULL;
    if(ellCount(pRootList)) p = (rootList_t *) ellLast(pRootList);
   
    
    return p->root;
}

char * cpswGetRootName(void)
{
    init_rootList();
    rootList_t *p = NULL;

    if(ellCount(pRootList)) {
        p = (rootList_t *) ellLast(pRootList);
        return p->name;
    } 

    return (char *) NULL;
    
}

Path cpswGetNamedRoot(const char *name)
{
    int rootList();

    rootList_t *p = (rootList_t *) ellFirst(pRootList);
    while(p) {
        if(!strcmp(p->name, name)) break;
        p = (rootList_t *) ellNext(&p->node);
    }

    return p->root;
}

void cpswPutRoot(Path root)
{
    init_rootList();

    char name[80];
    sprintf(name, "root_%d", ellCount(pRootList));
    rootList_t *p = (rootList_t*) mallocMustSucceed(sizeof(rootList_t), "Root List in yamlLoader driver");
    p->name = epicsStrDup(name);
    p->root = root;

    ellAdd(pRootList, &p->node);
    
}


void cpswPutNamedRoot(Path root, const char *name)
{
    init_rootList();

    rootList_t *p = (rootList_t *) mallocMustSucceed(sizeof(rootList_t), "Root List in yamlLoader driver");
    p->name = epicsStrDup(name);
    p->root = root;

    ellAdd(pRootList, &p->node);

}





int cpswLoadYamlFile(const char *yaml_file, const char *root, const char *yaml_dir, const char *ip_addr, const char *named_root)
{
    Path theRoot;

    if(!yaml_file) {
        fprintf(stderr, "Missing YAML file name\n");
        return -1;
    }
    
    try {
        if(!ip_addr) theRoot = IPath::loadYamlFile(yaml_file, root, yaml_dir);
        else {
              IYamlSetIP setIP(ip_addr);
              theRoot = IPath::loadYamlFile(yaml_file, root, yaml_dir, &setIP);
        }

        if(!named_root || !strlen(named_root)) cpswPutRoot(theRoot);
        else                                   cpswPutNamedRoot(theRoot, named_root);
        
    } catch (CPSWError &e) {
        fprintf(stderr, "Unable to load YAML file: %s\n", e.getInfo().c_str());
        return -1;
    }

    
    return 0;
}

int cpswLoadConfigFile(const char *yaml_file, const char *prefix, const char *yaml_dir, const char *named_root)
{
    Path pre;
    
    if(!cpswIsThereRoot()) {
        fprintf(stderr, "No hierarchy found; use 'cpswLoadYamlFile(with HIERARCHY yaml, not config yaml)\n");
        return -1;
    }
    
    if(!yaml_file) {
        fprintf(stderr, "Missing YAML config file name argument\n");
        return -1;
    }
    
    try {
        Path theRoot = (!named_root || !strlen(named_root))? cpswGetRoot(): cpswGetNamedRoot(named_root);
        pre = prefix ? theRoot->findByName(prefix) : theRoot;
    } catch (CPSWError &e) {
        fprintf(stderr, "Unable to load CONFIG file: %s\n", e.getInfo().c_str());
        return -1;
    }
    
    if(!pre) {
        throw InternalError("cpswLoadConfigFile: should have a Path object here!");
    }
    
    return pre->loadConfigFromYamlFile(yaml_file, yaml_dir);
}

int cpswDumpConfigFile(const char *yaml_file, const char *prefix, const char *yaml_dir, const char *named_root)
{
    Path pre;
    
    if(!cpswIsThereRoot()) {
        fprintf(stderr, "No hierarchy found; use 'clswLoadYamlFile(with HIERARCHY yaml, cont config yaml)\n");
        return -1;
    }
    
    if(!yaml_file) {
        fprintf(stderr, "Missing YAML config dump file name argument\n");
        return -1;
    }
    
    try {
        Path theRoot = (!named_root || !strlen(named_root))? cpswGetRoot(): cpswGetNamedRoot(named_root);
        pre = prefix ? theRoot->findByName(prefix) : theRoot;
    } catch (CPSWError &e) {
        fprintf(stderr, "Unable to dump Config file: %s\n", e.getInfo().c_str());
        return -1;
    }
    
    if(!pre) {
        throw InternalError("cpswDumpConfigFile: should have a Path object here!");
    }
    
    
    YAML::Node config;  YAML::Emitter emit;
    std::fstream strm(yaml_file, std::fstream::out);
    
    pre->dumpConfigToYaml(config);
    emit << config;
    strm << emit.c_str() << "\n";
    
    return 0;
}


static const iocshArg loadYamlArg0   = {"YAML hierarchy descrtiption file",                                        iocshArgString};
static const iocshArg loadYamlArg1   = {"Root Device Name (optional; default = 'root')",                           iocshArgString};
static const iocshArg loadYamlArg2   = {"directory where YAML includes can be found (optional)",                   iocshArgString};
static const iocshArg loadYamlArg3   = {"IP address for carrier board (orverride the IP address in YAML)",         iocshArgString};
static const iocshArg loadYamlArg4   = {"Named Root - base name for multiple roots extension (optional)",          iocshArgString};
static const iocshArg * const loadYamlArgs[] = { &loadYamlArg0,
                                                 &loadYamlArg1,
                                                 &loadYamlArg2,
                                                 &loadYamlArg3,
                                                 &loadYamlArg4 };
static const iocshFuncDef loadYamlFuncDef = { "cpswLoadYamlFile", 5, loadYamlArgs };

static void loadYamlCallFunc(const iocshArgBuf *args)
{
    // cpswLoadYamlFile(const char *yaml_file, const char *root, const char *yaml_dir);
    if(!args[0].sval ||!strlen(args[0].sval)) {
        printf("Missing YAML HIERARCHY file name\n");
        printf("Please, try \"help cpswLoadYamlLoadFile\" to get more information\n");
        return;
    }

    cpswLoadYamlFile((args[0].sval && strlen(args[0].sval))? args[0].sval : NULL, 
                     (args[1].sval && strlen(args[1].sval))? args[1].sval : NULL, 
                     (args[2].sval && strlen(args[2].sval))? args[2].sval : NULL, 
                     (args[3].sval && strlen(args[3].sval))? args[3].sval : NULL,
                     (args[4].sval && strlen(args[4].sval))? args[4].sval : NULL);
    
}


static const iocshArg loadConfigArg0 = {"CONFIG YAML file (NOT hierarchy YAML!)",                                  iocshArgString};
static const iocshArg loadConfigArg1 = {"Path prefix (if config YAML is not based of the root device; optional)",  iocshArgString};
static const iocshArg loadConfigArg2 = {"directory where YAML includes can be found (optional)",                   iocshArgString};
static const iocshArg loadConfigArg3 = {"Named Root - base name for multiple roots extension (optional)",          iocshArgString};
static const iocshArg * const loadConfigArgs[] = { &loadConfigArg0,
                                                   &loadConfigArg1,
                                                   &loadConfigArg2,
                                                   &loadConfigArg3 };
static const iocshFuncDef loadConfigFuncDef = { "cpswLoadConfigFile", 4, loadConfigArgs };

static void loadConfigCallFunc(const iocshArgBuf *args)
{
    //cpswLoadConfigFile(const char *yaml_file, const char *prefix, const char *yaml_dir);
    if(!args[0].sval || !strlen(args[0].sval)) {
        printf("Missing YAML CONFIG file name argument\n");
        printf("Please, try \"help cpswLoadConfigFile\" to get more information\n");
        return;
    }
    
    cpswLoadConfigFile((args[0].sval && strlen(args[0].sval)) ? args[0].sval : NULL, 
                       (args[1].sval && strlen(args[1].sval)) ? args[1].sval : NULL, 
                       (args[2].sval && strlen(args[2].sval)) ? args[2].sval : NULL,
                       (args[3].sval && strlen(args[3].sval)) ? args[3].sval : NULL);
}

static const iocshArg dumpConfigArg0 = {"DUMP YAML file (NOT hierarch YAML!)",                                  iocshArgString};
static const iocshArg dumpConfigArg1 = {"Path prefix (if dump YAML is not based of the root device; optional)", iocshArgString};
static const iocshArg dumpConfigArg2 = {"directory where will be the DUMP YAML located (optional)",             iocshArgString};
static const iocshArg dumpConfigArg3 = {"Named Root - base name for multiple roots extension (optional)",       iocshArgString};
static const iocshArg * const dumpConfigArgs[] = { &dumpConfigArg0,
                                                   &dumpConfigArg1,
                                                   &dumpConfigArg2,
                                                   &dumpConfigArg3 };
static const iocshFuncDef dumpConfigFuncDef = { "cpswDumpConfigFile", 4, dumpConfigArgs };

static void dumpConfigCallFunc(const iocshArgBuf *args)
{

    /* coswDumpConfigFile( const char *yaml_file, const char *prefix, const char *yaml_dir) */
    
    if(!args[0].sval || !strlen(args[0].sval)) {
        printf("Missing YAML CONFIG file name argument\n");
        printf("Please, try \"help cpswDumpConfigFile\" to get more information\n");
        return;
    }
    
    cpswDumpConfigFile((args[0].sval && strlen(args[0].sval)) ? args[0].sval : NULL, 
                       (args[1].sval && strlen(args[1].sval)) ? args[1].sval : NULL, 
                       (args[2].sval && strlen(args[2].sval)) ? args[2].sval : NULL,
                       (args[3].sval && strlen(args[3].sval)) ? args[3].sval : NULL);
} 

static void yamlLoaderRegister(void)
{
    iocshRegister(&loadYamlFuncDef, loadYamlCallFunc);
    iocshRegister(&loadConfigFuncDef, loadConfigCallFunc);
    iocshRegister(&dumpConfigFuncDef, dumpConfigCallFunc);
}

epicsExportRegistrar(yamlLoaderRegister);







/* EPICS driver support for yamlLoader */

static int yamlLoaderReport(int interest);
static int yamlLoaderInitialize(void);

static struct drvet yamlLoaderDriver = {
    2, 
    (DRVSUPFUN) yamlLoaderReport,
    (DRVSUPFUN) yamlLoaderInitialize
};

epicsExportAddress(drvet, yamlLoaderDriver);

static int yamlLoaderReport(int interest)
{
    printf("found %d root(s)\n", cpswIsThereRoot());
    if(cpswIsThereRoot()) {
        rootList_t *p = (rootList_t *) ellFirst(pRootList);
        while(p) {
            printf("\t%s\n", p->name);
            p = (rootList_t *) ellNext(&p->node);
        }
    }

    return 0;
}

static int yamlLoaderInitialize(void)
{
    return 0;
}
                                                    

} /* extern "C" */
