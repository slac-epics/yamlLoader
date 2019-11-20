#ifndef CPSW_YAML_LOADER_H
#define CPSW_YAML_LOADER_H

#include <cpsw_api_user.h>

#ifdef __cplusplus
extern "C" { 
#endif /* __cplusplus */

int cpswIsThereRoot(void);
Path cpswGetRoot(void);
void cpswPutRoot(Path root);
Path cpswGetNamedRoot(const char *named_root);
void cpswPutNamedRoot(Path root, const char *named_root);
int cpswLoadYamlFile(const char *yaml_file, const char *root, const char *yaml_dir, const char *ip_addr, const char *named_root);
int cpswLoadConfigFile(const char *yaml_file, const char *prefix, const char *yaml_dir, const char *named_root);
int cpswDumpConfigFile(const char *yaml_file, const char *prefix, const char *yaml_dir, const char *named_root);

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif  /* CPSW_YAML_LOADER_H */
