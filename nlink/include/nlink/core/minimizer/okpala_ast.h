#ifndef OKPALA_AST_H
#define OKPALA_AST_H


#include "nlink/core/common/types.h"
#include "nlink/core/common/result.h"
#include <stddef.h>
   #include <stdlib.h>
   #include <string.h>
   #include <stdio.h>
   #include <stdbool.h>
   #include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// Node structure for the AST
typedef struct OkpalaNode {
	char* value;
	struct OkpalaNode** children;
	struct OkpalaNode* parent;
	size_t child_count;
} OkpalaNode;

// AST structure
typedef struct OkpalaAST {
	OkpalaNode* root;
	size_t node_count;
} OkpalaAST;

// Public API functions
OkpalaAST* okpala_ast_create(void);
NexusResult okpala_ast_add_node(OkpalaAST* ast, OkpalaNode* parent, const char* value);
OkpalaAST* okpala_optimize_ast(OkpalaAST* ast, bool use_boolean_reduction);
void okpala_ast_free(OkpalaAST* ast);

#ifdef __cplusplus
}
#endif

#endif // OKPALA_AST_H
