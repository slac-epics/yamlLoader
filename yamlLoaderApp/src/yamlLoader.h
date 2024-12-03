//////////////////////////////////////////////////////////////////////////////
// This file is part of 'yamlLoader'.
// It is subject to the license terms in the LICENSE.txt file found in the 
// top-level directory of this distribution and at: 
//    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
// No part of 'yamlLoader', including this file, 
// may be copied, modified, propagated, or distributed except according to 
// the terms contained in the LICENSE.txt file.
//////////////////////////////////////////////////////////////////////////////
#ifndef CPSW_YAML_LOADER_H
#define CPSW_YAML_LOADER_H

#include <cpsw_api_user.h>

#ifdef __cplusplus
extern "C" { 
#endif /* __cplusplus */

int cpswIsThereRoot(void);
Path cpswGetRoot(void);
char * cpswGetRootName(void);
void cpswPutRoot(Path root);
Path cpswGetNamedRoot(const char *named_root);
char * cpswGetDescofNamedRoot(const char *named_root);
void cpswPutNamedRoot(Path root, const char *named_root);
void cpswPutNamedRootwithDesc(Path root, const char *desc, const char *named_root);
int cpswLoadYamlFile(const char *yaml_file, const char *root, const char *yaml_dir, const char *ip_addr, const char *named_root);
int cpswLoadConfigFile(const char *yaml_file, const char *prefix, const char *yaml_dir, const char *named_root);
int cpswDumpConfigFile(const char *yaml_file, const char *prefix, const char *yaml_dir, const char *named_root);

#ifdef __cplusplus
} /* extern C */
#endif /* __cplusplus */

#endif  /* CPSW_YAML_LOADER_H */
