#ifndef CPSW_YAML_LOADER_H
#define CPSW_YAML_LOADER_H

#include <cpsw_api_user.h>

#ifdef __cplusplus
extern "C" { 
#endif /* __cplusplus */

Path cpswGetRoot(void);
void cpswPutRoot(Path root);
int cpswLoadYamlFile(const char *yaml_file, const char *root, const char *yaml_dir, const char *ip_addr);
int cpswLoadConfigFile(const char *yaml_file, const char *prefix, const char *yaml_dir);
int cpswDumpConfigFile(const char *yaml_file, const char *prefix, const char *yaml_dir);

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif  /* CPSW_YAML_LOADER_H */
