/* generated deterministically by self-hosted slimc */
#include "slim_rt.h"
#include <string.h>

typedef struct Slim_type_cache_95Key Slim_type_cache_95Key;
typedef struct Slim_type_cache_95Number Slim_type_cache_95Number;
typedef struct Slim_type_cache_95Probe Slim_type_cache_95Probe;
typedef struct Slim_type_check_95Diagnostic Slim_type_check_95Diagnostic;
typedef struct Slim_type_ir_95DeclarationKind Slim_type_ir_95DeclarationKind;
typedef struct Slim_type_ir_95Declaration Slim_type_ir_95Declaration;
typedef struct Slim_type_memory_95ValuePlan Slim_type_memory_95ValuePlan;
typedef struct Slim_type_memory_95AllocationPlan Slim_type_memory_95AllocationPlan;
typedef struct Slim_type_memory_95DestructionPlan Slim_type_memory_95DestructionPlan;
typedef struct Slim_type_memory_95FunctionPlan Slim_type_memory_95FunctionPlan;
typedef struct Slim_type_memory_95Plan Slim_type_memory_95Plan;
typedef struct Slim_type_project_95LoadedModule Slim_type_project_95LoadedModule;
typedef struct Slim_type_query_95Snapshot Slim_type_query_95Snapshot;
typedef struct Slim_type_query_95Work Slim_type_query_95Work;
typedef struct Slim_type_query_95Dependency Slim_type_query_95Dependency;
typedef struct Slim_type_query_95State Slim_type_query_95State;
typedef struct Slim_type_scheduler_95Task Slim_type_scheduler_95Task;
typedef struct Slim_type_scheduler_95Batch Slim_type_scheduler_95Batch;
typedef struct Slim_type_scheduler_95Schedule Slim_type_scheduler_95Schedule;
typedef struct Slim_type_syntax_95Token Slim_type_syntax_95Token;

struct Slim_type_cache_95Key {
bool slim_field_valid;
SlimBytes slim_field_bytes;
};

struct Slim_type_cache_95Number {
bool slim_field_valid;
int64_t slim_field_value;
int64_t slim_field_next;
};

struct Slim_type_cache_95Probe {
bool slim_field_valid;
int64_t slim_field_artifact_95start;
int64_t slim_field_artifact_95end;
};

struct Slim_type_check_95Diagnostic {
SlimBytes slim_field_code;
int64_t slim_field_start;
int64_t slim_field_end;
};

#define SLIM_TAG_ir_95DeclarationKind_Function ((int32_t)0)
#define SLIM_TAG_ir_95DeclarationKind_Record ((int32_t)1)
#define SLIM_TAG_ir_95DeclarationKind_Variant ((int32_t)2)
struct Slim_type_ir_95DeclarationKind {
int32_t tag;
union {
uint8_t slim_unused;
} data;
};

struct Slim_type_ir_95Declaration {
Slim_type_ir_95DeclarationKind slim_field_kind;
int64_t slim_field_form;
int64_t slim_field_name;
int64_t slim_field_next;
};

struct Slim_type_memory_95ValuePlan {
int64_t slim_field_name;
int64_t slim_field_type_95index;
int64_t slim_field_storage;
bool slim_field_borrowed;
int64_t slim_field_declared;
int64_t slim_field_last_95use;
int64_t slim_field_escape;
};

struct Slim_type_memory_95AllocationPlan {
int64_t slim_field_site;
int64_t slim_field_region;
};

struct Slim_type_memory_95DestructionPlan {
int64_t slim_field_region;
int64_t slim_field_end;
bool slim_field_reverse;
};

struct Slim_type_memory_95FunctionPlan {
int64_t slim_field_function;
int64_t slim_field_return_95storage;
bool slim_field_local_95region;
bool slim_field_liveness_95precise;
SlimVec slim_field_values;
SlimVec slim_field_allocations;
SlimVec slim_field_destructions;
int64_t slim_field_escaping_95values;
};

struct Slim_type_memory_95Plan {
SlimVec slim_field_functions;
};

struct Slim_type_project_95LoadedModule {
int64_t slim_field_manifest;
int64_t slim_field_name;
int64_t slim_field_path;
SlimBytes slim_field_source;
int64_t slim_field_root;
};

struct Slim_type_query_95Snapshot {
SlimBytes slim_field_module;
int64_t slim_field_kind;
SlimBytes slim_field_name;
SlimBytes slim_field_source;
int64_t slim_field_form;
int64_t slim_field_next;
int64_t slim_field_start;
int64_t slim_field_interface_95end;
int64_t slim_field_end;
};

struct Slim_type_query_95Work {
int64_t slim_field_parsed;
int64_t slim_field_lowered;
int64_t slim_field_checked;
int64_t slim_field_generated;
};

struct Slim_type_query_95Dependency {
int64_t slim_field_consumer;
int64_t slim_field_provider;
};

struct Slim_type_query_95State {
SlimVec slim_field_snapshots;
SlimVec slim_field_dependencies;
};

struct Slim_type_scheduler_95Task {
int64_t slim_field_module;
int64_t slim_field_layer;
};

struct Slim_type_scheduler_95Batch {
int64_t slim_field_start;
int64_t slim_field_end;
};

struct Slim_type_scheduler_95Schedule {
bool slim_field_valid;
int64_t slim_field_workers;
SlimVec slim_field_tasks;
SlimVec slim_field_batches;
};

struct Slim_type_syntax_95Token {
int64_t slim_field_kind;
int64_t slim_field_start;
int64_t slim_field_end;
};

static Slim_type_cache_95Number slim_fn_cache_95valid_95number(int64_t slim_v_value, int64_t slim_v_next, SlimRegion *slim_region);
static Slim_type_cache_95Number slim_fn_cache_95invalid_95number(int64_t slim_v_next, SlimRegion *slim_region);
static Slim_type_cache_95Key slim_fn_cache_95invalid_95key(SlimRegion *slim_region);
static Slim_type_cache_95Key slim_fn_cache_95make_95key(bool slim_v_flag, SlimBytes slim_v_content, SlimRegion *slim_region);
static SlimUnit slim_fn_cache_95append_95u64(int64_t slim_v_value, int64_t slim_v_divisor, int64_t slim_v_remaining, SlimVec * slim_v_output, SlimRegion *slim_region);
static Slim_type_cache_95Number slim_fn_cache_95read_95bounded_95u64_95from(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_remaining, int64_t slim_v_limit, int64_t slim_v_value, SlimRegion *slim_region);
static Slim_type_cache_95Number slim_fn_cache_95read_95bounded_95u64(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_limit, SlimRegion *slim_region);
static int64_t slim_fn_cache_95weighted_95checksum(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_sum, SlimRegion *slim_region);
static bool slim_fn_cache_95append_95module_95key(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static Slim_type_cache_95Key slim_fn_cache_95project_95key(SlimBytes slim_v_manifest_95path, SlimRegion *slim_region);
static SlimBytes slim_fn_cache_95encode(SlimBytes slim_v_key, SlimBytes slim_v_artifact, SlimRegion *slim_region);
static Slim_type_cache_95Probe slim_fn_cache_95invalid_95probe(SlimRegion *slim_region);
static Slim_type_cache_95Probe slim_fn_cache_95probe(SlimBytes slim_v_cache, SlimBytes slim_v_key, SlimRegion *slim_region);
static int64_t slim_fn_cache_95emit_95hit(SlimBytes slim_v_cache, Slim_type_cache_95Probe slim_v_cache_95probe, SlimRegion *slim_region);
static int64_t slim_fn_cache_95emit_95miss(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_key, SlimRegion *slim_region);
static int64_t slim_fn_cache_95run(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_cache_95path, SlimRegion *slim_region);
static bool slim_fn_check_95parameter_95has_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static bool slim_fn_check_95prior_95let_95has_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static bool slim_fn_check_95match_95name_95is_95known(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body_95start, int64_t slim_v_name, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95boolean_95match_95arms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body_95start, int64_t slim_v_expected, int64_t slim_v_cursor, bool slim_v_seen_95true, bool slim_v_seen_95false, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95boolean_95match_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body_95start, int64_t slim_v_expected, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95boolean_95match_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_95check_95path_95all(SlimBytes slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_check_95recur_95parameter_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95recur_95arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_argument, int64_t slim_v_parameter, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95recur_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95recur_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_95check_95path_95recur(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static bool slim_fn_check_95ownership_95type_95is_95copy(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_type_95index, SlimRegion *slim_region);
static int64_t slim_fn_check_95ownership_95parameter_95type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region);
static bool slim_fn_check_95moved_95has(SlimBytes slim_v_source, SlimVec * slim_v_moved, int64_t slim_v_index, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95owned_95arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_current_95params, int64_t slim_v_argument, int64_t slim_v_parameter, SlimVec * slim_v_moved, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95builtin_95move(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_current_95params, int64_t slim_v_callee, int64_t slim_v_argument, SlimVec * slim_v_moved, SlimRegion *slim_region);
static bool slim_fn_check_95builtin_95group_95math(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static bool slim_fn_check_95builtin_95group_95compare(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static bool slim_fn_check_95builtin_95group_95scalar(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static bool slim_fn_check_95builtin_95group_95bytes_95io(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static bool slim_fn_check_95builtin_95group_95storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static bool slim_fn_check_95builtin_95is_95known(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95call_95move(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_current_95params, int64_t slim_v_callee, int64_t slim_v_argument, SlimVec * slim_v_moved, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95moves_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_current_95params, int64_t slim_v_cursor, int64_t slim_v_end, SlimVec * slim_v_moved, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95inout_95return(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_return_95type, int64_t slim_v_body, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95move_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, SlimVec * slim_v_declarations, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_check_95check_95path_95moves(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static bool slim_fn_check_95parameter_95is_95inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95inout_95temporary_95args(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_argument, int64_t slim_v_parameter, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95inout_95temporary_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95inout_95temporary_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95inout_95temporary_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_check_95prior_95inout_95alias(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_argument, int64_t slim_v_parameter, int64_t slim_v_current_95argument, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95inout_95alias_95args(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_first_95argument, int64_t slim_v_first_95parameter, int64_t slim_v_argument, int64_t slim_v_parameter, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95inout_95alias_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95inout_95alias_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95inout_95alias_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_check_95check_95path_95inout(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static bool slim_fn_check_95boolean_95arms_95have(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimBytes slim_v_pattern, SlimRegion *slim_region);
static bool slim_fn_check_95boolean_95match_95incomplete(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95nonexhaustive_95in_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95nonexhaustive_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_check_95check_95path_95exhaustive(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static bool slim_fn_check_95effect_95list_95has(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_effect_95code, SlimRegion *slim_region);
static bool slim_fn_check_95builtin_95requires_95effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, int64_t slim_v_effect_95code, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95function_95effects(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_callee_95start, int64_t slim_v_callee_95end, SlimRegion *slim_region);
static bool slim_fn_check_95call_95requires_95effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_callee, int64_t slim_v_effect_95code, SlimRegion *slim_region);
static bool slim_fn_check_95call_95missing_95effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_declared_95effects, int64_t slim_v_callee, int64_t slim_v_effect_95code, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95missing_95effect_95in_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_declared_95effects, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95missing_95effect_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_check_95check_95path_95effects(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95overqualified(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static int64_t slim_fn_check_95check_95path_95project(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static int64_t slim_fn_check_95result_95expr(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region);
static int64_t slim_fn_check_95atom_95type_95kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95type_95mismatch_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_check_95check_95path_95typed(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95unterminated(SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_depth, int64_t slim_v_form_95start, SlimRegion *slim_region);
static SlimUnit slim_fn_check_95append_95diagnostic_95identity(Slim_type_check_95Diagnostic slim_v_diagnostic, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_check_95check_95path(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static int64_t slim_fn_check_95last_95form_95in_95list(SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_previous, SlimRegion *slim_region);
static bool slim_fn_check_95known_95expr_95head(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_head, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95unknown_95pair(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_value, int64_t slim_v_body, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95unknown_95control(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, int64_t slim_v_head, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95unknown_95data(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, int64_t slim_v_head, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95unknown_95known(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, int64_t slim_v_head, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95unknown_95expr(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95unknown_95forms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95unknown_95fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95unknown_95arms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_check_95find_95unknown_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_check_95report_95diagnostic(SlimBytes slim_v_code, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region);
static int64_t slim_fn_check_95check_95path_95full(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95value_95address(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_value, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_95emit_95vec_95set_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_95emit_95arena_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_95emit_95vec_95push_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_95emit_95collection_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95let_95full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95match_95value_95binding(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_value, bool slim_v_boolean_95match, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95match_95full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95set_95full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95expr_95full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95mangled_95span(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95mangled_95token(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95source_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95function_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95named_95type_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95field_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95case_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95tag_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_variant, int64_t slim_v_case_95name, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95source_95type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95forward_95types(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95record_95fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95record(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95variant_95tags(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_variant_95name, int64_t slim_v_cursor, int64_t slim_v_tag, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95variant_95payload_95fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_field, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95variant_95payloads(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95variant(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95data_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95binding_95value(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_name, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95binding_95address(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_name, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_codegen_95builtin_95argument_95type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, int64_t slim_v_position, SlimRegion *slim_region);
static int64_t slim_fn_codegen_95parameter_95type_95index(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95call_95argument_95bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_argument, int64_t slim_v_callee_95param, int64_t slim_v_position, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95call_95bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95value(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_value, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimBytes slim_fn_codegen_95runtime_95math_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static SlimBytes slim_fn_codegen_95runtime_95bytes_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static SlimBytes slim_fn_codegen_95runtime_95io_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static SlimBytes slim_fn_codegen_95runtime_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95value_95arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95user_95arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_current_95params, int64_t slim_v_argument, int64_t slim_v_callee_95param, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95parameters(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95prototype(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95prototypes(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_95contains_95atom(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, SlimBytes slim_v_text, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95parameter_95voids(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95temp_95name(int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95destination(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95recur_95bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_param, int64_t slim_v_argument, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95recur_95updates(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_param, int64_t slim_v_argument, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95recur_95assignments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_param, int64_t slim_v_argument, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95runtime_95assignment(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimBytes slim_v_runtime, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95binary_95operator(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimBytes slim_v_operator, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_95emit_95comparison(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_95emit_95boolean_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_95emit_95u8_95conversion(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_95emit_95vector_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95user_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_call_95form, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_codegen_95find_95record_95item(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static int64_t slim_fn_codegen_95find_95record_95field(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95make_95bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_definition, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95make_95fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95make(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95get(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95case_95bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_payload_95type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95case_95payload(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_field, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95case_95value(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_codegen_95find_95parameter_95type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static int64_t slim_fn_codegen_95find_95variant_95item(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static int64_t slim_fn_codegen_95find_95variant_95case(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95variant_95bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_value, int64_t slim_v_case_95name, int64_t slim_v_bindings, int64_t slim_v_payload_95type, int64_t slim_v_field, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95variant_95arms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_variant_95name, int64_t slim_v_variant_95cases, int64_t slim_v_cursor, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95boolean_95match(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_first_95arm, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95variant_95match(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_first_95arm, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95atom_95assignment(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95function(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95functions(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_codegen_95emit_95program(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_codegen_95emit_95read_95file_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_compiler_95check_95input_95path(SlimBytes slim_v_path, SlimRegion *slim_region);
static SlimUnit slim_fn_compiler_95format_95tokens(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_previous_95kind, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_compiler_95format_95path(SlimBytes slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_compiler_95compile_95path(SlimBytes slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_compiler_95run_95compile(SlimVec * slim_v_args, int64_t slim_v_arg_95count, SlimBytes slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_compiler_95run_95cache(SlimVec * slim_v_args, int64_t slim_v_arg_95count, SlimRegion *slim_region);
static int64_t slim_fn_compiler_95run_95session(SlimVec * slim_v_args, int64_t slim_v_arg_95count, SlimRegion *slim_region);
static int64_t slim_fn_compiler_95run(SlimVec slim_v_args, SlimRegion *slim_region);
static int64_t slim_fn_main(SlimVec slim_v_args, SlimRegion *slim_region);
static int64_t slim_fn_memory_95storage_95join(int64_t slim_v_left, int64_t slim_v_right, SlimRegion *slim_region);
static int64_t slim_fn_memory_95find_95type_95item(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static int64_t slim_fn_memory_95record_95storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_depth, int64_t slim_v_current, SlimRegion *slim_region);
static int64_t slim_fn_memory_95payload_95storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_depth, int64_t slim_v_current, SlimRegion *slim_region);
static int64_t slim_fn_memory_95variant_95storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_depth, int64_t slim_v_current, SlimRegion *slim_region);
static int64_t slim_fn_memory_95type_95storage_95kind_95from(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_type_95index, int64_t slim_v_depth, SlimRegion *slim_region);
static int64_t slim_fn_memory_95type_95storage_95kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_type_95index, SlimRegion *slim_region);
static bool slim_fn_memory_95parameters_95have_95inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static bool slim_fn_memory_95function_95uses_95local_95region(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_function, SlimRegion *slim_region);
static bool slim_fn_memory_95effect_95list_95has_95alloc(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static bool slim_fn_memory_95params_95have_95alloc_95effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, SlimRegion *slim_region);
static bool slim_fn_memory_95function_95has_95alloc_95effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_function, SlimRegion *slim_region);
static int64_t slim_fn_memory_95last_95named_95use(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_95start, int64_t slim_v_name_95end, int64_t slim_v_last, SlimRegion *slim_region);
static bool slim_fn_memory_95span_95has_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static int64_t slim_fn_memory_95result_95expression(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region);
static bool slim_fn_memory_95escapes_95to_95inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static int64_t slim_fn_memory_95escape_95kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_95end, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static SlimUnit slim_fn_memory_95append_95value_95plan(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_95end, int64_t slim_v_name, int64_t slim_v_type_95index, bool slim_v_borrowed, int64_t slim_v_declared, SlimVec * slim_v_values, SlimRegion *slim_region);
static SlimUnit slim_fn_memory_95append_95parameter_95plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_95end, int64_t slim_v_cursor, SlimVec * slim_v_values, SlimRegion *slim_region);
static SlimUnit slim_fn_memory_95append_95local_95plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_95end, int64_t slim_v_cursor, SlimVec * slim_v_values, SlimRegion *slim_region);
static SlimUnit slim_fn_memory_95append_95allocation_95plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_region, SlimVec * slim_v_allocations, SlimRegion *slim_region);
static int64_t slim_fn_memory_95conservative_95escape(int64_t slim_v_return_95storage, bool slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_memory_95finalize_95value_95plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_95end, int64_t slim_v_return_95storage, bool slim_v_output, SlimVec * slim_v_values, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_memory_95count_95escaping_95values(SlimVec * slim_v_values, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region);
static Slim_type_memory_95FunctionPlan slim_fn_memory_95build_95function_95plan(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_function, SlimRegion *slim_region);
static SlimUnit slim_fn_memory_95append_95function_95plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimVec * slim_v_functions, SlimRegion *slim_region);
static Slim_type_memory_95Plan slim_fn_memory_95analyze(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static bool slim_fn_project_95cross_95span_95chars_95equal(SlimBytes slim_v_left, int64_t slim_v_left_95start, SlimBytes slim_v_right, int64_t slim_v_right_95start, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static bool slim_fn_project_95cross_95spans_95equal(SlimBytes slim_v_left, int64_t slim_v_left_95start, int64_t slim_v_left_95end, SlimBytes slim_v_right, int64_t slim_v_right_95start, int64_t slim_v_right_95end, SlimRegion *slim_region);
static int64_t slim_fn_project_95last_95path_95slash(SlimBytes slim_v_path, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_last, SlimRegion *slim_region);
static SlimBytes slim_fn_project_95project_95module_95path(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_path_95token, SlimRegion *slim_region);
static bool slim_fn_project_95declaration_95has_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95slash_95replaced(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95project_95atom(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, bool slim_v_entry, int64_t slim_v_atom, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95project_95tokens(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, bool slim_v_entry, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_previous_95kind, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95project_95modules(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_entry_95name, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimBytes slim_fn_project_95flatten_95project(SlimBytes slim_v_path, SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimRegion *slim_region);
static bool slim_fn_project_95path_95is_95project(SlimBytes slim_v_path, SlimRegion *slim_region);
static SlimBytes slim_fn_project_95generate_95project_95path(SlimBytes slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_project_95compile_95project_95path_95jobs(SlimBytes slim_v_path, int64_t slim_v_jobs, SlimRegion *slim_region);
static int64_t slim_fn_project_95compile_95project_95path(SlimBytes slim_v_path, SlimRegion *slim_region);
static SlimBytes slim_fn_project_95token_95bytes(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95project_95diagnostic(SlimBytes slim_v_code, SlimBytes slim_v_module_95name, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region);
static bool slim_fn_project_95imports_95has_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95manifest_95module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static bool slim_fn_project_95module_95imports_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_module_95name, int64_t slim_v_import_95start, int64_t slim_v_import_95end, SlimRegion *slim_region);
static bool slim_fn_project_95module_95cycle_95imports(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_module_95name, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95reciprocal_95cycle(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95span_95slash(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimRegion *slim_region);
static bool slim_fn_project_95exports_95has_95segment(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_cursor, SlimBytes slim_v_module_95source, int64_t slim_v_segment_95start, int64_t slim_v_segment_95end, SlimRegion *slim_region);
static int64_t slim_fn_project_95check_95project_95path(SlimBytes slim_v_path, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95interface_95type(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_type_95index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95interface_95parameters(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95interface_95effects(SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95export_95declaration(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_export_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95interface_95fields(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95interface_95case_95types(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95interface_95cases(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95interface_95declaration(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95interface_95exports(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, int64_t slim_v_cursor, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_project_95append_95project_95interfaces(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region);
static int64_t slim_fn_project_95interfaces_95path(SlimBytes slim_v_path, SlimRegion *slim_region);
static bool slim_fn_project_95span_95less_95chars(SlimBytes slim_v_source, int64_t slim_v_left, int64_t slim_v_left_95end, int64_t slim_v_right, int64_t slim_v_right_95end, SlimRegion *slim_region);
static bool slim_fn_project_95span_95less(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_left, int64_t slim_v_right, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95unsorted_95module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_previous, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95prior_95module_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_current, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95duplicate_95module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95prior_95path(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_current, int64_t slim_v_path_95start, int64_t slim_v_path_95end, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95duplicate_95path(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region);
static bool slim_fn_project_95path_95segment_95valid(SlimBytes slim_v_source, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region);
static bool slim_fn_project_95path_95segments_95valid(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_segment_95start, SlimRegion *slim_region);
static bool slim_fn_project_95path_95token_95invalid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95invalid_95path(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95self_95import_95in(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95self_95import(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95unknown_95import_95in(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95unknown_95import(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95manifest_95rules(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_entry_95form, int64_t slim_v_modules, SlimRegion *slim_region);
static int64_t slim_fn_project_95load_95project_95modules(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_cursor, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95module_95identities(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95missing_95export(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_cursor, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95missing_95exports(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95named_95declaration(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimBytes slim_v_name, SlimRegion *slim_region);
static bool slim_fn_project_95loaded_95is_95entry(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_entry, Slim_type_project_95LoadedModule slim_v_module, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95project_95mains(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_entry, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95loaded_95segment(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_loaded, SlimBytes slim_v_reference_95source, int64_t slim_v_segment_95start, int64_t slim_v_segment_95end, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95declaration_95segment(SlimBytes slim_v_declaration_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, SlimBytes slim_v_reference_95source, int64_t slim_v_segment_95start, int64_t slim_v_segment_95end, SlimRegion *slim_region);
static int64_t slim_fn_project_95target_95call_95status(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, Slim_type_project_95LoadedModule slim_v_current, int64_t slim_v_target_95index, SlimBytes slim_v_reference_95source, int64_t slim_v_prefix_95start, int64_t slim_v_slash, int64_t slim_v_reference_95end, SlimRegion *slim_region);
static int64_t slim_fn_project_95qualified_95call_95status(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, Slim_type_project_95LoadedModule slim_v_current, int64_t slim_v_callee, SlimRegion *slim_region);
static int64_t slim_fn_project_95find_95invalid_95loaded_95call(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, Slim_type_project_95LoadedModule slim_v_current, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95qualified_95calls(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95loaded_95project(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_entry, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95interface_95type(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_first_95export, SlimBytes slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, int64_t slim_v_type_95index, int64_t slim_v_diagnostic_95start, int64_t slim_v_diagnostic_95end, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95interface_95bindings(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_first_95export, SlimBytes slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_diagnostic_95start, int64_t slim_v_diagnostic_95end, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95interface_95payload(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_first_95export, SlimBytes slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_diagnostic_95start, int64_t slim_v_diagnostic_95end, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95interface_95cases(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_first_95export, SlimBytes slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_diagnostic_95start, int64_t slim_v_diagnostic_95end, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95export_95interfaces(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_first_95export, SlimBytes slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95private_95type_95leaks_95from(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_project_95report_95private_95type_95leaks(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region);
static SlimBytes slim_fn_query_95copy_95span(SlimBytes slim_v_source, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region);
static bool slim_fn_query_95bytes_95equal_95from(SlimBytes slim_v_left, SlimBytes slim_v_right, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static bool slim_fn_query_95bytes_95equal(SlimBytes slim_v_left, SlimBytes slim_v_right, SlimRegion *slim_region);
static int64_t slim_fn_query_95declaration_95kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimRegion *slim_region);
static int64_t slim_fn_query_95declaration_95interface_95end(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimRegion *slim_region);
static SlimUnit slim_fn_query_95append_95module_95snapshots(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, Slim_type_project_95LoadedModule slim_v_module, int64_t slim_v_cursor, SlimBytes slim_v_module_95name, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_query_95build_95snapshots_95from(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimVec slim_fn_query_95build_95snapshots(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region);
static bool slim_fn_query_95snapshot_95identity_95equal(Slim_type_query_95Snapshot slim_v_left, Slim_type_query_95Snapshot slim_v_right, SlimRegion *slim_region);
static bool slim_fn_query_95snapshot_95body_95equal(Slim_type_query_95Snapshot slim_v_left, Slim_type_query_95Snapshot slim_v_right, SlimRegion *slim_region);
static int64_t slim_fn_query_95find_95snapshot(SlimVec * slim_v_snapshots, Slim_type_query_95Snapshot slim_v_candidate, int64_t slim_v_index, SlimRegion *slim_region);
static SlimBytes slim_fn_query_95snapshot_95qualified_95name(Slim_type_query_95Snapshot slim_v_snapshot, SlimRegion *slim_region);
static int64_t slim_fn_query_95find_95provider(SlimBytes slim_v_atom, SlimVec * slim_v_snapshots, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_query_95dependency_95exists(SlimVec * slim_v_dependencies, int64_t slim_v_consumer, int64_t slim_v_provider, int64_t slim_v_index, SlimRegion *slim_region);
static SlimUnit slim_fn_query_95append_95snapshot_95dependencies(SlimVec * slim_v_tokens, SlimVec * slim_v_snapshots, int64_t slim_v_consumer, int64_t slim_v_cursor, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_query_95build_95dependencies_95from(SlimVec * slim_v_tokens, SlimVec * slim_v_snapshots, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimVec slim_fn_query_95build_95dependencies(SlimVec * slim_v_tokens, SlimVec * slim_v_snapshots, SlimRegion *slim_region);
static Slim_type_query_95State slim_fn_query_95build_95state(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region);
static bool slim_fn_query_95snapshot_95interface_95equal(Slim_type_query_95Snapshot slim_v_left, Slim_type_query_95Snapshot slim_v_right, SlimRegion *slim_region);
static SlimUnit slim_fn_query_95fill_95flags(int64_t slim_v_length, int64_t slim_v_index, SlimVec * slim_v_flags, SlimRegion *slim_region);
static int64_t slim_fn_query_95classify_95changes(SlimVec * slim_v_previous, SlimVec * slim_v_current, int64_t slim_v_index, int64_t slim_v_count, SlimVec * slim_v_invalidated, SlimVec * slim_v_queue, SlimRegion *slim_region);
static SlimUnit slim_fn_query_95mark_95consumers(SlimVec * slim_v_dependencies, int64_t slim_v_provider, int64_t slim_v_index, SlimVec * slim_v_invalidated, SlimVec * slim_v_queue, SlimRegion *slim_region);
static SlimUnit slim_fn_query_95propagate_95invalidations(SlimVec * slim_v_dependencies, SlimVec * slim_v_queue, int64_t slim_v_cursor, SlimVec * slim_v_invalidated, SlimRegion *slim_region);
static int64_t slim_fn_query_95count_95flags(SlimVec * slim_v_flags, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region);
static Slim_type_query_95Work slim_fn_query_95measure_95update(SlimVec * slim_v_previous, SlimVec * slim_v_current, SlimVec * slim_v_dependencies, SlimRegion *slim_region);
static SlimUnit slim_fn_scheduler_95collect_95modules(SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_modules, SlimRegion *slim_region);
static SlimUnit slim_fn_scheduler_95fill_95pending(int64_t slim_v_length, int64_t slim_v_index, SlimVec * slim_v_scheduled, SlimRegion *slim_region);
static int64_t slim_fn_scheduler_95find_95module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, int64_t slim_v_name, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_scheduler_95imports_95ready(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, SlimVec * slim_v_scheduled, int64_t slim_v_cursor, SlimRegion *slim_region);
static SlimUnit slim_fn_scheduler_95collect_95ready(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, SlimVec * slim_v_scheduled, int64_t slim_v_index, SlimVec * slim_v_ready, SlimRegion *slim_region);
static SlimUnit slim_fn_scheduler_95append_95tasks(SlimVec * slim_v_ready, int64_t slim_v_index, int64_t slim_v_layer, SlimVec * slim_v_tasks, SlimRegion *slim_region);
static SlimUnit slim_fn_scheduler_95append_95batches(int64_t slim_v_start, int64_t slim_v_end, int64_t slim_v_workers, SlimVec * slim_v_batches, SlimRegion *slim_region);
static SlimUnit slim_fn_scheduler_95mark_95ready(SlimVec * slim_v_ready, int64_t slim_v_index, SlimVec * slim_v_scheduled, SlimRegion *slim_region);
static bool slim_fn_scheduler_95schedule_95layers(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, SlimVec * slim_v_scheduled, int64_t slim_v_completed, int64_t slim_v_layer, int64_t slim_v_workers, SlimVec * slim_v_tasks, SlimVec * slim_v_batches, SlimRegion *slim_region);
static int64_t slim_fn_scheduler_95bounded_95workers(int64_t slim_v_requested, int64_t slim_v_module_95count, SlimRegion *slim_region);
static Slim_type_scheduler_95Schedule slim_fn_scheduler_95make_95schedule(bool slim_v_valid, int64_t slim_v_workers, SlimVec slim_v_tasks, SlimVec slim_v_batches, SlimRegion *slim_region);
static Slim_type_scheduler_95Schedule slim_fn_scheduler_95plan(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_requested, SlimRegion *slim_region);
static int64_t slim_fn_scheduler_95parse_95jobs_95from(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_value, SlimRegion *slim_region);
static int64_t slim_fn_scheduler_95parse_95jobs(SlimBytes slim_v_input, SlimRegion *slim_region);
static Slim_type_query_95State slim_fn_session_95state_95for_95path(SlimBytes slim_v_path, SlimRegion *slim_region);
static int64_t slim_fn_session_95print_95work(Slim_type_query_95Work slim_v_work, SlimRegion *slim_region);
static int64_t slim_fn_session_95run(SlimBytes slim_v_initial_95path, SlimBytes slim_v_updated_95path, SlimRegion *slim_region);
static int64_t slim_fn_session_95run_95recovery(SlimBytes slim_v_initial_95path, SlimBytes slim_v_invalid_95path, SlimBytes slim_v_recovered_95path, SlimRegion *slim_region);
static bool slim_fn_syntax_95token_95index_95valid(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_syntax_95token_95kind(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_syntax_95token_95start(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_syntax_95token_95end(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_syntax_95span_95equal_95chars(SlimBytes slim_v_source, int64_t slim_v_start, SlimBytes slim_v_text, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static bool slim_fn_syntax_95span_95equal(SlimBytes slim_v_source, int64_t slim_v_start, int64_t slim_v_end, SlimBytes slim_v_text, SlimRegion *slim_region);
static bool slim_fn_syntax_95spans_95equal_95chars(SlimBytes slim_v_source, int64_t slim_v_left_95start, int64_t slim_v_right_95start, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static bool slim_fn_syntax_95spans_95equal(SlimBytes slim_v_source, int64_t slim_v_left_95start, int64_t slim_v_left_95end, int64_t slim_v_right_95start, int64_t slim_v_right_95end, SlimRegion *slim_region);
static bool slim_fn_syntax_95is_95delimiter(int64_t slim_v_code, SlimRegion *slim_region);
static int64_t slim_fn_syntax_95scan_95atom(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static int64_t slim_fn_syntax_95scan_95string(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static int64_t slim_fn_syntax_95scan_95comment(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static SlimUnit slim_fn_syntax_95push_95token(SlimVec * slim_v_tokens, int64_t slim_v_kind, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region);
static SlimUnit slim_fn_syntax_95lex(SlimBytes slim_v_input, int64_t slim_v_index, SlimVec * slim_v_tokens, SlimRegion *slim_region);
static bool slim_fn_syntax_95token_95equal(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimBytes slim_v_text, SlimRegion *slim_region);
static int64_t slim_fn_syntax_95skip_95nested(SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_depth, SlimRegion *slim_region);
static int64_t slim_fn_syntax_95skip_95form(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_syntax_95item_95is(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimBytes slim_v_text, SlimRegion *slim_region);
static bool slim_fn_syntax_95binding_95is_95inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region);
static bool slim_fn_syntax_95name_95is_95inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_name, SlimRegion *slim_region);
static bool slim_fn_syntax_95first_95byte_95is_95number(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static int64_t slim_fn_syntax_95find_95function_95params(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_callee_95start, int64_t slim_v_callee_95end, SlimRegion *slim_region);
static int64_t slim_fn_syntax_95span_95count_95byte(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_code, int64_t slim_v_count, SlimRegion *slim_region);
static int64_t slim_fn_syntax_95scalar_95type_95kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_type_95index, SlimRegion *slim_region);
static Slim_type_ir_95DeclarationKind slim_fn_syntax_95declaration_95kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_form, SlimRegion *slim_region);
static SlimUnit slim_fn_syntax_95index_95declarations_95from(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_declarations, SlimRegion *slim_region);
static SlimVec slim_fn_syntax_95index_95declarations(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimRegion *slim_region);
static SlimUnit slim_fn_text_95append_95span(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_text_95append_95text_95chars(SlimBytes slim_v_text, int64_t slim_v_index, int64_t slim_v_length, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_text_95append_95text(SlimVec * slim_v_output, SlimBytes slim_v_text, SlimRegion *slim_region);
static SlimUnit slim_fn_text_95append_95nonnegative_95i64(int64_t slim_v_value, SlimVec * slim_v_output, SlimRegion *slim_region);
static SlimUnit slim_fn_text_95append_95token(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region);
static bool slim_fn_validate_95atom_95at(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_validate_95string_95token_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_validate_95string_95tokens_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region);
static bool slim_fn_validate_95form_95count_95at_95least(SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_count, int64_t slim_v_minimum, SlimRegion *slim_region);
static bool slim_fn_validate_95expression_95head_95known(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_head, SlimRegion *slim_region);
static int64_t slim_fn_validate_95expression_95minimum(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_head, SlimRegion *slim_region);
static bool slim_fn_validate_95expression_95shape_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_validate_95function_95item_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_validate_95data_95item_95valid(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_validate_95item_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region);
static bool slim_fn_validate_95item_95list_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_closing, SlimRegion *slim_region);
static int64_t slim_fn_validate_95main_95count(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_closing, int64_t slim_v_count, SlimRegion *slim_region);
static bool slim_fn_validate_95module_95shape_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimRegion *slim_region);
static bool slim_fn_validate_95executable_95shape_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimRegion *slim_region);

static Slim_type_cache_95Number slim_fn_cache_95valid_95number(int64_t slim_v_value, int64_t slim_v_next, SlimRegion *slim_region) {
(void)slim_v_value;
(void)slim_v_next;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
Slim_type_cache_95Number slim_result = {0};
{
bool slim_t_73 = {0};
slim_t_73 = true;
int64_t slim_t_77 = {0};
slim_t_77 = slim_v_value;
int64_t slim_t_81 = {0};
slim_t_81 = slim_v_next;
slim_result = (Slim_type_cache_95Number){.slim_field_valid = slim_t_73, .slim_field_value = slim_t_77, .slim_field_next = slim_t_81};
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static Slim_type_cache_95Number slim_fn_cache_95invalid_95number(int64_t slim_v_next, SlimRegion *slim_region) {
(void)slim_v_next;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
Slim_type_cache_95Number slim_result = {0};
{
bool slim_t_103 = {0};
slim_t_103 = false;
int64_t slim_t_107 = {0};
slim_t_107 = INT64_C(0);
int64_t slim_t_111 = {0};
slim_t_111 = slim_v_next;
slim_result = (Slim_type_cache_95Number){.slim_field_valid = slim_t_103, .slim_field_value = slim_t_107, .slim_field_next = slim_t_111};
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static Slim_type_cache_95Key slim_fn_cache_95invalid_95key(SlimRegion *slim_region) {
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_cache_95Key slim_result = {0};
{
bool slim_t_129 = {0};
slim_t_129 = false;
SlimBytes slim_t_133 = {0};
slim_t_133 = slim_bytes_static((const uint8_t *)"", (int64_t)(sizeof("") - 1));
slim_result = (Slim_type_cache_95Key){.slim_field_valid = slim_t_129, .slim_field_bytes = slim_t_133};
}
return slim_result;
}

static Slim_type_cache_95Key slim_fn_cache_95make_95key(bool slim_v_flag, SlimBytes slim_v_content, SlimRegion *slim_region) {
(void)slim_v_flag;
(void)slim_v_content;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_cache_95Key slim_result = {0};
{
bool slim_t_159 = {0};
slim_t_159 = slim_v_flag;
SlimBytes slim_t_163 = {0};
slim_t_163 = slim_v_content;
slim_result = (Slim_type_cache_95Key){.slim_field_valid = slim_t_159, .slim_field_bytes = slim_t_163};
}
return slim_result;
}

static SlimUnit slim_fn_cache_95append_95u64(int64_t slim_v_value, int64_t slim_v_divisor, int64_t slim_v_remaining, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_value;
(void)slim_v_divisor;
(void)slim_v_remaining;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_remaining == INT64_C(0);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_quotient = {0};
slim_v_quotient = slim_i64_div(slim_v_value, slim_v_divisor);
(void)slim_v_quotient;
{
int64_t slim_v_code = {0};
slim_v_code = slim_i64_rem(slim_v_quotient, INT64_C(256));
(void)slim_v_code;
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_i64_to_u8(slim_v_code);
(void)slim_v_byte;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_output, &slim_v_byte)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next_95divisor = {0};
slim_v_next_95divisor = slim_i64_div(slim_v_divisor, INT64_C(256));
(void)slim_v_next_95divisor;
{
int64_t slim_v_next_95remaining = {0};
slim_v_next_95remaining = slim_i64_sub(slim_v_remaining, INT64_C(1));
(void)slim_v_next_95remaining;
int64_t slim_t_280 = {0};
slim_t_280 = slim_v_value;
int64_t slim_t_281 = {0};
slim_t_281 = slim_v_next_95divisor;
int64_t slim_t_282 = {0};
slim_t_282 = slim_v_next_95remaining;
slim_v_value = slim_t_280;
slim_v_divisor = slim_t_281;
slim_v_remaining = slim_t_282;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static Slim_type_cache_95Number slim_fn_cache_95read_95bounded_95u64_95from(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_remaining, int64_t slim_v_limit, int64_t slim_v_value, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_remaining;
(void)slim_v_limit;
(void)slim_v_value;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
Slim_type_cache_95Number slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_remaining == INT64_C(0);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_cache_95valid_95number(slim_v_value, slim_v_index, slim_allocation_region);
}
else {
{
int64_t slim_v_length = {0};
slim_v_length = slim_bytes_len(slim_v_source);
(void)slim_v_length;
{
bool slim_v_available = {0};
slim_v_available = slim_v_index < slim_v_length;
(void)slim_v_available;
if (!slim_v_available) {
slim_result = slim_fn_cache_95invalid_95number(slim_v_index, slim_allocation_region);
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
int64_t slim_v_maximum_95prefix = {0};
slim_v_maximum_95prefix = slim_i64_div(slim_v_limit, INT64_C(256));
(void)slim_v_maximum_95prefix;
{
bool slim_v_below = {0};
slim_v_below = slim_v_value < slim_v_maximum_95prefix;
(void)slim_v_below;
{
bool slim_v_equal = {0};
slim_v_equal = slim_v_value == slim_v_maximum_95prefix;
(void)slim_v_equal;
{
int64_t slim_v_maximum_95byte = {0};
slim_v_maximum_95byte = slim_i64_rem(slim_v_limit, INT64_C(256));
(void)slim_v_maximum_95byte;
{
bool slim_v_byte_95fits = {0};
slim_v_byte_95fits = slim_v_code <= slim_v_maximum_95byte;
(void)slim_v_byte_95fits;
{
bool slim_v_equal_95fits = {0};
slim_v_equal_95fits = slim_v_equal && slim_v_byte_95fits;
(void)slim_v_equal_95fits;
{
bool slim_v_fits = {0};
slim_v_fits = slim_v_below || slim_v_equal_95fits;
(void)slim_v_fits;
if (!slim_v_fits) {
slim_result = slim_fn_cache_95invalid_95number(slim_v_index, slim_allocation_region);
}
else {
{
int64_t slim_v_shifted = {0};
slim_v_shifted = slim_i64_mul(slim_v_value, INT64_C(256));
(void)slim_v_shifted;
{
int64_t slim_v_next_95value = {0};
slim_v_next_95value = slim_i64_add(slim_v_shifted, slim_v_code);
(void)slim_v_next_95value;
{
int64_t slim_v_next_95index = {0};
slim_v_next_95index = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next_95index;
{
int64_t slim_v_next_95remaining = {0};
slim_v_next_95remaining = slim_i64_sub(slim_v_remaining, INT64_C(1));
(void)slim_v_next_95remaining;
SlimBytes slim_t_525 = {0};
slim_t_525 = slim_v_source;
int64_t slim_t_526 = {0};
slim_t_526 = slim_v_next_95index;
int64_t slim_t_527 = {0};
slim_t_527 = slim_v_next_95remaining;
int64_t slim_t_528 = {0};
slim_t_528 = slim_v_limit;
int64_t slim_t_529 = {0};
slim_t_529 = slim_v_next_95value;
slim_v_source = slim_t_525;
slim_v_index = slim_t_526;
slim_v_remaining = slim_t_527;
slim_v_limit = slim_t_528;
slim_v_value = slim_t_529;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static Slim_type_cache_95Number slim_fn_cache_95read_95bounded_95u64(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_limit, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_limit;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
Slim_type_cache_95Number slim_result = {0};
slim_result = slim_fn_cache_95read_95bounded_95u64_95from(slim_v_source, slim_v_index, INT64_C(8), slim_v_limit, INT64_C(0), slim_allocation_region);
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_cache_95weighted_95checksum(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_sum, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_sum;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_sum;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
int64_t slim_v_position = {0};
slim_v_position = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_position;
{
int64_t slim_v_weighted = {0};
slim_v_weighted = slim_i64_mul(slim_v_code, slim_v_position);
(void)slim_v_weighted;
{
int64_t slim_v_next_95sum = {0};
slim_v_next_95sum = slim_i64_add(slim_v_sum, slim_v_weighted);
(void)slim_v_next_95sum;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_692 = {0};
slim_t_692 = slim_v_source;
int64_t slim_t_693 = {0};
slim_t_693 = slim_v_next;
int64_t slim_t_694 = {0};
slim_t_694 = slim_v_end;
int64_t slim_t_695 = {0};
slim_t_695 = slim_v_next_95sum;
slim_v_source = slim_t_692;
slim_v_index = slim_t_693;
slim_v_end = slim_t_694;
slim_v_sum = slim_t_695;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_cache_95append_95module_95key(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95path;
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
int64_t slim_v_path_95token = {0};
slim_v_path_95token = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path_95token;
{
SlimBytes slim_v_module_95path = {0};
slim_v_module_95path = slim_fn_project_95project_95module_95path(slim_v_manifest_95path, slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_path_95token, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95path;
{
SlimVec slim_v_source_95bytes = {0};
slim_v_source_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_source_95bytes;
{
bool slim_v_read = {0};
slim_v_read = slim_read_file(slim_v_module_95path, &slim_v_source_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_read;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_bytes_freeze(slim_v_source_95bytes);
(void)slim_v_source;
if (!slim_v_read) {
slim_result = false;
}
else {
{
int64_t slim_v_source_95length = {0};
slim_v_source_95length = slim_bytes_len(slim_v_source);
(void)slim_v_source_95length;
{
int64_t slim_v_current = {0};
slim_v_current = ((*slim_v_output)).len;
(void)slim_v_current;
{
int64_t slim_v_framing_95room = {0};
slim_v_framing_95room = slim_i64_sub(INT64_C(67108864), slim_v_current);
(void)slim_v_framing_95room;
{
bool slim_v_has_95frame = {0};
slim_v_has_95frame = slim_v_framing_95room >= INT64_C(8);
(void)slim_v_has_95frame;
{
int64_t slim_v_content_95room = {0};
if (slim_v_has_95frame) {
slim_v_content_95room = slim_i64_sub(slim_v_framing_95room, INT64_C(8));
}
else {
slim_v_content_95room = INT64_C(0);
}
(void)slim_v_content_95room;
{
bool slim_v_fits = {0};
slim_v_fits = slim_v_source_95length <= slim_v_content_95room;
(void)slim_v_fits;
{
bool slim_v_valid = {0};
slim_v_valid = slim_v_has_95frame && slim_v_fits;
(void)slim_v_valid;
if (!slim_v_valid) {
slim_result = false;
}
else {
{
SlimUnit slim_v_length_95bytes = {0};
slim_v_length_95bytes = slim_fn_cache_95append_95u64(slim_v_source_95length, INT64_C(72057594037927936), INT64_C(8), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_length_95bytes;
{
SlimUnit slim_v_content = {0};
slim_v_content = slim_fn_text_95append_95text(slim_v_output, slim_v_source, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_content;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_959 = {0};
slim_t_959 = slim_v_manifest_95path;
SlimBytes slim_t_960 = {0};
slim_t_960 = slim_v_manifest_95source;
int64_t slim_t_962 = {0};
slim_t_962 = slim_v_next;
slim_v_manifest_95path = slim_t_959;
slim_v_manifest_95source = slim_t_960;
slim_v_cursor = slim_t_962;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static Slim_type_cache_95Key slim_fn_cache_95project_95key(SlimBytes slim_v_manifest_95path, SlimRegion *slim_region) {
(void)slim_v_manifest_95path;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_cache_95Key slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_manifest_95bytes = {0};
slim_v_manifest_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_manifest_95bytes;
{
bool slim_v_read = {0};
slim_v_read = slim_read_file(slim_v_manifest_95path, &slim_v_manifest_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_read;
{
SlimBytes slim_v_manifest_95source = {0};
slim_v_manifest_95source = slim_bytes_freeze(slim_v_manifest_95bytes);
(void)slim_v_manifest_95source;
if (!slim_v_read) {
slim_result = slim_fn_cache_95invalid_95key(slim_allocation_region);
}
else {
{
int64_t slim_v_manifest_95length = {0};
slim_v_manifest_95length = slim_bytes_len(slim_v_manifest_95source);
(void)slim_v_manifest_95length;
{
int64_t slim_v_maximum_95manifest = {0};
slim_v_maximum_95manifest = INT64_C(67108856);
(void)slim_v_maximum_95manifest;
{
bool slim_v_manifest_95fits = {0};
slim_v_manifest_95fits = slim_v_manifest_95length <= slim_v_maximum_95manifest;
(void)slim_v_manifest_95fits;
if (!slim_v_manifest_95fits) {
slim_result = slim_fn_cache_95invalid_95key(slim_allocation_region);
}
else {
{
SlimVec slim_v_manifest_95tokens = {0};
slim_v_manifest_95tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_manifest_95tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_manifest_95source, INT64_C(0), &slim_v_manifest_95tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_length_95bytes = {0};
slim_v_length_95bytes = slim_fn_cache_95append_95u64(slim_v_manifest_95length, INT64_C(72057594037927936), INT64_C(8), &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_length_95bytes;
{
SlimUnit slim_v_manifest = {0};
slim_v_manifest = slim_fn_text_95append_95text(&slim_v_output, slim_v_manifest_95source, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_manifest;
{
int64_t slim_v_entry_95form = {0};
slim_v_entry_95form = INT64_C(3);
(void)slim_v_entry_95form;
{
int64_t slim_v_modules = {0};
slim_v_modules = slim_fn_syntax_95skip_95form(&slim_v_manifest_95tokens, slim_v_entry_95form, slim_allocation_region);
(void)slim_v_modules;
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_cache_95append_95module_95key(slim_v_manifest_95path, slim_v_manifest_95source, &slim_v_manifest_95tokens, slim_v_modules, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_valid;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
slim_result = slim_fn_cache_95make_95key(slim_v_valid, slim_v_bytes, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimBytes slim_fn_cache_95encode(SlimBytes slim_v_key, SlimBytes slim_v_artifact, SlimRegion *slim_region) {
(void)slim_v_key;
(void)slim_v_artifact;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_key_95length = {0};
slim_v_key_95length = slim_bytes_len(slim_v_key);
(void)slim_v_key_95length;
{
int64_t slim_v_artifact_95length = {0};
slim_v_artifact_95length = slim_bytes_len(slim_v_artifact);
(void)slim_v_artifact_95length;
{
SlimVec slim_v_prefix = {0};
slim_v_prefix = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_prefix;
{
SlimUnit slim_v_magic = {0};
slim_v_magic = slim_fn_text_95append_95text(&slim_v_prefix, slim_bytes_static((const uint8_t *)"SLIMCACHE\x00", (int64_t)(sizeof("SLIMCACHE\x00") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_magic;
{
uint8_t slim_v_schema = {0};
slim_v_schema = slim_i64_to_u8(INT64_C(1));
(void)slim_v_schema;
{
SlimUnit slim_v_schema_95byte = {0};
if (!slim_vec_push(&slim_v_prefix, &slim_v_schema)) goto slim_allocation_failed; slim_v_schema_95byte = (SlimUnit){0};
(void)slim_v_schema_95byte;
{
SlimUnit slim_v_key_95length_95bytes = {0};
slim_v_key_95length_95bytes = slim_fn_cache_95append_95u64(slim_v_key_95length, INT64_C(72057594037927936), INT64_C(8), &slim_v_prefix, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_key_95length_95bytes;
{
SlimUnit slim_v_artifact_95length_95bytes = {0};
slim_v_artifact_95length_95bytes = slim_fn_cache_95append_95u64(slim_v_artifact_95length, INT64_C(72057594037927936), INT64_C(8), &slim_v_prefix, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_artifact_95length_95bytes;
{
SlimUnit slim_v_key_95bytes = {0};
slim_v_key_95bytes = slim_fn_text_95append_95text(&slim_v_prefix, slim_v_key, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_key_95bytes;
{
SlimUnit slim_v_artifact_95bytes = {0};
slim_v_artifact_95bytes = slim_fn_text_95append_95text(&slim_v_prefix, slim_v_artifact, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_artifact_95bytes;
{
SlimBytes slim_v_prefix_95bytes = {0};
slim_v_prefix_95bytes = slim_bytes_freeze(slim_v_prefix);
(void)slim_v_prefix_95bytes;
{
int64_t slim_v_prefix_95length = {0};
slim_v_prefix_95length = slim_bytes_len(slim_v_prefix_95bytes);
(void)slim_v_prefix_95length;
{
int64_t slim_v_checksum = {0};
slim_v_checksum = slim_fn_cache_95weighted_95checksum(slim_v_prefix_95bytes, INT64_C(0), slim_v_prefix_95length, INT64_C(0), slim_allocation_region);
(void)slim_v_checksum;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_copied = {0};
slim_v_copied = slim_fn_text_95append_95text(&slim_v_output, slim_v_prefix_95bytes, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_copied;
{
SlimUnit slim_v_checksum_95bytes = {0};
slim_v_checksum_95bytes = slim_fn_cache_95append_95u64(slim_v_checksum, INT64_C(72057594037927936), INT64_C(8), &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checksum_95bytes;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static Slim_type_cache_95Probe slim_fn_cache_95invalid_95probe(SlimRegion *slim_region) {
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
Slim_type_cache_95Probe slim_result = {0};
{
bool slim_t_1419 = {0};
slim_t_1419 = false;
int64_t slim_t_1423 = {0};
slim_t_1423 = INT64_C(0);
int64_t slim_t_1427 = {0};
slim_t_1427 = INT64_C(0);
slim_result = (Slim_type_cache_95Probe){.slim_field_valid = slim_t_1419, .slim_field_artifact_95start = slim_t_1423, .slim_field_artifact_95end = slim_t_1427};
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static Slim_type_cache_95Probe slim_fn_cache_95probe(SlimBytes slim_v_cache, SlimBytes slim_v_key, SlimRegion *slim_region) {
(void)slim_v_cache;
(void)slim_v_key;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
Slim_type_cache_95Probe slim_result = {0};
{
int64_t slim_v_length = {0};
slim_v_length = slim_bytes_len(slim_v_cache);
(void)slim_v_length;
{
bool slim_v_minimum = {0};
slim_v_minimum = slim_v_length >= INT64_C(35);
(void)slim_v_minimum;
if (!slim_v_minimum) {
slim_result = slim_fn_cache_95invalid_95probe(slim_allocation_region);
}
else {
{
SlimBytes slim_v_magic = {0};
slim_v_magic = slim_bytes_static((const uint8_t *)"SLIMCACHE\x00", (int64_t)(sizeof("SLIMCACHE\x00") - 1));
(void)slim_v_magic;
{
bool slim_v_magic_95equal = {0};
slim_v_magic_95equal = slim_fn_project_95cross_95span_95chars_95equal(slim_v_cache, INT64_C(0), slim_v_magic, INT64_C(0), INT64_C(0), INT64_C(10), slim_allocation_region);
(void)slim_v_magic_95equal;
{
uint8_t slim_v_schema_95byte = {0};
slim_v_schema_95byte = slim_bytes_get(slim_v_cache, INT64_C(10));
(void)slim_v_schema_95byte;
{
int64_t slim_v_schema_95code = {0};
slim_v_schema_95code = (int64_t)slim_v_schema_95byte;
(void)slim_v_schema_95code;
{
bool slim_v_schema_95equal = {0};
slim_v_schema_95equal = slim_v_schema_95code == INT64_C(1);
(void)slim_v_schema_95equal;
{
bool slim_v_header_95valid = {0};
slim_v_header_95valid = slim_v_magic_95equal && slim_v_schema_95equal;
(void)slim_v_header_95valid;
{
Slim_type_cache_95Number slim_v_key_95number = {0};
slim_v_key_95number = slim_fn_cache_95read_95bounded_95u64(slim_v_cache, INT64_C(11), INT64_C(67108864), slim_allocation_region);
(void)slim_v_key_95number;
{
bool slim_v_key_95valid = {0};
slim_v_key_95valid = slim_v_key_95number.slim_field_valid;
(void)slim_v_key_95valid;
{
int64_t slim_v_key_95length = {0};
slim_v_key_95length = slim_v_key_95number.slim_field_value;
(void)slim_v_key_95length;
{
Slim_type_cache_95Number slim_v_artifact_95number = {0};
slim_v_artifact_95number = slim_fn_cache_95read_95bounded_95u64(slim_v_cache, INT64_C(19), INT64_C(67108864), slim_allocation_region);
(void)slim_v_artifact_95number;
{
bool slim_v_artifact_95valid = {0};
slim_v_artifact_95valid = slim_v_artifact_95number.slim_field_valid;
(void)slim_v_artifact_95valid;
{
bool slim_v_lengths_95valid = {0};
slim_v_lengths_95valid = slim_v_key_95valid && slim_v_artifact_95valid;
(void)slim_v_lengths_95valid;
{
int64_t slim_v_artifact_95length = {0};
slim_v_artifact_95length = slim_v_artifact_95number.slim_field_value;
(void)slim_v_artifact_95length;
{
int64_t slim_v_artifact_95start = {0};
slim_v_artifact_95start = slim_i64_add(INT64_C(27), slim_v_key_95length);
(void)slim_v_artifact_95start;
{
int64_t slim_v_artifact_95end = {0};
slim_v_artifact_95end = slim_i64_add(slim_v_artifact_95start, slim_v_artifact_95length);
(void)slim_v_artifact_95end;
{
int64_t slim_v_expected_95length = {0};
slim_v_expected_95length = slim_i64_add(slim_v_artifact_95end, INT64_C(8));
(void)slim_v_expected_95length;
{
bool slim_v_exact_95length = {0};
slim_v_exact_95length = slim_v_length == slim_v_expected_95length;
(void)slim_v_exact_95length;
{
int64_t slim_v_expected_95key_95length = {0};
slim_v_expected_95key_95length = slim_bytes_len(slim_v_key);
(void)slim_v_expected_95key_95length;
{
bool slim_v_same_95key_95length = {0};
slim_v_same_95key_95length = slim_v_key_95length == slim_v_expected_95key_95length;
(void)slim_v_same_95key_95length;
{
bool slim_v_key_95equal = {0};
if (slim_v_same_95key_95length) {
slim_v_key_95equal = slim_fn_project_95cross_95span_95chars_95equal(slim_v_cache, INT64_C(27), slim_v_key, INT64_C(0), INT64_C(0), slim_v_key_95length, slim_allocation_region);
}
else {
slim_v_key_95equal = false;
}
(void)slim_v_key_95equal;
{
int64_t slim_v_checksum = {0};
if (slim_v_exact_95length) {
slim_v_checksum = slim_fn_cache_95weighted_95checksum(slim_v_cache, INT64_C(0), slim_v_artifact_95end, INT64_C(0), slim_allocation_region);
}
else {
slim_v_checksum = INT64_C(0);
}
(void)slim_v_checksum;
{
Slim_type_cache_95Number slim_v_checksum_95number = {0};
if (slim_v_exact_95length) {
slim_v_checksum_95number = slim_fn_cache_95read_95bounded_95u64(slim_v_cache, slim_v_artifact_95end, INT64_C(2296836751160866950), slim_allocation_region);
}
else {
slim_v_checksum_95number = slim_fn_cache_95invalid_95number(INT64_C(0), slim_allocation_region);
}
(void)slim_v_checksum_95number;
{
bool slim_v_checksum_95valid = {0};
slim_v_checksum_95valid = slim_v_checksum_95number.slim_field_valid;
(void)slim_v_checksum_95valid;
{
int64_t slim_v_expected_95checksum = {0};
slim_v_expected_95checksum = slim_v_checksum_95number.slim_field_value;
(void)slim_v_expected_95checksum;
{
bool slim_v_checksum_95equal = {0};
slim_v_checksum_95equal = slim_v_checksum == slim_v_expected_95checksum;
(void)slim_v_checksum_95equal;
{
bool slim_v_first = {0};
slim_v_first = slim_v_header_95valid && slim_v_lengths_95valid;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_exact_95length && slim_v_key_95equal;
(void)slim_v_second;
{
bool slim_v_third = {0};
slim_v_third = slim_v_checksum_95valid && slim_v_checksum_95equal;
(void)slim_v_third;
{
bool slim_v_prefix_95valid = {0};
slim_v_prefix_95valid = slim_v_first && slim_v_second;
(void)slim_v_prefix_95valid;
{
bool slim_v_valid = {0};
slim_v_valid = slim_v_prefix_95valid && slim_v_third;
(void)slim_v_valid;
{
bool slim_t_1822 = {0};
slim_t_1822 = slim_v_valid;
int64_t slim_t_1826 = {0};
slim_t_1826 = slim_v_artifact_95start;
int64_t slim_t_1830 = {0};
slim_t_1830 = slim_v_artifact_95end;
slim_result = (Slim_type_cache_95Probe){.slim_field_valid = slim_t_1822, .slim_field_artifact_95start = slim_t_1826, .slim_field_artifact_95end = slim_t_1830};
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_cache_95emit_95hit(SlimBytes slim_v_cache, Slim_type_cache_95Probe slim_v_cache_95probe, SlimRegion *slim_region) {
(void)slim_v_cache;
(void)slim_v_cache_95probe;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_marker = {0};
slim_v_marker = slim_fn_text_95append_95text(&slim_v_output, slim_bytes_static((const uint8_t *)"H", (int64_t)(sizeof("H") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_marker;
{
int64_t slim_v_start = {0};
slim_v_start = slim_v_cache_95probe.slim_field_artifact_95start;
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_v_cache_95probe.slim_field_artifact_95end;
(void)slim_v_end;
{
SlimUnit slim_v_artifact = {0};
slim_v_artifact = slim_fn_text_95append_95span(slim_v_cache, slim_v_start, slim_v_end, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_artifact;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_cache_95emit_95miss(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_key, SlimRegion *slim_region) {
(void)slim_v_manifest_95path;
(void)slim_v_key;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_project_95check_95project_95path(slim_v_manifest_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_checked > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_checked;
}
else {
{
SlimBytes slim_v_artifact = {0};
slim_v_artifact = slim_fn_project_95generate_95project_95path(slim_v_manifest_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_artifact;
{
SlimBytes slim_v_entry = {0};
slim_v_entry = slim_fn_cache_95encode(slim_v_key, slim_v_artifact, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_entry;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_marker = {0};
slim_v_marker = slim_fn_text_95append_95text(&slim_v_output, slim_bytes_static((const uint8_t *)"M", (int64_t)(sizeof("M") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_marker;
{
SlimUnit slim_v_cache_95bytes = {0};
slim_v_cache_95bytes = slim_fn_text_95append_95text(&slim_v_output, slim_v_entry, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cache_95bytes;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_cache_95run(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_cache_95path, SlimRegion *slim_region) {
(void)slim_v_manifest_95path;
(void)slim_v_cache_95path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
Slim_type_cache_95Key slim_v_key_95result = {0};
slim_v_key_95result = slim_fn_cache_95project_95key(slim_v_manifest_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_key_95result;
{
bool slim_v_key_95valid = {0};
slim_v_key_95valid = slim_v_key_95result.slim_field_valid;
(void)slim_v_key_95valid;
if (!slim_v_key_95valid) {
slim_result = slim_fn_project_95check_95project_95path(slim_v_manifest_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimBytes slim_v_key = {0};
slim_v_key = slim_v_key_95result.slim_field_bytes;
(void)slim_v_key;
{
SlimVec slim_v_cache_95bytes = {0};
slim_v_cache_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_cache_95bytes;
{
bool slim_v_cache_95read = {0};
slim_v_cache_95read = slim_read_file(slim_v_cache_95path, &slim_v_cache_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cache_95read;
{
SlimBytes slim_v_cache = {0};
slim_v_cache = slim_bytes_freeze(slim_v_cache_95bytes);
(void)slim_v_cache;
{
Slim_type_cache_95Probe slim_v_cache_95probe = {0};
if (slim_v_cache_95read) {
slim_v_cache_95probe = slim_fn_cache_95probe(slim_v_cache, slim_v_key, slim_allocation_region);
}
else {
slim_v_cache_95probe = slim_fn_cache_95invalid_95probe(slim_allocation_region);
}
(void)slim_v_cache_95probe;
{
bool slim_v_hit = {0};
slim_v_hit = slim_v_cache_95probe.slim_field_valid;
(void)slim_v_hit;
if (slim_v_hit) {
slim_result = slim_fn_cache_95emit_95hit(slim_v_cache, slim_v_cache_95probe, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_cache_95emit_95miss(slim_v_manifest_95path, slim_v_key, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_check_95parameter_95has_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_parameter;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_parameter_95name = {0};
slim_v_parameter_95name = slim_fn_check_95recur_95parameter_95name(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_parameter_95name;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_parameter_95name, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_parameter_95name, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_2399 = {0};
slim_t_2399 = slim_v_source;
int64_t slim_t_2401 = {0};
slim_t_2401 = slim_v_next;
int64_t slim_t_2402 = {0};
slim_t_2402 = slim_v_name_95start;
int64_t slim_t_2403 = {0};
slim_t_2403 = slim_v_name_95end;
slim_v_source = slim_t_2399;
slim_v_parameter = slim_t_2401;
slim_v_name_95start = slim_t_2402;
slim_v_name_95end = slim_t_2403;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95prior_95let_95has_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_let_95form = {0};
if (slim_v_form) {
slim_v_let_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
}
else {
slim_v_let_95form = false;
}
(void)slim_v_let_95form;
{
int64_t slim_v_candidate = {0};
slim_v_candidate = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_candidate;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_candidate, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_candidate, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
if (slim_v_let_95form) {
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
}
else {
slim_v_same = false;
}
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_2602 = {0};
slim_t_2602 = slim_v_source;
int64_t slim_t_2604 = {0};
slim_t_2604 = slim_v_next;
int64_t slim_t_2605 = {0};
slim_t_2605 = slim_v_end;
int64_t slim_t_2606 = {0};
slim_t_2606 = slim_v_name_95start;
int64_t slim_t_2607 = {0};
slim_t_2607 = slim_v_name_95end;
slim_v_source = slim_t_2602;
slim_v_cursor = slim_t_2604;
slim_v_end = slim_t_2605;
slim_v_name_95start = slim_t_2606;
slim_v_name_95end = slim_t_2607;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95match_95name_95is_95known(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body_95start, int64_t slim_v_name, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body_95start;
(void)slim_v_name;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_actual = {0};
slim_v_actual = slim_fn_check_95atom_95type_95kind(slim_v_source, slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_actual;
{
bool slim_v_scalar = {0};
slim_v_scalar = slim_v_actual >= INT64_C(0);
(void)slim_v_scalar;
if (slim_v_scalar) {
slim_result = true;
}
else {
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
{
int64_t slim_v_first_95parameter = {0};
slim_v_first_95parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_95parameter;
{
bool slim_v_parameter = {0};
slim_v_parameter = slim_fn_check_95parameter_95has_95name(slim_v_source, slim_v_tokens, slim_v_first_95parameter, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_parameter;
if (slim_v_parameter) {
slim_result = true;
}
else {
slim_result = slim_fn_check_95prior_95let_95has_95name(slim_v_source, slim_v_tokens, slim_v_body_95start, slim_v_name, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95report_95boolean_95match_95arms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body_95start, int64_t slim_v_expected, int64_t slim_v_cursor, bool slim_v_seen_95true, bool slim_v_seen_95false, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body_95start;
(void)slim_v_expected;
(void)slim_v_cursor;
(void)slim_v_seen_95true;
(void)slim_v_seen_95false;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_pattern = {0};
slim_v_pattern = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_pattern;
{
bool slim_v_true_95pattern = {0};
slim_v_true_95pattern = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_pattern, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_true_95pattern;
{
bool slim_v_false_95pattern = {0};
slim_v_false_95pattern = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_pattern, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_false_95pattern;
{
bool slim_v_duplicate = {0};
if (slim_v_true_95pattern) {
slim_v_duplicate = slim_v_seen_95true;
}
else {
if (slim_v_false_95pattern) {
slim_v_duplicate = slim_v_seen_95false;
}
else {
slim_v_duplicate = false;
}
}
(void)slim_v_duplicate;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_arm_95start = {0};
slim_v_arm_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_arm_95start;
{
int64_t slim_v_arm_95end = {0};
slim_v_arm_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_arm_95end;
{
int64_t slim_v_duplicate_95report = {0};
if (slim_v_duplicate) {
slim_v_duplicate_95report = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0335", (int64_t)(sizeof("E0335") - 1)), slim_v_arm_95start, slim_v_arm_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_duplicate_95report = INT64_C(0);
}
(void)slim_v_duplicate_95report;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_first;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_check_95last_95form_95in_95list(slim_v_tokens, slim_v_first, slim_v_first, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_95kind = {0};
slim_v_body_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_body_95kind == INT64_C(2);
(void)slim_v_named;
{
bool slim_v_known = {0};
if (slim_v_named) {
slim_v_known = slim_fn_check_95match_95name_95is_95known(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body_95start, slim_v_body, slim_allocation_region);
}
else {
slim_v_known = true;
}
(void)slim_v_known;
{
bool slim_v_unknown = {0};
slim_v_unknown = !slim_v_known;
(void)slim_v_unknown;
{
int64_t slim_v_body_95token_95start = {0};
slim_v_body_95token_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95token_95start;
{
int64_t slim_v_body_95token_95end = {0};
slim_v_body_95token_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95token_95end;
{
int64_t slim_v_unknown_95report = {0};
if (slim_v_unknown) {
slim_v_unknown_95report = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0314", (int64_t)(sizeof("E0314") - 1)), slim_v_body_95token_95start, slim_v_body_95token_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_unknown_95report = INT64_C(0);
}
(void)slim_v_unknown_95report;
{
int64_t slim_v_actual = {0};
slim_v_actual = slim_fn_check_95atom_95type_95kind(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_actual;
{
bool slim_v_expected_95known = {0};
slim_v_expected_95known = slim_v_expected >= INT64_C(0);
(void)slim_v_expected_95known;
{
bool slim_v_actual_95known = {0};
slim_v_actual_95known = slim_v_actual >= INT64_C(0);
(void)slim_v_actual_95known;
{
bool slim_v_both_95known = {0};
slim_v_both_95known = slim_v_expected_95known && slim_v_actual_95known;
(void)slim_v_both_95known;
{
bool slim_v_same_95type = {0};
slim_v_same_95type = slim_v_expected == slim_v_actual;
(void)slim_v_same_95type;
{
bool slim_v_different = {0};
slim_v_different = !slim_v_same_95type;
(void)slim_v_different;
{
bool slim_v_mismatch = {0};
slim_v_mismatch = slim_v_both_95known && slim_v_different;
(void)slim_v_mismatch;
{
int64_t slim_v_mismatch_95report = {0};
if (slim_v_mismatch) {
slim_v_mismatch_95report = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0344", (int64_t)(sizeof("E0344") - 1)), slim_v_body_95token_95start, slim_v_body_95token_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_mismatch_95report = INT64_C(0);
}
(void)slim_v_mismatch_95report;
{
int64_t slim_v_first_95total = {0};
slim_v_first_95total = slim_i64_add(slim_v_duplicate_95report, slim_v_unknown_95report);
(void)slim_v_first_95total;
{
int64_t slim_v_reported = {0};
slim_v_reported = slim_i64_add(slim_v_first_95total, slim_v_mismatch_95report);
(void)slim_v_reported;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
{
bool slim_v_next_95seen_95true = {0};
slim_v_next_95seen_95true = slim_v_seen_95true || slim_v_true_95pattern;
(void)slim_v_next_95seen_95true;
{
bool slim_v_next_95seen_95false = {0};
slim_v_next_95seen_95false = slim_v_seen_95false || slim_v_false_95pattern;
(void)slim_v_next_95seen_95false;
SlimBytes slim_t_3225 = {0};
slim_t_3225 = slim_v_source;
int64_t slim_t_3227 = {0};
slim_t_3227 = slim_v_params;
int64_t slim_t_3228 = {0};
slim_t_3228 = slim_v_body_95start;
int64_t slim_t_3229 = {0};
slim_t_3229 = slim_v_expected;
int64_t slim_t_3230 = {0};
slim_t_3230 = slim_v_next;
bool slim_t_3231 = {0};
slim_t_3231 = slim_v_next_95seen_95true;
bool slim_t_3232 = {0};
slim_t_3232 = slim_v_next_95seen_95false;
int64_t slim_t_3233 = {0};
slim_t_3233 = slim_v_total;
slim_v_source = slim_t_3225;
slim_v_params = slim_t_3227;
slim_v_body_95start = slim_t_3228;
slim_v_expected = slim_t_3229;
slim_v_cursor = slim_t_3230;
slim_v_seen_95true = slim_t_3231;
slim_v_seen_95false = slim_t_3232;
slim_v_count = slim_t_3233;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95report_95boolean_95match_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body_95start, int64_t slim_v_expected, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body_95start;
(void)slim_v_expected;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_match_95form = {0};
if (slim_v_form) {
slim_v_match_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"match", (int64_t)(sizeof("match") - 1)), slim_allocation_region);
}
else {
slim_v_match_95form = false;
}
(void)slim_v_match_95form;
{
bool slim_v_incomplete = {0};
if (slim_v_match_95form) {
slim_v_incomplete = slim_fn_check_95boolean_95match_95incomplete(slim_v_source, slim_v_tokens, slim_v_cursor, slim_allocation_region);
}
else {
slim_v_incomplete = false;
}
(void)slim_v_incomplete;
{
int64_t slim_v_reported = {0};
if (slim_v_incomplete) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_value;
{
int64_t slim_v_arms = {0};
slim_v_arms = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_arms;
slim_v_reported = slim_fn_check_95report_95boolean_95match_95arms(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body_95start, slim_v_expected, slim_v_arms, false, false, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_3485 = {0};
slim_t_3485 = slim_v_source;
int64_t slim_t_3487 = {0};
slim_t_3487 = slim_v_params;
int64_t slim_t_3488 = {0};
slim_t_3488 = slim_v_body_95start;
int64_t slim_t_3489 = {0};
slim_t_3489 = slim_v_expected;
int64_t slim_t_3490 = {0};
slim_t_3490 = slim_v_next;
int64_t slim_t_3491 = {0};
slim_t_3491 = slim_v_end;
int64_t slim_t_3492 = {0};
slim_t_3492 = slim_v_total;
slim_v_source = slim_t_3485;
slim_v_params = slim_t_3487;
slim_v_body_95start = slim_t_3488;
slim_v_expected = slim_t_3489;
slim_v_cursor = slim_t_3490;
slim_v_end = slim_t_3491;
slim_v_count = slim_t_3492;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95report_95boolean_95match_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_declaration_95count = {0};
slim_v_declaration_95count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_95count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_95count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
Slim_type_ir_95Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_95Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
int64_t slim_v_reported = {0};
if (slim_v_function_95item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_expected = {0};
slim_v_expected = slim_fn_syntax_95scalar_95type_95kind(slim_v_source, slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_expected;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_95end = {0};
slim_v_body_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95end;
slim_v_reported = slim_fn_check_95report_95boolean_95match_95span(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_expected, slim_v_body, slim_v_body_95end, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_3721 = {0};
slim_t_3721 = slim_v_source;
int64_t slim_t_3724 = {0};
slim_t_3724 = slim_v_next;
int64_t slim_t_3725 = {0};
slim_t_3725 = slim_v_total;
slim_v_source = slim_t_3721;
slim_v_index = slim_t_3724;
slim_v_count = slim_t_3725;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95check_95path_95all(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_input_95bytes = {0};
slim_v_input_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_input_95bytes;
{
bool slim_v_input_95read = {0};
slim_v_input_95read = slim_read_file(slim_v_path, &slim_v_input_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_input_95read;
if (!slim_v_input_95read) {
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_println(slim_bytes_static((const uint8_t *)"E0409@0:0", (int64_t)(sizeof("E0409@0:0") - 1)));
(void)slim_v_shown;
slim_result = INT64_C(1);
}
}
else {
{
SlimBytes slim_v_input = {0};
slim_v_input = slim_bytes_freeze(slim_v_input_95bytes);
(void)slim_v_input;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_input, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_validate_95executable_95shape_95valid(slim_v_input, &slim_v_tokens, slim_allocation_region);
(void)slim_v_valid;
if (!slim_v_valid) {
{
SlimVec slim_v_declarations = {0};
slim_v_declarations = slim_vec_new(sizeof(Slim_type_ir_95Declaration), slim_allocation_region);
(void)slim_v_declarations;
slim_result = slim_fn_check_95check_95path(slim_v_input, &slim_v_tokens, &slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
SlimVec slim_v_declarations = {0};
slim_v_declarations = slim_fn_syntax_95index_95declarations(slim_v_input, &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_declarations;
{
Slim_type_memory_95Plan slim_v_plan = {0};
slim_v_plan = slim_fn_memory_95analyze(slim_v_input, &slim_v_tokens, &slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_plan;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_check_95check_95path_95moves(slim_v_input, &slim_v_tokens, &slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_prior_95error = {0};
slim_v_prior_95error = slim_v_checked > INT64_C(0);
(void)slim_v_prior_95error;
if (!slim_v_prior_95error) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_recovered = {0};
slim_v_recovered = slim_fn_check_95report_95boolean_95match_95items(slim_v_input, &slim_v_tokens, &slim_v_declarations, INT64_C(0), INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_recovered;
slim_result = slim_v_checked;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_check_95recur_95parameter_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_parameter;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_95parameter_95is_95inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
if (slim_v_borrowed) {
slim_result = slim_i64_add(slim_v_parameter, INT64_C(2));
}
else {
slim_result = slim_i64_add(slim_v_parameter, INT64_C(1));
}
}
return slim_result;
}

static int64_t slim_fn_check_95report_95recur_95arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_argument, int64_t slim_v_parameter, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_argument;
(void)slim_v_parameter;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_95parameter_95is_95inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_parameter_95name = {0};
slim_v_parameter_95name = slim_fn_check_95recur_95parameter_95name(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_parameter_95name;
{
int64_t slim_v_parameter_95start = {0};
slim_v_parameter_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_parameter_95name, slim_allocation_region);
(void)slim_v_parameter_95start;
{
int64_t slim_v_parameter_95end = {0};
slim_v_parameter_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_parameter_95name, slim_allocation_region);
(void)slim_v_parameter_95end;
{
int64_t slim_v_argument_95start = {0};
slim_v_argument_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_95start;
{
int64_t slim_v_argument_95end = {0};
slim_v_argument_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_parameter_95start, slim_v_parameter_95end, slim_v_argument_95start, slim_v_argument_95end, slim_allocation_region);
(void)slim_v_same;
{
bool slim_v_different = {0};
slim_v_different = !slim_v_same;
(void)slim_v_different;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_borrowed && slim_v_different;
(void)slim_v_invalid;
{
int64_t slim_v_reported = {0};
if (slim_v_invalid) {
slim_v_reported = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0350", (int64_t)(sizeof("E0350") - 1)), slim_v_argument_95start, slim_v_argument_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next_95argument = {0};
slim_v_next_95argument = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_95argument;
{
int64_t slim_v_next_95parameter = {0};
slim_v_next_95parameter = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_next_95parameter;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_4222 = {0};
slim_t_4222 = slim_v_source;
int64_t slim_t_4224 = {0};
slim_t_4224 = slim_v_next_95argument;
int64_t slim_t_4225 = {0};
slim_t_4225 = slim_v_next_95parameter;
int64_t slim_t_4226 = {0};
slim_t_4226 = slim_v_total;
slim_v_source = slim_t_4222;
slim_v_argument = slim_t_4224;
slim_v_parameter = slim_t_4225;
slim_v_count = slim_t_4226;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95report_95recur_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_recur_95form = {0};
if (slim_v_form) {
slim_v_recur_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"recur", (int64_t)(sizeof("recur") - 1)), slim_allocation_region);
}
else {
slim_v_recur_95form = false;
}
(void)slim_v_recur_95form;
{
int64_t slim_v_reported = {0};
if (slim_v_recur_95form) {
{
int64_t slim_v_argument = {0};
slim_v_argument = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_argument;
{
int64_t slim_v_parameter = {0};
slim_v_parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_parameter;
slim_v_reported = slim_fn_check_95report_95recur_95arguments(slim_v_source, slim_v_tokens, slim_v_argument, slim_v_parameter, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_4426 = {0};
slim_t_4426 = slim_v_source;
int64_t slim_t_4428 = {0};
slim_t_4428 = slim_v_params;
int64_t slim_t_4429 = {0};
slim_t_4429 = slim_v_next;
int64_t slim_t_4430 = {0};
slim_t_4430 = slim_v_end;
int64_t slim_t_4431 = {0};
slim_t_4431 = slim_v_total;
slim_v_source = slim_t_4426;
slim_v_params = slim_t_4428;
slim_v_cursor = slim_t_4429;
slim_v_end = slim_t_4430;
slim_v_count = slim_t_4431;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95report_95recur_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_declaration_95count = {0};
slim_v_declaration_95count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_95count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_95count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
Slim_type_ir_95Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_95Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
int64_t slim_v_reported = {0};
if (slim_v_function_95item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_95end = {0};
slim_v_body_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95end;
slim_v_reported = slim_fn_check_95report_95recur_95span(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_95end, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_4645 = {0};
slim_t_4645 = slim_v_source;
int64_t slim_t_4648 = {0};
slim_t_4648 = slim_v_next;
int64_t slim_t_4649 = {0};
slim_t_4649 = slim_v_total;
slim_v_source = slim_t_4645;
slim_v_index = slim_t_4648;
slim_v_count = slim_t_4649;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95check_95path_95recur(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_check_95check_95path_95inout(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_prior_95error = {0};
slim_v_prior_95error = slim_v_checked > INT64_C(0);
(void)slim_v_prior_95error;
if (slim_v_prior_95error) {
slim_result = slim_v_checked;
}
else {
{
int64_t slim_v_errors = {0};
slim_v_errors = slim_fn_check_95report_95recur_95items(slim_v_input, slim_v_tokens, slim_v_declarations, INT64_C(0), INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_errors;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_errors > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = INT64_C(1);
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_check_95ownership_95type_95is_95copy(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_type_95index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_type_95index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_fn_memory_95type_95storage_95kind(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_storage;
slim_result = slim_v_storage < INT64_C(2);
}
return slim_result;
}

static int64_t slim_fn_check_95ownership_95parameter_95type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_parameter;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_95parameter_95is_95inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
if (slim_v_borrowed) {
slim_result = slim_i64_add(slim_v_parameter, INT64_C(3));
}
else {
slim_result = slim_i64_add(slim_v_parameter, INT64_C(2));
}
}
return slim_result;
}

static bool slim_fn_check_95moved_95has(SlimBytes slim_v_source, SlimVec * slim_v_moved, int64_t slim_v_index, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_moved;
(void)slim_v_index;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_moved)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
Slim_type_syntax_95Token slim_v_candidate = {0};
slim_v_candidate = ((Slim_type_syntax_95Token *)((*slim_v_moved)).data)[slim_vec_check_index(&((*slim_v_moved)), slim_v_index)];
(void)slim_v_candidate;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_v_candidate.slim_field_start;
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_v_candidate.slim_field_end;
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_4996 = {0};
slim_t_4996 = slim_v_source;
int64_t slim_t_4998 = {0};
slim_t_4998 = slim_v_next;
int64_t slim_t_4999 = {0};
slim_t_4999 = slim_v_name_95start;
int64_t slim_t_5000 = {0};
slim_t_5000 = slim_v_name_95end;
slim_v_source = slim_t_4996;
slim_v_index = slim_t_4998;
slim_v_name_95start = slim_t_4999;
slim_v_name_95end = slim_t_5000;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95report_95owned_95arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_current_95params, int64_t slim_v_argument, int64_t slim_v_parameter, SlimVec * slim_v_moved, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_current_95params;
(void)slim_v_argument;
(void)slim_v_parameter;
(void)slim_v_moved;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_95parameter_95is_95inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_type_95index = {0};
slim_v_type_95index = slim_fn_check_95ownership_95parameter_95type(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_type_95index;
{
bool slim_v_copy = {0};
slim_v_copy = slim_fn_check_95ownership_95type_95is_95copy(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_copy;
{
int64_t slim_v_argument_95kind = {0};
slim_v_argument_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_95kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_argument_95kind == INT64_C(2);
(void)slim_v_named;
{
bool slim_v_owned = {0};
slim_v_owned = !slim_v_borrowed;
(void)slim_v_owned;
{
bool slim_v_noncopy = {0};
slim_v_noncopy = !slim_v_copy;
(void)slim_v_noncopy;
{
bool slim_v_first = {0};
slim_v_first = slim_v_owned && slim_v_noncopy;
(void)slim_v_first;
{
bool slim_v_moving_95name = {0};
slim_v_moving_95name = slim_v_first && slim_v_named;
(void)slim_v_moving_95name;
{
int64_t slim_v_reported = {0};
if (slim_v_moving_95name) {
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_name_95end;
{
bool slim_v_already_95moved = {0};
slim_v_already_95moved = slim_fn_check_95moved_95has(slim_v_source, slim_v_moved, INT64_C(0), slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_already_95moved;
if (slim_v_already_95moved) {
slim_v_reported = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0315", (int64_t)(sizeof("E0315") - 1)), slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_from_95inout = {0};
slim_v_from_95inout = slim_fn_syntax_95name_95is_95inout(slim_v_source, slim_v_tokens, slim_v_current_95params, slim_v_argument, slim_allocation_region);
(void)slim_v_from_95inout;
if (slim_v_from_95inout) {
slim_v_reported = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0347", (int64_t)(sizeof("E0347") - 1)), slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
Slim_type_syntax_95Token slim_v_token = {0};
slim_v_token = ((Slim_type_syntax_95Token *)((*slim_v_tokens)).data)[slim_vec_check_index(&((*slim_v_tokens)), slim_v_argument)];
(void)slim_v_token;
{
SlimUnit slim_v_added = {0};
if (!slim_vec_push(slim_v_moved, &slim_v_token)) goto slim_allocation_failed; slim_v_added = (SlimUnit){0};
(void)slim_v_added;
slim_v_reported = INT64_C(0);
}
}
}
}
}
}
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next_95argument = {0};
slim_v_next_95argument = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_95argument;
{
int64_t slim_v_next_95parameter = {0};
slim_v_next_95parameter = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_next_95parameter;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_5335 = {0};
slim_t_5335 = slim_v_source;
int64_t slim_t_5337 = {0};
slim_t_5337 = slim_v_current_95params;
int64_t slim_t_5338 = {0};
slim_t_5338 = slim_v_next_95argument;
int64_t slim_t_5339 = {0};
slim_t_5339 = slim_v_next_95parameter;
int64_t slim_t_5341 = {0};
slim_t_5341 = slim_v_total;
slim_v_source = slim_t_5335;
slim_v_current_95params = slim_t_5337;
slim_v_argument = slim_t_5338;
slim_v_parameter = slim_t_5339;
slim_v_count = slim_t_5341;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95report_95builtin_95move(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_current_95params, int64_t slim_v_callee, int64_t slim_v_argument, SlimVec * slim_v_moved, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_current_95params;
(void)slim_v_callee;
(void)slim_v_argument;
(void)slim_v_moved;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_freezing = {0};
slim_v_freezing = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.freeze", (int64_t)(sizeof("bytes.freeze") - 1)), slim_allocation_region);
(void)slim_v_freezing;
if (!slim_v_freezing) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_argument_95kind = {0};
slim_v_argument_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_95kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_argument_95kind == INT64_C(2);
(void)slim_v_named;
if (!slim_v_named) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_name_95end;
{
bool slim_v_already_95moved = {0};
slim_v_already_95moved = slim_fn_check_95moved_95has(slim_v_source, slim_v_moved, INT64_C(0), slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_already_95moved;
if (slim_v_already_95moved) {
slim_result = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0315", (int64_t)(sizeof("E0315") - 1)), slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_from_95inout = {0};
slim_v_from_95inout = slim_fn_syntax_95name_95is_95inout(slim_v_source, slim_v_tokens, slim_v_current_95params, slim_v_argument, slim_allocation_region);
(void)slim_v_from_95inout;
if (slim_v_from_95inout) {
slim_result = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0347", (int64_t)(sizeof("E0347") - 1)), slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
Slim_type_syntax_95Token slim_v_token = {0};
slim_v_token = ((Slim_type_syntax_95Token *)((*slim_v_tokens)).data)[slim_vec_check_index(&((*slim_v_tokens)), slim_v_argument)];
(void)slim_v_token;
{
SlimUnit slim_v_added = {0};
if (!slim_vec_push(slim_v_moved, &slim_v_token)) goto slim_allocation_failed; slim_v_added = (SlimUnit){0};
(void)slim_v_added;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_check_95builtin_95group_95math(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_a = {0};
slim_v_a = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.add", (int64_t)(sizeof("i64.add") - 1)), slim_allocation_region);
(void)slim_v_a;
{
bool slim_v_b = {0};
slim_v_b = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.sub", (int64_t)(sizeof("i64.sub") - 1)), slim_allocation_region);
(void)slim_v_b;
{
bool slim_v_c = {0};
slim_v_c = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.mul", (int64_t)(sizeof("i64.mul") - 1)), slim_allocation_region);
(void)slim_v_c;
{
bool slim_v_d = {0};
slim_v_d = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.div", (int64_t)(sizeof("i64.div") - 1)), slim_allocation_region);
(void)slim_v_d;
{
bool slim_v_e = {0};
slim_v_e = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.rem", (int64_t)(sizeof("i64.rem") - 1)), slim_allocation_region);
(void)slim_v_e;
{
bool slim_v_first = {0};
slim_v_first = slim_v_a || slim_v_b;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_c || slim_v_d;
(void)slim_v_second;
{
bool slim_v_left = {0};
slim_v_left = slim_v_first || slim_v_second;
(void)slim_v_left;
slim_result = slim_v_left || slim_v_e;
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95builtin_95group_95compare(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_a = {0};
slim_v_a = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.eq", (int64_t)(sizeof("i64.eq") - 1)), slim_allocation_region);
(void)slim_v_a;
{
bool slim_v_b = {0};
slim_v_b = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.lt", (int64_t)(sizeof("i64.lt") - 1)), slim_allocation_region);
(void)slim_v_b;
{
bool slim_v_c = {0};
slim_v_c = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.le", (int64_t)(sizeof("i64.le") - 1)), slim_allocation_region);
(void)slim_v_c;
{
bool slim_v_d = {0};
slim_v_d = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.gt", (int64_t)(sizeof("i64.gt") - 1)), slim_allocation_region);
(void)slim_v_d;
{
bool slim_v_e = {0};
slim_v_e = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.ge", (int64_t)(sizeof("i64.ge") - 1)), slim_allocation_region);
(void)slim_v_e;
{
bool slim_v_first = {0};
slim_v_first = slim_v_a || slim_v_b;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_c || slim_v_d;
(void)slim_v_second;
{
bool slim_v_left = {0};
slim_v_left = slim_v_first || slim_v_second;
(void)slim_v_left;
slim_result = slim_v_left || slim_v_e;
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95builtin_95group_95scalar(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_a = {0};
slim_v_a = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bool.not", (int64_t)(sizeof("bool.not") - 1)), slim_allocation_region);
(void)slim_v_a;
{
bool slim_v_b = {0};
slim_v_b = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bool.and", (int64_t)(sizeof("bool.and") - 1)), slim_allocation_region);
(void)slim_v_b;
{
bool slim_v_c = {0};
slim_v_c = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bool.or", (int64_t)(sizeof("bool.or") - 1)), slim_allocation_region);
(void)slim_v_c;
{
bool slim_v_d = {0};
slim_v_d = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"u8.to-i64", (int64_t)(sizeof("u8.to-i64") - 1)), slim_allocation_region);
(void)slim_v_d;
{
bool slim_v_e = {0};
slim_v_e = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.to-u8", (int64_t)(sizeof("i64.to-u8") - 1)), slim_allocation_region);
(void)slim_v_e;
{
bool slim_v_first = {0};
slim_v_first = slim_v_a || slim_v_b;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_c || slim_v_d;
(void)slim_v_second;
{
bool slim_v_left = {0};
slim_v_left = slim_v_first || slim_v_second;
(void)slim_v_left;
slim_result = slim_v_left || slim_v_e;
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95builtin_95group_95bytes_95io(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_a = {0};
slim_v_a = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.len", (int64_t)(sizeof("bytes.len") - 1)), slim_allocation_region);
(void)slim_v_a;
{
bool slim_v_b = {0};
slim_v_b = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.get", (int64_t)(sizeof("bytes.get") - 1)), slim_allocation_region);
(void)slim_v_b;
{
bool slim_v_c = {0};
slim_v_c = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.freeze", (int64_t)(sizeof("bytes.freeze") - 1)), slim_allocation_region);
(void)slim_v_c;
{
bool slim_v_d = {0};
slim_v_d = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.print-i64", (int64_t)(sizeof("io.print-i64") - 1)), slim_allocation_region);
(void)slim_v_d;
{
bool slim_v_e = {0};
slim_v_e = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.print-bytes", (int64_t)(sizeof("io.print-bytes") - 1)), slim_allocation_region);
(void)slim_v_e;
{
bool slim_v_f = {0};
slim_v_f = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.println", (int64_t)(sizeof("io.println") - 1)), slim_allocation_region);
(void)slim_v_f;
{
bool slim_v_g = {0};
slim_v_g = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.read-file", (int64_t)(sizeof("io.read-file") - 1)), slim_allocation_region);
(void)slim_v_g;
{
bool slim_v_first = {0};
slim_v_first = slim_v_a || slim_v_b;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_c || slim_v_d;
(void)slim_v_second;
{
bool slim_v_third = {0};
slim_v_third = slim_v_e || slim_v_f;
(void)slim_v_third;
{
bool slim_v_left = {0};
slim_v_left = slim_v_first || slim_v_second;
(void)slim_v_left;
{
bool slim_v_right = {0};
slim_v_right = slim_v_third || slim_v_g;
(void)slim_v_right;
slim_result = slim_v_left || slim_v_right;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95builtin_95group_95storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_a = {0};
slim_v_a = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.new", (int64_t)(sizeof("vec.new") - 1)), slim_allocation_region);
(void)slim_v_a;
{
bool slim_v_b = {0};
slim_v_b = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.len", (int64_t)(sizeof("vec.len") - 1)), slim_allocation_region);
(void)slim_v_b;
{
bool slim_v_c = {0};
slim_v_c = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.get", (int64_t)(sizeof("vec.get") - 1)), slim_allocation_region);
(void)slim_v_c;
{
bool slim_v_d = {0};
slim_v_d = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.push", (int64_t)(sizeof("vec.push") - 1)), slim_allocation_region);
(void)slim_v_d;
{
bool slim_v_e = {0};
slim_v_e = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.set", (int64_t)(sizeof("vec.set") - 1)), slim_allocation_region);
(void)slim_v_e;
{
bool slim_v_f = {0};
slim_v_f = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.new", (int64_t)(sizeof("arena.new") - 1)), slim_allocation_region);
(void)slim_v_f;
{
bool slim_v_g = {0};
slim_v_g = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.add", (int64_t)(sizeof("arena.add") - 1)), slim_allocation_region);
(void)slim_v_g;
{
bool slim_v_h = {0};
slim_v_h = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.get", (int64_t)(sizeof("arena.get") - 1)), slim_allocation_region);
(void)slim_v_h;
{
bool slim_v_first = {0};
slim_v_first = slim_v_a || slim_v_b;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_c || slim_v_d;
(void)slim_v_second;
{
bool slim_v_third = {0};
slim_v_third = slim_v_e || slim_v_f;
(void)slim_v_third;
{
bool slim_v_fourth = {0};
slim_v_fourth = slim_v_g || slim_v_h;
(void)slim_v_fourth;
{
bool slim_v_left = {0};
slim_v_left = slim_v_first || slim_v_second;
(void)slim_v_left;
{
bool slim_v_right = {0};
slim_v_right = slim_v_third || slim_v_fourth;
(void)slim_v_right;
slim_result = slim_v_left || slim_v_right;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95builtin_95is_95known(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_math = {0};
slim_v_math = slim_fn_check_95builtin_95group_95math(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_math;
{
bool slim_v_compare = {0};
slim_v_compare = slim_fn_check_95builtin_95group_95compare(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_compare;
{
bool slim_v_scalar = {0};
slim_v_scalar = slim_fn_check_95builtin_95group_95scalar(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_scalar;
{
bool slim_v_bytes_95io = {0};
slim_v_bytes_95io = slim_fn_check_95builtin_95group_95bytes_95io(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_bytes_95io;
{
bool slim_v_storage = {0};
slim_v_storage = slim_fn_check_95builtin_95group_95storage(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_storage;
{
bool slim_v_first = {0};
slim_v_first = slim_v_math || slim_v_compare;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_scalar || slim_v_bytes_95io;
(void)slim_v_second;
{
bool slim_v_left = {0};
slim_v_left = slim_v_first || slim_v_second;
(void)slim_v_left;
slim_result = slim_v_left || slim_v_storage;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95report_95call_95move(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_current_95params, int64_t slim_v_callee, int64_t slim_v_argument, SlimVec * slim_v_moved, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_current_95params;
(void)slim_v_callee;
(void)slim_v_argument;
(void)slim_v_moved;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_builtin = {0};
slim_v_builtin = slim_fn_check_95builtin_95is_95known(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_builtin;
if (slim_v_builtin) {
slim_result = slim_fn_check_95report_95builtin_95move(slim_v_source, slim_v_tokens, slim_v_current_95params, slim_v_callee, slim_v_argument, slim_v_moved, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_callee_95start = {0};
slim_v_callee_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_95start;
{
int64_t slim_v_callee_95end = {0};
slim_v_callee_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_95end;
{
int64_t slim_v_callee_95params = {0};
slim_v_callee_95params = slim_fn_syntax_95find_95function_95params(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_callee_95start, slim_v_callee_95end, slim_allocation_region);
(void)slim_v_callee_95params;
{
bool slim_v_found = {0};
slim_v_found = slim_v_callee_95params >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_parameter = {0};
slim_v_parameter = slim_i64_add(slim_v_callee_95params, INT64_C(1));
(void)slim_v_parameter;
slim_result = slim_fn_check_95report_95owned_95arguments(slim_v_source, slim_v_tokens, slim_v_current_95params, slim_v_argument, slim_v_parameter, slim_v_moved, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95report_95moves_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_current_95params, int64_t slim_v_cursor, int64_t slim_v_end, SlimVec * slim_v_moved, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_current_95params;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_moved;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_95form = {0};
if (slim_v_form) {
slim_v_call_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
}
else {
slim_v_call_95form = false;
}
(void)slim_v_call_95form;
{
int64_t slim_v_reported = {0};
if (!slim_v_call_95form) {
slim_v_reported = INT64_C(0);
}
else {
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
{
int64_t slim_v_argument = {0};
slim_v_argument = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_argument;
slim_v_reported = slim_fn_check_95report_95call_95move(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_current_95params, slim_v_callee, slim_v_argument, slim_v_moved, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_6825 = {0};
slim_t_6825 = slim_v_source;
int64_t slim_t_6827 = {0};
slim_t_6827 = slim_v_module_95items;
int64_t slim_t_6828 = {0};
slim_t_6828 = slim_v_current_95params;
int64_t slim_t_6829 = {0};
slim_t_6829 = slim_v_next;
int64_t slim_t_6830 = {0};
slim_t_6830 = slim_v_end;
int64_t slim_t_6832 = {0};
slim_t_6832 = slim_v_total;
slim_v_source = slim_t_6825;
slim_v_module_95items = slim_t_6827;
slim_v_current_95params = slim_t_6828;
slim_v_cursor = slim_t_6829;
slim_v_end = slim_t_6830;
slim_v_count = slim_t_6832;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95report_95inout_95return(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_return_95type, int64_t slim_v_body, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_return_95type;
(void)slim_v_body;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_copy = {0};
slim_v_copy = slim_fn_check_95ownership_95type_95is_95copy(slim_v_source, slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_copy;
if (slim_v_copy) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_result = {0};
slim_v_result = slim_fn_check_95result_95expr(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_result;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_result, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_kind == INT64_C(2);
(void)slim_v_named;
if (!slim_v_named) {
slim_result = INT64_C(0);
}
else {
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95name_95is_95inout(slim_v_source, slim_v_tokens, slim_v_params, slim_v_result, slim_allocation_region);
(void)slim_v_borrowed;
if (!slim_v_borrowed) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_result, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_result, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0347", (int64_t)(sizeof("E0347") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95report_95move_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, SlimVec * slim_v_declarations, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_declaration_95count = {0};
slim_v_declaration_95count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_95count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_95count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
Slim_type_ir_95Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_95Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
int64_t slim_v_reported = {0};
if (slim_v_function_95item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_95end = {0};
slim_v_body_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95end;
{
SlimVec slim_v_moved = {0};
slim_v_moved = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_moved;
{
int64_t slim_v_returns = {0};
slim_v_returns = slim_fn_check_95report_95inout_95return(slim_v_source, slim_v_tokens, slim_v_params, slim_v_return_95type, slim_v_body, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_returns;
{
int64_t slim_v_body_95moves = {0};
slim_v_body_95moves = slim_fn_check_95report_95moves_95span(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_body, slim_v_body_95end, &slim_v_moved, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_body_95moves;
slim_v_reported = slim_i64_add(slim_v_returns, slim_v_body_95moves);
}
}
}
}
}
}
}
}
}
else {
slim_v_reported = INT64_C(0);
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_7247 = {0};
slim_t_7247 = slim_v_source;
int64_t slim_t_7249 = {0};
slim_t_7249 = slim_v_module_95items;
int64_t slim_t_7251 = {0};
slim_t_7251 = slim_v_next;
int64_t slim_t_7252 = {0};
slim_t_7252 = slim_v_total;
slim_v_source = slim_t_7247;
slim_v_module_95items = slim_t_7249;
slim_v_index = slim_t_7251;
slim_v_count = slim_t_7252;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95check_95path_95moves(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_check_95check_95path_95recur(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_prior_95error = {0};
slim_v_prior_95error = slim_v_checked > INT64_C(0);
(void)slim_v_prior_95error;
if (slim_v_prior_95error) {
slim_result = slim_v_checked;
}
else {
{
int64_t slim_v_errors = {0};
slim_v_errors = slim_fn_check_95report_95move_95items(slim_v_input, slim_v_tokens, INT64_C(3), slim_v_declarations, INT64_C(0), INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_errors;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_errors > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = INT64_C(1);
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_check_95parameter_95is_95inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_parameter;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_parameter, INT64_C(1));
(void)slim_v_head;
slim_result = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
}
return slim_result;
}

static int64_t slim_fn_check_95find_95inout_95temporary_95args(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_argument, int64_t slim_v_parameter, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_argument;
(void)slim_v_parameter;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_95parameter_95is_95inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_argument_95kind = {0};
slim_v_argument_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_95kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_argument_95kind == INT64_C(2);
(void)slim_v_named;
{
bool slim_v_not_95named = {0};
slim_v_not_95named = !slim_v_named;
(void)slim_v_not_95named;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_borrowed && slim_v_not_95named;
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_argument;
}
else {
{
int64_t slim_v_next_95argument = {0};
slim_v_next_95argument = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_95argument;
{
int64_t slim_v_next_95parameter = {0};
slim_v_next_95parameter = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_next_95parameter;
SlimBytes slim_t_7556 = {0};
slim_t_7556 = slim_v_source;
int64_t slim_t_7558 = {0};
slim_t_7558 = slim_v_next_95argument;
int64_t slim_t_7559 = {0};
slim_t_7559 = slim_v_next_95parameter;
slim_v_source = slim_t_7556;
slim_v_argument = slim_t_7558;
slim_v_parameter = slim_t_7559;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95inout_95temporary_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
{
bool slim_v_builtin = {0};
slim_v_builtin = slim_fn_check_95builtin_95is_95known(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_builtin;
if (slim_v_builtin) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_callee_95start = {0};
slim_v_callee_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_95start;
{
int64_t slim_v_callee_95end = {0};
slim_v_callee_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_95end;
{
int64_t slim_v_params = {0};
slim_v_params = slim_fn_syntax_95find_95function_95params(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_callee_95start, slim_v_callee_95end, slim_allocation_region);
(void)slim_v_params;
{
bool slim_v_found = {0};
slim_v_found = slim_v_params >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_argument = {0};
slim_v_argument = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_argument;
{
int64_t slim_v_parameter = {0};
slim_v_parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_parameter;
slim_result = slim_fn_check_95find_95inout_95temporary_95args(slim_v_source, slim_v_tokens, slim_v_argument, slim_v_parameter, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95inout_95temporary_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_95form = {0};
if (slim_v_form) {
slim_v_call_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
}
else {
slim_v_call_95form = false;
}
(void)slim_v_call_95form;
{
int64_t slim_v_invalid = {0};
if (slim_v_call_95form) {
slim_v_invalid = slim_fn_check_95find_95inout_95temporary_95call(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_cursor, slim_allocation_region);
}
else {
slim_v_invalid = INT64_C(-1);
}
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_invalid;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_7888 = {0};
slim_t_7888 = slim_v_source;
int64_t slim_t_7890 = {0};
slim_t_7890 = slim_v_module_95items;
int64_t slim_t_7891 = {0};
slim_t_7891 = slim_v_next;
int64_t slim_t_7892 = {0};
slim_t_7892 = slim_v_end;
slim_v_source = slim_t_7888;
slim_v_module_95items = slim_t_7890;
slim_v_cursor = slim_t_7891;
slim_v_end = slim_t_7892;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95inout_95temporary_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_declaration_95count = {0};
slim_v_declaration_95count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_95count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_95count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_ir_95Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_95Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
int64_t slim_v_invalid = {0};
if (slim_v_function_95item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_95end = {0};
slim_v_body_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95end;
slim_v_invalid = slim_fn_check_95find_95inout_95temporary_95span(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_body, slim_v_body_95end, slim_allocation_region);
}
}
}
}
}
}
else {
slim_v_invalid = INT64_C(-1);
}
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_invalid;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_8114 = {0};
slim_t_8114 = slim_v_source;
int64_t slim_t_8116 = {0};
slim_t_8116 = slim_v_module_95items;
int64_t slim_t_8118 = {0};
slim_t_8118 = slim_v_next;
slim_v_source = slim_t_8114;
slim_v_module_95items = slim_t_8116;
slim_v_index = slim_t_8118;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95prior_95inout_95alias(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_argument, int64_t slim_v_parameter, int64_t slim_v_current_95argument, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_argument;
(void)slim_v_parameter;
(void)slim_v_current_95argument;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_reached = {0};
slim_v_reached = slim_v_argument == slim_v_current_95argument;
(void)slim_v_reached;
if (slim_v_reached) {
slim_result = false;
}
else {
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_95parameter_95is_95inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_kind == INT64_C(2);
(void)slim_v_named;
{
bool slim_v_comparable = {0};
slim_v_comparable = slim_v_borrowed && slim_v_named;
(void)slim_v_comparable;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
if (slim_v_comparable) {
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
}
else {
slim_v_same = false;
}
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next_95argument = {0};
slim_v_next_95argument = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_95argument;
{
int64_t slim_v_next_95parameter = {0};
slim_v_next_95parameter = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_next_95parameter;
SlimBytes slim_t_8310 = {0};
slim_t_8310 = slim_v_source;
int64_t slim_t_8312 = {0};
slim_t_8312 = slim_v_next_95argument;
int64_t slim_t_8313 = {0};
slim_t_8313 = slim_v_next_95parameter;
int64_t slim_t_8314 = {0};
slim_t_8314 = slim_v_current_95argument;
int64_t slim_t_8315 = {0};
slim_t_8315 = slim_v_name_95start;
int64_t slim_t_8316 = {0};
slim_t_8316 = slim_v_name_95end;
slim_v_source = slim_t_8310;
slim_v_argument = slim_t_8312;
slim_v_parameter = slim_t_8313;
slim_v_current_95argument = slim_t_8314;
slim_v_name_95start = slim_t_8315;
slim_v_name_95end = slim_t_8316;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95inout_95alias_95args(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_first_95argument, int64_t slim_v_first_95parameter, int64_t slim_v_argument, int64_t slim_v_parameter, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_first_95argument;
(void)slim_v_first_95parameter;
(void)slim_v_argument;
(void)slim_v_parameter;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_check_95parameter_95is_95inout(slim_v_source, slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_argument_95kind = {0};
slim_v_argument_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_argument_95kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_argument_95kind == INT64_C(2);
(void)slim_v_named;
{
bool slim_v_comparable = {0};
slim_v_comparable = slim_v_borrowed && slim_v_named;
(void)slim_v_comparable;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_name_95end;
{
bool slim_v_duplicate = {0};
if (slim_v_comparable) {
slim_v_duplicate = slim_fn_check_95prior_95inout_95alias(slim_v_source, slim_v_tokens, slim_v_first_95argument, slim_v_first_95parameter, slim_v_argument, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
}
else {
slim_v_duplicate = false;
}
(void)slim_v_duplicate;
if (slim_v_duplicate) {
slim_result = slim_v_argument;
}
else {
{
int64_t slim_v_next_95argument = {0};
slim_v_next_95argument = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_95argument;
{
int64_t slim_v_next_95parameter = {0};
slim_v_next_95parameter = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_parameter, slim_allocation_region);
(void)slim_v_next_95parameter;
SlimBytes slim_t_8518 = {0};
slim_t_8518 = slim_v_source;
int64_t slim_t_8520 = {0};
slim_t_8520 = slim_v_first_95argument;
int64_t slim_t_8521 = {0};
slim_t_8521 = slim_v_first_95parameter;
int64_t slim_t_8522 = {0};
slim_t_8522 = slim_v_next_95argument;
int64_t slim_t_8523 = {0};
slim_t_8523 = slim_v_next_95parameter;
slim_v_source = slim_t_8518;
slim_v_first_95argument = slim_t_8520;
slim_v_first_95parameter = slim_t_8521;
slim_v_argument = slim_t_8522;
slim_v_parameter = slim_t_8523;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95inout_95alias_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
{
bool slim_v_builtin = {0};
slim_v_builtin = slim_fn_check_95builtin_95is_95known(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_builtin;
if (slim_v_builtin) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_callee_95start = {0};
slim_v_callee_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_95start;
{
int64_t slim_v_callee_95end = {0};
slim_v_callee_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_95end;
{
int64_t slim_v_params = {0};
slim_v_params = slim_fn_syntax_95find_95function_95params(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_callee_95start, slim_v_callee_95end, slim_allocation_region);
(void)slim_v_params;
{
bool slim_v_found = {0};
slim_v_found = slim_v_params >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_argument = {0};
slim_v_argument = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_argument;
{
int64_t slim_v_parameter = {0};
slim_v_parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_parameter;
slim_result = slim_fn_check_95find_95inout_95alias_95args(slim_v_source, slim_v_tokens, slim_v_argument, slim_v_parameter, slim_v_argument, slim_v_parameter, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95inout_95alias_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_95form = {0};
if (slim_v_form) {
slim_v_call_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
}
else {
slim_v_call_95form = false;
}
(void)slim_v_call_95form;
{
int64_t slim_v_invalid = {0};
if (slim_v_call_95form) {
slim_v_invalid = slim_fn_check_95find_95inout_95alias_95call(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_cursor, slim_allocation_region);
}
else {
slim_v_invalid = INT64_C(-1);
}
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_invalid;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_8856 = {0};
slim_t_8856 = slim_v_source;
int64_t slim_t_8858 = {0};
slim_t_8858 = slim_v_module_95items;
int64_t slim_t_8859 = {0};
slim_t_8859 = slim_v_next;
int64_t slim_t_8860 = {0};
slim_t_8860 = slim_v_end;
slim_v_source = slim_t_8856;
slim_v_module_95items = slim_t_8858;
slim_v_cursor = slim_t_8859;
slim_v_end = slim_t_8860;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95inout_95alias_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_declaration_95count = {0};
slim_v_declaration_95count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_95count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_95count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_ir_95Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_95Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
int64_t slim_v_invalid = {0};
if (slim_v_function_95item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_95end = {0};
slim_v_body_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95end;
slim_v_invalid = slim_fn_check_95find_95inout_95alias_95span(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_body, slim_v_body_95end, slim_allocation_region);
}
}
}
}
}
}
else {
slim_v_invalid = INT64_C(-1);
}
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_invalid;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_9082 = {0};
slim_t_9082 = slim_v_source;
int64_t slim_t_9084 = {0};
slim_t_9084 = slim_v_module_95items;
int64_t slim_t_9086 = {0};
slim_t_9086 = slim_v_next;
slim_v_source = slim_t_9082;
slim_v_module_95items = slim_t_9084;
slim_v_index = slim_t_9086;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95check_95path_95inout(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_exhaustive = {0};
slim_v_exhaustive = slim_fn_check_95check_95path_95exhaustive(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_exhaustive;
{
bool slim_v_prior_95error = {0};
slim_v_prior_95error = slim_v_exhaustive > INT64_C(0);
(void)slim_v_prior_95error;
if (slim_v_prior_95error) {
slim_result = slim_v_exhaustive;
}
else {
{
int64_t slim_v_temporary = {0};
slim_v_temporary = slim_fn_check_95find_95inout_95temporary_95items(slim_v_input, slim_v_tokens, INT64_C(3), slim_v_declarations, INT64_C(0), slim_allocation_region);
(void)slim_v_temporary;
{
bool slim_v_has_95temporary = {0};
slim_v_has_95temporary = slim_v_temporary >= INT64_C(0);
(void)slim_v_has_95temporary;
if (slim_v_has_95temporary) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_temporary, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_temporary, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0348", (int64_t)(sizeof("E0348") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
else {
{
int64_t slim_v_duplicate = {0};
slim_v_duplicate = slim_fn_check_95find_95inout_95alias_95items(slim_v_input, slim_v_tokens, INT64_C(3), slim_v_declarations, INT64_C(0), slim_allocation_region);
(void)slim_v_duplicate;
{
bool slim_v_has_95duplicate = {0};
slim_v_has_95duplicate = slim_v_duplicate >= INT64_C(0);
(void)slim_v_has_95duplicate;
if (slim_v_has_95duplicate) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_duplicate, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_duplicate, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0349", (int64_t)(sizeof("E0349") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_check_95boolean_95arms_95have(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimBytes slim_v_pattern, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_pattern;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_arm_95pattern = {0};
slim_v_arm_95pattern = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_arm_95pattern;
{
bool slim_v_found = {0};
slim_v_found = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_arm_95pattern, slim_v_pattern, slim_allocation_region);
(void)slim_v_found;
if (slim_v_found) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_9421 = {0};
slim_t_9421 = slim_v_source;
int64_t slim_t_9423 = {0};
slim_t_9423 = slim_v_next;
SlimBytes slim_t_9424 = {0};
slim_t_9424 = slim_v_pattern;
slim_v_source = slim_t_9421;
slim_v_cursor = slim_t_9423;
slim_v_pattern = slim_t_9424;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95boolean_95match_95incomplete(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_value;
{
bool slim_v_true_95value = {0};
slim_v_true_95value = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_value, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_true_95value;
{
bool slim_v_false_95value = {0};
slim_v_false_95value = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_value, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_false_95value;
{
bool slim_v_boolean_95value = {0};
slim_v_boolean_95value = slim_v_true_95value || slim_v_false_95value;
(void)slim_v_boolean_95value;
if (!slim_v_boolean_95value) {
slim_result = false;
}
else {
{
int64_t slim_v_arms = {0};
slim_v_arms = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_arms;
{
bool slim_v_has_95true = {0};
slim_v_has_95true = slim_fn_check_95boolean_95arms_95have(slim_v_source, slim_v_tokens, slim_v_arms, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_has_95true;
{
bool slim_v_has_95false = {0};
slim_v_has_95false = slim_fn_check_95boolean_95arms_95have(slim_v_source, slim_v_tokens, slim_v_arms, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_has_95false;
{
bool slim_v_complete = {0};
slim_v_complete = slim_v_has_95true && slim_v_has_95false;
(void)slim_v_complete;
slim_result = !slim_v_complete;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95nonexhaustive_95in_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_match_95form = {0};
if (slim_v_form) {
slim_v_match_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"match", (int64_t)(sizeof("match") - 1)), slim_allocation_region);
}
else {
slim_v_match_95form = false;
}
(void)slim_v_match_95form;
{
bool slim_v_incomplete = {0};
if (slim_v_match_95form) {
slim_v_incomplete = slim_fn_check_95boolean_95match_95incomplete(slim_v_source, slim_v_tokens, slim_v_cursor, slim_allocation_region);
}
else {
slim_v_incomplete = false;
}
(void)slim_v_incomplete;
if (slim_v_incomplete) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_9720 = {0};
slim_t_9720 = slim_v_source;
int64_t slim_t_9722 = {0};
slim_t_9722 = slim_v_next;
int64_t slim_t_9723 = {0};
slim_t_9723 = slim_v_end;
slim_v_source = slim_t_9720;
slim_v_cursor = slim_t_9722;
slim_v_end = slim_t_9723;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95nonexhaustive_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_declaration_95count = {0};
slim_v_declaration_95count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_95count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_95count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_ir_95Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_95Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
int64_t slim_v_incomplete = {0};
if (slim_v_function_95item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_95end = {0};
slim_v_body_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95end;
slim_v_incomplete = slim_fn_check_95find_95nonexhaustive_95in_95span(slim_v_source, slim_v_tokens, slim_v_body, slim_v_body_95end, slim_allocation_region);
}
}
}
}
}
}
else {
slim_v_incomplete = INT64_C(-1);
}
(void)slim_v_incomplete;
{
bool slim_v_found = {0};
slim_v_found = slim_v_incomplete >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_incomplete;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_9943 = {0};
slim_t_9943 = slim_v_source;
int64_t slim_t_9945 = {0};
slim_t_9945 = slim_v_module_95items;
int64_t slim_t_9947 = {0};
slim_t_9947 = slim_v_next;
slim_v_source = slim_t_9943;
slim_v_module_95items = slim_t_9945;
slim_v_index = slim_t_9947;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95check_95path_95exhaustive(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_check_95check_95path_95effects(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_prior_95error = {0};
slim_v_prior_95error = slim_v_checked > INT64_C(0);
(void)slim_v_prior_95error;
if (slim_v_prior_95error) {
slim_result = slim_v_checked;
}
else {
{
int64_t slim_v_incomplete = {0};
slim_v_incomplete = slim_fn_check_95find_95nonexhaustive_95items(slim_v_input, slim_v_tokens, INT64_C(3), slim_v_declarations, INT64_C(0), slim_allocation_region);
(void)slim_v_incomplete;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_incomplete >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_incomplete, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_incomplete, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0336", (int64_t)(sizeof("E0336") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_check_95effect_95list_95has(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_effect_95code, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_effect_95code;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
bool slim_v_alloc_95effect = {0};
slim_v_alloc_95effect = slim_v_effect_95code == INT64_C(1);
(void)slim_v_alloc_95effect;
{
bool slim_v_io_95effect = {0};
slim_v_io_95effect = slim_v_effect_95code == INT64_C(2);
(void)slim_v_io_95effect;
{
bool slim_v_matches_95alloc = {0};
slim_v_matches_95alloc = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"alloc", (int64_t)(sizeof("alloc") - 1)), slim_allocation_region);
(void)slim_v_matches_95alloc;
{
bool slim_v_matches_95io = {0};
slim_v_matches_95io = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"io", (int64_t)(sizeof("io") - 1)), slim_allocation_region);
(void)slim_v_matches_95io;
{
bool slim_v_matches_95partial = {0};
slim_v_matches_95partial = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"partial", (int64_t)(sizeof("partial") - 1)), slim_allocation_region);
(void)slim_v_matches_95partial;
{
bool slim_v_selected = {0};
if (slim_v_alloc_95effect) {
slim_v_selected = slim_v_matches_95alloc;
}
else {
if (slim_v_io_95effect) {
slim_v_selected = slim_v_matches_95io;
}
else {
slim_v_selected = slim_v_matches_95partial;
}
}
(void)slim_v_selected;
if (slim_v_selected) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_10279 = {0};
slim_t_10279 = slim_v_source;
int64_t slim_t_10281 = {0};
slim_t_10281 = slim_v_next;
int64_t slim_t_10282 = {0};
slim_t_10282 = slim_v_effect_95code;
slim_v_source = slim_t_10279;
slim_v_cursor = slim_t_10281;
slim_v_effect_95code = slim_t_10282;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95builtin_95requires_95effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, int64_t slim_v_effect_95code, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_v_effect_95code;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_alloc_95effect = {0};
slim_v_alloc_95effect = slim_v_effect_95code == INT64_C(1);
(void)slim_v_alloc_95effect;
{
bool slim_v_io_95effect = {0};
slim_v_io_95effect = slim_v_effect_95code == INT64_C(2);
(void)slim_v_io_95effect;
if (slim_v_alloc_95effect) {
{
bool slim_v_vec_95new = {0};
slim_v_vec_95new = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.new", (int64_t)(sizeof("vec.new") - 1)), slim_allocation_region);
(void)slim_v_vec_95new;
{
bool slim_v_vec_95push = {0};
slim_v_vec_95push = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.push", (int64_t)(sizeof("vec.push") - 1)), slim_allocation_region);
(void)slim_v_vec_95push;
{
bool slim_v_arena_95new = {0};
slim_v_arena_95new = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.new", (int64_t)(sizeof("arena.new") - 1)), slim_allocation_region);
(void)slim_v_arena_95new;
{
bool slim_v_arena_95add = {0};
slim_v_arena_95add = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.add", (int64_t)(sizeof("arena.add") - 1)), slim_allocation_region);
(void)slim_v_arena_95add;
{
bool slim_v_read_95file = {0};
slim_v_read_95file = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.read-file", (int64_t)(sizeof("io.read-file") - 1)), slim_allocation_region);
(void)slim_v_read_95file;
{
bool slim_v_first = {0};
slim_v_first = slim_v_vec_95new || slim_v_vec_95push;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_arena_95new || slim_v_arena_95add;
(void)slim_v_second;
{
bool slim_v_storage = {0};
slim_v_storage = slim_v_first || slim_v_second;
(void)slim_v_storage;
slim_result = slim_v_storage || slim_v_read_95file;
}
}
}
}
}
}
}
}
}
else {
if (slim_v_io_95effect) {
{
bool slim_v_print_95i64 = {0};
slim_v_print_95i64 = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.print-i64", (int64_t)(sizeof("io.print-i64") - 1)), slim_allocation_region);
(void)slim_v_print_95i64;
{
bool slim_v_print_95bytes = {0};
slim_v_print_95bytes = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.print-bytes", (int64_t)(sizeof("io.print-bytes") - 1)), slim_allocation_region);
(void)slim_v_print_95bytes;
{
bool slim_v_println = {0};
slim_v_println = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.println", (int64_t)(sizeof("io.println") - 1)), slim_allocation_region);
(void)slim_v_println;
{
bool slim_v_read_95file = {0};
slim_v_read_95file = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.read-file", (int64_t)(sizeof("io.read-file") - 1)), slim_allocation_region);
(void)slim_v_read_95file;
{
bool slim_v_first = {0};
slim_v_first = slim_v_print_95i64 || slim_v_print_95bytes;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_println || slim_v_read_95file;
(void)slim_v_second;
slim_result = slim_v_first || slim_v_second;
}
}
}
}
}
}
}
else {
slim_result = false;
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95function_95effects(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_callee_95start, int64_t slim_v_callee_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_callee_95start;
(void)slim_v_callee_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
{
bool slim_v_same_95name = {0};
slim_v_same_95name = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_name_95start, slim_v_name_95end, slim_v_callee_95start, slim_v_callee_95end, slim_allocation_region);
(void)slim_v_same_95name;
{
bool slim_v_found = {0};
slim_v_found = slim_v_function_95item && slim_v_same_95name;
(void)slim_v_found;
if (slim_v_found) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
slim_result = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
}
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_10732 = {0};
slim_t_10732 = slim_v_source;
int64_t slim_t_10734 = {0};
slim_t_10734 = slim_v_next;
int64_t slim_t_10735 = {0};
slim_t_10735 = slim_v_callee_95start;
int64_t slim_t_10736 = {0};
slim_t_10736 = slim_v_callee_95end;
slim_v_source = slim_t_10732;
slim_v_cursor = slim_t_10734;
slim_v_callee_95start = slim_t_10735;
slim_v_callee_95end = slim_t_10736;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95call_95requires_95effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_callee, int64_t slim_v_effect_95code, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_callee;
(void)slim_v_effect_95code;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_required = {0};
slim_v_required = slim_fn_check_95builtin_95requires_95effect(slim_v_source, slim_v_tokens, slim_v_callee, slim_v_effect_95code, slim_allocation_region);
(void)slim_v_required;
if (slim_v_required) {
slim_result = true;
}
else {
{
bool slim_v_builtin = {0};
slim_v_builtin = slim_fn_check_95builtin_95is_95known(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_builtin;
if (slim_v_builtin) {
slim_result = false;
}
else {
{
int64_t slim_v_callee_95start = {0};
slim_v_callee_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_95start;
{
int64_t slim_v_callee_95end = {0};
slim_v_callee_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_95end;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_check_95find_95function_95effects(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_callee_95start, slim_v_callee_95end, slim_allocation_region);
(void)slim_v_effects;
{
bool slim_v_found = {0};
slim_v_found = slim_v_effects >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
{
int64_t slim_v_first_95effect = {0};
slim_v_first_95effect = slim_i64_add(slim_v_effects, INT64_C(2));
(void)slim_v_first_95effect;
slim_result = slim_fn_check_95effect_95list_95has(slim_v_source, slim_v_tokens, slim_v_first_95effect, slim_v_effect_95code, slim_allocation_region);
}
}
else {
slim_result = false;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95call_95missing_95effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_declared_95effects, int64_t slim_v_callee, int64_t slim_v_effect_95code, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_declared_95effects;
(void)slim_v_callee;
(void)slim_v_effect_95code;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_effect_95code > INT64_C(3);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
bool slim_v_required = {0};
slim_v_required = slim_fn_check_95call_95requires_95effect(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_callee, slim_v_effect_95code, slim_allocation_region);
(void)slim_v_required;
{
int64_t slim_v_first_95declared = {0};
slim_v_first_95declared = slim_i64_add(slim_v_declared_95effects, INT64_C(2));
(void)slim_v_first_95declared;
{
bool slim_v_declared = {0};
slim_v_declared = slim_fn_check_95effect_95list_95has(slim_v_source, slim_v_tokens, slim_v_first_95declared, slim_v_effect_95code, slim_allocation_region);
(void)slim_v_declared;
{
bool slim_v_not_95declared = {0};
slim_v_not_95declared = !slim_v_declared;
(void)slim_v_not_95declared;
{
bool slim_v_missing = {0};
slim_v_missing = slim_v_required && slim_v_not_95declared;
(void)slim_v_missing;
if (slim_v_missing) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_effect_95code, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_11043 = {0};
slim_t_11043 = slim_v_source;
int64_t slim_t_11045 = {0};
slim_t_11045 = slim_v_module_95items;
int64_t slim_t_11046 = {0};
slim_t_11046 = slim_v_declared_95effects;
int64_t slim_t_11047 = {0};
slim_t_11047 = slim_v_callee;
int64_t slim_t_11048 = {0};
slim_t_11048 = slim_v_next;
slim_v_source = slim_t_11043;
slim_v_module_95items = slim_t_11045;
slim_v_declared_95effects = slim_t_11046;
slim_v_callee = slim_t_11047;
slim_v_effect_95code = slim_t_11048;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95missing_95effect_95in_95span(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_declared_95effects, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_declared_95effects;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_95form = {0};
if (slim_v_form) {
slim_v_call_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
}
else {
slim_v_call_95form = false;
}
(void)slim_v_call_95form;
{
bool slim_v_missing = {0};
if (slim_v_call_95form) {
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
slim_v_missing = slim_fn_check_95call_95missing_95effect(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_declared_95effects, slim_v_callee, INT64_C(1), slim_allocation_region);
}
}
else {
slim_v_missing = false;
}
(void)slim_v_missing;
if (slim_v_missing) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_11229 = {0};
slim_t_11229 = slim_v_source;
int64_t slim_t_11231 = {0};
slim_t_11231 = slim_v_module_95items;
int64_t slim_t_11232 = {0};
slim_t_11232 = slim_v_declared_95effects;
int64_t slim_t_11233 = {0};
slim_t_11233 = slim_v_next;
int64_t slim_t_11234 = {0};
slim_t_11234 = slim_v_end;
slim_v_source = slim_t_11229;
slim_v_module_95items = slim_t_11231;
slim_v_declared_95effects = slim_t_11232;
slim_v_cursor = slim_t_11233;
slim_v_end = slim_t_11234;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95missing_95effect_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_declaration_95count = {0};
slim_v_declaration_95count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_95count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_95count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_ir_95Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_95Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
int64_t slim_v_missing = {0};
if (slim_v_function_95item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_declared_95effects = {0};
slim_v_declared_95effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_declared_95effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_declared_95effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_95end = {0};
slim_v_body_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95end;
slim_v_missing = slim_fn_check_95find_95missing_95effect_95in_95span(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_declared_95effects, slim_v_body, slim_v_body_95end, slim_allocation_region);
}
}
}
}
}
}
else {
slim_v_missing = INT64_C(-1);
}
(void)slim_v_missing;
{
bool slim_v_found = {0};
slim_v_found = slim_v_missing >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_missing;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_11456 = {0};
slim_t_11456 = slim_v_source;
int64_t slim_t_11458 = {0};
slim_t_11458 = slim_v_module_95items;
int64_t slim_t_11460 = {0};
slim_t_11460 = slim_v_next;
slim_v_source = slim_t_11456;
slim_v_module_95items = slim_t_11458;
slim_v_index = slim_t_11460;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95check_95path_95effects(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_typed = {0};
slim_v_typed = slim_fn_check_95check_95path_95typed(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
bool slim_v_typed_95error = {0};
slim_v_typed_95error = slim_v_typed > INT64_C(0);
(void)slim_v_typed_95error;
if (slim_v_typed_95error) {
slim_result = slim_v_typed;
}
else {
{
int64_t slim_v_missing = {0};
slim_v_missing = slim_fn_check_95find_95missing_95effect_95items(slim_v_input, slim_v_tokens, INT64_C(3), slim_v_declarations, INT64_C(0), slim_allocation_region);
(void)slim_v_missing;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_missing >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_missing, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_missing, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0343", (int64_t)(sizeof("E0343") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95find_95overqualified(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_atom = {0};
slim_v_atom = slim_v_kind == INT64_C(2);
(void)slim_v_atom;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_slashes = {0};
if (slim_v_atom) {
slim_v_slashes = slim_fn_syntax_95span_95count_95byte(slim_v_source, slim_v_start, slim_v_end, INT64_C(47), INT64_C(0), slim_allocation_region);
}
else {
slim_v_slashes = INT64_C(0);
}
(void)slim_v_slashes;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_slashes > INT64_C(1);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_11773 = {0};
slim_t_11773 = slim_v_source;
int64_t slim_t_11775 = {0};
slim_t_11775 = slim_v_next;
int64_t slim_t_11776 = {0};
slim_t_11776 = slim_v_length;
slim_v_source = slim_t_11773;
slim_v_index = slim_t_11775;
slim_v_length = slim_t_11776;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95check_95path_95project(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_core = {0};
slim_v_core = slim_fn_check_95check_95path_95full(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_core;
{
bool slim_v_core_95error = {0};
slim_v_core_95error = slim_v_core > INT64_C(0);
(void)slim_v_core_95error;
if (slim_v_core_95error) {
slim_result = slim_v_core;
}
else {
{
int64_t slim_v_token_95count = {0};
slim_v_token_95count = ((*slim_v_tokens)).len;
(void)slim_v_token_95count;
{
int64_t slim_v_invalid_95token = {0};
slim_v_invalid_95token = slim_fn_check_95find_95overqualified(slim_v_input, slim_v_tokens, INT64_C(0), slim_v_token_95count, slim_allocation_region);
(void)slim_v_invalid_95token;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_invalid_95token >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_invalid_95token, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_invalid_95token, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0228", (int64_t)(sizeof("E0228") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95result_95expr(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_expr, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
if (!slim_v_form) {
slim_result = slim_v_expr;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_expr, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_let_95form = {0};
slim_v_let_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
(void)slim_v_let_95form;
if (slim_v_let_95form) {
{
int64_t slim_v_type_95index = {0};
slim_v_type_95index = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_type_95index;
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
SlimBytes slim_t_12046 = {0};
slim_t_12046 = slim_v_source;
int64_t slim_t_12048 = {0};
slim_t_12048 = slim_v_body;
slim_v_source = slim_t_12046;
slim_v_expr = slim_t_12048;
goto slim_recur;
}
}
}
}
else {
{
bool slim_v_set_95form = {0};
slim_v_set_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"set", (int64_t)(sizeof("set") - 1)), slim_allocation_region);
(void)slim_v_set_95form;
if (slim_v_set_95form) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
SlimBytes slim_t_12095 = {0};
slim_t_12095 = slim_v_source;
int64_t slim_t_12097 = {0};
slim_t_12097 = slim_v_body;
slim_v_source = slim_t_12095;
slim_v_expr = slim_t_12097;
goto slim_recur;
}
}
}
else {
slim_result = slim_v_expr;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95atom_95type_95kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_expr, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_string_95value = {0};
slim_v_string_95value = slim_v_kind == INT64_C(3);
(void)slim_v_string_95value;
if (slim_v_string_95value) {
slim_result = INT64_C(4);
}
else {
{
bool slim_v_true_95value = {0};
slim_v_true_95value = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_expr, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_true_95value;
{
bool slim_v_false_95value = {0};
slim_v_false_95value = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_expr, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_false_95value;
{
bool slim_v_bool_95value = {0};
slim_v_bool_95value = slim_v_true_95value || slim_v_false_95value;
(void)slim_v_bool_95value;
if (slim_v_bool_95value) {
slim_result = INT64_C(1);
}
else {
{
bool slim_v_unit_95value = {0};
slim_v_unit_95value = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_expr, slim_bytes_static((const uint8_t *)"unit", (int64_t)(sizeof("unit") - 1)), slim_allocation_region);
(void)slim_v_unit_95value;
if (slim_v_unit_95value) {
slim_result = INT64_C(0);
}
else {
{
bool slim_v_number = {0};
slim_v_number = slim_fn_syntax_95first_95byte_95is_95number(slim_v_source, slim_v_tokens, slim_v_expr, slim_allocation_region);
(void)slim_v_number;
if (slim_v_number) {
slim_result = INT64_C(3);
}
else {
slim_result = INT64_C(-1);
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95type_95mismatch_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_declaration_95count = {0};
slim_v_declaration_95count = ((*slim_v_declarations)).len;
(void)slim_v_declaration_95count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_declaration_95count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_ir_95Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_95Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
int64_t slim_v_mismatch = {0};
if (slim_v_function_95item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_result = {0};
slim_v_result = slim_fn_check_95result_95expr(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_result;
{
int64_t slim_v_expected = {0};
slim_v_expected = slim_fn_syntax_95scalar_95type_95kind(slim_v_source, slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_expected;
{
int64_t slim_v_actual = {0};
slim_v_actual = slim_fn_check_95atom_95type_95kind(slim_v_source, slim_v_tokens, slim_v_result, slim_allocation_region);
(void)slim_v_actual;
{
bool slim_v_expected_95known = {0};
slim_v_expected_95known = slim_v_expected >= INT64_C(0);
(void)slim_v_expected_95known;
{
bool slim_v_actual_95known = {0};
slim_v_actual_95known = slim_v_actual >= INT64_C(0);
(void)slim_v_actual_95known;
{
bool slim_v_known = {0};
slim_v_known = slim_v_expected_95known && slim_v_actual_95known;
(void)slim_v_known;
{
bool slim_v_same = {0};
slim_v_same = slim_v_expected == slim_v_actual;
(void)slim_v_same;
{
bool slim_v_different = {0};
slim_v_different = !slim_v_same;
(void)slim_v_different;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_known && slim_v_different;
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_v_mismatch = slim_v_result;
}
else {
slim_v_mismatch = INT64_C(-1);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
else {
slim_v_mismatch = INT64_C(-1);
}
(void)slim_v_mismatch;
{
bool slim_v_found = {0};
slim_v_found = slim_v_mismatch >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_mismatch;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_12569 = {0};
slim_t_12569 = slim_v_source;
int64_t slim_t_12572 = {0};
slim_t_12572 = slim_v_next;
slim_v_source = slim_t_12569;
slim_v_index = slim_t_12572;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95check_95path_95typed(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_syntax = {0};
slim_v_syntax = slim_fn_check_95check_95path_95project(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_syntax;
{
bool slim_v_syntax_95error = {0};
slim_v_syntax_95error = slim_v_syntax > INT64_C(0);
(void)slim_v_syntax_95error;
if (slim_v_syntax_95error) {
slim_result = slim_v_syntax;
}
else {
{
int64_t slim_v_mismatch = {0};
slim_v_mismatch = slim_fn_check_95find_95type_95mismatch_95items(slim_v_input, slim_v_tokens, slim_v_declarations, INT64_C(0), slim_allocation_region);
(void)slim_v_mismatch;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_mismatch >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_mismatch, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_mismatch, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0344", (int64_t)(sizeof("E0344") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95find_95unterminated(SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_depth, int64_t slim_v_form_95start, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_v_depth;
(void)slim_v_form_95start;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
{
bool slim_v_open = {0};
slim_v_open = slim_v_depth > INT64_C(0);
(void)slim_v_open;
if (slim_v_open) {
slim_result = slim_v_form_95start;
}
else {
slim_result = INT64_C(-1);
}
}
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_left = {0};
slim_v_left = slim_v_kind == INT64_C(0);
(void)slim_v_left;
{
bool slim_v_right = {0};
slim_v_right = slim_v_kind == INT64_C(1);
(void)slim_v_right;
{
bool slim_v_at_95root = {0};
slim_v_at_95root = slim_v_depth == INT64_C(0);
(void)slim_v_at_95root;
{
bool slim_v_root_95left = {0};
slim_v_root_95left = slim_v_left && slim_v_at_95root;
(void)slim_v_root_95left;
{
int64_t slim_v_next_95start = {0};
if (slim_v_root_95left) {
slim_v_next_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_index, slim_allocation_region);
}
else {
slim_v_next_95start = slim_v_form_95start;
}
(void)slim_v_next_95start;
{
int64_t slim_v_increased = {0};
if (slim_v_left) {
slim_v_increased = slim_i64_add(slim_v_depth, INT64_C(1));
}
else {
slim_v_increased = slim_v_depth;
}
(void)slim_v_increased;
{
int64_t slim_v_next_95depth = {0};
if (slim_v_right) {
slim_v_next_95depth = slim_i64_sub(slim_v_increased, INT64_C(1));
}
else {
slim_v_next_95depth = slim_v_increased;
}
(void)slim_v_next_95depth;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_12919 = {0};
slim_t_12919 = slim_v_next;
int64_t slim_t_12920 = {0};
slim_t_12920 = slim_v_length;
int64_t slim_t_12921 = {0};
slim_t_12921 = slim_v_next_95depth;
int64_t slim_t_12922 = {0};
slim_t_12922 = slim_v_next_95start;
slim_v_index = slim_t_12919;
slim_v_length = slim_t_12920;
slim_v_depth = slim_t_12921;
slim_v_form_95start = slim_t_12922;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_check_95append_95diagnostic_95identity(Slim_type_check_95Diagnostic slim_v_diagnostic, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_diagnostic;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimBytes slim_v_code = {0};
slim_v_code = slim_v_diagnostic.slim_field_code;
(void)slim_v_code;
{
int64_t slim_v_start = {0};
slim_v_start = slim_v_diagnostic.slim_field_start;
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_v_diagnostic.slim_field_end;
(void)slim_v_end;
{
SlimUnit slim_v_code_95text = {0};
slim_v_code_95text = slim_fn_text_95append_95text(slim_v_output, slim_v_code, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_code_95text;
{
SlimUnit slim_v_at = {0};
slim_v_at = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"@", (int64_t)(sizeof("@") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_at;
{
SlimUnit slim_v_start_95text = {0};
slim_v_start_95text = slim_fn_text_95append_95nonnegative_95i64(slim_v_start, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_start_95text;
{
SlimUnit slim_v_colon = {0};
slim_v_colon = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)":", (int64_t)(sizeof(":") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_colon;
{
SlimUnit slim_v_end_95text = {0};
slim_v_end_95text = slim_fn_text_95append_95nonnegative_95i64(slim_v_end, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_end_95text;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"\n", (int64_t)(sizeof("\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95check_95path(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_token_95count = {0};
slim_v_token_95count = ((*slim_v_tokens)).len;
(void)slim_v_token_95count;
{
int64_t slim_v_unterminated = {0};
slim_v_unterminated = slim_fn_check_95find_95unterminated(slim_v_tokens, INT64_C(0), slim_v_token_95count, INT64_C(0), INT64_C(0), slim_allocation_region);
(void)slim_v_unterminated;
{
bool slim_v_delimiters_95valid = {0};
slim_v_delimiters_95valid = slim_v_unterminated < INT64_C(0);
(void)slim_v_delimiters_95valid;
{
bool slim_v_shape_95valid = {0};
slim_v_shape_95valid = slim_fn_validate_95executable_95shape_95valid(slim_v_input, slim_v_tokens, slim_allocation_region);
(void)slim_v_shape_95valid;
{
bool slim_v_valid = {0};
slim_v_valid = slim_v_delimiters_95valid && slim_v_shape_95valid;
(void)slim_v_valid;
if (slim_v_valid) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_input_95length = {0};
slim_v_input_95length = slim_bytes_len(slim_v_input);
(void)slim_v_input_95length;
{
Slim_type_check_95Diagnostic slim_v_diagnostic = {0};
{
SlimBytes slim_t_13161 = {0};
slim_t_13161 = slim_bytes_static((const uint8_t *)"E0102", (int64_t)(sizeof("E0102") - 1));
int64_t slim_t_13165 = {0};
slim_t_13165 = INT64_C(0);
int64_t slim_t_13169 = {0};
slim_t_13169 = slim_v_input_95length;
slim_v_diagnostic = (Slim_type_check_95Diagnostic){.slim_field_code = slim_t_13161, .slim_field_start = slim_t_13165, .slim_field_end = slim_t_13169};
}
(void)slim_v_diagnostic;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_rendered = {0};
slim_v_rendered = slim_fn_check_95append_95diagnostic_95identity(slim_v_diagnostic, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_rendered;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(1);
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_check_95last_95form_95in_95list(SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_previous, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_previous;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_previous;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_old_95cursor = {0};
slim_v_old_95cursor = slim_v_cursor;
(void)slim_v_old_95cursor;
int64_t slim_t_13301 = {0};
slim_t_13301 = slim_v_next;
int64_t slim_t_13302 = {0};
slim_t_13302 = slim_v_old_95cursor;
slim_v_cursor = slim_t_13301;
slim_v_previous = slim_t_13302;
goto slim_recur;
}
}
}
}
}
return slim_result;
}

static bool slim_fn_check_95known_95expr_95head(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_head, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_head;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_let_95form = {0};
slim_v_let_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
(void)slim_v_let_95form;
{
bool slim_v_call_95form = {0};
slim_v_call_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
(void)slim_v_call_95form;
{
bool slim_v_make_95form = {0};
slim_v_make_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"make", (int64_t)(sizeof("make") - 1)), slim_allocation_region);
(void)slim_v_make_95form;
{
bool slim_v_get_95form = {0};
slim_v_get_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"get", (int64_t)(sizeof("get") - 1)), slim_allocation_region);
(void)slim_v_get_95form;
{
bool slim_v_case_95form = {0};
slim_v_case_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"case", (int64_t)(sizeof("case") - 1)), slim_allocation_region);
(void)slim_v_case_95form;
{
bool slim_v_match_95form = {0};
slim_v_match_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"match", (int64_t)(sizeof("match") - 1)), slim_allocation_region);
(void)slim_v_match_95form;
{
bool slim_v_recur_95form = {0};
slim_v_recur_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"recur", (int64_t)(sizeof("recur") - 1)), slim_allocation_region);
(void)slim_v_recur_95form;
{
bool slim_v_set_95form = {0};
slim_v_set_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"set", (int64_t)(sizeof("set") - 1)), slim_allocation_region);
(void)slim_v_set_95form;
{
bool slim_v_first = {0};
slim_v_first = slim_v_let_95form || slim_v_call_95form;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_make_95form || slim_v_get_95form;
(void)slim_v_second;
{
bool slim_v_third = {0};
slim_v_third = slim_v_case_95form || slim_v_match_95form;
(void)slim_v_third;
{
bool slim_v_fourth = {0};
slim_v_fourth = slim_v_recur_95form || slim_v_set_95form;
(void)slim_v_fourth;
{
bool slim_v_left = {0};
slim_v_left = slim_v_first || slim_v_second;
(void)slim_v_left;
{
bool slim_v_right = {0};
slim_v_right = slim_v_third || slim_v_fourth;
(void)slim_v_right;
slim_result = slim_v_left || slim_v_right;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95unknown_95pair(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_value, int64_t slim_v_body, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_value;
(void)slim_v_body;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_value_95unknown = {0};
slim_v_value_95unknown = slim_fn_check_95find_95unknown_95expr(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_value_95unknown;
{
bool slim_v_found = {0};
slim_v_found = slim_v_value_95unknown >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_value_95unknown;
}
else {
slim_result = slim_fn_check_95find_95unknown_95expr(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95unknown_95control(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, int64_t slim_v_head, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_v_head;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_match_95form = {0};
slim_v_match_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"match", (int64_t)(sizeof("match") - 1)), slim_allocation_region);
(void)slim_v_match_95form;
if (slim_v_match_95form) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_value;
{
int64_t slim_v_first_95arm = {0};
slim_v_first_95arm = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_first_95arm;
{
int64_t slim_v_value_95unknown = {0};
slim_v_value_95unknown = slim_fn_check_95find_95unknown_95expr(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_value_95unknown;
{
bool slim_v_found = {0};
slim_v_found = slim_v_value_95unknown >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_value_95unknown;
}
else {
slim_result = slim_fn_check_95find_95unknown_95arms(slim_v_source, slim_v_tokens, slim_v_first_95arm, slim_allocation_region);
}
}
}
}
}
}
else {
{
bool slim_v_call_95form = {0};
slim_v_call_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
(void)slim_v_call_95form;
{
int64_t slim_v_first = {0};
if (slim_v_call_95form) {
slim_v_first = slim_i64_add(slim_v_expr, INT64_C(3));
}
else {
slim_v_first = slim_i64_add(slim_v_expr, INT64_C(2));
}
(void)slim_v_first;
slim_result = slim_fn_check_95find_95unknown_95forms(slim_v_source, slim_v_tokens, slim_v_first, slim_allocation_region);
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95unknown_95data(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, int64_t slim_v_head, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_v_head;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_make_95form = {0};
slim_v_make_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"make", (int64_t)(sizeof("make") - 1)), slim_allocation_region);
(void)slim_v_make_95form;
if (slim_v_make_95form) {
{
int64_t slim_v_first_95field = {0};
slim_v_first_95field = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_first_95field;
slim_result = slim_fn_check_95find_95unknown_95fields(slim_v_source, slim_v_tokens, slim_v_first_95field, slim_allocation_region);
}
}
else {
{
bool slim_v_get_95form = {0};
slim_v_get_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"get", (int64_t)(sizeof("get") - 1)), slim_allocation_region);
(void)slim_v_get_95form;
if (slim_v_get_95form) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_value;
slim_result = slim_fn_check_95find_95unknown_95expr(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
}
}
else {
{
bool slim_v_case_95form = {0};
slim_v_case_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"case", (int64_t)(sizeof("case") - 1)), slim_allocation_region);
(void)slim_v_case_95form;
if (slim_v_case_95form) {
{
int64_t slim_v_first_95value = {0};
slim_v_first_95value = slim_i64_add(slim_v_expr, INT64_C(4));
(void)slim_v_first_95value;
slim_result = slim_fn_check_95find_95unknown_95forms(slim_v_source, slim_v_tokens, slim_v_first_95value, slim_allocation_region);
}
}
else {
slim_result = slim_fn_check_95find_95unknown_95control(slim_v_source, slim_v_tokens, slim_v_expr, slim_v_head, slim_allocation_region);
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95unknown_95known(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, int64_t slim_v_head, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_v_head;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_let_95form = {0};
slim_v_let_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
(void)slim_v_let_95form;
if (slim_v_let_95form) {
{
int64_t slim_v_type_95index = {0};
slim_v_type_95index = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_type_95index;
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
slim_result = slim_fn_check_95find_95unknown_95pair(slim_v_source, slim_v_tokens, slim_v_value, slim_v_body, slim_allocation_region);
}
}
}
}
else {
{
bool slim_v_set_95form = {0};
slim_v_set_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"set", (int64_t)(sizeof("set") - 1)), slim_allocation_region);
(void)slim_v_set_95form;
if (slim_v_set_95form) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
slim_result = slim_fn_check_95find_95unknown_95pair(slim_v_source, slim_v_tokens, slim_v_value, slim_v_body, slim_allocation_region);
}
}
}
else {
slim_result = slim_fn_check_95find_95unknown_95data(slim_v_source, slim_v_tokens, slim_v_expr, slim_v_head, slim_allocation_region);
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95unknown_95expr(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_expr, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_atom = {0};
slim_v_atom = slim_v_kind == INT64_C(2);
(void)slim_v_atom;
{
bool slim_v_string_95value = {0};
slim_v_string_95value = slim_v_kind == INT64_C(3);
(void)slim_v_string_95value;
{
bool slim_v_scalar = {0};
slim_v_scalar = slim_v_atom || slim_v_string_95value;
(void)slim_v_scalar;
if (slim_v_scalar) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_expr, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_known = {0};
slim_v_known = slim_fn_check_95known_95expr_95head(slim_v_source, slim_v_tokens, slim_v_head, slim_allocation_region);
(void)slim_v_known;
if (slim_v_known) {
slim_result = slim_fn_check_95find_95unknown_95known(slim_v_source, slim_v_tokens, slim_v_expr, slim_v_head, slim_allocation_region);
}
else {
slim_result = slim_v_head;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95unknown_95forms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_unknown = {0};
slim_v_unknown = slim_fn_check_95find_95unknown_95expr(slim_v_source, slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_unknown;
{
bool slim_v_found = {0};
slim_v_found = slim_v_unknown >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_unknown;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_14289 = {0};
slim_t_14289 = slim_v_source;
int64_t slim_t_14291 = {0};
slim_t_14291 = slim_v_next;
slim_v_source = slim_t_14289;
slim_v_cursor = slim_t_14291;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95unknown_95fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_value;
{
int64_t slim_v_unknown = {0};
slim_v_unknown = slim_fn_check_95find_95unknown_95expr(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_unknown;
{
bool slim_v_found = {0};
slim_v_found = slim_v_unknown >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_unknown;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_14410 = {0};
slim_t_14410 = slim_v_source;
int64_t slim_t_14412 = {0};
slim_t_14412 = slim_v_next;
slim_v_source = slim_t_14410;
slim_v_cursor = slim_t_14412;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95unknown_95arms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_first;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_check_95last_95form_95in_95list(slim_v_tokens, slim_v_first, slim_v_first, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_unknown = {0};
slim_v_unknown = slim_fn_check_95find_95unknown_95expr(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_unknown;
{
bool slim_v_found = {0};
slim_v_found = slim_v_unknown >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_unknown;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_14543 = {0};
slim_t_14543 = slim_v_source;
int64_t slim_t_14545 = {0};
slim_t_14545 = slim_v_next;
slim_v_source = slim_t_14543;
slim_v_cursor = slim_t_14545;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95find_95unknown_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_count = {0};
slim_v_count = ((*slim_v_declarations)).len;
(void)slim_v_count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_ir_95Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_95Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_v_declaration.slim_field_form;
(void)slim_v_cursor;
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
int64_t slim_v_unknown = {0};
if (slim_v_function_95item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
slim_v_unknown = slim_fn_check_95find_95unknown_95expr(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
}
}
}
}
}
else {
slim_v_unknown = INT64_C(-1);
}
(void)slim_v_unknown;
{
bool slim_v_found = {0};
slim_v_found = slim_v_unknown >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_unknown;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_14749 = {0};
slim_t_14749 = slim_v_source;
int64_t slim_t_14752 = {0};
slim_t_14752 = slim_v_next;
slim_v_source = slim_t_14749;
slim_v_index = slim_t_14752;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_check_95report_95diagnostic(SlimBytes slim_v_code, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_code;
(void)slim_v_start;
(void)slim_v_end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
Slim_type_check_95Diagnostic slim_v_diagnostic = {0};
{
SlimBytes slim_t_14798 = {0};
slim_t_14798 = slim_v_code;
int64_t slim_t_14802 = {0};
slim_t_14802 = slim_v_start;
int64_t slim_t_14806 = {0};
slim_t_14806 = slim_v_end;
slim_v_diagnostic = (Slim_type_check_95Diagnostic){.slim_field_code = slim_t_14798, .slim_field_start = slim_t_14802, .slim_field_end = slim_t_14806};
}
(void)slim_v_diagnostic;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_rendered = {0};
slim_v_rendered = slim_fn_check_95append_95diagnostic_95identity(slim_v_diagnostic, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_rendered;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(1);
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_check_95check_95path_95full(SlimBytes slim_v_input, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_lexical = {0};
slim_v_lexical = slim_fn_check_95check_95path(slim_v_input, slim_v_tokens, slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_lexical;
{
bool slim_v_lexical_95error = {0};
slim_v_lexical_95error = slim_v_lexical > INT64_C(0);
(void)slim_v_lexical_95error;
if (slim_v_lexical_95error) {
slim_result = slim_v_lexical;
}
else {
{
int64_t slim_v_unknown = {0};
slim_v_unknown = slim_fn_check_95find_95unknown_95items(slim_v_input, slim_v_tokens, slim_v_declarations, INT64_C(0), slim_allocation_region);
(void)slim_v_unknown;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_unknown >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_unknown, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_unknown, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_check_95report_95diagnostic(slim_bytes_static((const uint8_t *)"E0221", (int64_t)(sizeof("E0221") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95value_95address(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_value, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
if (slim_v_form) {
{
SlimUnit slim_v_address = {0};
slim_v_address = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"&", (int64_t)(sizeof("&") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_address;
slim_result = slim_fn_codegen_95emit_95temp_95name(slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
bool slim_v_true_95value = {0};
slim_v_true_95value = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_value, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_true_95value;
{
bool slim_v_false_95value = {0};
slim_v_false_95value = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_value, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_false_95value;
{
bool slim_v_boolean = {0};
slim_v_boolean = slim_v_true_95value || slim_v_false_95value;
(void)slim_v_boolean;
if (slim_v_boolean) {
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"&(bool){", (int64_t)(sizeof("&(bool){") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"}", (int64_t)(sizeof("}") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
bool slim_v_number = {0};
slim_v_number = slim_fn_syntax_95first_95byte_95is_95number(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_number;
if (slim_v_number) {
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"&(int64_t){", (int64_t)(sizeof("&(int64_t){") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"}", (int64_t)(sizeof("}") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = slim_fn_codegen_95emit_95binding_95address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_95emit_95vec_95set_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_set_95call = {0};
slim_v_set_95call = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.set", (int64_t)(sizeof("vec.set") - 1)), slim_allocation_region);
(void)slim_v_set_95call;
if (slim_v_set_95call) {
{
int64_t slim_v_index = {0};
slim_v_index = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_index;
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_value;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_vec_set(", (int64_t)(sizeof("slim_vec_set(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_vector_95address = {0};
slim_v_vector_95address = slim_fn_codegen_95emit_95binding_95address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_vector_95address;
{
SlimUnit slim_v_first_95comma = {0};
slim_v_first_95comma = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_first_95comma;
{
SlimUnit slim_v_index_95value = {0};
slim_v_index_95value = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_index_95value;
{
SlimUnit slim_v_second_95comma = {0};
slim_v_second_95comma = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_second_95comma;
{
SlimUnit slim_v_value_95address = {0};
slim_v_value_95address = slim_fn_codegen_95emit_95value_95address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value_95address;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"); ", (int64_t)(sizeof("); ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = (SlimUnit){0};\n", (int64_t)(sizeof(" = (SlimUnit){0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
}
}
}
else {
slim_result = false;
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_95emit_95arena_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_95type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_new_95call = {0};
slim_v_new_95call = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.new", (int64_t)(sizeof("arena.new") - 1)), slim_allocation_region);
(void)slim_v_new_95call;
if (slim_v_new_95call) {
{
int64_t slim_v_inner_95type = {0};
slim_v_inner_95type = slim_i64_add(slim_v_destination_95type, INT64_C(2));
(void)slim_v_inner_95type;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = slim_vec_new(sizeof(", (int64_t)(sizeof(" = slim_vec_new(sizeof(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_inner_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"), slim_allocation_region);\n", (int64_t)(sizeof("), slim_allocation_region);\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
else {
{
bool slim_v_add_95call = {0};
slim_v_add_95call = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.add", (int64_t)(sizeof("arena.add") - 1)), slim_allocation_region);
(void)slim_v_add_95call;
if (slim_v_add_95call) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_value;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = 0;\nif (!slim_arena_add(", (int64_t)(sizeof(" = 0;\nif (!slim_arena_add(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_arena_95address = {0};
slim_v_arena_95address = slim_fn_codegen_95emit_95binding_95address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_arena_95address;
{
SlimUnit slim_v_comma = {0};
slim_v_comma = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comma;
{
SlimUnit slim_v_value_95address = {0};
slim_v_value_95address = slim_fn_codegen_95emit_95value_95address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value_95address;
{
SlimUnit slim_v_output_95comma = {0};
slim_v_output_95comma = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", &", (int64_t)(sizeof(", &") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_output_95comma;
{
SlimUnit slim_v_output_95dest = {0};
slim_v_output_95dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_output_95dest;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")) goto slim_allocation_failed;\n", (int64_t)(sizeof(")) goto slim_allocation_failed;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
}
else {
{
bool slim_v_get_95call = {0};
slim_v_get_95call = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.get", (int64_t)(sizeof("arena.get") - 1)), slim_allocation_region);
(void)slim_v_get_95call;
if (slim_v_get_95call) {
{
int64_t slim_v_identity = {0};
slim_v_identity = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_identity;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_cast_95prefix = {0};
slim_v_cast_95prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = ((", (int64_t)(sizeof(" = ((") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cast_95prefix;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_cast_95suffix = {0};
slim_v_cast_95suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" *)(", (int64_t)(sizeof(" *)(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cast_95suffix;
{
SlimUnit slim_v_arena_95data = {0};
slim_v_arena_95data = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_arena_95data;
{
SlimUnit slim_v_check_95prefix = {0};
slim_v_check_95prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)").data)[slim_vec_check_index(&(", (int64_t)(sizeof(").data)[slim_vec_check_index(&(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_check_95prefix;
{
SlimUnit slim_v_arena_95check = {0};
slim_v_arena_95check = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_arena_95check;
{
SlimUnit slim_v_comma = {0};
slim_v_comma = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"), ", (int64_t)(sizeof("), ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comma;
{
SlimUnit slim_v_identity_95value = {0};
slim_v_identity_95value = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_identity, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_identity_95value;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")];\n", (int64_t)(sizeof(")];\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
}
}
}
else {
slim_result = false;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_95emit_95vec_95push_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_push_95call = {0};
slim_v_push_95call = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.push", (int64_t)(sizeof("vec.push") - 1)), slim_allocation_region);
(void)slim_v_push_95call;
if (!slim_v_push_95call) {
slim_result = false;
}
else {
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_value;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"if (!slim_vec_push(", (int64_t)(sizeof("if (!slim_vec_push(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_address = {0};
slim_v_address = slim_fn_codegen_95emit_95binding_95address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_address;
{
SlimUnit slim_v_comma = {0};
slim_v_comma = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comma;
{
SlimUnit slim_v_value_95address = {0};
slim_v_value_95address = slim_fn_codegen_95emit_95value_95address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value_95address;
{
SlimUnit slim_v_close = {0};
slim_v_close = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")) goto slim_allocation_failed; ", (int64_t)(sizeof(")) goto slim_allocation_failed; ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_close;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = (SlimUnit){0};\n", (int64_t)(sizeof(" = (SlimUnit){0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_95emit_95collection_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_95type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_set_95call = {0};
slim_v_set_95call = slim_fn_codegen_95emit_95vec_95set_95call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_set_95call;
if (slim_v_set_95call) {
slim_result = true;
}
else {
{
bool slim_v_push_95call = {0};
slim_v_push_95call = slim_fn_codegen_95emit_95vec_95push_95call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_push_95call;
if (slim_v_push_95call) {
slim_result = true;
}
else {
{
bool slim_v_vector_95call = {0};
slim_v_vector_95call = slim_fn_codegen_95emit_95vector_95call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_vector_95call;
if (slim_v_vector_95call) {
slim_result = true;
}
else {
slim_result = slim_fn_codegen_95emit_95arena_95call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95let_95full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_95type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_type_95index = {0};
slim_v_type_95index = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_type_95index;
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"{\n", (int64_t)(sizeof("{\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95source_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = {0};\n", (int64_t)(sizeof(" = {0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
SlimUnit slim_v_value_95emitted = {0};
slim_v_value_95emitted = slim_fn_codegen_95emit_95expr_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_value, slim_v_name, false, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value_95emitted;
{
SlimUnit slim_v_unused = {0};
slim_v_unused = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(void)", (int64_t)(sizeof("(void)") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_unused;
{
SlimUnit slim_v_unused_95name = {0};
slim_v_unused_95name = slim_fn_codegen_95emit_95source_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_unused_95name;
{
SlimUnit slim_v_unused_95end = {0};
slim_v_unused_95end = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_unused_95end;
{
SlimUnit slim_v_body_95emitted = {0};
slim_v_body_95emitted = slim_fn_codegen_95emit_95expr_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_body, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_body_95emitted;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"}\n", (int64_t)(sizeof("}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95match_95value_95binding(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_value, bool slim_v_boolean_95match, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_boolean_95match;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
bool slim_v_computed_95boolean = {0};
slim_v_computed_95boolean = slim_v_form && slim_v_boolean_95match;
(void)slim_v_computed_95boolean;
if (!slim_v_computed_95boolean) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, INT64_C(-2), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95temp_95name(slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = {0};\n", (int64_t)(sizeof(" = {0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
int64_t slim_v_temporary = {0};
slim_v_temporary = slim_i64_sub(INT64_C(0), slim_v_value);
(void)slim_v_temporary;
slim_result = slim_fn_codegen_95emit_95expr_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_value, slim_v_temporary, false, INT64_C(-2), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95match_95full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_95type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_value;
{
int64_t slim_v_first_95arm = {0};
slim_v_first_95arm = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_first_95arm;
{
int64_t slim_v_first_95pattern = {0};
slim_v_first_95pattern = slim_i64_add(slim_v_first_95arm, INT64_C(1));
(void)slim_v_first_95pattern;
{
bool slim_v_first_95true = {0};
slim_v_first_95true = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_first_95pattern, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_first_95true;
{
bool slim_v_first_95false = {0};
slim_v_first_95false = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_first_95pattern, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_first_95false;
{
bool slim_v_boolean_95match = {0};
slim_v_boolean_95match = slim_v_first_95true || slim_v_first_95false;
(void)slim_v_boolean_95match;
{
SlimUnit slim_v_bound = {0};
slim_v_bound = slim_fn_codegen_95emit_95match_95value_95binding(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_value, slim_v_boolean_95match, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_bound;
if (slim_v_boolean_95match) {
slim_result = slim_fn_codegen_95emit_95boolean_95match(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_value, slim_v_first_95arm, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_codegen_95emit_95variant_95match(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_value, slim_v_first_95arm, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95set_95full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_95type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95source_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_assigned = {0};
slim_v_assigned = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_assigned;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = slim_fn_codegen_95emit_95expr_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_body, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95expr_95full(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_95type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_expr, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
if (!slim_v_form) {
slim_result = slim_fn_codegen_95emit_95atom_95assignment(slim_v_source, slim_v_tokens, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_expr, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_let_95form = {0};
slim_v_let_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
(void)slim_v_let_95form;
if (slim_v_let_95form) {
slim_result = slim_fn_codegen_95emit_95let_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_call_95form = {0};
slim_v_call_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
(void)slim_v_call_95form;
if (slim_v_call_95form) {
slim_result = slim_fn_codegen_95emit_95call(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_match_95form = {0};
slim_v_match_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"match", (int64_t)(sizeof("match") - 1)), slim_allocation_region);
(void)slim_v_match_95form;
if (slim_v_match_95form) {
slim_result = slim_fn_codegen_95emit_95match_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_recur_95form = {0};
slim_v_recur_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"recur", (int64_t)(sizeof("recur") - 1)), slim_allocation_region);
(void)slim_v_recur_95form;
if (slim_v_recur_95form) {
{
int64_t slim_v_arguments = {0};
slim_v_arguments = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_arguments;
{
int64_t slim_v_first_95param = {0};
slim_v_first_95param = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_95param;
slim_result = slim_fn_codegen_95emit_95recur_95assignments(slim_v_source, slim_v_tokens, slim_v_params, slim_v_first_95param, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
bool slim_v_set_95form = {0};
slim_v_set_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"set", (int64_t)(sizeof("set") - 1)), slim_allocation_region);
(void)slim_v_set_95form;
if (slim_v_set_95form) {
slim_result = slim_fn_codegen_95emit_95set_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_make_95form = {0};
slim_v_make_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"make", (int64_t)(sizeof("make") - 1)), slim_allocation_region);
(void)slim_v_make_95form;
if (slim_v_make_95form) {
slim_result = slim_fn_codegen_95emit_95make(slim_v_source, slim_v_tokens, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_get_95form = {0};
slim_v_get_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"get", (int64_t)(sizeof("get") - 1)), slim_allocation_region);
(void)slim_v_get_95form;
if (slim_v_get_95form) {
slim_result = slim_fn_codegen_95emit_95get(slim_v_source, slim_v_tokens, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_case_95form = {0};
slim_v_case_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"case", (int64_t)(sizeof("case") - 1)), slim_allocation_region);
(void)slim_v_case_95form;
if (slim_v_case_95form) {
slim_result = slim_fn_codegen_95emit_95case_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_expr, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_rt_trap(\"unsupported bootstrap expression\");\n", (int64_t)(sizeof("slim_rt_trap(\"unsupported bootstrap expression\");\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95mangled_95span(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_lower_95start = {0};
slim_v_lower_95start = slim_v_code >= INT64_C(97);
(void)slim_v_lower_95start;
{
bool slim_v_lower_95end = {0};
slim_v_lower_95end = slim_v_code <= INT64_C(122);
(void)slim_v_lower_95end;
{
bool slim_v_lower = {0};
slim_v_lower = slim_v_lower_95start && slim_v_lower_95end;
(void)slim_v_lower;
{
bool slim_v_upper_95start = {0};
slim_v_upper_95start = slim_v_code >= INT64_C(65);
(void)slim_v_upper_95start;
{
bool slim_v_upper_95end = {0};
slim_v_upper_95end = slim_v_code <= INT64_C(90);
(void)slim_v_upper_95end;
{
bool slim_v_upper = {0};
slim_v_upper = slim_v_upper_95start && slim_v_upper_95end;
(void)slim_v_upper;
{
bool slim_v_digit_95start = {0};
slim_v_digit_95start = slim_v_code >= INT64_C(48);
(void)slim_v_digit_95start;
{
bool slim_v_digit_95end = {0};
slim_v_digit_95end = slim_v_code <= INT64_C(57);
(void)slim_v_digit_95end;
{
bool slim_v_digit = {0};
slim_v_digit = slim_v_digit_95start && slim_v_digit_95end;
(void)slim_v_digit;
{
bool slim_v_letter = {0};
slim_v_letter = slim_v_lower || slim_v_upper;
(void)slim_v_letter;
{
bool slim_v_safe = {0};
slim_v_safe = slim_v_letter || slim_v_digit;
(void)slim_v_safe;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_safe) {
if (!slim_vec_push(slim_v_output, &slim_v_byte)) goto slim_allocation_failed; slim_v_emitted = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_marker = {0};
slim_v_marker = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"_", (int64_t)(sizeof("_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_marker;
slim_v_emitted = slim_fn_text_95append_95nonnegative_95i64(slim_v_code, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_17564 = {0};
slim_t_17564 = slim_v_source;
int64_t slim_t_17565 = {0};
slim_t_17565 = slim_v_next;
int64_t slim_t_17566 = {0};
slim_t_17566 = slim_v_end;
slim_v_source = slim_t_17564;
slim_v_index = slim_t_17565;
slim_v_end = slim_t_17566;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95mangled_95token(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_codegen_95emit_95mangled_95span(slim_v_source, slim_v_start, slim_v_end, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95source_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_v_", (int64_t)(sizeof("slim_v_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
slim_result = slim_fn_codegen_95emit_95mangled_95token(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95function_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_fn_", (int64_t)(sizeof("slim_fn_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
slim_result = slim_fn_codegen_95emit_95mangled_95token(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95named_95type_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"Slim_type_", (int64_t)(sizeof("Slim_type_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
slim_result = slim_fn_codegen_95emit_95mangled_95token(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95field_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_field_", (int64_t)(sizeof("slim_field_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
slim_result = slim_fn_codegen_95emit_95mangled_95token(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95case_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_case_", (int64_t)(sizeof("slim_case_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
slim_result = slim_fn_codegen_95emit_95mangled_95token(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95tag_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_variant, int64_t slim_v_case_95name, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_variant;
(void)slim_v_case_95name;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"SLIM_TAG_", (int64_t)(sizeof("SLIM_TAG_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_variant_95name = {0};
slim_v_variant_95name = slim_fn_codegen_95emit_95mangled_95token(slim_v_source, slim_v_tokens, slim_v_variant, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_variant_95name;
{
SlimUnit slim_v_separator = {0};
slim_v_separator = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"_", (int64_t)(sizeof("_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_separator;
slim_result = slim_fn_codegen_95emit_95mangled_95token(slim_v_source, slim_v_tokens, slim_v_case_95name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95source_95type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_is_95unit = {0};
slim_v_is_95unit = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_index, slim_bytes_static((const uint8_t *)"Unit", (int64_t)(sizeof("Unit") - 1)), slim_allocation_region);
(void)slim_v_is_95unit;
if (slim_v_is_95unit) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimUnit", (int64_t)(sizeof("SlimUnit") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_is_95bool = {0};
slim_v_is_95bool = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_index, slim_bytes_static((const uint8_t *)"Bool", (int64_t)(sizeof("Bool") - 1)), slim_allocation_region);
(void)slim_v_is_95bool;
if (slim_v_is_95bool) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"bool", (int64_t)(sizeof("bool") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_is_95u8 = {0};
slim_v_is_95u8 = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_index, slim_bytes_static((const uint8_t *)"U8", (int64_t)(sizeof("U8") - 1)), slim_allocation_region);
(void)slim_v_is_95u8;
if (slim_v_is_95u8) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"uint8_t", (int64_t)(sizeof("uint8_t") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_is_95i64 = {0};
slim_v_is_95i64 = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_index, slim_bytes_static((const uint8_t *)"I64", (int64_t)(sizeof("I64") - 1)), slim_allocation_region);
(void)slim_v_is_95i64;
if (slim_v_is_95i64) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"int64_t", (int64_t)(sizeof("int64_t") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_is_95bytes = {0};
slim_v_is_95bytes = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_index, slim_bytes_static((const uint8_t *)"Bytes", (int64_t)(sizeof("Bytes") - 1)), slim_allocation_region);
(void)slim_v_is_95bytes;
if (slim_v_is_95bytes) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimBytes", (int64_t)(sizeof("SlimBytes") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_composite = {0};
slim_v_composite = slim_v_kind == INT64_C(0);
(void)slim_v_composite;
if (slim_v_composite) {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_id_95type = {0};
slim_v_id_95type = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"Id", (int64_t)(sizeof("Id") - 1)), slim_allocation_region);
(void)slim_v_id_95type;
if (slim_v_id_95type) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimId", (int64_t)(sizeof("SlimId") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimVec", (int64_t)(sizeof("SlimVec") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
else {
slim_result = slim_fn_codegen_95emit_95named_95type_95name(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_i64_95type = {0};
slim_v_i64_95type = slim_v_index == INT64_C(-1);
(void)slim_v_i64_95type;
if (slim_v_i64_95type) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"int64_t", (int64_t)(sizeof("int64_t") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_bool_95type = {0};
slim_v_bool_95type = slim_v_index == INT64_C(-2);
(void)slim_v_bool_95type;
if (slim_v_bool_95type) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"bool", (int64_t)(sizeof("bool") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_u8_95type = {0};
slim_v_u8_95type = slim_v_index == INT64_C(-3);
(void)slim_v_u8_95type;
if (slim_v_u8_95type) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"uint8_t", (int64_t)(sizeof("uint8_t") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_unit_95type = {0};
slim_v_unit_95type = slim_v_index == INT64_C(-4);
(void)slim_v_unit_95type;
if (slim_v_unit_95type) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimUnit", (int64_t)(sizeof("SlimUnit") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_bytes_95type = {0};
slim_v_bytes_95type = slim_v_index == INT64_C(-5);
(void)slim_v_bytes_95type;
if (slim_v_bytes_95type) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimBytes", (int64_t)(sizeof("SlimBytes") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_byte_95vector = {0};
slim_v_byte_95vector = slim_v_index == INT64_C(-6);
(void)slim_v_byte_95vector;
if (slim_v_byte_95vector) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimVec", (int64_t)(sizeof("SlimVec") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_codegen_95emit_95source_95type(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95forward_95types(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"\n", (int64_t)(sizeof("\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_record_95item = {0};
slim_v_record_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_95item;
{
bool slim_v_variant_95item = {0};
slim_v_variant_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
(void)slim_v_variant_95item;
{
bool slim_v_data_95item = {0};
slim_v_data_95item = slim_v_record_95item || slim_v_variant_95item;
(void)slim_v_data_95item;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_data_95item) {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"typedef struct ", (int64_t)(sizeof("typedef struct ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_first_95name = {0};
slim_v_first_95name = slim_fn_codegen_95emit_95named_95type_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_first_95name;
{
SlimUnit slim_v_separator = {0};
slim_v_separator = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_separator;
{
SlimUnit slim_v_second_95name = {0};
slim_v_second_95name = slim_fn_codegen_95emit_95named_95type_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_second_95name;
slim_v_emitted = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
else {
slim_v_emitted = (SlimUnit){0};
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_18680 = {0};
slim_t_18680 = slim_v_source;
int64_t slim_t_18682 = {0};
slim_t_18682 = slim_v_next;
slim_v_source = slim_t_18680;
slim_v_cursor = slim_t_18682;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95record_95fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
if (slim_v_first) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimUnit slim_unit;\n", (int64_t)(sizeof("SlimUnit slim_unit;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = (SlimUnit){0};
}
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_name;
{
int64_t slim_v_type_95index = {0};
slim_v_type_95index = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_type_95index;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_separated = {0};
slim_v_separated = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_separated;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95field_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_terminated = {0};
slim_v_terminated = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_terminated;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_18855 = {0};
slim_t_18855 = slim_v_source;
int64_t slim_t_18857 = {0};
slim_t_18857 = slim_v_next;
bool slim_t_18858 = {0};
slim_t_18858 = false;
slim_v_source = slim_t_18855;
slim_v_cursor = slim_t_18857;
slim_v_first = slim_t_18858;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95record(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_item;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_item, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_fields = {0};
slim_v_fields = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_fields;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"struct ", (int64_t)(sizeof("struct ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95named_95type_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" {\n", (int64_t)(sizeof(" {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
int64_t slim_v_first_95field = {0};
slim_v_first_95field = slim_i64_add(slim_v_fields, INT64_C(1));
(void)slim_v_first_95field;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95record_95fields(slim_v_source, slim_v_tokens, slim_v_first_95field, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"};\n\n", (int64_t)(sizeof("};\n\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95variant_95tags(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_variant_95name, int64_t slim_v_cursor, int64_t slim_v_tag, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_variant_95name;
(void)slim_v_cursor;
(void)slim_v_tag;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_case_95name = {0};
slim_v_case_95name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_case_95name;
{
SlimUnit slim_v_define = {0};
slim_v_define = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"#define ", (int64_t)(sizeof("#define ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_define;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95tag_95name(slim_v_source, slim_v_tokens, slim_v_variant_95name, slim_v_case_95name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_cast = {0};
slim_v_cast = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ((int32_t)", (int64_t)(sizeof(" ((int32_t)") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cast;
{
SlimUnit slim_v_number = {0};
slim_v_number = slim_fn_text_95append_95nonnegative_95i64(slim_v_tag, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_number;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")\n", (int64_t)(sizeof(")\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_next_95tag = {0};
slim_v_next_95tag = slim_i64_add(slim_v_tag, INT64_C(1));
(void)slim_v_next_95tag;
SlimBytes slim_t_19154 = {0};
slim_t_19154 = slim_v_source;
int64_t slim_t_19156 = {0};
slim_t_19156 = slim_v_variant_95name;
int64_t slim_t_19157 = {0};
slim_t_19157 = slim_v_next;
int64_t slim_t_19158 = {0};
slim_t_19158 = slim_v_next_95tag;
slim_v_source = slim_t_19154;
slim_v_variant_95name = slim_t_19156;
slim_v_cursor = slim_t_19157;
slim_v_tag = slim_t_19158;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95variant_95payload_95fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_field, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_field;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" slim_", (int64_t)(sizeof(" slim_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_number = {0};
slim_v_number = slim_fn_text_95append_95nonnegative_95i64(slim_v_field, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_number;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_next_95field = {0};
slim_v_next_95field = slim_i64_add(slim_v_field, INT64_C(1));
(void)slim_v_next_95field;
SlimBytes slim_t_19306 = {0};
slim_t_19306 = slim_v_source;
int64_t slim_t_19308 = {0};
slim_t_19308 = slim_v_next;
int64_t slim_t_19309 = {0};
slim_t_19309 = slim_v_next_95field;
slim_v_source = slim_t_19306;
slim_v_cursor = slim_t_19308;
slim_v_field = slim_t_19309;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95variant_95payloads(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_case_95name = {0};
slim_v_case_95name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_case_95name;
{
int64_t slim_v_payload = {0};
slim_v_payload = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_payload;
{
int64_t slim_v_payload_95kind = {0};
slim_v_payload_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_payload, slim_allocation_region);
(void)slim_v_payload_95kind;
{
bool slim_v_empty = {0};
slim_v_empty = slim_v_payload_95kind == INT64_C(1);
(void)slim_v_empty;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_empty) {
slim_v_emitted = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"struct {\n", (int64_t)(sizeof("struct {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_fields = {0};
slim_v_fields = slim_fn_codegen_95emit_95variant_95payload_95fields(slim_v_source, slim_v_tokens, slim_v_payload, INT64_C(0), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_fields;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"} ", (int64_t)(sizeof("} ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95case_95name(slim_v_source, slim_v_tokens, slim_v_case_95name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
slim_v_emitted = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_19509 = {0};
slim_t_19509 = slim_v_source;
int64_t slim_t_19511 = {0};
slim_t_19511 = slim_v_next;
slim_v_source = slim_t_19509;
slim_v_cursor = slim_t_19511;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95variant(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_item;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_item, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_cases = {0};
slim_v_cases = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_cases;
{
int64_t slim_v_first_95case = {0};
slim_v_first_95case = slim_i64_add(slim_v_cases, INT64_C(1));
(void)slim_v_first_95case;
{
SlimUnit slim_v_tags = {0};
slim_v_tags = slim_fn_codegen_95emit_95variant_95tags(slim_v_source, slim_v_tokens, slim_v_name, slim_v_first_95case, INT64_C(0), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tags;
{
SlimUnit slim_v_struct_95prefix = {0};
slim_v_struct_95prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"struct ", (int64_t)(sizeof("struct ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_struct_95prefix;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95named_95type_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" {\nint32_t tag;\nunion {\nuint8_t slim_unused;\n", (int64_t)(sizeof(" {\nint32_t tag;\nunion {\nuint8_t slim_unused;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_payloads = {0};
slim_v_payloads = slim_fn_codegen_95emit_95variant_95payloads(slim_v_source, slim_v_tokens, slim_v_first_95case, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_payloads;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"} data;\n};\n\n", (int64_t)(sizeof("} data;\n};\n\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95data_95items(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_record_95item = {0};
slim_v_record_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_95item;
{
SlimUnit slim_v_emitted_95record = {0};
if (slim_v_record_95item) {
slim_v_emitted_95record = slim_fn_codegen_95emit_95record(slim_v_source, slim_v_tokens, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_emitted_95record = (SlimUnit){0};
}
(void)slim_v_emitted_95record;
{
bool slim_v_variant_95item = {0};
slim_v_variant_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
(void)slim_v_variant_95item;
{
SlimUnit slim_v_emitted_95variant = {0};
if (slim_v_variant_95item) {
slim_v_emitted_95variant = slim_fn_codegen_95emit_95variant(slim_v_source, slim_v_tokens, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_emitted_95variant = (SlimUnit){0};
}
(void)slim_v_emitted_95variant;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_19809 = {0};
slim_t_19809 = slim_v_source;
int64_t slim_t_19811 = {0};
slim_t_19811 = slim_v_next;
slim_v_source = slim_t_19809;
slim_v_cursor = slim_t_19811;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95binding_95value(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_name, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_name;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95name_95is_95inout(slim_v_source, slim_v_tokens, slim_v_params, slim_v_name, slim_allocation_region);
(void)slim_v_borrowed;
if (slim_v_borrowed) {
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(*", (int64_t)(sizeof("(*") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95source_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = slim_fn_codegen_95emit_95source_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95binding_95address(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_name, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_name;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95name_95is_95inout(slim_v_source, slim_v_tokens, slim_v_params, slim_v_name, slim_allocation_region);
(void)slim_v_borrowed;
if (slim_v_borrowed) {
slim_result = slim_fn_codegen_95emit_95source_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimUnit slim_v_address = {0};
slim_v_address = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"&", (int64_t)(sizeof("&") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_address;
slim_result = slim_fn_codegen_95emit_95source_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_codegen_95builtin_95argument_95type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, int64_t slim_v_position, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_v_position;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_bool_95not = {0};
slim_v_bool_95not = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bool.not", (int64_t)(sizeof("bool.not") - 1)), slim_allocation_region);
(void)slim_v_bool_95not;
{
bool slim_v_bool_95and = {0};
slim_v_bool_95and = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bool.and", (int64_t)(sizeof("bool.and") - 1)), slim_allocation_region);
(void)slim_v_bool_95and;
{
bool slim_v_bool_95or = {0};
slim_v_bool_95or = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bool.or", (int64_t)(sizeof("bool.or") - 1)), slim_allocation_region);
(void)slim_v_bool_95or;
{
bool slim_v_bool_95first = {0};
slim_v_bool_95first = slim_v_bool_95not || slim_v_bool_95and;
(void)slim_v_bool_95first;
{
bool slim_v_boolean = {0};
slim_v_boolean = slim_v_bool_95first || slim_v_bool_95or;
(void)slim_v_boolean;
if (slim_v_boolean) {
slim_result = INT64_C(-2);
}
else {
{
bool slim_v_widen = {0};
slim_v_widen = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"u8.to-i64", (int64_t)(sizeof("u8.to-i64") - 1)), slim_allocation_region);
(void)slim_v_widen;
if (slim_v_widen) {
slim_result = INT64_C(-3);
}
else {
{
bool slim_v_freeze = {0};
slim_v_freeze = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.freeze", (int64_t)(sizeof("bytes.freeze") - 1)), slim_allocation_region);
(void)slim_v_freeze;
if (slim_v_freeze) {
slim_result = INT64_C(-6);
}
else {
{
bool slim_v_bytes_95get = {0};
slim_v_bytes_95get = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.get", (int64_t)(sizeof("bytes.get") - 1)), slim_allocation_region);
(void)slim_v_bytes_95get;
{
bool slim_v_first = {0};
slim_v_first = slim_v_position == INT64_C(0);
(void)slim_v_first;
{
bool slim_v_bytes_95get_95first = {0};
slim_v_bytes_95get_95first = slim_v_bytes_95get && slim_v_first;
(void)slim_v_bytes_95get_95first;
{
bool slim_v_bytes_95len = {0};
slim_v_bytes_95len = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.len", (int64_t)(sizeof("bytes.len") - 1)), slim_allocation_region);
(void)slim_v_bytes_95len;
{
bool slim_v_print_95bytes = {0};
slim_v_print_95bytes = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.print-bytes", (int64_t)(sizeof("io.print-bytes") - 1)), slim_allocation_region);
(void)slim_v_print_95bytes;
{
bool slim_v_println = {0};
slim_v_println = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.println", (int64_t)(sizeof("io.println") - 1)), slim_allocation_region);
(void)slim_v_println;
{
bool slim_v_read_95file = {0};
slim_v_read_95file = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.read-file", (int64_t)(sizeof("io.read-file") - 1)), slim_allocation_region);
(void)slim_v_read_95file;
{
bool slim_v_bytes_95one = {0};
slim_v_bytes_95one = slim_v_bytes_95get_95first || slim_v_bytes_95len;
(void)slim_v_bytes_95one;
{
bool slim_v_bytes_95two = {0};
slim_v_bytes_95two = slim_v_print_95bytes || slim_v_println;
(void)slim_v_bytes_95two;
{
bool slim_v_bytes_95three = {0};
slim_v_bytes_95three = slim_v_bytes_95two || slim_v_read_95file;
(void)slim_v_bytes_95three;
{
bool slim_v_bytes_95value = {0};
slim_v_bytes_95value = slim_v_bytes_95one || slim_v_bytes_95three;
(void)slim_v_bytes_95value;
if (slim_v_bytes_95value) {
slim_result = INT64_C(-5);
}
else {
slim_result = INT64_C(-1);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_codegen_95parameter_95type_95index(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_parameter, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_parameter;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_parameter, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
if (slim_v_borrowed) {
slim_result = slim_i64_add(slim_v_parameter, INT64_C(3));
}
else {
slim_result = slim_i64_add(slim_v_parameter, INT64_C(2));
}
}
}
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95call_95argument_95bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_argument, int64_t slim_v_callee_95param, int64_t slim_v_position, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_argument;
(void)slim_v_callee_95param;
(void)slim_v_position;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
bool slim_v_user = {0};
slim_v_user = slim_v_callee_95param >= INT64_C(0);
(void)slim_v_user;
{
int64_t slim_v_type_95index = {0};
if (slim_v_user) {
slim_v_type_95index = slim_fn_codegen_95parameter_95type_95index(slim_v_source, slim_v_tokens, slim_v_callee_95param, slim_allocation_region);
}
else {
slim_v_type_95index = slim_fn_codegen_95builtin_95argument_95type(slim_v_source, slim_v_tokens, slim_v_callee, slim_v_position, slim_allocation_region);
}
(void)slim_v_type_95index;
{
SlimUnit slim_v_emitted = {0};
if (!slim_v_form) {
slim_v_emitted = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95temp_95name(slim_v_argument, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = {0};\n", (int64_t)(sizeof(" = {0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
int64_t slim_v_destination = {0};
slim_v_destination = slim_i64_sub(INT64_C(0), slim_v_argument);
(void)slim_v_destination;
slim_v_emitted = slim_fn_codegen_95emit_95expr_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_argument, slim_v_destination, false, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
(void)slim_v_emitted;
{
int64_t slim_v_next_95argument = {0};
slim_v_next_95argument = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_95argument;
{
int64_t slim_v_next_95param = {0};
if (slim_v_user) {
slim_v_next_95param = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_callee_95param, slim_allocation_region);
}
else {
slim_v_next_95param = INT64_C(-1);
}
(void)slim_v_next_95param;
{
int64_t slim_v_next_95position = {0};
slim_v_next_95position = slim_i64_add(slim_v_position, INT64_C(1));
(void)slim_v_next_95position;
SlimBytes slim_t_20643 = {0};
slim_t_20643 = slim_v_source;
int64_t slim_t_20645 = {0};
slim_t_20645 = slim_v_module_95items;
int64_t slim_t_20646 = {0};
slim_t_20646 = slim_v_params;
int64_t slim_t_20647 = {0};
slim_t_20647 = slim_v_callee;
int64_t slim_t_20648 = {0};
slim_t_20648 = slim_v_next_95argument;
int64_t slim_t_20649 = {0};
slim_t_20649 = slim_v_next_95param;
int64_t slim_t_20650 = {0};
slim_t_20650 = slim_v_next_95position;
slim_v_source = slim_t_20643;
slim_v_module_95items = slim_t_20645;
slim_v_params = slim_t_20646;
slim_v_callee = slim_t_20647;
slim_v_argument = slim_t_20648;
slim_v_callee_95param = slim_t_20649;
slim_v_position = slim_t_20650;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95call_95bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_callee_95start = {0};
slim_v_callee_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_95start;
{
int64_t slim_v_callee_95end = {0};
slim_v_callee_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_95end;
{
int64_t slim_v_callee_95params = {0};
slim_v_callee_95params = slim_fn_syntax_95find_95function_95params(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_callee_95start, slim_v_callee_95end, slim_allocation_region);
(void)slim_v_callee_95params;
{
bool slim_v_found = {0};
slim_v_found = slim_v_callee_95params >= INT64_C(0);
(void)slim_v_found;
{
int64_t slim_v_first_95param = {0};
if (slim_v_found) {
slim_v_first_95param = slim_i64_add(slim_v_callee_95params, INT64_C(1));
}
else {
slim_v_first_95param = INT64_C(-1);
}
(void)slim_v_first_95param;
slim_result = slim_fn_codegen_95emit_95call_95argument_95bindings(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_first_95param, INT64_C(0), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95value(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_value, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
if (slim_v_form) {
slim_result = slim_fn_codegen_95emit_95temp_95name(slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_string_95value = {0};
slim_v_string_95value = slim_v_kind == INT64_C(3);
(void)slim_v_string_95value;
if (slim_v_string_95value) {
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_bytes_static((const uint8_t *)", (int64_t)(sizeof("slim_bytes_static((const uint8_t *)") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_first = {0};
slim_v_first = slim_fn_text_95append_95token(slim_v_source, slim_v_tokens, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_first;
{
SlimUnit slim_v_middle = {0};
slim_v_middle = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", (int64_t)(sizeof(", (int64_t)(sizeof(", (int64_t)(sizeof(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_middle;
{
SlimUnit slim_v_second = {0};
slim_v_second = slim_fn_text_95append_95token(slim_v_source, slim_v_tokens, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_second;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)") - 1))", (int64_t)(sizeof(") - 1))") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
else {
{
bool slim_v_is_95true = {0};
slim_v_is_95true = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_value, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_is_95true;
if (slim_v_is_95true) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_is_95false = {0};
slim_v_is_95false = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_value, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
(void)slim_v_is_95false;
if (slim_v_is_95false) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"false", (int64_t)(sizeof("false") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_is_95unit = {0};
slim_v_is_95unit = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_value, slim_bytes_static((const uint8_t *)"unit", (int64_t)(sizeof("unit") - 1)), slim_allocation_region);
(void)slim_v_is_95unit;
if (slim_v_is_95unit) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(SlimUnit){0}", (int64_t)(sizeof("(SlimUnit){0}") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_number = {0};
slim_v_number = slim_fn_syntax_95first_95byte_95is_95number(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_number;
if (slim_v_number) {
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"INT64_C(", (int64_t)(sizeof("INT64_C(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_literal = {0};
slim_v_literal = slim_fn_text_95append_95token(slim_v_source, slim_v_tokens, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_literal;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = slim_fn_codegen_95emit_95binding_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimBytes slim_fn_codegen_95runtime_95math_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
{
bool slim_v_add = {0};
slim_v_add = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.add", (int64_t)(sizeof("i64.add") - 1)), slim_allocation_region);
(void)slim_v_add;
if (slim_v_add) {
slim_result = slim_bytes_static((const uint8_t *)"slim_i64_add", (int64_t)(sizeof("slim_i64_add") - 1));
}
else {
{
bool slim_v_sub = {0};
slim_v_sub = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.sub", (int64_t)(sizeof("i64.sub") - 1)), slim_allocation_region);
(void)slim_v_sub;
if (slim_v_sub) {
slim_result = slim_bytes_static((const uint8_t *)"slim_i64_sub", (int64_t)(sizeof("slim_i64_sub") - 1));
}
else {
{
bool slim_v_mul = {0};
slim_v_mul = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.mul", (int64_t)(sizeof("i64.mul") - 1)), slim_allocation_region);
(void)slim_v_mul;
if (slim_v_mul) {
slim_result = slim_bytes_static((const uint8_t *)"slim_i64_mul", (int64_t)(sizeof("slim_i64_mul") - 1));
}
else {
{
bool slim_v_div = {0};
slim_v_div = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.div", (int64_t)(sizeof("i64.div") - 1)), slim_allocation_region);
(void)slim_v_div;
if (slim_v_div) {
slim_result = slim_bytes_static((const uint8_t *)"slim_i64_div", (int64_t)(sizeof("slim_i64_div") - 1));
}
else {
{
bool slim_v_rem = {0};
slim_v_rem = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.rem", (int64_t)(sizeof("i64.rem") - 1)), slim_allocation_region);
(void)slim_v_rem;
if (slim_v_rem) {
slim_result = slim_bytes_static((const uint8_t *)"slim_i64_rem", (int64_t)(sizeof("slim_i64_rem") - 1));
}
else {
{
bool slim_v_to_95u8 = {0};
slim_v_to_95u8 = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.to-u8", (int64_t)(sizeof("i64.to-u8") - 1)), slim_allocation_region);
(void)slim_v_to_95u8;
if (slim_v_to_95u8) {
slim_result = slim_bytes_static((const uint8_t *)"slim_i64_to_u8", (int64_t)(sizeof("slim_i64_to_u8") - 1));
}
else {
slim_result = slim_bytes_static((const uint8_t *)"", (int64_t)(sizeof("") - 1));
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimBytes slim_fn_codegen_95runtime_95bytes_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
{
bool slim_v_bytes_95len = {0};
slim_v_bytes_95len = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.len", (int64_t)(sizeof("bytes.len") - 1)), slim_allocation_region);
(void)slim_v_bytes_95len;
if (slim_v_bytes_95len) {
slim_result = slim_bytes_static((const uint8_t *)"slim_bytes_len", (int64_t)(sizeof("slim_bytes_len") - 1));
}
else {
{
bool slim_v_bytes_95get = {0};
slim_v_bytes_95get = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.get", (int64_t)(sizeof("bytes.get") - 1)), slim_allocation_region);
(void)slim_v_bytes_95get;
if (slim_v_bytes_95get) {
slim_result = slim_bytes_static((const uint8_t *)"slim_bytes_get", (int64_t)(sizeof("slim_bytes_get") - 1));
}
else {
{
bool slim_v_freeze = {0};
slim_v_freeze = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bytes.freeze", (int64_t)(sizeof("bytes.freeze") - 1)), slim_allocation_region);
(void)slim_v_freeze;
if (slim_v_freeze) {
slim_result = slim_bytes_static((const uint8_t *)"slim_bytes_freeze", (int64_t)(sizeof("slim_bytes_freeze") - 1));
}
else {
slim_result = slim_bytes_static((const uint8_t *)"", (int64_t)(sizeof("") - 1));
}
}
}
}
}
}
return slim_result;
}

static SlimBytes slim_fn_codegen_95runtime_95io_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
{
bool slim_v_print_95i64 = {0};
slim_v_print_95i64 = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.print-i64", (int64_t)(sizeof("io.print-i64") - 1)), slim_allocation_region);
(void)slim_v_print_95i64;
if (slim_v_print_95i64) {
slim_result = slim_bytes_static((const uint8_t *)"slim_print_i64", (int64_t)(sizeof("slim_print_i64") - 1));
}
else {
{
bool slim_v_print_95bytes = {0};
slim_v_print_95bytes = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.print-bytes", (int64_t)(sizeof("io.print-bytes") - 1)), slim_allocation_region);
(void)slim_v_print_95bytes;
if (slim_v_print_95bytes) {
slim_result = slim_bytes_static((const uint8_t *)"slim_print_bytes", (int64_t)(sizeof("slim_print_bytes") - 1));
}
else {
{
bool slim_v_println = {0};
slim_v_println = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.println", (int64_t)(sizeof("io.println") - 1)), slim_allocation_region);
(void)slim_v_println;
if (slim_v_println) {
slim_result = slim_bytes_static((const uint8_t *)"slim_println", (int64_t)(sizeof("slim_println") - 1));
}
else {
slim_result = slim_bytes_static((const uint8_t *)"", (int64_t)(sizeof("") - 1));
}
}
}
}
}
}
return slim_result;
}

static SlimBytes slim_fn_codegen_95runtime_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
{
SlimBytes slim_v_math = {0};
slim_v_math = slim_fn_codegen_95runtime_95math_95name(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_math;
{
int64_t slim_v_math_95length = {0};
slim_v_math_95length = slim_bytes_len(slim_v_math);
(void)slim_v_math_95length;
{
bool slim_v_has_95math = {0};
slim_v_has_95math = slim_v_math_95length > INT64_C(0);
(void)slim_v_has_95math;
if (slim_v_has_95math) {
slim_result = slim_v_math;
}
else {
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_fn_codegen_95runtime_95bytes_95name(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_bytes;
{
int64_t slim_v_bytes_95length = {0};
slim_v_bytes_95length = slim_bytes_len(slim_v_bytes);
(void)slim_v_bytes_95length;
{
bool slim_v_has_95bytes = {0};
slim_v_has_95bytes = slim_v_bytes_95length > INT64_C(0);
(void)slim_v_has_95bytes;
if (slim_v_has_95bytes) {
slim_result = slim_v_bytes;
}
else {
slim_result = slim_fn_codegen_95runtime_95io_95name(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95value_95arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
if (slim_v_first) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_21692 = {0};
slim_t_21692 = slim_v_source;
int64_t slim_t_21694 = {0};
slim_t_21694 = slim_v_params;
int64_t slim_t_21695 = {0};
slim_t_21695 = slim_v_next;
bool slim_t_21696 = {0};
slim_t_21696 = false;
slim_v_source = slim_t_21692;
slim_v_params = slim_t_21694;
slim_v_cursor = slim_t_21695;
slim_v_first = slim_t_21696;
goto slim_recur;
}
}
}
else {
{
SlimUnit slim_v_comma = {0};
slim_v_comma = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comma;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_21739 = {0};
slim_t_21739 = slim_v_source;
int64_t slim_t_21741 = {0};
slim_t_21741 = slim_v_params;
int64_t slim_t_21742 = {0};
slim_t_21742 = slim_v_next;
bool slim_t_21743 = {0};
slim_t_21743 = false;
slim_v_source = slim_t_21739;
slim_v_params = slim_t_21741;
slim_v_cursor = slim_t_21742;
slim_v_first = slim_t_21743;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95user_95arguments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_current_95params, int64_t slim_v_argument, int64_t slim_v_callee_95param, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_current_95params;
(void)slim_v_argument;
(void)slim_v_callee_95param;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_allocation_region", (int64_t)(sizeof("slim_allocation_region") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
SlimUnit slim_v_separated = {0};
if (slim_v_first) {
slim_v_separated = (SlimUnit){0};
}
else {
slim_v_separated = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separated;
{
int64_t slim_v_param_95head = {0};
slim_v_param_95head = slim_i64_add(slim_v_callee_95param, INT64_C(1));
(void)slim_v_param_95head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_param_95head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_borrowed) {
slim_v_emitted = slim_fn_codegen_95emit_95binding_95address(slim_v_source, slim_v_tokens, slim_v_current_95params, slim_v_argument, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_emitted = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_current_95params, slim_v_argument, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_emitted;
{
int64_t slim_v_next_95argument = {0};
slim_v_next_95argument = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_95argument;
{
int64_t slim_v_next_95param = {0};
slim_v_next_95param = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_callee_95param, slim_allocation_region);
(void)slim_v_next_95param;
SlimBytes slim_t_21956 = {0};
slim_t_21956 = slim_v_source;
int64_t slim_t_21958 = {0};
slim_t_21958 = slim_v_current_95params;
int64_t slim_t_21959 = {0};
slim_t_21959 = slim_v_next_95argument;
int64_t slim_t_21960 = {0};
slim_t_21960 = slim_v_next_95param;
bool slim_t_21961 = {0};
slim_t_21961 = false;
slim_v_source = slim_t_21956;
slim_v_current_95params = slim_t_21958;
slim_v_argument = slim_t_21959;
slim_v_callee_95param = slim_t_21960;
slim_v_first = slim_t_21961;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95parameters(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimRegion *slim_region", (int64_t)(sizeof("SlimRegion *slim_region") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
SlimUnit slim_v_separated = {0};
if (slim_v_first) {
slim_v_separated = (SlimUnit){0};
}
else {
slim_v_separated = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separated;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_name = {0};
if (slim_v_borrowed) {
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
}
else {
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
}
(void)slim_v_name;
{
int64_t slim_v_type = {0};
if (slim_v_borrowed) {
slim_v_type = slim_i64_add(slim_v_cursor, INT64_C(3));
}
else {
slim_v_type = slim_i64_add(slim_v_cursor, INT64_C(2));
}
(void)slim_v_type;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_pointer = {0};
if (slim_v_borrowed) {
slim_v_pointer = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" *", (int64_t)(sizeof(" *") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_pointer = (SlimUnit){0};
}
(void)slim_v_pointer;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95source_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_22232 = {0};
slim_t_22232 = slim_v_source;
int64_t slim_t_22234 = {0};
slim_t_22234 = slim_v_next;
bool slim_t_22235 = {0};
slim_t_22235 = false;
slim_v_source = slim_t_22232;
slim_v_cursor = slim_t_22234;
slim_v_first = slim_t_22235;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95prototype(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_item;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_item, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"static ", (int64_t)(sizeof("static ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_returned = {0};
slim_v_returned = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_return_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_returned;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95function_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
int64_t slim_v_first_95param = {0};
slim_v_first_95param = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_95param;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95parameters(slim_v_source, slim_v_tokens, slim_v_first_95param, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)");\n", (int64_t)(sizeof(");\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95prototypes(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"\n", (int64_t)(sizeof("\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_function_95item) {
slim_v_emitted = slim_fn_codegen_95emit_95prototype(slim_v_source, slim_v_tokens, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_emitted = (SlimUnit){0};
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_22528 = {0};
slim_t_22528 = slim_v_source;
int64_t slim_t_22530 = {0};
slim_t_22530 = slim_v_next;
slim_v_source = slim_t_22528;
slim_v_cursor = slim_t_22530;
goto slim_recur;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_95contains_95atom(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, SlimBytes slim_v_text, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_text;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_atom = {0};
slim_v_atom = slim_v_kind == INT64_C(2);
(void)slim_v_atom;
{
bool slim_v_same = {0};
if (slim_v_atom) {
slim_v_same = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_cursor, slim_v_text, slim_allocation_region);
}
else {
slim_v_same = false;
}
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_22658 = {0};
slim_t_22658 = slim_v_source;
int64_t slim_t_22660 = {0};
slim_t_22660 = slim_v_next;
int64_t slim_t_22661 = {0};
slim_t_22661 = slim_v_end;
SlimBytes slim_t_22662 = {0};
slim_t_22662 = slim_v_text;
slim_v_source = slim_t_22658;
slim_v_cursor = slim_t_22660;
slim_v_end = slim_t_22661;
slim_v_text = slim_t_22662;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95parameter_95voids(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(void)slim_region;\n", (int64_t)(sizeof("(void)slim_region;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_name = {0};
if (slim_v_borrowed) {
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
}
else {
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
}
(void)slim_v_name;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(void)", (int64_t)(sizeof("(void)") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95source_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_22835 = {0};
slim_t_22835 = slim_v_source;
int64_t slim_t_22837 = {0};
slim_t_22837 = slim_v_next;
slim_v_source = slim_t_22835;
slim_v_cursor = slim_t_22837;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95temp_95name(int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_t_", (int64_t)(sizeof("slim_t_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
slim_result = slim_fn_text_95append_95nonnegative_95i64(slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95destination(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
if (slim_v_result) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_result", (int64_t)(sizeof("slim_result") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_temporary = {0};
slim_v_temporary = slim_v_destination < INT64_C(0);
(void)slim_v_temporary;
if (slim_v_temporary) {
{
int64_t slim_v_index = {0};
slim_v_index = slim_i64_sub(INT64_C(0), slim_v_destination);
(void)slim_v_index;
slim_result = slim_fn_codegen_95emit_95temp_95name(slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
slim_result = slim_fn_codegen_95emit_95source_95name(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95recur_95bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_param, int64_t slim_v_argument, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_param;
(void)slim_v_argument;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_param, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_param, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
SlimUnit slim_v_bound = {0};
if (slim_v_borrowed) {
slim_v_bound = (SlimUnit){0};
}
else {
{
int64_t slim_v_type_95index = {0};
slim_v_type_95index = slim_fn_codegen_95parameter_95type_95index(slim_v_source, slim_v_tokens, slim_v_param, slim_allocation_region);
(void)slim_v_type_95index;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95temp_95name(slim_v_argument, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = {0};\n", (int64_t)(sizeof(" = {0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
int64_t slim_v_destination = {0};
slim_v_destination = slim_i64_sub(INT64_C(0), slim_v_argument);
(void)slim_v_destination;
slim_v_bound = slim_fn_codegen_95emit_95expr_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_argument, slim_v_destination, false, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
(void)slim_v_bound;
{
int64_t slim_v_next_95param = {0};
slim_v_next_95param = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_param, slim_allocation_region);
(void)slim_v_next_95param;
{
int64_t slim_v_next_95argument = {0};
slim_v_next_95argument = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_95argument;
SlimBytes slim_t_23212 = {0};
slim_t_23212 = slim_v_source;
int64_t slim_t_23214 = {0};
slim_t_23214 = slim_v_module_95items;
int64_t slim_t_23215 = {0};
slim_t_23215 = slim_v_params;
int64_t slim_t_23216 = {0};
slim_t_23216 = slim_v_next_95param;
int64_t slim_t_23217 = {0};
slim_t_23217 = slim_v_next_95argument;
slim_v_source = slim_t_23212;
slim_v_module_95items = slim_t_23214;
slim_v_params = slim_t_23215;
slim_v_param = slim_t_23216;
slim_v_argument = slim_t_23217;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95recur_95updates(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_param, int64_t slim_v_argument, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_param;
(void)slim_v_argument;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_param, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"goto slim_recur;\n", (int64_t)(sizeof("goto slim_recur;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_param, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_name = {0};
if (slim_v_borrowed) {
slim_v_name = slim_i64_add(slim_v_param, INT64_C(2));
}
else {
slim_v_name = slim_i64_add(slim_v_param, INT64_C(1));
}
(void)slim_v_name;
{
SlimUnit slim_v_assigned = {0};
if (slim_v_borrowed) {
slim_v_assigned = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95source_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_value = {0};
slim_v_value = slim_fn_codegen_95emit_95temp_95name(slim_v_argument, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value;
slim_v_assigned = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
(void)slim_v_assigned;
{
int64_t slim_v_next_95param = {0};
slim_v_next_95param = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_param, slim_allocation_region);
(void)slim_v_next_95param;
{
int64_t slim_v_next_95argument = {0};
slim_v_next_95argument = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_argument, slim_allocation_region);
(void)slim_v_next_95argument;
SlimBytes slim_t_23433 = {0};
slim_t_23433 = slim_v_source;
int64_t slim_t_23435 = {0};
slim_t_23435 = slim_v_params;
int64_t slim_t_23436 = {0};
slim_t_23436 = slim_v_next_95param;
int64_t slim_t_23437 = {0};
slim_t_23437 = slim_v_next_95argument;
slim_v_source = slim_t_23433;
slim_v_params = slim_t_23435;
slim_v_param = slim_t_23436;
slim_v_argument = slim_t_23437;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95recur_95assignments(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_param, int64_t slim_v_argument, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_param;
(void)slim_v_argument;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_bound = {0};
slim_v_bound = slim_fn_codegen_95emit_95recur_95bindings(slim_v_source, slim_v_tokens, INT64_C(3), slim_v_params, slim_v_param, slim_v_argument, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_bound;
slim_result = slim_fn_codegen_95emit_95recur_95updates(slim_v_source, slim_v_tokens, slim_v_params, slim_v_param, slim_v_argument, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95runtime_95assignment(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimBytes slim_v_runtime, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_runtime;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_95append_95text(slim_v_output, slim_v_runtime, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
SlimUnit slim_v_args = {0};
slim_v_args = slim_fn_codegen_95emit_95value_95arguments(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_args;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)");\n", (int64_t)(sizeof(");\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95binary_95operator(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimBytes slim_v_operator, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_operator;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_right = {0};
slim_v_right = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_right;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_left_95value = {0};
slim_v_left_95value = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_left_95value;
{
SlimUnit slim_v_op = {0};
slim_v_op = slim_fn_text_95append_95text(slim_v_output, slim_v_operator, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_op;
{
SlimUnit slim_v_right_95value = {0};
slim_v_right_95value = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_right, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_right_95value;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_95emit_95comparison(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_eq = {0};
slim_v_eq = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.eq", (int64_t)(sizeof("i64.eq") - 1)), slim_allocation_region);
(void)slim_v_eq;
if (slim_v_eq) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95binary_95operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" == ", (int64_t)(sizeof(" == ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
{
bool slim_v_lt = {0};
slim_v_lt = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.lt", (int64_t)(sizeof("i64.lt") - 1)), slim_allocation_region);
(void)slim_v_lt;
if (slim_v_lt) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95binary_95operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" < ", (int64_t)(sizeof(" < ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
{
bool slim_v_le = {0};
slim_v_le = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.le", (int64_t)(sizeof("i64.le") - 1)), slim_allocation_region);
(void)slim_v_le;
if (slim_v_le) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95binary_95operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" <= ", (int64_t)(sizeof(" <= ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
{
bool slim_v_gt = {0};
slim_v_gt = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.gt", (int64_t)(sizeof("i64.gt") - 1)), slim_allocation_region);
(void)slim_v_gt;
if (slim_v_gt) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95binary_95operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" > ", (int64_t)(sizeof(" > ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
{
bool slim_v_ge = {0};
slim_v_ge = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"i64.ge", (int64_t)(sizeof("i64.ge") - 1)), slim_allocation_region);
(void)slim_v_ge;
if (slim_v_ge) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95binary_95operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" >= ", (int64_t)(sizeof(" >= ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
slim_result = false;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_95emit_95boolean_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_and_95call = {0};
slim_v_and_95call = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bool.and", (int64_t)(sizeof("bool.and") - 1)), slim_allocation_region);
(void)slim_v_and_95call;
if (slim_v_and_95call) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95binary_95operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" && ", (int64_t)(sizeof(" && ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
{
bool slim_v_or_95call = {0};
slim_v_or_95call = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bool.or", (int64_t)(sizeof("bool.or") - 1)), slim_allocation_region);
(void)slim_v_or_95call;
if (slim_v_or_95call) {
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95binary_95operator(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_bytes_static((const uint8_t *)" || ", (int64_t)(sizeof(" || ") - 1)), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = true;
}
}
else {
{
bool slim_v_not_95call = {0};
slim_v_not_95call = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"bool.not", (int64_t)(sizeof("bool.not") - 1)), slim_allocation_region);
(void)slim_v_not_95call;
if (slim_v_not_95call) {
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = !", (int64_t)(sizeof(" = !") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_value = {0};
slim_v_value = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
else {
slim_result = false;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_95emit_95u8_95conversion(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_conversion = {0};
slim_v_conversion = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"u8.to-i64", (int64_t)(sizeof("u8.to-i64") - 1)), slim_allocation_region);
(void)slim_v_conversion;
if (slim_v_conversion) {
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = (int64_t)", (int64_t)(sizeof(" = (int64_t)") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_value = {0};
slim_v_value = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
else {
slim_result = false;
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_95emit_95vector_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_95type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_new_95call = {0};
slim_v_new_95call = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.new", (int64_t)(sizeof("vec.new") - 1)), slim_allocation_region);
(void)slim_v_new_95call;
if (slim_v_new_95call) {
{
int64_t slim_v_inner_95type = {0};
slim_v_inner_95type = slim_i64_add(slim_v_destination_95type, INT64_C(2));
(void)slim_v_inner_95type;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = slim_vec_new(sizeof(", (int64_t)(sizeof(" = slim_vec_new(sizeof(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_inner_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"), slim_allocation_region);\n", (int64_t)(sizeof("), slim_allocation_region);\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
else {
{
bool slim_v_len_95call = {0};
slim_v_len_95call = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.len", (int64_t)(sizeof("vec.len") - 1)), slim_allocation_region);
(void)slim_v_len_95call;
if (slim_v_len_95call) {
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = (", (int64_t)(sizeof(" = (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_vector = {0};
slim_v_vector = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_vector;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)").len;\n", (int64_t)(sizeof(").len;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
else {
{
bool slim_v_get_95call = {0};
slim_v_get_95call = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.get", (int64_t)(sizeof("vec.get") - 1)), slim_allocation_region);
(void)slim_v_get_95call;
if (slim_v_get_95call) {
{
int64_t slim_v_index = {0};
slim_v_index = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_index;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_cast_95prefix = {0};
slim_v_cast_95prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = ((", (int64_t)(sizeof(" = ((") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cast_95prefix;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_cast_95suffix = {0};
slim_v_cast_95suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" *)(", (int64_t)(sizeof(" *)(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_cast_95suffix;
{
SlimUnit slim_v_vector_95data = {0};
slim_v_vector_95data = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_vector_95data;
{
SlimUnit slim_v_check_95prefix = {0};
slim_v_check_95prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)").data)[slim_vec_check_index(&(", (int64_t)(sizeof(").data)[slim_vec_check_index(&(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_check_95prefix;
{
SlimUnit slim_v_vector_95check = {0};
slim_v_vector_95check = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_vector_95check;
{
SlimUnit slim_v_comma = {0};
slim_v_comma = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"), ", (int64_t)(sizeof("), ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comma;
{
SlimUnit slim_v_index_95value = {0};
slim_v_index_95value = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_index_95value;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")];\n", (int64_t)(sizeof(")];\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
}
}
}
else {
{
bool slim_v_push_95call = {0};
slim_v_push_95call = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.push", (int64_t)(sizeof("vec.push") - 1)), slim_allocation_region);
(void)slim_v_push_95call;
if (slim_v_push_95call) {
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_value;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"if (!slim_vec_push(", (int64_t)(sizeof("if (!slim_vec_push(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_address = {0};
slim_v_address = slim_fn_codegen_95emit_95binding_95address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_address;
{
bool slim_v_number = {0};
slim_v_number = slim_fn_syntax_95first_95byte_95is_95number(slim_v_source, slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_number;
{
SlimUnit slim_v_middle = {0};
if (slim_v_number) {
slim_v_middle = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", &(int64_t){", (int64_t)(sizeof(", &(int64_t){") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_middle = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", &(", (int64_t)(sizeof(", &(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_middle;
{
SlimUnit slim_v_pushed_95value = {0};
slim_v_pushed_95value = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_pushed_95value;
{
SlimUnit slim_v_close = {0};
if (slim_v_number) {
slim_v_close = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"})) goto slim_allocation_failed; ", (int64_t)(sizeof("})) goto slim_allocation_failed; ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_close = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"))) goto slim_allocation_failed; ", (int64_t)(sizeof("))) goto slim_allocation_failed; ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_close;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = (SlimUnit){0};\n", (int64_t)(sizeof(" = (SlimUnit){0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
}
else {
slim_result = false;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95user_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_callee_95start = {0};
slim_v_callee_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_95start;
{
int64_t slim_v_callee_95end = {0};
slim_v_callee_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_callee_95end;
{
int64_t slim_v_callee_95params = {0};
slim_v_callee_95params = slim_fn_syntax_95find_95function_95params(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_callee_95start, slim_v_callee_95end, slim_allocation_region);
(void)slim_v_callee_95params;
{
int64_t slim_v_first_95callee_95param = {0};
slim_v_first_95callee_95param = slim_i64_add(slim_v_callee_95params, INT64_C(1));
(void)slim_v_first_95callee_95param;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95function_95name(slim_v_source, slim_v_tokens, slim_v_callee, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
SlimUnit slim_v_args = {0};
slim_v_args = slim_fn_codegen_95emit_95user_95arguments(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_first_95callee_95param, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_args;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)");\n", (int64_t)(sizeof(");\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
bool slim_v_allocation_95effect = {0};
slim_v_allocation_95effect = slim_fn_memory_95params_95have_95alloc_95effect(slim_v_source, slim_v_tokens, slim_v_callee_95params, slim_allocation_region);
(void)slim_v_allocation_95effect;
if (slim_v_allocation_95effect) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;\n", (int64_t)(sizeof("if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = (SlimUnit){0};
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_call_95form, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_call_95form;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_95type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_call_95form, INT64_C(2));
(void)slim_v_callee;
{
int64_t slim_v_arguments = {0};
slim_v_arguments = slim_i64_add(slim_v_call_95form, INT64_C(3));
(void)slim_v_arguments;
{
SlimUnit slim_v_bindings = {0};
slim_v_bindings = slim_fn_codegen_95emit_95call_95bindings(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_bindings;
{
bool slim_v_read_95file = {0};
slim_v_read_95file = slim_fn_codegen_95emit_95read_95file_95call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_read_95file;
if (slim_v_read_95file) {
slim_result = (SlimUnit){0};
}
else {
{
SlimBytes slim_v_runtime = {0};
slim_v_runtime = slim_fn_codegen_95runtime_95name(slim_v_source, slim_v_tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_runtime;
{
int64_t slim_v_runtime_95length = {0};
slim_v_runtime_95length = slim_bytes_len(slim_v_runtime);
(void)slim_v_runtime_95length;
{
bool slim_v_has_95runtime = {0};
slim_v_has_95runtime = slim_v_runtime_95length > INT64_C(0);
(void)slim_v_has_95runtime;
if (slim_v_has_95runtime) {
slim_result = slim_fn_codegen_95emit_95runtime_95assignment(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_runtime, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_comparison = {0};
slim_v_comparison = slim_fn_codegen_95emit_95comparison(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comparison;
if (slim_v_comparison) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_boolean_95call = {0};
slim_v_boolean_95call = slim_fn_codegen_95emit_95boolean_95call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_boolean_95call;
if (slim_v_boolean_95call) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_conversion = {0};
slim_v_conversion = slim_fn_codegen_95emit_95u8_95conversion(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_conversion;
if (slim_v_conversion) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_vector_95call = {0};
slim_v_vector_95call = slim_fn_codegen_95emit_95collection_95call(slim_v_source, slim_v_tokens, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_vector_95call;
if (slim_v_vector_95call) {
slim_result = (SlimUnit){0};
}
else {
slim_result = slim_fn_codegen_95emit_95user_95call(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_callee, slim_v_arguments, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_codegen_95find_95record_95item(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
bool slim_v_record_95item = {0};
slim_v_record_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_95item;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same_95name = {0};
slim_v_same_95name = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same_95name;
{
bool slim_v_found = {0};
slim_v_found = slim_v_record_95item && slim_v_same_95name;
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_25567 = {0};
slim_t_25567 = slim_v_source;
int64_t slim_t_25569 = {0};
slim_t_25569 = slim_v_next;
int64_t slim_t_25570 = {0};
slim_t_25570 = slim_v_name_95start;
int64_t slim_t_25571 = {0};
slim_t_25571 = slim_v_name_95end;
slim_v_source = slim_t_25567;
slim_v_cursor = slim_t_25569;
slim_v_name_95start = slim_t_25570;
slim_v_name_95end = slim_t_25571;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_codegen_95find_95record_95field(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_name;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_25714 = {0};
slim_t_25714 = slim_v_source;
int64_t slim_t_25716 = {0};
slim_t_25716 = slim_v_next;
int64_t slim_t_25717 = {0};
slim_t_25717 = slim_v_name_95start;
int64_t slim_t_25718 = {0};
slim_t_25718 = slim_v_name_95end;
slim_v_source = slim_t_25714;
slim_v_cursor = slim_t_25716;
slim_v_name_95start = slim_t_25717;
slim_v_name_95end = slim_t_25718;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95make_95bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_definition, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_definition;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_field = {0};
slim_v_field = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_field;
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_value;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_field, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_field, slim_allocation_region);
(void)slim_v_name_95end;
{
int64_t slim_v_declared = {0};
slim_v_declared = slim_fn_codegen_95find_95record_95field(slim_v_source, slim_v_tokens, slim_v_definition, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_declared;
{
int64_t slim_v_type_95index = {0};
slim_v_type_95index = slim_i64_add(slim_v_declared, INT64_C(2));
(void)slim_v_type_95index;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95temp_95name(slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = {0};\n", (int64_t)(sizeof(" = {0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
int64_t slim_v_destination = {0};
slim_v_destination = slim_i64_sub(INT64_C(0), slim_v_cursor);
(void)slim_v_destination;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95expr_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_value, slim_v_destination, false, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_25952 = {0};
slim_t_25952 = slim_v_source;
int64_t slim_t_25954 = {0};
slim_t_25954 = slim_v_module_95items;
int64_t slim_t_25955 = {0};
slim_t_25955 = slim_v_params;
int64_t slim_t_25956 = {0};
slim_t_25956 = slim_v_definition;
int64_t slim_t_25957 = {0};
slim_t_25957 = slim_v_next;
slim_v_source = slim_t_25952;
slim_v_module_95items = slim_t_25954;
slim_v_params = slim_t_25955;
slim_v_definition = slim_t_25956;
slim_v_cursor = slim_t_25957;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95make_95fields(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
{
int64_t slim_v_field = {0};
slim_v_field = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_field;
{
SlimUnit slim_v_dot = {0};
slim_v_dot = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)".", (int64_t)(sizeof(".") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dot;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95field_95name(slim_v_source, slim_v_tokens, slim_v_field, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95temp_95name(slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_26135 = {0};
slim_t_26135 = slim_v_source;
int64_t slim_t_26137 = {0};
slim_t_26137 = slim_v_params;
int64_t slim_t_26138 = {0};
slim_t_26138 = slim_v_next;
bool slim_t_26139 = {0};
slim_t_26139 = false;
slim_v_source = slim_t_26135;
slim_v_params = slim_t_26137;
slim_v_cursor = slim_t_26138;
slim_v_first = slim_t_26139;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95make(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_module_95items = {0};
slim_v_module_95items = INT64_C(3);
(void)slim_v_module_95items;
{
int64_t slim_v_record = {0};
slim_v_record = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_record;
{
int64_t slim_v_first_95field = {0};
slim_v_first_95field = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_first_95field;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_record, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_record, slim_allocation_region);
(void)slim_v_name_95end;
{
int64_t slim_v_item = {0};
slim_v_item = slim_fn_codegen_95find_95record_95item(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_item;
{
int64_t slim_v_fields_95form = {0};
slim_v_fields_95form = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_fields_95form;
{
int64_t slim_v_definition = {0};
slim_v_definition = slim_i64_add(slim_v_fields_95form, INT64_C(1));
(void)slim_v_definition;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"{\n", (int64_t)(sizeof("{\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
SlimUnit slim_v_bindings = {0};
slim_v_bindings = slim_fn_codegen_95emit_95make_95bindings(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_definition, slim_v_first_95field, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_bindings;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = (", (int64_t)(sizeof(" = (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95named_95type_95name(slim_v_source, slim_v_tokens, slim_v_record, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"){", (int64_t)(sizeof("){") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_fields = {0};
slim_v_fields = slim_fn_codegen_95emit_95make_95fields(slim_v_source, slim_v_tokens, slim_v_params, slim_v_first_95field, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_fields;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"};\n", (int64_t)(sizeof("};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"}\n", (int64_t)(sizeof("}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95get(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_value = {0};
slim_v_value = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_value;
{
int64_t slim_v_field = {0};
slim_v_field = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_field;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
SlimUnit slim_v_dot = {0};
slim_v_dot = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)".", (int64_t)(sizeof(".") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dot;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95field_95name(slim_v_source, slim_v_tokens, slim_v_field, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95case_95bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_payload_95type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_payload_95type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_payload_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95temp_95name(slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = {0};\n", (int64_t)(sizeof(" = {0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
int64_t slim_v_destination = {0};
slim_v_destination = slim_i64_sub(INT64_C(0), slim_v_cursor);
(void)slim_v_destination;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95expr_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_cursor, slim_v_destination, false, slim_v_payload_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_next_95type = {0};
slim_v_next_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_payload_95type, slim_allocation_region);
(void)slim_v_next_95type;
SlimBytes slim_t_26702 = {0};
slim_t_26702 = slim_v_source;
int64_t slim_t_26704 = {0};
slim_t_26704 = slim_v_module_95items;
int64_t slim_t_26705 = {0};
slim_t_26705 = slim_v_params;
int64_t slim_t_26706 = {0};
slim_t_26706 = slim_v_next;
int64_t slim_t_26707 = {0};
slim_t_26707 = slim_v_next_95type;
slim_v_source = slim_t_26702;
slim_v_module_95items = slim_t_26704;
slim_v_params = slim_t_26705;
slim_v_cursor = slim_t_26706;
slim_v_payload_95type = slim_t_26707;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95case_95payload(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_field, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_field;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_first = {0};
slim_v_first = slim_v_field == INT64_C(0);
(void)slim_v_first;
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)".slim_", (int64_t)(sizeof(".slim_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_number = {0};
slim_v_number = slim_fn_text_95append_95nonnegative_95i64(slim_v_field, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_number;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_value = {0};
slim_v_value = slim_fn_codegen_95emit_95temp_95name(slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_value;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_next_95field = {0};
slim_v_next_95field = slim_i64_add(slim_v_field, INT64_C(1));
(void)slim_v_next_95field;
SlimBytes slim_t_26888 = {0};
slim_t_26888 = slim_v_source;
int64_t slim_t_26890 = {0};
slim_t_26890 = slim_v_params;
int64_t slim_t_26891 = {0};
slim_t_26891 = slim_v_next;
int64_t slim_t_26892 = {0};
slim_t_26892 = slim_v_next_95field;
slim_v_source = slim_t_26888;
slim_v_params = slim_t_26890;
slim_v_cursor = slim_t_26891;
slim_v_field = slim_t_26892;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95case_95value(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_expr, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_expr;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_module_95items = {0};
slim_v_module_95items = INT64_C(3);
(void)slim_v_module_95items;
{
int64_t slim_v_variant = {0};
slim_v_variant = slim_i64_add(slim_v_expr, INT64_C(2));
(void)slim_v_variant;
{
int64_t slim_v_case_95name = {0};
slim_v_case_95name = slim_i64_add(slim_v_expr, INT64_C(3));
(void)slim_v_case_95name;
{
int64_t slim_v_payload = {0};
slim_v_payload = slim_i64_add(slim_v_expr, INT64_C(4));
(void)slim_v_payload;
{
int64_t slim_v_variant_95start = {0};
slim_v_variant_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_variant, slim_allocation_region);
(void)slim_v_variant_95start;
{
int64_t slim_v_variant_95end = {0};
slim_v_variant_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_variant, slim_allocation_region);
(void)slim_v_variant_95end;
{
int64_t slim_v_item = {0};
slim_v_item = slim_fn_codegen_95find_95variant_95item(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_variant_95start, slim_v_variant_95end, slim_allocation_region);
(void)slim_v_item;
{
int64_t slim_v_cases_95form = {0};
slim_v_cases_95form = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_cases_95form;
{
int64_t slim_v_cases = {0};
slim_v_cases = slim_i64_add(slim_v_cases_95form, INT64_C(1));
(void)slim_v_cases;
{
int64_t slim_v_case_95start = {0};
slim_v_case_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_case_95name, slim_allocation_region);
(void)slim_v_case_95start;
{
int64_t slim_v_case_95end = {0};
slim_v_case_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_case_95name, slim_allocation_region);
(void)slim_v_case_95end;
{
int64_t slim_v_case_95form = {0};
slim_v_case_95form = slim_fn_codegen_95find_95variant_95case(slim_v_source, slim_v_tokens, slim_v_cases, slim_v_case_95start, slim_v_case_95end, slim_allocation_region);
(void)slim_v_case_95form;
{
int64_t slim_v_payload_95type = {0};
slim_v_payload_95type = slim_i64_add(slim_v_case_95form, INT64_C(2));
(void)slim_v_payload_95type;
{
int64_t slim_v_payload_95kind = {0};
slim_v_payload_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_payload, slim_allocation_region);
(void)slim_v_payload_95kind;
{
bool slim_v_empty = {0};
slim_v_empty = slim_v_payload_95kind == INT64_C(1);
(void)slim_v_empty;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"{\n", (int64_t)(sizeof("{\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
SlimUnit slim_v_bindings = {0};
slim_v_bindings = slim_fn_codegen_95emit_95case_95bindings(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_payload, slim_v_payload_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_bindings;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = (", (int64_t)(sizeof(" = (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95named_95type_95name(slim_v_source, slim_v_tokens, slim_v_variant, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_tag_95prefix = {0};
slim_v_tag_95prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"){.tag = ", (int64_t)(sizeof("){.tag = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tag_95prefix;
{
SlimUnit slim_v_tag = {0};
slim_v_tag = slim_fn_codegen_95emit_95tag_95name(slim_v_source, slim_v_tokens, slim_v_variant, slim_v_case_95name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tag;
{
SlimUnit slim_v_data = {0};
if (slim_v_empty) {
slim_v_data = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_data_95prefix = {0};
slim_v_data_95prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", .data.", (int64_t)(sizeof(", .data.") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_data_95prefix;
{
SlimUnit slim_v_case_95field = {0};
slim_v_case_95field = slim_fn_codegen_95emit_95case_95name(slim_v_source, slim_v_tokens, slim_v_case_95name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_case_95field;
{
SlimUnit slim_v_payload_95open = {0};
slim_v_payload_95open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = {", (int64_t)(sizeof(" = {") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_payload_95open;
{
SlimUnit slim_v_payload_95values = {0};
slim_v_payload_95values = slim_fn_codegen_95emit_95case_95payload(slim_v_source, slim_v_tokens, slim_v_params, slim_v_payload, INT64_C(0), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_payload_95values;
slim_v_data = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"}", (int64_t)(sizeof("}") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
(void)slim_v_data;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"};\n", (int64_t)(sizeof("};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"}\n", (int64_t)(sizeof("}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_codegen_95find_95parameter_95type(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_name = {0};
if (slim_v_borrowed) {
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
}
else {
slim_v_name = slim_v_head;
}
(void)slim_v_name;
{
int64_t slim_v_type_95index = {0};
if (slim_v_borrowed) {
slim_v_type_95index = slim_i64_add(slim_v_cursor, INT64_C(3));
}
else {
slim_v_type_95index = slim_i64_add(slim_v_cursor, INT64_C(2));
}
(void)slim_v_type_95index;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_type_95index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_27487 = {0};
slim_t_27487 = slim_v_source;
int64_t slim_t_27489 = {0};
slim_t_27489 = slim_v_next;
int64_t slim_t_27490 = {0};
slim_t_27490 = slim_v_name_95start;
int64_t slim_t_27491 = {0};
slim_t_27491 = slim_v_name_95end;
slim_v_source = slim_t_27487;
slim_v_cursor = slim_t_27489;
slim_v_name_95start = slim_t_27490;
slim_v_name_95end = slim_t_27491;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_codegen_95find_95variant_95item(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
bool slim_v_variant_95item = {0};
slim_v_variant_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
(void)slim_v_variant_95item;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same_95name = {0};
slim_v_same_95name = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same_95name;
{
bool slim_v_found = {0};
slim_v_found = slim_v_variant_95item && slim_v_same_95name;
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_27657 = {0};
slim_t_27657 = slim_v_source;
int64_t slim_t_27659 = {0};
slim_t_27659 = slim_v_next;
int64_t slim_t_27660 = {0};
slim_t_27660 = slim_v_name_95start;
int64_t slim_t_27661 = {0};
slim_t_27661 = slim_v_name_95end;
slim_v_source = slim_t_27657;
slim_v_cursor = slim_t_27659;
slim_v_name_95start = slim_t_27660;
slim_v_name_95end = slim_t_27661;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_codegen_95find_95variant_95case(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_name;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_27804 = {0};
slim_t_27804 = slim_v_source;
int64_t slim_t_27806 = {0};
slim_t_27806 = slim_v_next;
int64_t slim_t_27807 = {0};
slim_t_27807 = slim_v_name_95start;
int64_t slim_t_27808 = {0};
slim_t_27808 = slim_v_name_95end;
slim_v_source = slim_t_27804;
slim_v_cursor = slim_t_27806;
slim_v_name_95start = slim_t_27807;
slim_v_name_95end = slim_t_27808;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95variant_95bindings(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_value, int64_t slim_v_case_95name, int64_t slim_v_bindings, int64_t slim_v_payload_95type, int64_t slim_v_field, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_value;
(void)slim_v_case_95name;
(void)slim_v_bindings;
(void)slim_v_payload_95type;
(void)slim_v_field;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_binding = {0};
slim_v_binding = slim_i64_add(slim_v_bindings, INT64_C(1));
(void)slim_v_binding;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_binding, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_payload_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95source_95name(slim_v_source, slim_v_tokens, slim_v_binding, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_scrutinee = {0};
slim_v_scrutinee = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, INT64_C(0), slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_scrutinee;
{
SlimUnit slim_v_data = {0};
slim_v_data = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)".data.", (int64_t)(sizeof(".data.") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_data;
{
SlimUnit slim_v_case_95field = {0};
slim_v_case_95field = slim_fn_codegen_95emit_95case_95name(slim_v_source, slim_v_tokens, slim_v_case_95name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_case_95field;
{
SlimUnit slim_v_payload_95field = {0};
slim_v_payload_95field = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)".slim_", (int64_t)(sizeof(".slim_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_payload_95field;
{
SlimUnit slim_v_number = {0};
slim_v_number = slim_fn_text_95append_95nonnegative_95i64(slim_v_field, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_number;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
{
int64_t slim_v_next_95bindings = {0};
slim_v_next_95bindings = slim_i64_add(slim_v_bindings, INT64_C(1));
(void)slim_v_next_95bindings;
{
int64_t slim_v_next_95type = {0};
slim_v_next_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_payload_95type, slim_allocation_region);
(void)slim_v_next_95type;
{
int64_t slim_v_next_95field = {0};
slim_v_next_95field = slim_i64_add(slim_v_field, INT64_C(1));
(void)slim_v_next_95field;
SlimBytes slim_t_28053 = {0};
slim_t_28053 = slim_v_source;
int64_t slim_t_28055 = {0};
slim_t_28055 = slim_v_value;
int64_t slim_t_28056 = {0};
slim_t_28056 = slim_v_case_95name;
int64_t slim_t_28057 = {0};
slim_t_28057 = slim_v_next_95bindings;
int64_t slim_t_28058 = {0};
slim_t_28058 = slim_v_next_95type;
int64_t slim_t_28059 = {0};
slim_t_28059 = slim_v_next_95field;
slim_v_source = slim_t_28053;
slim_v_value = slim_t_28055;
slim_v_case_95name = slim_t_28056;
slim_v_bindings = slim_t_28057;
slim_v_payload_95type = slim_t_28058;
slim_v_field = slim_t_28059;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95variant_95arms(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_variant_95name, int64_t slim_v_variant_95cases, int64_t slim_v_cursor, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_variant_95name;
(void)slim_v_variant_95cases;
(void)slim_v_cursor;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_95type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_case_95name = {0};
slim_v_case_95name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_case_95name;
{
int64_t slim_v_case_95start = {0};
slim_v_case_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_case_95name, slim_allocation_region);
(void)slim_v_case_95start;
{
int64_t slim_v_case_95end = {0};
slim_v_case_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_case_95name, slim_allocation_region);
(void)slim_v_case_95end;
{
int64_t slim_v_case_95form = {0};
slim_v_case_95form = slim_fn_codegen_95find_95variant_95case(slim_v_source, slim_v_tokens, slim_v_variant_95cases, slim_v_case_95start, slim_v_case_95end, slim_allocation_region);
(void)slim_v_case_95form;
{
int64_t slim_v_payload_95type = {0};
slim_v_payload_95type = slim_i64_add(slim_v_case_95form, INT64_C(2));
(void)slim_v_payload_95type;
{
int64_t slim_v_payload_95kind = {0};
slim_v_payload_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_payload_95type, slim_allocation_region);
(void)slim_v_payload_95kind;
{
bool slim_v_empty = {0};
slim_v_empty = slim_v_payload_95kind == INT64_C(1);
(void)slim_v_empty;
{
int64_t slim_v_bindings = {0};
slim_v_bindings = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_bindings;
{
int64_t slim_v_body = {0};
if (slim_v_empty) {
slim_v_body = slim_v_bindings;
}
else {
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_bindings, slim_allocation_region);
}
(void)slim_v_body;
{
SlimUnit slim_v_case_95prefix = {0};
slim_v_case_95prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"case ", (int64_t)(sizeof("case ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_case_95prefix;
{
SlimUnit slim_v_tag = {0};
slim_v_tag = slim_fn_codegen_95emit_95tag_95name(slim_v_source, slim_v_tokens, slim_v_variant_95name, slim_v_case_95name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tag;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)": {\n", (int64_t)(sizeof(": {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_bound = {0};
if (slim_v_empty) {
slim_v_bound = (SlimUnit){0};
}
else {
slim_v_bound = slim_fn_codegen_95emit_95variant_95bindings(slim_v_source, slim_v_tokens, slim_v_value, slim_v_case_95name, slim_v_bindings, slim_v_payload_95type, INT64_C(0), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_bound;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95expr_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_body, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"break;\n}\n", (int64_t)(sizeof("break;\n}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_28380 = {0};
slim_t_28380 = slim_v_source;
int64_t slim_t_28382 = {0};
slim_t_28382 = slim_v_module_95items;
int64_t slim_t_28383 = {0};
slim_t_28383 = slim_v_params;
int64_t slim_t_28384 = {0};
slim_t_28384 = slim_v_value;
int64_t slim_t_28385 = {0};
slim_t_28385 = slim_v_variant_95name;
int64_t slim_t_28386 = {0};
slim_t_28386 = slim_v_variant_95cases;
int64_t slim_t_28387 = {0};
slim_t_28387 = slim_v_next;
int64_t slim_t_28388 = {0};
slim_t_28388 = slim_v_destination;
bool slim_t_28389 = {0};
slim_t_28389 = slim_v_result;
int64_t slim_t_28390 = {0};
slim_t_28390 = slim_v_destination_95type;
slim_v_source = slim_t_28380;
slim_v_module_95items = slim_t_28382;
slim_v_params = slim_t_28383;
slim_v_value = slim_t_28384;
slim_v_variant_95name = slim_t_28385;
slim_v_variant_95cases = slim_t_28386;
slim_v_cursor = slim_t_28387;
slim_v_destination = slim_t_28388;
slim_v_result = slim_t_28389;
slim_v_destination_95type = slim_t_28390;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95boolean_95match(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_first_95arm, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_first_95arm;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_95type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_first_95pattern = {0};
slim_v_first_95pattern = slim_i64_add(slim_v_first_95arm, INT64_C(1));
(void)slim_v_first_95pattern;
{
int64_t slim_v_first_95body = {0};
slim_v_first_95body = slim_i64_add(slim_v_first_95arm, INT64_C(2));
(void)slim_v_first_95body;
{
int64_t slim_v_second_95arm = {0};
slim_v_second_95arm = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_first_95arm, slim_allocation_region);
(void)slim_v_second_95arm;
{
int64_t slim_v_second_95body = {0};
slim_v_second_95body = slim_i64_add(slim_v_second_95arm, INT64_C(2));
(void)slim_v_second_95body;
{
bool slim_v_first_95true = {0};
slim_v_first_95true = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_first_95pattern, slim_bytes_static((const uint8_t *)"true", (int64_t)(sizeof("true") - 1)), slim_allocation_region);
(void)slim_v_first_95true;
{
SlimUnit slim_v_if_95open = {0};
slim_v_if_95open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"if (", (int64_t)(sizeof("if (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_if_95open;
{
SlimUnit slim_v_negate = {0};
if (slim_v_first_95true) {
slim_v_negate = (SlimUnit){0};
}
else {
slim_v_negate = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"!", (int64_t)(sizeof("!") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_negate;
{
SlimUnit slim_v_condition = {0};
slim_v_condition = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_condition;
{
SlimUnit slim_v_branch_95open = {0};
slim_v_branch_95open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)") {\n", (int64_t)(sizeof(") {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_branch_95open;
{
SlimUnit slim_v_first_95emitted = {0};
slim_v_first_95emitted = slim_fn_codegen_95emit_95expr_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_first_95body, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_first_95emitted;
{
SlimUnit slim_v_branch_95middle = {0};
slim_v_branch_95middle = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"}\nelse {\n", (int64_t)(sizeof("}\nelse {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_branch_95middle;
{
SlimUnit slim_v_second_95emitted = {0};
slim_v_second_95emitted = slim_fn_codegen_95emit_95expr_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_second_95body, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_second_95emitted;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"}\n", (int64_t)(sizeof("}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95variant_95match(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_first_95arm, int64_t slim_v_destination, bool slim_v_result, int64_t slim_v_destination_95type, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_first_95arm;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_destination_95type;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_name_95end;
{
int64_t slim_v_first_95param = {0};
slim_v_first_95param = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_95param;
{
int64_t slim_v_variant_95type = {0};
slim_v_variant_95type = slim_fn_codegen_95find_95parameter_95type(slim_v_source, slim_v_tokens, slim_v_first_95param, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_variant_95type;
{
int64_t slim_v_variant_95start = {0};
slim_v_variant_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_variant_95type, slim_allocation_region);
(void)slim_v_variant_95start;
{
int64_t slim_v_variant_95end = {0};
slim_v_variant_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_variant_95type, slim_allocation_region);
(void)slim_v_variant_95end;
{
int64_t slim_v_variant_95item = {0};
slim_v_variant_95item = slim_fn_codegen_95find_95variant_95item(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_variant_95start, slim_v_variant_95end, slim_allocation_region);
(void)slim_v_variant_95item;
{
int64_t slim_v_cases = {0};
slim_v_cases = slim_i64_add(slim_v_variant_95item, INT64_C(3));
(void)slim_v_cases;
{
int64_t slim_v_variant_95cases = {0};
slim_v_variant_95cases = slim_i64_add(slim_v_cases, INT64_C(1));
(void)slim_v_variant_95cases;
{
SlimUnit slim_v_switch_95open = {0};
slim_v_switch_95open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"switch (", (int64_t)(sizeof("switch (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_switch_95open;
{
SlimUnit slim_v_scrutinee = {0};
slim_v_scrutinee = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_scrutinee;
{
SlimUnit slim_v_tag = {0};
slim_v_tag = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)".tag) {\n", (int64_t)(sizeof(".tag) {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tag;
{
SlimUnit slim_v_arms = {0};
slim_v_arms = slim_fn_codegen_95emit_95variant_95arms(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_value, slim_v_variant_95type, slim_v_variant_95cases, slim_v_first_95arm, slim_v_destination, slim_v_result, slim_v_destination_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_arms;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"default: slim_rt_trap(\"invalid variant tag\");\n}\n", (int64_t)(sizeof("default: slim_rt_trap(\"invalid variant tag\");\n}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95atom_95assignment(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_value, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_value;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_equals = {0};
slim_v_equals = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = ", (int64_t)(sizeof(" = ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_equals;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_value, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)";\n", (int64_t)(sizeof(";\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95function(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_item;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_item, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_95end = {0};
slim_v_body_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95end;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"static ", (int64_t)(sizeof("static ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_returned = {0};
slim_v_returned = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_return_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_returned;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_codegen_95emit_95function_95name(slim_v_source, slim_v_tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_open = {0};
slim_v_open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_open;
{
int64_t slim_v_first_95param = {0};
slim_v_first_95param = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_95param;
{
SlimUnit slim_v_parameters = {0};
slim_v_parameters = slim_fn_codegen_95emit_95parameters(slim_v_source, slim_v_tokens, slim_v_first_95param, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_parameters;
{
SlimUnit slim_v_block = {0};
slim_v_block = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)") {\n", (int64_t)(sizeof(") {\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_block;
{
SlimUnit slim_v_voids = {0};
slim_v_voids = slim_fn_codegen_95emit_95parameter_95voids(slim_v_source, slim_v_tokens, slim_v_first_95param, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_voids;
{
bool slim_v_local_95region = {0};
slim_v_local_95region = slim_fn_memory_95function_95uses_95local_95region(slim_v_source, slim_v_tokens, slim_v_item, slim_allocation_region);
(void)slim_v_local_95region;
{
bool slim_v_allocation_95effect = {0};
slim_v_allocation_95effect = slim_fn_memory_95function_95has_95alloc_95effect(slim_v_source, slim_v_tokens, slim_v_item, slim_allocation_region);
(void)slim_v_allocation_95effect;
{
SlimUnit slim_v_region = {0};
if (slim_v_local_95region) {
slim_v_region = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimRegion slim_function_region;\nslim_region_init(&slim_function_region, slim_region);\nSlimRegion *slim_allocation_region = &slim_function_region;\n(void)slim_allocation_region;\n", (int64_t)(sizeof("SlimRegion slim_function_region;\nslim_region_init(&slim_function_region, slim_region);\nSlimRegion *slim_allocation_region = &slim_function_region;\n(void)slim_allocation_region;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_region = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"SlimRegion *slim_allocation_region = slim_region;\n(void)slim_allocation_region;\n", (int64_t)(sizeof("SlimRegion *slim_allocation_region = slim_region;\n(void)slim_allocation_region;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_region;
{
SlimUnit slim_v_result_95type = {0};
slim_v_result_95type = slim_fn_codegen_95emit_95type(slim_v_source, slim_v_tokens, slim_v_return_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_result_95type;
{
SlimUnit slim_v_result_95decl = {0};
slim_v_result_95decl = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" slim_result = {0};\n", (int64_t)(sizeof(" slim_result = {0};\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_result_95decl;
{
SlimUnit slim_v_initial_95failure = {0};
if (slim_v_allocation_95effect) {
slim_v_initial_95failure = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;\n", (int64_t)(sizeof("if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_initial_95failure = (SlimUnit){0};
}
(void)slim_v_initial_95failure;
{
bool slim_v_recursive = {0};
slim_v_recursive = slim_fn_codegen_95contains_95atom(slim_v_source, slim_v_tokens, slim_v_body, slim_v_body_95end, slim_bytes_static((const uint8_t *)"recur", (int64_t)(sizeof("recur") - 1)), slim_allocation_region);
(void)slim_v_recursive;
{
SlimUnit slim_v_label = {0};
if (slim_v_recursive) {
slim_v_label = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_recur: ;\n", (int64_t)(sizeof("slim_recur: ;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_label = (SlimUnit){0};
}
(void)slim_v_label;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95expr_95full(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_params, slim_v_body, INT64_C(0), true, slim_v_return_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
SlimUnit slim_v_failure_95label = {0};
if (slim_v_allocation_95effect) {
slim_v_failure_95label = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_allocation_failed: ;\n", (int64_t)(sizeof("slim_allocation_failed: ;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_failure_95label = (SlimUnit){0};
}
(void)slim_v_failure_95label;
{
SlimUnit slim_v_destroyed = {0};
if (slim_v_local_95region) {
slim_v_destroyed = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"slim_region_destroy(&slim_function_region);\n", (int64_t)(sizeof("slim_region_destroy(&slim_function_region);\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_destroyed = (SlimUnit){0};
}
(void)slim_v_destroyed;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"return slim_result;\n}\n\n", (int64_t)(sizeof("return slim_result;\n}\n\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95functions(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_function_95item) {
slim_v_emitted = slim_fn_codegen_95emit_95function(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_emitted = (SlimUnit){0};
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_29494 = {0};
slim_t_29494 = slim_v_source;
int64_t slim_t_29496 = {0};
slim_t_29496 = slim_v_module_95items;
int64_t slim_t_29497 = {0};
slim_t_29497 = slim_v_next;
slim_v_source = slim_t_29494;
slim_v_module_95items = slim_t_29496;
slim_v_cursor = slim_t_29497;
goto slim_recur;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_codegen_95emit_95program(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimUnit slim_v_header = {0};
slim_v_header = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"/* generated deterministically by self-hosted slimc */\n#include \"slim_rt.h\"\n#include <string.h>\n\n", (int64_t)(sizeof("/* generated deterministically by self-hosted slimc */\n#include \"slim_rt.h\"\n#include <string.h>\n\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_header;
{
int64_t slim_v_module_95items = {0};
slim_v_module_95items = INT64_C(3);
(void)slim_v_module_95items;
{
SlimUnit slim_v_forwards = {0};
slim_v_forwards = slim_fn_codegen_95emit_95forward_95types(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_forwards;
{
SlimUnit slim_v_data_95items = {0};
slim_v_data_95items = slim_fn_codegen_95emit_95data_95items(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_data_95items;
{
SlimUnit slim_v_prototypes = {0};
slim_v_prototypes = slim_fn_codegen_95emit_95prototypes(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prototypes;
{
SlimUnit slim_v_functions = {0};
slim_v_functions = slim_fn_codegen_95emit_95functions(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_module_95items, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_functions;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"int main(int argc, char **argv) {\nSlimAllocStatus slim_status;\nslim_alloc_status_init(&slim_status);\nSlimRegion slim_root;\nslim_rt_init(&slim_root, &slim_status);\nSlimVec slim_args = slim_vec_new(sizeof(SlimBytes), &slim_root);\nfor (int slim_i = 0; slim_i < argc; ++slim_i) {\nSlimBytes slim_arg = slim_bytes_static((const uint8_t *)argv[slim_i], (int64_t)strlen(argv[slim_i]));\nif (!slim_vec_push(&slim_args, &slim_arg)) {\nslim_alloc_report(&slim_status);\nslim_rt_shutdown();\nreturn 71;\n}\n}\nint64_t slim_exit_code = slim_fn_main(slim_args, &slim_root);\nif (slim_region_failed(&slim_root)) {\nslim_alloc_report(&slim_status);\nslim_rt_shutdown();\nreturn 71;\n}\nif (slim_exit_code < 0 || slim_exit_code > 255) {\nslim_rt_trap(\"main result is outside 0..255\");\n}\nslim_rt_shutdown();\nreturn (int)slim_exit_code;\n}\n", (int64_t)(sizeof("int main(int argc, char **argv) {\nSlimAllocStatus slim_status;\nslim_alloc_status_init(&slim_status);\nSlimRegion slim_root;\nslim_rt_init(&slim_root, &slim_status);\nSlimVec slim_args = slim_vec_new(sizeof(SlimBytes), &slim_root);\nfor (int slim_i = 0; slim_i < argc; ++slim_i) {\nSlimBytes slim_arg = slim_bytes_static((const uint8_t *)argv[slim_i], (int64_t)strlen(argv[slim_i]));\nif (!slim_vec_push(&slim_args, &slim_arg)) {\nslim_alloc_report(&slim_status);\nslim_rt_shutdown();\nreturn 71;\n}\n}\nint64_t slim_exit_code = slim_fn_main(slim_args, &slim_root);\nif (slim_region_failed(&slim_root)) {\nslim_alloc_report(&slim_status);\nslim_rt_shutdown();\nreturn 71;\n}\nif (slim_exit_code < 0 || slim_exit_code > 255) {\nslim_rt_trap(\"main result is outside 0..255\");\n}\nslim_rt_shutdown();\nreturn (int)slim_exit_code;\n}\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_codegen_95emit_95read_95file_95call(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_callee, int64_t slim_v_arguments, int64_t slim_v_destination, bool slim_v_result, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_callee;
(void)slim_v_arguments;
(void)slim_v_destination;
(void)slim_v_result;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_read_95file = {0};
slim_v_read_95file = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"io.read-file", (int64_t)(sizeof("io.read-file") - 1)), slim_allocation_region);
(void)slim_v_read_95file;
if (!slim_v_read_95file) {
slim_result = false;
}
else {
{
int64_t slim_v_output_95argument = {0};
slim_v_output_95argument = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_arguments, slim_allocation_region);
(void)slim_v_output_95argument;
{
SlimUnit slim_v_dest = {0};
slim_v_dest = slim_fn_codegen_95emit_95destination(slim_v_source, slim_v_tokens, slim_v_destination, slim_v_result, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dest;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" = slim_read_file(", (int64_t)(sizeof(" = slim_read_file(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_path_95value = {0};
slim_v_path_95value = slim_fn_codegen_95emit_95value(slim_v_source, slim_v_tokens, slim_v_params, slim_v_arguments, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_path_95value;
{
SlimUnit slim_v_comma = {0};
slim_v_comma = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)", ", (int64_t)(sizeof(", ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_comma;
{
SlimUnit slim_v_output_95address = {0};
slim_v_output_95address = slim_fn_codegen_95emit_95binding_95address(slim_v_source, slim_v_tokens, slim_v_params, slim_v_output_95argument, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_output_95address;
{
SlimUnit slim_v_suffix = {0};
slim_v_suffix = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)");\nif (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;\n", (int64_t)(sizeof(");\nif (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_suffix;
slim_result = true;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_compiler_95check_95input_95path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_project = {0};
slim_v_project = slim_fn_project_95path_95is_95project(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_project;
if (slim_v_project) {
slim_result = slim_fn_project_95check_95project_95path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_check_95check_95path_95all(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimUnit slim_fn_compiler_95format_95tokens(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_previous_95kind, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_v_previous_95kind;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"\n", (int64_t)(sizeof("\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_is_95right = {0};
slim_v_is_95right = slim_v_kind == INT64_C(1);
(void)slim_v_is_95right;
{
bool slim_v_previous_95left = {0};
slim_v_previous_95left = slim_v_previous_95kind == INT64_C(0);
(void)slim_v_previous_95left;
{
bool slim_v_noninitial = {0};
slim_v_noninitial = slim_v_index > INT64_C(0);
(void)slim_v_noninitial;
{
bool slim_v_not_95right = {0};
slim_v_not_95right = !slim_v_is_95right;
(void)slim_v_not_95right;
{
bool slim_v_not_95previous_95left = {0};
slim_v_not_95previous_95left = !slim_v_previous_95left;
(void)slim_v_not_95previous_95left;
{
bool slim_v_after_95form = {0};
slim_v_after_95form = slim_v_noninitial && slim_v_not_95previous_95left;
(void)slim_v_after_95form;
{
bool slim_v_separated = {0};
slim_v_separated = slim_v_after_95form && slim_v_not_95right;
(void)slim_v_separated;
{
SlimUnit slim_v_separator = {0};
if (slim_v_separated) {
slim_v_separator = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_separator = (SlimUnit){0};
}
(void)slim_v_separator;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_text_95append_95token(slim_v_source, slim_v_tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_30016 = {0};
slim_t_30016 = slim_v_source;
int64_t slim_t_30018 = {0};
slim_t_30018 = slim_v_next;
int64_t slim_t_30019 = {0};
slim_t_30019 = slim_v_length;
int64_t slim_t_30020 = {0};
slim_t_30020 = slim_v_kind;
slim_v_source = slim_t_30016;
slim_v_index = slim_t_30018;
slim_v_length = slim_t_30019;
slim_v_previous_95kind = slim_t_30020;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_compiler_95format_95path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_project = {0};
slim_v_project = slim_fn_project_95path_95is_95project(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_project;
{
SlimVec slim_v_input_95bytes = {0};
slim_v_input_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_input_95bytes;
{
bool slim_v_input_95read = {0};
slim_v_input_95read = slim_read_file(slim_v_path, &slim_v_input_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_input_95read;
if (!slim_v_input_95read) {
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_println(slim_bytes_static((const uint8_t *)"E0409@0:0", (int64_t)(sizeof("E0409@0:0") - 1)));
(void)slim_v_shown;
slim_result = INT64_C(1);
}
}
else {
{
SlimBytes slim_v_input = {0};
slim_v_input = slim_bytes_freeze(slim_v_input_95bytes);
(void)slim_v_input;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_input, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
bool slim_v_valid = {0};
if (slim_v_project) {
slim_v_valid = true;
}
else {
slim_v_valid = slim_fn_validate_95module_95shape_95valid(slim_v_input, &slim_v_tokens, slim_allocation_region);
}
(void)slim_v_valid;
if (!slim_v_valid) {
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_println(slim_bytes_static((const uint8_t *)"E0102@0:0", (int64_t)(sizeof("E0102@0:0") - 1)));
(void)slim_v_shown;
slim_result = INT64_C(1);
}
}
else {
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
int64_t slim_v_length = {0};
slim_v_length = (slim_v_tokens).len;
(void)slim_v_length;
{
SlimUnit slim_v_formatted = {0};
slim_v_formatted = slim_fn_compiler_95format_95tokens(slim_v_input, &slim_v_tokens, INT64_C(0), slim_v_length, INT64_C(0), &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_formatted;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_compiler_95compile_95path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_project = {0};
slim_v_project = slim_fn_project_95path_95is_95project(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_project;
if (slim_v_project) {
slim_result = slim_fn_project_95compile_95project_95path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_check_95check_95path_95all(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_checked > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_checked;
}
else {
{
SlimVec slim_v_input_95bytes = {0};
slim_v_input_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_input_95bytes;
{
bool slim_v_input_95read = {0};
slim_v_input_95read = slim_read_file(slim_v_path, &slim_v_input_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_input_95read;
if (!slim_v_input_95read) {
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_println(slim_bytes_static((const uint8_t *)"E0409@0:0", (int64_t)(sizeof("E0409@0:0") - 1)));
(void)slim_v_shown;
slim_result = INT64_C(1);
}
}
else {
{
SlimBytes slim_v_input = {0};
slim_v_input = slim_bytes_freeze(slim_v_input_95bytes);
(void)slim_v_input;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_input, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95program(slim_v_input, &slim_v_tokens, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
SlimBytes slim_v_generated = {0};
slim_v_generated = slim_bytes_freeze(slim_v_output);
(void)slim_v_generated;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_generated);
(void)slim_v_shown;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_compiler_95run_95compile(SlimVec * slim_v_args, int64_t slim_v_arg_95count, SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_args;
(void)slim_v_arg_95count;
(void)slim_v_path;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_project = {0};
slim_v_project = slim_fn_project_95path_95is_95project(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_project;
if (!slim_v_project) {
slim_result = slim_fn_compiler_95compile_95path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_enough = {0};
slim_v_enough = slim_v_arg_95count >= INT64_C(4);
(void)slim_v_enough;
{
bool slim_v_scheduled = {0};
if (!slim_v_enough) {
slim_v_scheduled = false;
}
else {
{
SlimBytes slim_v_option = {0};
slim_v_option = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(2))];
(void)slim_v_option;
{
int64_t slim_v_option_95length = {0};
slim_v_option_95length = slim_bytes_len(slim_v_option);
(void)slim_v_option_95length;
slim_v_scheduled = slim_fn_syntax_95span_95equal(slim_v_option, INT64_C(0), slim_v_option_95length, slim_bytes_static((const uint8_t *)"--jobs", (int64_t)(sizeof("--jobs") - 1)), slim_allocation_region);
}
}
}
(void)slim_v_scheduled;
{
int64_t slim_v_jobs = {0};
if (!slim_v_scheduled) {
slim_v_jobs = INT64_C(1);
}
else {
{
SlimBytes slim_v_requested = {0};
slim_v_requested = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(3))];
(void)slim_v_requested;
slim_v_jobs = slim_fn_scheduler_95parse_95jobs(slim_v_requested, slim_allocation_region);
}
}
(void)slim_v_jobs;
slim_result = slim_fn_project_95compile_95project_95path_95jobs(slim_v_path, slim_v_jobs, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_compiler_95run_95cache(SlimVec * slim_v_args, int64_t slim_v_arg_95count, SlimRegion *slim_region) {
(void)slim_v_args;
(void)slim_v_arg_95count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_has_95paths = {0};
slim_v_has_95paths = slim_v_arg_95count >= INT64_C(4);
(void)slim_v_has_95paths;
if (!slim_v_has_95paths) {
{
SlimUnit slim_v_usage = {0};
slim_v_usage = slim_println(slim_bytes_static((const uint8_t *)"usage: slimc cache MANIFEST CACHE", (int64_t)(sizeof("usage: slimc cache MANIFEST CACHE") - 1)));
(void)slim_v_usage;
slim_result = INT64_C(64);
}
}
else {
{
SlimBytes slim_v_manifest_95path = {0};
slim_v_manifest_95path = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(2))];
(void)slim_v_manifest_95path;
{
SlimBytes slim_v_cache_95path = {0};
slim_v_cache_95path = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(3))];
(void)slim_v_cache_95path;
slim_result = slim_fn_cache_95run(slim_v_manifest_95path, slim_v_cache_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_compiler_95run_95session(SlimVec * slim_v_args, int64_t slim_v_arg_95count, SlimRegion *slim_region) {
(void)slim_v_args;
(void)slim_v_arg_95count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_has_95paths = {0};
slim_v_has_95paths = slim_v_arg_95count >= INT64_C(4);
(void)slim_v_has_95paths;
if (!slim_v_has_95paths) {
{
SlimUnit slim_v_usage = {0};
slim_v_usage = slim_println(slim_bytes_static((const uint8_t *)"usage: slimc session INITIAL UPDATED [RECOVERED]", (int64_t)(sizeof("usage: slimc session INITIAL UPDATED [RECOVERED]") - 1)));
(void)slim_v_usage;
slim_result = INT64_C(64);
}
}
else {
{
SlimBytes slim_v_initial_95path = {0};
slim_v_initial_95path = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(2))];
(void)slim_v_initial_95path;
{
SlimBytes slim_v_updated_95path = {0};
slim_v_updated_95path = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(3))];
(void)slim_v_updated_95path;
{
bool slim_v_recovering = {0};
slim_v_recovering = slim_v_arg_95count >= INT64_C(5);
(void)slim_v_recovering;
if (!slim_v_recovering) {
slim_result = slim_fn_session_95run(slim_v_initial_95path, slim_v_updated_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimBytes slim_v_recovered_95path = {0};
slim_v_recovered_95path = ((SlimBytes *)((*slim_v_args)).data)[slim_vec_check_index(&((*slim_v_args)), INT64_C(4))];
(void)slim_v_recovered_95path;
slim_result = slim_fn_session_95run_95recovery(slim_v_initial_95path, slim_v_updated_95path, slim_v_recovered_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_compiler_95run(SlimVec slim_v_args, SlimRegion *slim_region) {
(void)slim_v_args;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_arg_95count = {0};
slim_v_arg_95count = (slim_v_args).len;
(void)slim_v_arg_95count;
{
bool slim_v_enough = {0};
slim_v_enough = slim_v_arg_95count >= INT64_C(2);
(void)slim_v_enough;
if (!slim_v_enough) {
{
SlimUnit slim_v_usage = {0};
slim_v_usage = slim_println(slim_bytes_static((const uint8_t *)"usage: slimc SOURCE | slimc check SOURCE | slimc fmt SOURCE", (int64_t)(sizeof("usage: slimc SOURCE | slimc check SOURCE | slimc fmt SOURCE") - 1)));
(void)slim_v_usage;
slim_result = INT64_C(64);
}
}
else {
{
SlimBytes slim_v_command = {0};
slim_v_command = ((SlimBytes *)(slim_v_args).data)[slim_vec_check_index(&(slim_v_args), INT64_C(1))];
(void)slim_v_command;
{
int64_t slim_v_command_95length = {0};
slim_v_command_95length = slim_bytes_len(slim_v_command);
(void)slim_v_command_95length;
{
bool slim_v_formatting = {0};
slim_v_formatting = slim_fn_syntax_95span_95equal(slim_v_command, INT64_C(0), slim_v_command_95length, slim_bytes_static((const uint8_t *)"fmt", (int64_t)(sizeof("fmt") - 1)), slim_allocation_region);
(void)slim_v_formatting;
if (slim_v_formatting) {
{
bool slim_v_has_95path = {0};
slim_v_has_95path = slim_v_arg_95count >= INT64_C(3);
(void)slim_v_has_95path;
if (slim_v_has_95path) {
{
SlimBytes slim_v_path = {0};
slim_v_path = ((SlimBytes *)(slim_v_args).data)[slim_vec_check_index(&(slim_v_args), INT64_C(2))];
(void)slim_v_path;
slim_result = slim_fn_compiler_95format_95path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
SlimUnit slim_v_usage = {0};
slim_v_usage = slim_println(slim_bytes_static((const uint8_t *)"usage: slimc fmt SOURCE", (int64_t)(sizeof("usage: slimc fmt SOURCE") - 1)));
(void)slim_v_usage;
slim_result = INT64_C(64);
}
}
}
}
else {
{
bool slim_v_checking = {0};
slim_v_checking = slim_fn_syntax_95span_95equal(slim_v_command, INT64_C(0), slim_v_command_95length, slim_bytes_static((const uint8_t *)"check", (int64_t)(sizeof("check") - 1)), slim_allocation_region);
(void)slim_v_checking;
if (slim_v_checking) {
{
bool slim_v_has_95source = {0};
slim_v_has_95source = slim_v_arg_95count >= INT64_C(3);
(void)slim_v_has_95source;
if (slim_v_has_95source) {
{
SlimBytes slim_v_source_95path = {0};
slim_v_source_95path = ((SlimBytes *)(slim_v_args).data)[slim_vec_check_index(&(slim_v_args), INT64_C(2))];
(void)slim_v_source_95path;
slim_result = slim_fn_compiler_95check_95input_95path(slim_v_source_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
SlimUnit slim_v_usage = {0};
slim_v_usage = slim_println(slim_bytes_static((const uint8_t *)"usage: slimc check SOURCE", (int64_t)(sizeof("usage: slimc check SOURCE") - 1)));
(void)slim_v_usage;
slim_result = INT64_C(64);
}
}
}
}
else {
{
bool slim_v_interface_95command = {0};
slim_v_interface_95command = slim_fn_syntax_95span_95equal(slim_v_command, INT64_C(0), slim_v_command_95length, slim_bytes_static((const uint8_t *)"interfaces", (int64_t)(sizeof("interfaces") - 1)), slim_allocation_region);
(void)slim_v_interface_95command;
if (slim_v_interface_95command) {
{
bool slim_v_has_95manifest = {0};
slim_v_has_95manifest = slim_v_arg_95count >= INT64_C(3);
(void)slim_v_has_95manifest;
if (slim_v_has_95manifest) {
{
SlimBytes slim_v_manifest_95path = {0};
slim_v_manifest_95path = ((SlimBytes *)(slim_v_args).data)[slim_vec_check_index(&(slim_v_args), INT64_C(2))];
(void)slim_v_manifest_95path;
slim_result = slim_fn_project_95interfaces_95path(slim_v_manifest_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
SlimUnit slim_v_usage = {0};
slim_v_usage = slim_println(slim_bytes_static((const uint8_t *)"usage: slimc interfaces MANIFEST", (int64_t)(sizeof("usage: slimc interfaces MANIFEST") - 1)));
(void)slim_v_usage;
slim_result = INT64_C(64);
}
}
}
}
else {
{
bool slim_v_cache_95command = {0};
slim_v_cache_95command = slim_fn_syntax_95span_95equal(slim_v_command, INT64_C(0), slim_v_command_95length, slim_bytes_static((const uint8_t *)"cache", (int64_t)(sizeof("cache") - 1)), slim_allocation_region);
(void)slim_v_cache_95command;
if (slim_v_cache_95command) {
slim_result = slim_fn_compiler_95run_95cache(&slim_v_args, slim_v_arg_95count, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_session_95command = {0};
slim_v_session_95command = slim_fn_syntax_95span_95equal(slim_v_command, INT64_C(0), slim_v_command_95length, slim_bytes_static((const uint8_t *)"session", (int64_t)(sizeof("session") - 1)), slim_allocation_region);
(void)slim_v_session_95command;
if (slim_v_session_95command) {
slim_result = slim_fn_compiler_95run_95session(&slim_v_args, slim_v_arg_95count, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_result = slim_fn_compiler_95run_95compile(&slim_v_args, slim_v_arg_95count, slim_v_command, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_main(SlimVec slim_v_args, SlimRegion *slim_region) {
(void)slim_v_args;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_result = slim_fn_compiler_95run(slim_v_args, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_memory_95storage_95join(int64_t slim_v_left, int64_t slim_v_right, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_right;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_left_95greater = {0};
slim_v_left_95greater = slim_v_left > slim_v_right;
(void)slim_v_left_95greater;
if (slim_v_left_95greater) {
slim_result = slim_v_left;
}
else {
slim_result = slim_v_right;
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_memory_95find_95type_95item(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
bool slim_v_record_95item = {0};
slim_v_record_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_95item;
{
bool slim_v_variant_95item = {0};
slim_v_variant_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
(void)slim_v_variant_95item;
{
bool slim_v_type_95item = {0};
slim_v_type_95item = slim_v_record_95item || slim_v_variant_95item;
(void)slim_v_type_95item;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same;
{
bool slim_v_found = {0};
slim_v_found = slim_v_type_95item && slim_v_same;
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_31559 = {0};
slim_t_31559 = slim_v_source;
int64_t slim_t_31561 = {0};
slim_t_31561 = slim_v_next;
int64_t slim_t_31562 = {0};
slim_t_31562 = slim_v_name_95start;
int64_t slim_t_31563 = {0};
slim_t_31563 = slim_v_name_95end;
slim_v_source = slim_t_31559;
slim_v_cursor = slim_t_31561;
slim_v_name_95start = slim_t_31562;
slim_v_name_95end = slim_t_31563;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_memory_95record_95storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_depth, int64_t slim_v_current, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_cursor;
(void)slim_v_depth;
(void)slim_v_current;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_current;
}
else {
{
int64_t slim_v_type_95index = {0};
slim_v_type_95index = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_type_95index;
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_fn_memory_95type_95storage_95kind_95from(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_type_95index, slim_v_depth, slim_allocation_region);
(void)slim_v_storage;
{
int64_t slim_v_joined = {0};
slim_v_joined = slim_fn_memory_95storage_95join(slim_v_current, slim_v_storage, slim_allocation_region);
(void)slim_v_joined;
{
bool slim_v_owned = {0};
slim_v_owned = slim_v_joined == INT64_C(2);
(void)slim_v_owned;
if (slim_v_owned) {
slim_result = INT64_C(2);
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_31712 = {0};
slim_t_31712 = slim_v_source;
int64_t slim_t_31714 = {0};
slim_t_31714 = slim_v_module_95items;
int64_t slim_t_31715 = {0};
slim_t_31715 = slim_v_next;
int64_t slim_t_31716 = {0};
slim_t_31716 = slim_v_depth;
int64_t slim_t_31717 = {0};
slim_t_31717 = slim_v_joined;
slim_v_source = slim_t_31712;
slim_v_module_95items = slim_t_31714;
slim_v_cursor = slim_t_31715;
slim_v_depth = slim_t_31716;
slim_v_current = slim_t_31717;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_memory_95payload_95storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_depth, int64_t slim_v_current, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_cursor;
(void)slim_v_depth;
(void)slim_v_current;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_current;
}
else {
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_fn_memory_95type_95storage_95kind_95from(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_cursor, slim_v_depth, slim_allocation_region);
(void)slim_v_storage;
{
int64_t slim_v_joined = {0};
slim_v_joined = slim_fn_memory_95storage_95join(slim_v_current, slim_v_storage, slim_allocation_region);
(void)slim_v_joined;
{
bool slim_v_owned = {0};
slim_v_owned = slim_v_joined == INT64_C(2);
(void)slim_v_owned;
if (slim_v_owned) {
slim_result = INT64_C(2);
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_31852 = {0};
slim_t_31852 = slim_v_source;
int64_t slim_t_31854 = {0};
slim_t_31854 = slim_v_module_95items;
int64_t slim_t_31855 = {0};
slim_t_31855 = slim_v_next;
int64_t slim_t_31856 = {0};
slim_t_31856 = slim_v_depth;
int64_t slim_t_31857 = {0};
slim_t_31857 = slim_v_joined;
slim_v_source = slim_t_31852;
slim_v_module_95items = slim_t_31854;
slim_v_cursor = slim_t_31855;
slim_v_depth = slim_t_31856;
slim_v_current = slim_t_31857;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_memory_95variant_95storage(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_depth, int64_t slim_v_current, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_cursor;
(void)slim_v_depth;
(void)slim_v_current;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_current;
}
else {
{
int64_t slim_v_payload = {0};
slim_v_payload = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_payload;
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_fn_memory_95payload_95storage(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_payload, slim_v_depth, slim_v_current, slim_allocation_region);
(void)slim_v_storage;
{
bool slim_v_owned = {0};
slim_v_owned = slim_v_storage == INT64_C(2);
(void)slim_v_owned;
if (slim_v_owned) {
slim_result = INT64_C(2);
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_31992 = {0};
slim_t_31992 = slim_v_source;
int64_t slim_t_31994 = {0};
slim_t_31994 = slim_v_module_95items;
int64_t slim_t_31995 = {0};
slim_t_31995 = slim_v_next;
int64_t slim_t_31996 = {0};
slim_t_31996 = slim_v_depth;
int64_t slim_t_31997 = {0};
slim_t_31997 = slim_v_storage;
slim_v_source = slim_t_31992;
slim_v_module_95items = slim_t_31994;
slim_v_cursor = slim_t_31995;
slim_v_depth = slim_t_31996;
slim_v_current = slim_t_31997;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_memory_95type_95storage_95kind_95from(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_module_95items, int64_t slim_v_type_95index, int64_t slim_v_depth, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_module_95items;
(void)slim_v_type_95index;
(void)slim_v_depth;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_too_95deep = {0};
slim_v_too_95deep = slim_v_depth >= INT64_C(64);
(void)slim_v_too_95deep;
if (slim_v_too_95deep) {
slim_result = INT64_C(2);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_composite = {0};
slim_v_composite = slim_v_kind == INT64_C(0);
(void)slim_v_composite;
if (slim_v_composite) {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_type_95index, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_id_95type = {0};
slim_v_id_95type = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"Id", (int64_t)(sizeof("Id") - 1)), slim_allocation_region);
(void)slim_v_id_95type;
if (slim_v_id_95type) {
slim_result = INT64_C(0);
}
else {
slim_result = INT64_C(2);
}
}
}
}
else {
{
int64_t slim_v_scalar_95kind = {0};
slim_v_scalar_95kind = slim_fn_syntax_95scalar_95type_95kind(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_scalar_95kind;
{
bool slim_v_scalar = {0};
slim_v_scalar = slim_v_scalar_95kind >= INT64_C(0);
(void)slim_v_scalar;
if (slim_v_scalar) {
{
bool slim_v_bytes_95type = {0};
slim_v_bytes_95type = slim_v_scalar_95kind == INT64_C(4);
(void)slim_v_bytes_95type;
if (slim_v_bytes_95type) {
slim_result = INT64_C(1);
}
else {
slim_result = INT64_C(0);
}
}
}
else {
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_name_95end;
{
int64_t slim_v_item = {0};
slim_v_item = slim_fn_memory_95find_95type_95item(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_item;
{
bool slim_v_found = {0};
slim_v_found = slim_v_item >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = INT64_C(2);
}
else {
{
int64_t slim_v_next_95depth = {0};
slim_v_next_95depth = slim_i64_add(slim_v_depth, INT64_C(1));
(void)slim_v_next_95depth;
{
int64_t slim_v_body = {0};
slim_v_body = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_body;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_body, INT64_C(1));
(void)slim_v_first;
{
bool slim_v_record_95item = {0};
slim_v_record_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_item, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_95item;
if (slim_v_record_95item) {
slim_result = slim_fn_memory_95record_95storage(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_first, slim_v_next_95depth, INT64_C(0), slim_allocation_region);
}
else {
slim_result = slim_fn_memory_95variant_95storage(slim_v_source, slim_v_tokens, slim_v_module_95items, slim_v_first, slim_v_next_95depth, INT64_C(0), slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_memory_95type_95storage_95kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_type_95index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_type_95index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_result = slim_fn_memory_95type_95storage_95kind_95from(slim_v_source, slim_v_tokens, INT64_C(3), slim_v_type_95index, INT64_C(0), slim_allocation_region);
return slim_result;
}

static bool slim_fn_memory_95parameters_95have_95inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
if (slim_v_borrowed) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_32459 = {0};
slim_t_32459 = slim_v_source;
int64_t slim_t_32461 = {0};
slim_t_32461 = slim_v_next;
slim_v_source = slim_t_32459;
slim_v_cursor = slim_t_32461;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_memory_95function_95uses_95local_95region(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_function, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_function;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_function, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_return_95storage = {0};
slim_v_return_95storage = slim_fn_memory_95type_95storage_95kind(slim_v_source, slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_return_95storage;
{
bool slim_v_no_95return_95storage = {0};
slim_v_no_95return_95storage = slim_v_return_95storage == INT64_C(0);
(void)slim_v_no_95return_95storage;
{
int64_t slim_v_first_95parameter = {0};
slim_v_first_95parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_95parameter;
{
bool slim_v_output = {0};
slim_v_output = slim_fn_memory_95parameters_95have_95inout(slim_v_source, slim_v_tokens, slim_v_first_95parameter, slim_allocation_region);
(void)slim_v_output;
{
bool slim_v_no_95output = {0};
slim_v_no_95output = !slim_v_output;
(void)slim_v_no_95output;
slim_result = slim_v_no_95return_95storage && slim_v_no_95output;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_memory_95effect_95list_95has_95alloc(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
bool slim_v_allocation = {0};
slim_v_allocation = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"alloc", (int64_t)(sizeof("alloc") - 1)), slim_allocation_region);
(void)slim_v_allocation;
if (slim_v_allocation) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_32672 = {0};
slim_t_32672 = slim_v_source;
int64_t slim_t_32674 = {0};
slim_t_32674 = slim_v_next;
slim_v_source = slim_t_32672;
slim_v_cursor = slim_t_32674;
goto slim_recur;
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_memory_95params_95have_95alloc_95effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_effects, INT64_C(2));
(void)slim_v_first;
slim_result = slim_fn_memory_95effect_95list_95has_95alloc(slim_v_source, slim_v_tokens, slim_v_first, slim_allocation_region);
}
}
}
return slim_result;
}

static bool slim_fn_memory_95function_95has_95alloc_95effect(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_function, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_function;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_function, INT64_C(3));
(void)slim_v_params;
slim_result = slim_fn_memory_95params_95have_95alloc_95effect(slim_v_source, slim_v_tokens, slim_v_params, slim_allocation_region);
}
return slim_result;
}

static int64_t slim_fn_memory_95last_95named_95use(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_95start, int64_t slim_v_name_95end, int64_t slim_v_last, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_v_last;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_last;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_named = {0};
slim_v_named = slim_v_kind == INT64_C(2);
(void)slim_v_named;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
if (slim_v_named) {
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
}
else {
slim_v_same = false;
}
(void)slim_v_same;
{
int64_t slim_v_next_95last = {0};
if (slim_v_same) {
slim_v_next_95last = slim_v_candidate_95end;
}
else {
slim_v_next_95last = slim_v_last;
}
(void)slim_v_next_95last;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_32950 = {0};
slim_t_32950 = slim_v_source;
int64_t slim_t_32952 = {0};
slim_t_32952 = slim_v_next;
int64_t slim_t_32953 = {0};
slim_t_32953 = slim_v_end;
int64_t slim_t_32954 = {0};
slim_t_32954 = slim_v_name_95start;
int64_t slim_t_32955 = {0};
slim_t_32955 = slim_v_name_95end;
int64_t slim_t_32956 = {0};
slim_t_32956 = slim_v_next_95last;
slim_v_source = slim_t_32950;
slim_v_cursor = slim_t_32952;
slim_v_end = slim_t_32953;
slim_v_name_95start = slim_t_32954;
slim_v_name_95end = slim_t_32955;
slim_v_last = slim_t_32956;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_memory_95span_95has_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_found = {0};
slim_v_found = slim_fn_memory_95last_95named_95use(slim_v_source, slim_v_tokens, slim_v_cursor, slim_v_end, slim_v_name_95start, slim_v_name_95end, INT64_C(-1), slim_allocation_region);
(void)slim_v_found;
slim_result = slim_v_found >= INT64_C(0);
}
return slim_result;
}

static int64_t slim_fn_memory_95result_95expression(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_expr, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_expr;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_expr, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
if (!slim_v_form) {
slim_result = slim_v_expr;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_expr, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_let_95form = {0};
slim_v_let_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
(void)slim_v_let_95form;
{
bool slim_v_set_95form = {0};
slim_v_set_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"set", (int64_t)(sizeof("set") - 1)), slim_allocation_region);
(void)slim_v_set_95form;
{
bool slim_v_pair_95form = {0};
slim_v_pair_95form = slim_v_let_95form || slim_v_set_95form;
(void)slim_v_pair_95form;
if (!slim_v_pair_95form) {
slim_result = slim_v_expr;
}
else {
{
int64_t slim_v_offset = {0};
if (slim_v_let_95form) {
slim_v_offset = INT64_C(3);
}
else {
slim_v_offset = INT64_C(2);
}
(void)slim_v_offset;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_expr, slim_v_offset);
(void)slim_v_first;
{
int64_t slim_v_value = {0};
if (slim_v_let_95form) {
slim_v_value = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_first, slim_allocation_region);
}
else {
slim_v_value = slim_v_first;
}
(void)slim_v_value;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_body;
SlimBytes slim_t_33197 = {0};
slim_t_33197 = slim_v_source;
int64_t slim_t_33199 = {0};
slim_t_33199 = slim_v_body;
slim_v_source = slim_t_33197;
slim_v_expr = slim_t_33199;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_memory_95escapes_95to_95inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
if (!slim_v_form) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_33314 = {0};
slim_t_33314 = slim_v_source;
int64_t slim_t_33316 = {0};
slim_t_33316 = slim_v_params;
int64_t slim_t_33317 = {0};
slim_t_33317 = slim_v_next;
int64_t slim_t_33318 = {0};
slim_t_33318 = slim_v_end;
int64_t slim_t_33319 = {0};
slim_t_33319 = slim_v_name_95start;
int64_t slim_t_33320 = {0};
slim_t_33320 = slim_v_name_95end;
slim_v_source = slim_t_33314;
slim_v_params = slim_t_33316;
slim_v_cursor = slim_t_33317;
slim_v_end = slim_t_33318;
slim_v_name_95start = slim_t_33319;
slim_v_name_95end = slim_t_33320;
goto slim_recur;
}
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_95form = {0};
slim_v_call_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
(void)slim_v_call_95form;
if (!slim_v_call_95form) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_33365 = {0};
slim_t_33365 = slim_v_source;
int64_t slim_t_33367 = {0};
slim_t_33367 = slim_v_params;
int64_t slim_t_33368 = {0};
slim_t_33368 = slim_v_next;
int64_t slim_t_33369 = {0};
slim_t_33369 = slim_v_end;
int64_t slim_t_33370 = {0};
slim_t_33370 = slim_v_name_95start;
int64_t slim_t_33371 = {0};
slim_t_33371 = slim_v_name_95end;
slim_v_source = slim_t_33365;
slim_v_params = slim_t_33367;
slim_v_cursor = slim_t_33368;
slim_v_end = slim_t_33369;
slim_v_name_95start = slim_t_33370;
slim_v_name_95end = slim_t_33371;
goto slim_recur;
}
}
else {
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
{
bool slim_v_vector_95push = {0};
slim_v_vector_95push = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.push", (int64_t)(sizeof("vec.push") - 1)), slim_allocation_region);
(void)slim_v_vector_95push;
{
bool slim_v_arena_95add = {0};
slim_v_arena_95add = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.add", (int64_t)(sizeof("arena.add") - 1)), slim_allocation_region);
(void)slim_v_arena_95add;
{
bool slim_v_store = {0};
slim_v_store = slim_v_vector_95push || slim_v_arena_95add;
(void)slim_v_store;
if (!slim_v_store) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_33438 = {0};
slim_t_33438 = slim_v_source;
int64_t slim_t_33440 = {0};
slim_t_33440 = slim_v_params;
int64_t slim_t_33441 = {0};
slim_t_33441 = slim_v_next;
int64_t slim_t_33442 = {0};
slim_t_33442 = slim_v_end;
int64_t slim_t_33443 = {0};
slim_t_33443 = slim_v_name_95start;
int64_t slim_t_33444 = {0};
slim_t_33444 = slim_v_name_95end;
slim_v_source = slim_t_33438;
slim_v_params = slim_t_33440;
slim_v_cursor = slim_t_33441;
slim_v_end = slim_t_33442;
slim_v_name_95start = slim_t_33443;
slim_v_name_95end = slim_t_33444;
goto slim_recur;
}
}
else {
{
int64_t slim_v_target = {0};
slim_v_target = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_target;
{
int64_t slim_v_value = {0};
slim_v_value = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_target, slim_allocation_region);
(void)slim_v_value;
{
int64_t slim_v_value_95start = {0};
slim_v_value_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_value_95start;
{
int64_t slim_v_value_95end = {0};
slim_v_value_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_value, slim_allocation_region);
(void)slim_v_value_95end;
{
bool slim_v_same_95value = {0};
slim_v_same_95value = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_value_95start, slim_v_value_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same_95value;
{
bool slim_v_target_95inout = {0};
if (slim_v_same_95value) {
slim_v_target_95inout = slim_fn_syntax_95name_95is_95inout(slim_v_source, slim_v_tokens, slim_v_params, slim_v_target, slim_allocation_region);
}
else {
slim_v_target_95inout = false;
}
(void)slim_v_target_95inout;
if (slim_v_target_95inout) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_33547 = {0};
slim_t_33547 = slim_v_source;
int64_t slim_t_33549 = {0};
slim_t_33549 = slim_v_params;
int64_t slim_t_33550 = {0};
slim_t_33550 = slim_v_next;
int64_t slim_t_33551 = {0};
slim_t_33551 = slim_v_end;
int64_t slim_t_33552 = {0};
slim_t_33552 = slim_v_name_95start;
int64_t slim_t_33553 = {0};
slim_t_33553 = slim_v_name_95end;
slim_v_source = slim_t_33547;
slim_v_params = slim_t_33549;
slim_v_cursor = slim_t_33550;
slim_v_end = slim_t_33551;
slim_v_name_95start = slim_t_33552;
slim_v_name_95end = slim_t_33553;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_memory_95escape_95kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_95end, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body;
(void)slim_v_body_95end;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_result = {0};
slim_v_result = slim_fn_memory_95result_95expression(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_result;
{
int64_t slim_v_result_95end = {0};
slim_v_result_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_result, slim_allocation_region);
(void)slim_v_result_95end;
{
bool slim_v_result_95escape = {0};
slim_v_result_95escape = slim_fn_memory_95span_95has_95name(slim_v_source, slim_v_tokens, slim_v_result, slim_v_result_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_result_95escape;
{
bool slim_v_output_95escape = {0};
slim_v_output_95escape = slim_fn_memory_95escapes_95to_95inout(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_output_95escape;
if (slim_v_result_95escape) {
if (slim_v_output_95escape) {
slim_result = INT64_C(3);
}
else {
slim_result = INT64_C(1);
}
}
else {
if (slim_v_output_95escape) {
slim_result = INT64_C(2);
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_memory_95append_95value_95plan(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_95end, int64_t slim_v_name, int64_t slim_v_type_95index, bool slim_v_borrowed, int64_t slim_v_declared, SlimVec * slim_v_values, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body;
(void)slim_v_body_95end;
(void)slim_v_name;
(void)slim_v_type_95index;
(void)slim_v_borrowed;
(void)slim_v_declared;
(void)slim_v_values;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_fn_memory_95type_95storage_95kind(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_storage;
{
Slim_type_memory_95ValuePlan slim_v_plan = {0};
{
int64_t slim_t_33810 = {0};
slim_t_33810 = slim_v_name;
int64_t slim_t_33814 = {0};
slim_t_33814 = slim_v_type_95index;
int64_t slim_t_33818 = {0};
slim_t_33818 = slim_v_storage;
bool slim_t_33822 = {0};
slim_t_33822 = slim_v_borrowed;
int64_t slim_t_33826 = {0};
slim_t_33826 = slim_v_declared;
int64_t slim_t_33830 = {0};
slim_t_33830 = slim_v_name_95end;
int64_t slim_t_33834 = {0};
slim_t_33834 = INT64_C(0);
slim_v_plan = (Slim_type_memory_95ValuePlan){.slim_field_name = slim_t_33810, .slim_field_type_95index = slim_t_33814, .slim_field_storage = slim_t_33818, .slim_field_borrowed = slim_t_33822, .slim_field_declared = slim_t_33826, .slim_field_last_95use = slim_t_33830, .slim_field_escape = slim_t_33834};
}
(void)slim_v_plan;
if (!slim_vec_push(slim_v_values, &slim_v_plan)) goto slim_allocation_failed; slim_result = (SlimUnit){0};
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_memory_95append_95parameter_95plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_95end, int64_t slim_v_cursor, SlimVec * slim_v_values, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body;
(void)slim_v_body_95end;
(void)slim_v_cursor;
(void)slim_v_values;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_name = {0};
if (slim_v_borrowed) {
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
}
else {
slim_v_name = slim_v_head;
}
(void)slim_v_name;
{
int64_t slim_v_type_95index = {0};
if (slim_v_borrowed) {
slim_v_type_95index = slim_i64_add(slim_v_cursor, INT64_C(3));
}
else {
slim_v_type_95index = slim_i64_add(slim_v_cursor, INT64_C(2));
}
(void)slim_v_type_95index;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_memory_95append_95value_95plan(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_95end, slim_v_name, slim_v_type_95index, slim_v_borrowed, slim_v_cursor, slim_v_values, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_34025 = {0};
slim_t_34025 = slim_v_source;
int64_t slim_t_34027 = {0};
slim_t_34027 = slim_v_params;
int64_t slim_t_34028 = {0};
slim_t_34028 = slim_v_body;
int64_t slim_t_34029 = {0};
slim_t_34029 = slim_v_body_95end;
int64_t slim_t_34030 = {0};
slim_t_34030 = slim_v_next;
slim_v_source = slim_t_34025;
slim_v_params = slim_t_34027;
slim_v_body = slim_t_34028;
slim_v_body_95end = slim_t_34029;
slim_v_cursor = slim_t_34030;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_memory_95append_95local_95plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_95end, int64_t slim_v_cursor, SlimVec * slim_v_values, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body;
(void)slim_v_body_95end;
(void)slim_v_cursor;
(void)slim_v_values;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_body_95end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_let_95form = {0};
if (slim_v_form) {
slim_v_let_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
}
else {
slim_v_let_95form = false;
}
(void)slim_v_let_95form;
{
SlimUnit slim_v_appended = {0};
if (slim_v_let_95form) {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_type_95index = {0};
slim_v_type_95index = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_type_95index;
slim_v_appended = slim_fn_memory_95append_95value_95plan(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_95end, slim_v_name, slim_v_type_95index, false, slim_v_cursor, slim_v_values, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_v_appended = (SlimUnit){0};
}
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_34226 = {0};
slim_t_34226 = slim_v_source;
int64_t slim_t_34228 = {0};
slim_t_34228 = slim_v_params;
int64_t slim_t_34229 = {0};
slim_t_34229 = slim_v_body;
int64_t slim_t_34230 = {0};
slim_t_34230 = slim_v_body_95end;
int64_t slim_t_34231 = {0};
slim_t_34231 = slim_v_next;
slim_v_source = slim_t_34226;
slim_v_params = slim_t_34228;
slim_v_body = slim_t_34229;
slim_v_body_95end = slim_t_34230;
slim_v_cursor = slim_t_34231;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_memory_95append_95allocation_95plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_end, int64_t slim_v_region, SlimVec * slim_v_allocations, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_region;
(void)slim_v_allocations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
bool slim_v_allocation = {0};
if (!slim_v_form) {
slim_v_allocation = false;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_95form = {0};
slim_v_call_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
(void)slim_v_call_95form;
if (!slim_v_call_95form) {
slim_v_allocation = false;
}
else {
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
{
bool slim_v_vector_95new = {0};
slim_v_vector_95new = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"vec.new", (int64_t)(sizeof("vec.new") - 1)), slim_allocation_region);
(void)slim_v_vector_95new;
{
bool slim_v_arena_95new = {0};
slim_v_arena_95new = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_callee, slim_bytes_static((const uint8_t *)"arena.new", (int64_t)(sizeof("arena.new") - 1)), slim_allocation_region);
(void)slim_v_arena_95new;
slim_v_allocation = slim_v_vector_95new || slim_v_arena_95new;
}
}
}
}
}
}
}
(void)slim_v_allocation;
{
SlimUnit slim_v_appended = {0};
if (slim_v_allocation) {
{
Slim_type_memory_95AllocationPlan slim_v_plan = {0};
{
int64_t slim_t_34435 = {0};
slim_t_34435 = slim_v_cursor;
int64_t slim_t_34439 = {0};
slim_t_34439 = slim_v_region;
slim_v_plan = (Slim_type_memory_95AllocationPlan){.slim_field_site = slim_t_34435, .slim_field_region = slim_t_34439};
}
(void)slim_v_plan;
if (!slim_vec_push(slim_v_allocations, &slim_v_plan)) goto slim_allocation_failed; slim_v_appended = (SlimUnit){0};
}
}
else {
slim_v_appended = (SlimUnit){0};
}
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_34469 = {0};
slim_t_34469 = slim_v_source;
int64_t slim_t_34471 = {0};
slim_t_34471 = slim_v_next;
int64_t slim_t_34472 = {0};
slim_t_34472 = slim_v_end;
int64_t slim_t_34473 = {0};
slim_t_34473 = slim_v_region;
slim_v_source = slim_t_34469;
slim_v_cursor = slim_t_34471;
slim_v_end = slim_t_34472;
slim_v_region = slim_t_34473;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_memory_95conservative_95escape(int64_t slim_v_return_95storage, bool slim_v_output, SlimRegion *slim_region) {
(void)slim_v_return_95storage;
(void)slim_v_output;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_returns = {0};
slim_v_returns = slim_v_return_95storage > INT64_C(0);
(void)slim_v_returns;
if (slim_v_returns) {
if (slim_v_output) {
slim_result = INT64_C(3);
}
else {
slim_result = INT64_C(1);
}
}
else {
if (slim_v_output) {
slim_result = INT64_C(2);
}
else {
slim_result = INT64_C(0);
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimUnit slim_fn_memory_95finalize_95value_95plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_body, int64_t slim_v_body_95end, int64_t slim_v_return_95storage, bool slim_v_output, SlimVec * slim_v_values, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_body;
(void)slim_v_body_95end;
(void)slim_v_return_95storage;
(void)slim_v_output;
(void)slim_v_values;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_values)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
Slim_type_memory_95ValuePlan slim_v_value = {0};
slim_v_value = ((Slim_type_memory_95ValuePlan *)((*slim_v_values)).data)[slim_vec_check_index(&((*slim_v_values)), slim_v_index)];
(void)slim_v_value;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_value.slim_field_name;
(void)slim_v_name;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
{
bool slim_v_bounded = {0};
slim_v_bounded = slim_v_index < INT64_C(64);
(void)slim_v_bounded;
{
int64_t slim_v_last_95use = {0};
if (slim_v_bounded) {
{
int64_t slim_v_declared = {0};
slim_v_declared = slim_v_value.slim_field_declared;
(void)slim_v_declared;
slim_v_last_95use = slim_fn_memory_95last_95named_95use(slim_v_source, slim_v_tokens, slim_v_declared, slim_v_body_95end, slim_v_name_95start, slim_v_name_95end, slim_v_name_95end, slim_allocation_region);
}
}
else {
slim_v_last_95use = slim_v_body_95end;
}
(void)slim_v_last_95use;
{
int64_t slim_v_escape = {0};
if (slim_v_bounded) {
slim_v_escape = slim_fn_memory_95escape_95kind(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
}
else {
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_v_value.slim_field_storage;
(void)slim_v_storage;
{
bool slim_v_stored = {0};
slim_v_stored = slim_v_storage > INT64_C(0);
(void)slim_v_stored;
if (slim_v_stored) {
slim_v_escape = slim_fn_memory_95conservative_95escape(slim_v_return_95storage, slim_v_output, slim_allocation_region);
}
else {
slim_v_escape = INT64_C(0);
}
}
}
}
(void)slim_v_escape;
{
Slim_type_memory_95ValuePlan slim_v_updated = {0};
{
int64_t slim_t_34786 = {0};
slim_t_34786 = slim_v_name;
int64_t slim_t_34790 = {0};
slim_t_34790 = slim_v_value.slim_field_type_95index;
int64_t slim_t_34798 = {0};
slim_t_34798 = slim_v_value.slim_field_storage;
bool slim_t_34806 = {0};
slim_t_34806 = slim_v_value.slim_field_borrowed;
int64_t slim_t_34814 = {0};
slim_t_34814 = slim_v_value.slim_field_declared;
int64_t slim_t_34822 = {0};
slim_t_34822 = slim_v_last_95use;
int64_t slim_t_34826 = {0};
slim_t_34826 = slim_v_escape;
slim_v_updated = (Slim_type_memory_95ValuePlan){.slim_field_name = slim_t_34786, .slim_field_type_95index = slim_t_34790, .slim_field_storage = slim_t_34798, .slim_field_borrowed = slim_t_34806, .slim_field_declared = slim_t_34814, .slim_field_last_95use = slim_t_34822, .slim_field_escape = slim_t_34826};
}
(void)slim_v_updated;
{
SlimUnit slim_v_replaced = {0};
slim_vec_set(slim_v_values, slim_v_index, &slim_v_updated); slim_v_replaced = (SlimUnit){0};
(void)slim_v_replaced;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_34854 = {0};
slim_t_34854 = slim_v_source;
int64_t slim_t_34856 = {0};
slim_t_34856 = slim_v_params;
int64_t slim_t_34857 = {0};
slim_t_34857 = slim_v_body;
int64_t slim_t_34858 = {0};
slim_t_34858 = slim_v_body_95end;
int64_t slim_t_34859 = {0};
slim_t_34859 = slim_v_return_95storage;
bool slim_t_34860 = {0};
slim_t_34860 = slim_v_output;
int64_t slim_t_34862 = {0};
slim_t_34862 = slim_v_next;
slim_v_source = slim_t_34854;
slim_v_params = slim_t_34856;
slim_v_body = slim_t_34857;
slim_v_body_95end = slim_t_34858;
slim_v_return_95storage = slim_t_34859;
slim_v_output = slim_t_34860;
slim_v_index = slim_t_34862;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_memory_95count_95escaping_95values(SlimVec * slim_v_values, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_values;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_values)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
Slim_type_memory_95ValuePlan slim_v_value = {0};
slim_v_value = ((Slim_type_memory_95ValuePlan *)((*slim_v_values)).data)[slim_vec_check_index(&((*slim_v_values)), slim_v_index)];
(void)slim_v_value;
{
int64_t slim_v_storage = {0};
slim_v_storage = slim_v_value.slim_field_storage;
(void)slim_v_storage;
{
int64_t slim_v_escape = {0};
slim_v_escape = slim_v_value.slim_field_escape;
(void)slim_v_escape;
{
bool slim_v_stored = {0};
slim_v_stored = slim_v_storage > INT64_C(0);
(void)slim_v_stored;
{
bool slim_v_escapes = {0};
slim_v_escapes = slim_v_escape > INT64_C(0);
(void)slim_v_escapes;
{
bool slim_v_escaping = {0};
slim_v_escaping = slim_v_stored && slim_v_escapes;
(void)slim_v_escaping;
{
int64_t slim_v_increment = {0};
if (slim_v_escaping) {
slim_v_increment = INT64_C(1);
}
else {
slim_v_increment = INT64_C(0);
}
(void)slim_v_increment;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_increment);
(void)slim_v_total;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_35030 = {0};
slim_t_35030 = slim_v_next;
int64_t slim_t_35031 = {0};
slim_t_35031 = slim_v_total;
slim_v_index = slim_t_35030;
slim_v_count = slim_t_35031;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static Slim_type_memory_95FunctionPlan slim_fn_memory_95build_95function_95plan(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_function, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_function;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_memory_95FunctionPlan slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_function, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_return_95storage = {0};
slim_v_return_95storage = slim_fn_memory_95type_95storage_95kind(slim_v_source, slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_return_95storage;
{
bool slim_v_local_95region = {0};
slim_v_local_95region = slim_fn_memory_95function_95uses_95local_95region(slim_v_source, slim_v_tokens, slim_v_function, slim_allocation_region);
(void)slim_v_local_95region;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
int64_t slim_v_body_95end = {0};
slim_v_body_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95end;
{
SlimVec slim_v_values = {0};
slim_v_values = slim_vec_new(sizeof(Slim_type_memory_95ValuePlan), slim_allocation_region);
(void)slim_v_values;
{
int64_t slim_v_first_95parameter = {0};
slim_v_first_95parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_95parameter;
{
SlimUnit slim_v_parameters = {0};
slim_v_parameters = slim_fn_memory_95append_95parameter_95plans(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_95end, slim_v_first_95parameter, &slim_v_values, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_parameters;
{
SlimUnit slim_v_locals = {0};
slim_v_locals = slim_fn_memory_95append_95local_95plans(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_95end, slim_v_body, &slim_v_values, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_locals;
{
int64_t slim_v_value_95count = {0};
slim_v_value_95count = (slim_v_values).len;
(void)slim_v_value_95count;
{
bool slim_v_liveness_95precise = {0};
slim_v_liveness_95precise = slim_v_value_95count <= INT64_C(64);
(void)slim_v_liveness_95precise;
{
int64_t slim_v_first_95parameter_95for_95output = {0};
slim_v_first_95parameter_95for_95output = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_95parameter_95for_95output;
{
bool slim_v_output_95escape = {0};
slim_v_output_95escape = slim_fn_memory_95parameters_95have_95inout(slim_v_source, slim_v_tokens, slim_v_first_95parameter_95for_95output, slim_allocation_region);
(void)slim_v_output_95escape;
{
SlimUnit slim_v_finalized = {0};
slim_v_finalized = slim_fn_memory_95finalize_95value_95plans(slim_v_source, slim_v_tokens, slim_v_params, slim_v_body, slim_v_body_95end, slim_v_return_95storage, slim_v_output_95escape, &slim_v_values, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_finalized;
{
SlimVec slim_v_allocations = {0};
slim_v_allocations = slim_vec_new(sizeof(Slim_type_memory_95AllocationPlan), slim_allocation_region);
(void)slim_v_allocations;
{
int64_t slim_v_region = {0};
if (slim_v_local_95region) {
slim_v_region = INT64_C(0);
}
else {
slim_v_region = INT64_C(1);
}
(void)slim_v_region;
{
SlimUnit slim_v_allocation_95plans = {0};
slim_v_allocation_95plans = slim_fn_memory_95append_95allocation_95plans(slim_v_source, slim_v_tokens, slim_v_body, slim_v_body_95end, slim_v_region, &slim_v_allocations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_allocation_95plans;
{
SlimVec slim_v_destructions = {0};
slim_v_destructions = slim_vec_new(sizeof(Slim_type_memory_95DestructionPlan), slim_allocation_region);
(void)slim_v_destructions;
{
SlimUnit slim_v_destruction = {0};
if (slim_v_local_95region) {
{
Slim_type_memory_95DestructionPlan slim_v_plan = {0};
{
int64_t slim_t_35322 = {0};
slim_t_35322 = INT64_C(0);
int64_t slim_t_35326 = {0};
slim_t_35326 = slim_v_body_95end;
bool slim_t_35330 = {0};
slim_t_35330 = true;
slim_v_plan = (Slim_type_memory_95DestructionPlan){.slim_field_region = slim_t_35322, .slim_field_end = slim_t_35326, .slim_field_reverse = slim_t_35330};
}
(void)slim_v_plan;
if (!slim_vec_push(&slim_v_destructions, &slim_v_plan)) goto slim_allocation_failed; slim_v_destruction = (SlimUnit){0};
}
}
else {
slim_v_destruction = (SlimUnit){0};
}
(void)slim_v_destruction;
{
int64_t slim_v_escaping = {0};
slim_v_escaping = slim_fn_memory_95count_95escaping_95values(&slim_v_values, INT64_C(0), INT64_C(0), slim_allocation_region);
(void)slim_v_escaping;
{
int64_t slim_t_35362 = {0};
slim_t_35362 = slim_v_function;
int64_t slim_t_35366 = {0};
slim_t_35366 = slim_v_return_95storage;
bool slim_t_35370 = {0};
slim_t_35370 = slim_v_local_95region;
bool slim_t_35374 = {0};
slim_t_35374 = slim_v_liveness_95precise;
SlimVec slim_t_35378 = {0};
slim_t_35378 = slim_v_values;
SlimVec slim_t_35382 = {0};
slim_t_35382 = slim_v_allocations;
SlimVec slim_t_35386 = {0};
slim_t_35386 = slim_v_destructions;
int64_t slim_t_35390 = {0};
slim_t_35390 = slim_v_escaping;
slim_result = (Slim_type_memory_95FunctionPlan){.slim_field_function = slim_t_35362, .slim_field_return_95storage = slim_t_35366, .slim_field_local_95region = slim_t_35370, .slim_field_liveness_95precise = slim_t_35374, .slim_field_values = slim_t_35378, .slim_field_allocations = slim_t_35382, .slim_field_destructions = slim_t_35386, .slim_field_escaping_95values = slim_t_35390};
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_memory_95append_95function_95plans(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, int64_t slim_v_index, SlimVec * slim_v_functions, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_v_index;
(void)slim_v_functions;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_declarations)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
Slim_type_ir_95Declaration slim_v_declaration = {0};
slim_v_declaration = ((Slim_type_ir_95Declaration *)((*slim_v_declarations)).data)[slim_vec_check_index(&((*slim_v_declarations)), slim_v_index)];
(void)slim_v_declaration;
{
int64_t slim_v_form = {0};
slim_v_form = slim_v_declaration.slim_field_form;
(void)slim_v_form;
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_form, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
SlimUnit slim_v_appended = {0};
if (slim_v_function_95item) {
{
Slim_type_memory_95FunctionPlan slim_v_plan = {0};
slim_v_plan = slim_fn_memory_95build_95function_95plan(slim_v_source, slim_v_tokens, slim_v_form, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_plan;
if (!slim_vec_push(slim_v_functions, &slim_v_plan)) goto slim_allocation_failed; slim_v_appended = (SlimUnit){0};
}
}
else {
slim_v_appended = (SlimUnit){0};
}
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_35565 = {0};
slim_t_35565 = slim_v_source;
int64_t slim_t_35568 = {0};
slim_t_35568 = slim_v_next;
slim_v_source = slim_t_35565;
slim_v_index = slim_t_35568;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static Slim_type_memory_95Plan slim_fn_memory_95analyze(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_memory_95Plan slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_functions = {0};
slim_v_functions = slim_vec_new(sizeof(Slim_type_memory_95FunctionPlan), slim_allocation_region);
(void)slim_v_functions;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_memory_95append_95function_95plans(slim_v_source, slim_v_tokens, slim_v_declarations, INT64_C(0), &slim_v_functions, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
{
SlimVec slim_t_35639 = {0};
slim_t_35639 = slim_v_functions;
slim_result = (Slim_type_memory_95Plan){.slim_field_functions = slim_t_35639};
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_project_95cross_95span_95chars_95equal(SlimBytes slim_v_left, int64_t slim_v_left_95start, SlimBytes slim_v_right, int64_t slim_v_right_95start, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_left_95start;
(void)slim_v_right;
(void)slim_v_right_95start;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index == slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
int64_t slim_v_left_95index = {0};
slim_v_left_95index = slim_i64_add(slim_v_left_95start, slim_v_index);
(void)slim_v_left_95index;
{
int64_t slim_v_right_95index = {0};
slim_v_right_95index = slim_i64_add(slim_v_right_95start, slim_v_index);
(void)slim_v_right_95index;
{
uint8_t slim_v_left_95byte = {0};
slim_v_left_95byte = slim_bytes_get(slim_v_left, slim_v_left_95index);
(void)slim_v_left_95byte;
{
uint8_t slim_v_right_95byte = {0};
slim_v_right_95byte = slim_bytes_get(slim_v_right, slim_v_right_95index);
(void)slim_v_right_95byte;
{
int64_t slim_v_left_95code = {0};
slim_v_left_95code = (int64_t)slim_v_left_95byte;
(void)slim_v_left_95code;
{
int64_t slim_v_right_95code = {0};
slim_v_right_95code = (int64_t)slim_v_right_95byte;
(void)slim_v_right_95code;
{
bool slim_v_same = {0};
slim_v_same = slim_v_left_95code == slim_v_right_95code;
(void)slim_v_same;
if (slim_v_same) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_35785 = {0};
slim_t_35785 = slim_v_left;
int64_t slim_t_35786 = {0};
slim_t_35786 = slim_v_left_95start;
SlimBytes slim_t_35787 = {0};
slim_t_35787 = slim_v_right;
int64_t slim_t_35788 = {0};
slim_t_35788 = slim_v_right_95start;
int64_t slim_t_35789 = {0};
slim_t_35789 = slim_v_next;
int64_t slim_t_35790 = {0};
slim_t_35790 = slim_v_length;
slim_v_left = slim_t_35785;
slim_v_left_95start = slim_t_35786;
slim_v_right = slim_t_35787;
slim_v_right_95start = slim_t_35788;
slim_v_index = slim_t_35789;
slim_v_length = slim_t_35790;
goto slim_recur;
}
}
else {
slim_result = false;
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_95cross_95spans_95equal(SlimBytes slim_v_left, int64_t slim_v_left_95start, int64_t slim_v_left_95end, SlimBytes slim_v_right, int64_t slim_v_right_95start, int64_t slim_v_right_95end, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_left_95start;
(void)slim_v_left_95end;
(void)slim_v_right;
(void)slim_v_right_95start;
(void)slim_v_right_95end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_left_95length = {0};
slim_v_left_95length = slim_i64_sub(slim_v_left_95end, slim_v_left_95start);
(void)slim_v_left_95length;
{
int64_t slim_v_right_95length = {0};
slim_v_right_95length = slim_i64_sub(slim_v_right_95end, slim_v_right_95start);
(void)slim_v_right_95length;
{
bool slim_v_same_95length = {0};
slim_v_same_95length = slim_v_left_95length == slim_v_right_95length;
(void)slim_v_same_95length;
if (slim_v_same_95length) {
slim_result = slim_fn_project_95cross_95span_95chars_95equal(slim_v_left, slim_v_left_95start, slim_v_right, slim_v_right_95start, INT64_C(0), slim_v_left_95length, slim_allocation_region);
}
else {
slim_result = false;
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_project_95last_95path_95slash(SlimBytes slim_v_path, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_last, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_v_last;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_last;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_path, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_slash = {0};
slim_v_slash = slim_v_code == INT64_C(47);
(void)slim_v_slash;
{
int64_t slim_v_next_95last = {0};
if (slim_v_slash) {
slim_v_next_95last = slim_i64_add(slim_v_index, INT64_C(1));
}
else {
slim_v_next_95last = slim_v_last;
}
(void)slim_v_next_95last;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_36006 = {0};
slim_t_36006 = slim_v_path;
int64_t slim_t_36007 = {0};
slim_t_36007 = slim_v_next;
int64_t slim_t_36008 = {0};
slim_t_36008 = slim_v_length;
int64_t slim_t_36009 = {0};
slim_t_36009 = slim_v_next_95last;
slim_v_path = slim_t_36006;
slim_v_index = slim_t_36007;
slim_v_length = slim_t_36008;
slim_v_last = slim_t_36009;
goto slim_recur;
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimBytes slim_fn_project_95project_95module_95path(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_path_95token, SlimRegion *slim_region) {
(void)slim_v_manifest_95path;
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_path_95token;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
int64_t slim_v_manifest_95length = {0};
slim_v_manifest_95length = slim_bytes_len(slim_v_manifest_95path);
(void)slim_v_manifest_95length;
{
int64_t slim_v_prefix_95end = {0};
slim_v_prefix_95end = slim_fn_project_95last_95path_95slash(slim_v_manifest_95path, INT64_C(0), slim_v_manifest_95length, INT64_C(0), slim_allocation_region);
(void)slim_v_prefix_95end;
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95span(slim_v_manifest_95path, INT64_C(0), slim_v_prefix_95end, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
int64_t slim_v_token_95start_95index = {0};
slim_v_token_95start_95index = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_path_95token, slim_allocation_region);
(void)slim_v_token_95start_95index;
{
int64_t slim_v_content_95start = {0};
slim_v_content_95start = slim_i64_add(slim_v_token_95start_95index, INT64_C(1));
(void)slim_v_content_95start;
{
int64_t slim_v_token_95end_95index = {0};
slim_v_token_95end_95index = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_path_95token, slim_allocation_region);
(void)slim_v_token_95end_95index;
{
int64_t slim_v_content_95end = {0};
slim_v_content_95end = slim_i64_sub(slim_v_token_95end_95index, INT64_C(1));
(void)slim_v_content_95end;
{
SlimUnit slim_v_path = {0};
slim_v_path = slim_fn_text_95append_95span(slim_v_manifest_95source, slim_v_content_95start, slim_v_content_95end, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_path;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_project_95declaration_95has_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
bool slim_v_record_95item = {0};
slim_v_record_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_95item;
{
bool slim_v_variant_95item = {0};
slim_v_variant_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
(void)slim_v_variant_95item;
{
bool slim_v_first = {0};
slim_v_first = slim_v_function_95item || slim_v_record_95item;
(void)slim_v_first;
{
bool slim_v_declaration = {0};
slim_v_declaration = slim_v_first || slim_v_variant_95item;
(void)slim_v_declaration;
{
int64_t slim_v_candidate = {0};
slim_v_candidate = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_candidate;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_candidate, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_candidate, slim_allocation_region);
(void)slim_v_candidate_95end;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
{
bool slim_v_same = {0};
if (slim_v_declaration) {
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
}
else {
slim_v_same = false;
}
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_36372 = {0};
slim_t_36372 = slim_v_source;
int64_t slim_t_36374 = {0};
slim_t_36374 = slim_v_next;
int64_t slim_t_36375 = {0};
slim_t_36375 = slim_v_name;
slim_v_source = slim_t_36372;
slim_v_cursor = slim_t_36374;
slim_v_name = slim_t_36375;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_project_95append_95slash_95replaced(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_slash = {0};
slim_v_slash = slim_v_code == INT64_C(47);
(void)slim_v_slash;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_slash) {
slim_v_emitted = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"_", (int64_t)(sizeof("_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
if (!slim_vec_push(slim_v_output, &slim_v_byte)) goto slim_allocation_failed; slim_v_emitted = (SlimUnit){0};
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_36513 = {0};
slim_t_36513 = slim_v_source;
int64_t slim_t_36514 = {0};
slim_t_36514 = slim_v_next;
int64_t slim_t_36515 = {0};
slim_t_36515 = slim_v_end;
slim_v_source = slim_t_36513;
slim_v_index = slim_t_36514;
slim_v_end = slim_t_36515;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_95append_95project_95atom(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, bool slim_v_entry, int64_t slim_v_atom, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_module_95items;
(void)slim_v_entry;
(void)slim_v_atom;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_module_95tokens, slim_v_atom, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_module_95tokens, slim_v_atom, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_slashes = {0};
slim_v_slashes = slim_fn_syntax_95span_95count_95byte(slim_v_module_95source, slim_v_start, slim_v_end, INT64_C(47), INT64_C(0), slim_allocation_region);
(void)slim_v_slashes;
{
bool slim_v_qualified = {0};
slim_v_qualified = slim_v_slashes > INT64_C(0);
(void)slim_v_qualified;
if (slim_v_qualified) {
slim_result = slim_fn_project_95append_95slash_95replaced(slim_v_module_95source, slim_v_start, slim_v_end, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
bool slim_v_declaration = {0};
slim_v_declaration = slim_fn_project_95declaration_95has_95name(slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_v_atom, slim_allocation_region);
(void)slim_v_declaration;
{
bool slim_v_main_95name = {0};
slim_v_main_95name = slim_fn_syntax_95token_95equal(slim_v_module_95source, slim_v_module_95tokens, slim_v_atom, slim_bytes_static((const uint8_t *)"main", (int64_t)(sizeof("main") - 1)), slim_allocation_region);
(void)slim_v_main_95name;
{
bool slim_v_entry_95main = {0};
slim_v_entry_95main = slim_v_entry && slim_v_main_95name;
(void)slim_v_entry_95main;
{
bool slim_v_prefix = {0};
if (slim_v_declaration) {
slim_v_prefix = !slim_v_entry_95main;
}
else {
slim_v_prefix = false;
}
(void)slim_v_prefix;
if (slim_v_prefix) {
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_95append_95token(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_separator = {0};
slim_v_separator = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"_", (int64_t)(sizeof("_") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_separator;
slim_result = slim_fn_text_95append_95token(slim_v_module_95source, slim_v_module_95tokens, slim_v_atom, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = slim_fn_text_95append_95token(slim_v_module_95source, slim_v_module_95tokens, slim_v_atom, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_95append_95project_95tokens(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, bool slim_v_entry, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_previous_95kind, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_module_95items;
(void)slim_v_entry;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_previous_95kind;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_right = {0};
slim_v_right = slim_v_kind == INT64_C(1);
(void)slim_v_right;
{
bool slim_v_previous_95left = {0};
slim_v_previous_95left = slim_v_previous_95kind == INT64_C(0);
(void)slim_v_previous_95left;
{
bool slim_v_not_95right = {0};
slim_v_not_95right = !slim_v_right;
(void)slim_v_not_95right;
{
bool slim_v_not_95previous_95left = {0};
slim_v_not_95previous_95left = !slim_v_previous_95left;
(void)slim_v_not_95previous_95left;
{
bool slim_v_separated = {0};
slim_v_separated = slim_v_not_95right && slim_v_not_95previous_95left;
(void)slim_v_separated;
{
SlimUnit slim_v_separator = {0};
if (slim_v_separated) {
slim_v_separator = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_separator = (SlimUnit){0};
}
(void)slim_v_separator;
{
bool slim_v_atom = {0};
slim_v_atom = slim_v_kind == INT64_C(2);
(void)slim_v_atom;
{
SlimUnit slim_v_emitted = {0};
if (slim_v_atom) {
slim_v_emitted = slim_fn_project_95append_95project_95atom(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_v_entry, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_emitted = slim_fn_text_95append_95token(slim_v_module_95source, slim_v_module_95tokens, slim_v_index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_emitted;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_36982 = {0};
slim_t_36982 = slim_v_manifest_95source;
int64_t slim_t_36984 = {0};
slim_t_36984 = slim_v_module_95name;
SlimBytes slim_t_36985 = {0};
slim_t_36985 = slim_v_module_95source;
int64_t slim_t_36987 = {0};
slim_t_36987 = slim_v_module_95items;
bool slim_t_36988 = {0};
slim_t_36988 = slim_v_entry;
int64_t slim_t_36989 = {0};
slim_t_36989 = slim_v_next;
int64_t slim_t_36990 = {0};
slim_t_36990 = slim_v_end;
int64_t slim_t_36991 = {0};
slim_t_36991 = slim_v_kind;
slim_v_manifest_95source = slim_t_36982;
slim_v_module_95name = slim_t_36984;
slim_v_module_95source = slim_t_36985;
slim_v_module_95items = slim_t_36987;
slim_v_entry = slim_t_36988;
slim_v_index = slim_t_36989;
slim_v_end = slim_t_36990;
slim_v_previous_95kind = slim_t_36991;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_95append_95project_95modules(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_entry_95name, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95path;
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_entry_95name;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_module_95name = {0};
slim_v_module_95name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_module_95name;
{
int64_t slim_v_path_95token = {0};
slim_v_path_95token = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path_95token;
{
SlimBytes slim_v_module_95path = {0};
slim_v_module_95path = slim_fn_project_95project_95module_95path(slim_v_manifest_95path, slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_path_95token, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95path;
{
SlimVec slim_v_module_95source_95bytes = {0};
slim_v_module_95source_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_module_95source_95bytes;
{
bool slim_v_module_95source_95read = {0};
slim_v_module_95source_95read = slim_read_file(slim_v_module_95path, &slim_v_module_95source_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95source_95read;
{
SlimBytes slim_v_module_95source = {0};
slim_v_module_95source = slim_bytes_freeze(slim_v_module_95source_95bytes);
(void)slim_v_module_95source;
{
SlimVec slim_v_module_95tokens = {0};
slim_v_module_95tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_module_95tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_module_95source, INT64_C(0), &slim_v_module_95tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
int64_t slim_v_module_95name_95start = {0};
slim_v_module_95name_95start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_module_95name, slim_allocation_region);
(void)slim_v_module_95name_95start;
{
int64_t slim_v_module_95name_95end = {0};
slim_v_module_95name_95end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_module_95name, slim_allocation_region);
(void)slim_v_module_95name_95end;
{
int64_t slim_v_source_95name = {0};
slim_v_source_95name = INT64_C(2);
(void)slim_v_source_95name;
{
int64_t slim_v_source_95name_95start = {0};
slim_v_source_95name_95start = slim_fn_syntax_95token_95start(&slim_v_module_95tokens, slim_v_source_95name, slim_allocation_region);
(void)slim_v_source_95name_95start;
{
int64_t slim_v_source_95name_95end = {0};
slim_v_source_95name_95end = slim_fn_syntax_95token_95end(&slim_v_module_95tokens, slim_v_source_95name, slim_allocation_region);
(void)slim_v_source_95name_95end;
{
bool slim_v_module_95identity = {0};
slim_v_module_95identity = slim_fn_project_95cross_95spans_95equal(slim_v_manifest_95source, slim_v_module_95name_95start, slim_v_module_95name_95end, slim_v_module_95source, slim_v_source_95name_95start, slim_v_source_95name_95end, slim_allocation_region);
(void)slim_v_module_95identity;
{
int64_t slim_v_entry_95name_95start = {0};
slim_v_entry_95name_95start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_entry_95name, slim_allocation_region);
(void)slim_v_entry_95name_95start;
{
int64_t slim_v_entry_95name_95end = {0};
slim_v_entry_95name_95end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_entry_95name, slim_allocation_region);
(void)slim_v_entry_95name_95end;
{
bool slim_v_entry = {0};
slim_v_entry = slim_fn_syntax_95spans_95equal(slim_v_manifest_95source, slim_v_module_95name_95start, slim_v_module_95name_95end, slim_v_entry_95name_95start, slim_v_entry_95name_95end, slim_allocation_region);
(void)slim_v_entry;
{
int64_t slim_v_module_95items = {0};
slim_v_module_95items = INT64_C(3);
(void)slim_v_module_95items;
{
int64_t slim_v_length = {0};
slim_v_length = (slim_v_module_95tokens).len;
(void)slim_v_length;
{
int64_t slim_v_end = {0};
slim_v_end = slim_i64_sub(slim_v_length, INT64_C(1));
(void)slim_v_end;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_project_95append_95project_95tokens(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_module_95source, &slim_v_module_95tokens, slim_v_module_95items, slim_v_entry, slim_v_module_95items, slim_v_end, INT64_C(2), slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_37312 = {0};
slim_t_37312 = slim_v_manifest_95path;
SlimBytes slim_t_37313 = {0};
slim_t_37313 = slim_v_manifest_95source;
int64_t slim_t_37315 = {0};
slim_t_37315 = slim_v_entry_95name;
int64_t slim_t_37316 = {0};
slim_t_37316 = slim_v_next;
slim_v_manifest_95path = slim_t_37312;
slim_v_manifest_95source = slim_t_37313;
slim_v_entry_95name = slim_t_37315;
slim_v_cursor = slim_t_37316;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimBytes slim_fn_project_95flatten_95project(SlimBytes slim_v_path, SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_entry_95form = {0};
slim_v_entry_95form = INT64_C(3);
(void)slim_v_entry_95form;
{
int64_t slim_v_entry_95name = {0};
slim_v_entry_95name = slim_i64_add(slim_v_entry_95form, INT64_C(2));
(void)slim_v_entry_95name;
{
int64_t slim_v_first_95module = {0};
slim_v_first_95module = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_entry_95form, slim_allocation_region);
(void)slim_v_first_95module;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_95append_95text(&slim_v_output, slim_bytes_static((const uint8_t *)"(module project", (int64_t)(sizeof("(module project") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_modules = {0};
slim_v_modules = slim_fn_project_95append_95project_95modules(slim_v_path, slim_v_source, slim_v_tokens, slim_v_entry_95name, slim_v_first_95module, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_modules;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_95append_95text(&slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_project_95path_95is_95project(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_source_95bytes = {0};
slim_v_source_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_source_95bytes;
{
bool slim_v_source_95read = {0};
slim_v_source_95read = slim_read_file(slim_v_path, &slim_v_source_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_source_95read;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_bytes_freeze(slim_v_source_95bytes);
(void)slim_v_source;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_source, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
slim_result = slim_fn_syntax_95token_95equal(slim_v_source, &slim_v_tokens, INT64_C(1), slim_bytes_static((const uint8_t *)"project", (int64_t)(sizeof("project") - 1)), slim_allocation_region);
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimBytes slim_fn_project_95generate_95project_95path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_source_95bytes = {0};
slim_v_source_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_source_95bytes;
{
bool slim_v_source_95read = {0};
slim_v_source_95read = slim_read_file(slim_v_path, &slim_v_source_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_source_95read;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_bytes_freeze(slim_v_source_95bytes);
(void)slim_v_source;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_source, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
SlimBytes slim_v_flattened = {0};
slim_v_flattened = slim_fn_project_95flatten_95project(slim_v_path, slim_v_source, &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_flattened;
{
SlimVec slim_v_flat_95tokens = {0};
slim_v_flat_95tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_flat_95tokens;
{
SlimUnit slim_v_flat_95tokenized = {0};
slim_v_flat_95tokenized = slim_fn_syntax_95lex(slim_v_flattened, INT64_C(0), &slim_v_flat_95tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_flat_95tokenized;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_codegen_95emit_95program(slim_v_flattened, &slim_v_flat_95tokens, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95compile_95project_95path_95jobs(SlimBytes slim_v_path, int64_t slim_v_jobs, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_v_jobs;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_project_95check_95project_95path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_checked > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_checked;
}
else {
{
SlimVec slim_v_manifest_95bytes = {0};
slim_v_manifest_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_manifest_95bytes;
{
bool slim_v_manifest_95read = {0};
slim_v_manifest_95read = slim_read_file(slim_v_path, &slim_v_manifest_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_manifest_95read;
{
SlimBytes slim_v_manifest_95source = {0};
slim_v_manifest_95source = slim_bytes_freeze(slim_v_manifest_95bytes);
(void)slim_v_manifest_95source;
{
SlimVec slim_v_manifest_95tokens = {0};
slim_v_manifest_95tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_manifest_95tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_manifest_95source, INT64_C(0), &slim_v_manifest_95tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
Slim_type_scheduler_95Schedule slim_v_schedule = {0};
slim_v_schedule = slim_fn_scheduler_95plan(slim_v_manifest_95source, &slim_v_manifest_95tokens, slim_v_jobs, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_schedule;
{
bool slim_v_schedule_95valid = {0};
slim_v_schedule_95valid = slim_v_schedule.slim_field_valid;
(void)slim_v_schedule_95valid;
if (!slim_v_schedule_95valid) {
slim_result = INT64_C(70);
}
else {
{
SlimBytes slim_v_generated = {0};
slim_v_generated = slim_fn_project_95generate_95project_95path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_generated;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_generated);
(void)slim_v_shown;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_project_95compile_95project_95path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_result = slim_fn_project_95compile_95project_95path_95jobs(slim_v_path, INT64_C(1), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimBytes slim_fn_project_95token_95bytes(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_end;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_text_95append_95span(slim_v_source, slim_v_start, slim_v_end, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95report_95project_95diagnostic(SlimBytes slim_v_code, SlimBytes slim_v_module_95name, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_code;
(void)slim_v_module_95name;
(void)slim_v_start;
(void)slim_v_end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_code_95text = {0};
slim_v_code_95text = slim_fn_text_95append_95text(&slim_v_output, slim_v_code, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_code_95text;
{
SlimUnit slim_v_first_95at = {0};
slim_v_first_95at = slim_fn_text_95append_95text(&slim_v_output, slim_bytes_static((const uint8_t *)"@", (int64_t)(sizeof("@") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_first_95at;
{
SlimUnit slim_v_module_95text = {0};
slim_v_module_95text = slim_fn_text_95append_95text(&slim_v_output, slim_v_module_95name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95text;
{
SlimUnit slim_v_second_95at = {0};
slim_v_second_95at = slim_fn_text_95append_95text(&slim_v_output, slim_bytes_static((const uint8_t *)"@", (int64_t)(sizeof("@") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_second_95at;
{
SlimUnit slim_v_start_95text = {0};
slim_v_start_95text = slim_fn_text_95append_95nonnegative_95i64(slim_v_start, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_start_95text;
{
SlimUnit slim_v_colon = {0};
slim_v_colon = slim_fn_text_95append_95text(&slim_v_output, slim_bytes_static((const uint8_t *)":", (int64_t)(sizeof(":") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_colon;
{
SlimUnit slim_v_end_95text = {0};
slim_v_end_95text = slim_fn_text_95append_95nonnegative_95i64(slim_v_end, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_end_95text;
{
SlimUnit slim_v_newline = {0};
slim_v_newline = slim_fn_text_95append_95text(&slim_v_output, slim_bytes_static((const uint8_t *)"\n", (int64_t)(sizeof("\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_newline;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(1);
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_95imports_95has_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_38212 = {0};
slim_t_38212 = slim_v_source;
int64_t slim_t_38214 = {0};
slim_t_38214 = slim_v_next;
int64_t slim_t_38215 = {0};
slim_t_38215 = slim_v_name_95start;
int64_t slim_t_38216 = {0};
slim_t_38216 = slim_v_name_95end;
slim_v_source = slim_t_38212;
slim_v_cursor = slim_t_38214;
slim_v_name_95start = slim_t_38215;
slim_v_name_95end = slim_t_38216;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95manifest_95module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_module_95name = {0};
slim_v_module_95name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_module_95name;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_module_95name, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_module_95name, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_38356 = {0};
slim_t_38356 = slim_v_source;
int64_t slim_t_38358 = {0};
slim_t_38358 = slim_v_next;
int64_t slim_t_38359 = {0};
slim_t_38359 = slim_v_name_95start;
int64_t slim_t_38360 = {0};
slim_t_38360 = slim_v_name_95end;
slim_v_source = slim_t_38356;
slim_v_cursor = slim_t_38358;
slim_v_name_95start = slim_t_38359;
slim_v_name_95end = slim_t_38360;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_project_95module_95imports_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_module_95name, int64_t slim_v_import_95start, int64_t slim_v_import_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_module_95name;
(void)slim_v_import_95start;
(void)slim_v_import_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_module_95name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_module_95name, slim_allocation_region);
(void)slim_v_name_95end;
{
int64_t slim_v_module_95form = {0};
slim_v_module_95form = slim_fn_project_95find_95manifest_95module(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_module_95form;
{
bool slim_v_found = {0};
slim_v_found = slim_v_module_95form >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = false;
}
else {
{
int64_t slim_v_imports_95form = {0};
slim_v_imports_95form = slim_i64_add(slim_v_module_95form, INT64_C(4));
(void)slim_v_imports_95form;
{
int64_t slim_v_first_95import = {0};
slim_v_first_95import = slim_i64_add(slim_v_imports_95form, INT64_C(2));
(void)slim_v_first_95import;
slim_result = slim_fn_project_95imports_95has_95name(slim_v_source, slim_v_tokens, slim_v_first_95import, slim_v_import_95start, slim_v_import_95end, slim_allocation_region);
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_project_95module_95cycle_95imports(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_module_95name, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_module_95name;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_import_95start = {0};
slim_v_import_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_import_95start;
{
int64_t slim_v_import_95end = {0};
slim_v_import_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_import_95end;
{
bool slim_v_reciprocal = {0};
slim_v_reciprocal = slim_fn_project_95module_95imports_95name(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_cursor, slim_v_import_95start, slim_v_import_95end, slim_allocation_region);
(void)slim_v_reciprocal;
{
int64_t slim_v_module_95name_95start = {0};
slim_v_module_95name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_module_95name, slim_allocation_region);
(void)slim_v_module_95name_95start;
{
int64_t slim_v_module_95name_95end = {0};
slim_v_module_95name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_module_95name, slim_allocation_region);
(void)slim_v_module_95name_95end;
{
int64_t slim_v_target_95form = {0};
slim_v_target_95form = slim_fn_project_95find_95manifest_95module(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_import_95start, slim_v_import_95end, slim_allocation_region);
(void)slim_v_target_95form;
{
bool slim_v_target_95found = {0};
slim_v_target_95found = slim_v_target_95form >= INT64_C(0);
(void)slim_v_target_95found;
{
bool slim_v_imports_95source = {0};
if (slim_v_target_95found) {
{
int64_t slim_v_target_95imports = {0};
slim_v_target_95imports = slim_i64_add(slim_v_target_95form, INT64_C(4));
(void)slim_v_target_95imports;
{
int64_t slim_v_first_95target_95import = {0};
slim_v_first_95target_95import = slim_i64_add(slim_v_target_95imports, INT64_C(2));
(void)slim_v_first_95target_95import;
slim_v_imports_95source = slim_fn_project_95imports_95has_95name(slim_v_source, slim_v_tokens, slim_v_first_95target_95import, slim_v_module_95name_95start, slim_v_module_95name_95end, slim_allocation_region);
}
}
}
else {
slim_v_imports_95source = false;
}
(void)slim_v_imports_95source;
if (slim_v_imports_95source) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_38709 = {0};
slim_t_38709 = slim_v_source;
int64_t slim_t_38711 = {0};
slim_t_38711 = slim_v_modules;
int64_t slim_t_38712 = {0};
slim_t_38712 = slim_v_module_95name;
int64_t slim_t_38713 = {0};
slim_t_38713 = slim_v_next;
slim_v_source = slim_t_38709;
slim_v_modules = slim_t_38711;
slim_v_module_95name = slim_t_38712;
slim_v_cursor = slim_t_38713;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95reciprocal_95cycle(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_module_95name = {0};
slim_v_module_95name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_module_95name;
{
int64_t slim_v_imports_95form = {0};
slim_v_imports_95form = slim_i64_add(slim_v_cursor, INT64_C(4));
(void)slim_v_imports_95form;
{
int64_t slim_v_first_95import = {0};
slim_v_first_95import = slim_i64_add(slim_v_imports_95form, INT64_C(2));
(void)slim_v_first_95import;
{
bool slim_v_cycle = {0};
slim_v_cycle = slim_fn_project_95module_95cycle_95imports(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_module_95name, slim_v_first_95import, slim_allocation_region);
(void)slim_v_cycle;
if (slim_v_cycle) {
slim_result = slim_v_module_95name;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_38854 = {0};
slim_t_38854 = slim_v_source;
int64_t slim_t_38856 = {0};
slim_t_38856 = slim_v_modules;
int64_t slim_t_38857 = {0};
slim_t_38857 = slim_v_next;
slim_v_source = slim_t_38854;
slim_v_modules = slim_t_38856;
slim_v_cursor = slim_t_38857;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95span_95slash(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_slash = {0};
slim_v_slash = slim_v_code == INT64_C(47);
(void)slim_v_slash;
if (slim_v_slash) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_38962 = {0};
slim_t_38962 = slim_v_source;
int64_t slim_t_38963 = {0};
slim_t_38963 = slim_v_next;
int64_t slim_t_38964 = {0};
slim_t_38964 = slim_v_end;
slim_v_source = slim_t_38962;
slim_v_index = slim_t_38963;
slim_v_end = slim_t_38964;
goto slim_recur;
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_95exports_95has_95segment(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_cursor, SlimBytes slim_v_module_95source, int64_t slim_v_segment_95start, int64_t slim_v_segment_95end, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_cursor;
(void)slim_v_module_95source;
(void)slim_v_segment_95start;
(void)slim_v_segment_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_export_95start = {0};
slim_v_export_95start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_export_95start;
{
int64_t slim_v_export_95end = {0};
slim_v_export_95end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_export_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_project_95cross_95spans_95equal(slim_v_manifest_95source, slim_v_export_95start, slim_v_export_95end, slim_v_module_95source, slim_v_segment_95start, slim_v_segment_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_39098 = {0};
slim_t_39098 = slim_v_manifest_95source;
int64_t slim_t_39100 = {0};
slim_t_39100 = slim_v_next;
SlimBytes slim_t_39101 = {0};
slim_t_39101 = slim_v_module_95source;
int64_t slim_t_39102 = {0};
slim_t_39102 = slim_v_segment_95start;
int64_t slim_t_39103 = {0};
slim_t_39103 = slim_v_segment_95end;
slim_v_manifest_95source = slim_t_39098;
slim_v_cursor = slim_t_39100;
slim_v_module_95source = slim_t_39101;
slim_v_segment_95start = slim_t_39102;
slim_v_segment_95end = slim_t_39103;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95check_95project_95path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_source_95bytes = {0};
slim_v_source_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_source_95bytes;
{
bool slim_v_source_95read = {0};
slim_v_source_95read = slim_read_file(slim_v_path, &slim_v_source_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_source_95read;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_bytes_freeze(slim_v_source_95bytes);
(void)slim_v_source;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_source, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
int64_t slim_v_version = {0};
slim_v_version = INT64_C(2);
(void)slim_v_version;
{
bool slim_v_valid_95version = {0};
slim_v_valid_95version = slim_fn_syntax_95token_95equal(slim_v_source, &slim_v_tokens, slim_v_version, slim_bytes_static((const uint8_t *)"1", (int64_t)(sizeof("1") - 1)), slim_allocation_region);
(void)slim_v_valid_95version;
if (!slim_v_valid_95version) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(&slim_v_tokens, slim_v_version, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(&slim_v_tokens, slim_v_version, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0402", (int64_t)(sizeof("E0402") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
int64_t slim_v_entry_95form = {0};
slim_v_entry_95form = INT64_C(3);
(void)slim_v_entry_95form;
{
int64_t slim_v_entry = {0};
slim_v_entry = slim_i64_add(slim_v_entry_95form, INT64_C(2));
(void)slim_v_entry;
{
int64_t slim_v_modules = {0};
slim_v_modules = slim_fn_syntax_95skip_95form(&slim_v_tokens, slim_v_entry_95form, slim_allocation_region);
(void)slim_v_modules;
{
int64_t slim_v_manifest_95errors = {0};
slim_v_manifest_95errors = slim_fn_project_95report_95manifest_95rules(slim_v_source, &slim_v_tokens, slim_v_entry_95form, slim_v_modules, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_manifest_95errors;
{
bool slim_v_invalid_95manifest = {0};
slim_v_invalid_95manifest = slim_v_manifest_95errors > INT64_C(0);
(void)slim_v_invalid_95manifest;
if (slim_v_invalid_95manifest) {
slim_result = slim_v_manifest_95errors;
}
else {
{
int64_t slim_v_cycle = {0};
slim_v_cycle = slim_fn_project_95find_95reciprocal_95cycle(slim_v_source, &slim_v_tokens, slim_v_modules, slim_v_modules, slim_allocation_region);
(void)slim_v_cycle;
{
bool slim_v_has_95cycle = {0};
slim_v_has_95cycle = slim_v_cycle >= INT64_C(0);
(void)slim_v_has_95cycle;
if (slim_v_has_95cycle) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(&slim_v_tokens, slim_v_cycle, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(&slim_v_tokens, slim_v_cycle, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0413", (int64_t)(sizeof("E0413") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
SlimVec slim_v_module_95tokens = {0};
slim_v_module_95tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_module_95tokens;
{
SlimVec slim_v_loaded = {0};
slim_v_loaded = slim_vec_new(sizeof(Slim_type_project_95LoadedModule), slim_allocation_region);
(void)slim_v_loaded;
{
int64_t slim_v_loaded_95modules = {0};
slim_v_loaded_95modules = slim_fn_project_95load_95project_95modules(slim_v_path, slim_v_source, &slim_v_tokens, slim_v_modules, &slim_v_module_95tokens, &slim_v_loaded, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_loaded_95modules;
{
bool slim_v_invalid_95loading = {0};
slim_v_invalid_95loading = slim_v_loaded_95modules > INT64_C(0);
(void)slim_v_invalid_95loading;
if (slim_v_invalid_95loading) {
slim_result = slim_v_loaded_95modules;
}
else {
{
int64_t slim_v_project_95errors = {0};
slim_v_project_95errors = slim_fn_project_95report_95loaded_95project(slim_v_source, &slim_v_tokens, slim_v_entry, &slim_v_module_95tokens, &slim_v_loaded, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_project_95errors;
{
bool slim_v_invalid_95project = {0};
slim_v_invalid_95project = slim_v_project_95errors > INT64_C(0);
(void)slim_v_invalid_95project;
if (slim_v_invalid_95project) {
slim_result = slim_v_project_95errors;
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimUnit slim_fn_project_95append_95interface_95type(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_type_95index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_type_95index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_composite = {0};
slim_v_composite = slim_v_kind == INT64_C(0);
(void)slim_v_composite;
if (slim_v_composite) {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_type_95index, INT64_C(1));
(void)slim_v_head;
{
int64_t slim_v_inner = {0};
slim_v_inner = slim_i64_add(slim_v_type_95index, INT64_C(2));
(void)slim_v_inner;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_constructor = {0};
slim_v_constructor = slim_fn_text_95append_95token(slim_v_module_95source, slim_v_module_95tokens, slim_v_head, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_constructor;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_nested = {0};
slim_v_nested = slim_fn_project_95append_95interface_95type(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_inner, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_nested;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
else {
{
int64_t slim_v_scalar_95kind = {0};
slim_v_scalar_95kind = slim_fn_syntax_95scalar_95type_95kind(slim_v_module_95source, slim_v_module_95tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_scalar_95kind;
{
bool slim_v_scalar = {0};
slim_v_scalar = slim_v_scalar_95kind >= INT64_C(0);
(void)slim_v_scalar;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_module_95tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_module_95tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_slashes = {0};
slim_v_slashes = slim_fn_syntax_95span_95count_95byte(slim_v_module_95source, slim_v_start, slim_v_end, INT64_C(47), INT64_C(0), slim_allocation_region);
(void)slim_v_slashes;
{
bool slim_v_qualified = {0};
slim_v_qualified = slim_v_slashes > INT64_C(0);
(void)slim_v_qualified;
{
bool slim_v_raw = {0};
slim_v_raw = slim_v_scalar || slim_v_qualified;
(void)slim_v_raw;
if (slim_v_raw) {
slim_result = slim_fn_text_95append_95token(slim_v_module_95source, slim_v_module_95tokens, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimUnit slim_v_prefix = {0};
slim_v_prefix = slim_fn_text_95append_95token(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_prefix;
{
SlimUnit slim_v_slash = {0};
slim_v_slash = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"/", (int64_t)(sizeof("/") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_slash;
slim_result = slim_fn_text_95append_95token(slim_v_module_95source, slim_v_module_95tokens, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_95append_95interface_95parameters(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95token_95equal(slim_v_module_95source, slim_v_module_95tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_type_95index = {0};
if (slim_v_borrowed) {
slim_v_type_95index = slim_i64_add(slim_v_cursor, INT64_C(3));
}
else {
slim_v_type_95index = slim_i64_add(slim_v_cursor, INT64_C(2));
}
(void)slim_v_type_95index;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_mode = {0};
if (slim_v_borrowed) {
slim_v_mode = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"inout ", (int64_t)(sizeof("inout ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_mode = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"owned ", (int64_t)(sizeof("owned ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_mode;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_project_95append_95interface_95type(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_39999 = {0};
slim_t_39999 = slim_v_manifest_95source;
int64_t slim_t_40001 = {0};
slim_t_40001 = slim_v_module_95name;
SlimBytes slim_t_40002 = {0};
slim_t_40002 = slim_v_module_95source;
int64_t slim_t_40004 = {0};
slim_t_40004 = slim_v_next;
bool slim_t_40005 = {0};
slim_t_40005 = false;
slim_v_manifest_95source = slim_t_39999;
slim_v_module_95name = slim_t_40001;
slim_v_module_95source = slim_t_40002;
slim_v_cursor = slim_t_40004;
slim_v_first = slim_t_40005;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_95append_95interface_95effects(SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_effect = {0};
slim_v_effect = slim_fn_text_95append_95token(slim_v_module_95source, slim_v_module_95tokens, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_effect;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_40125 = {0};
slim_t_40125 = slim_v_module_95source;
int64_t slim_t_40127 = {0};
slim_t_40127 = slim_v_next;
slim_v_module_95source = slim_t_40125;
slim_v_cursor = slim_t_40127;
goto slim_recur;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95find_95export_95declaration(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_export_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_export_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_export_95start = {0};
slim_v_export_95start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_export_95name, slim_allocation_region);
(void)slim_v_export_95start;
{
int64_t slim_v_export_95end = {0};
slim_v_export_95end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_export_95name, slim_allocation_region);
(void)slim_v_export_95end;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_module_95tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_module_95tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_project_95cross_95spans_95equal(slim_v_manifest_95source, slim_v_export_95start, slim_v_export_95end, slim_v_module_95source, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_40294 = {0};
slim_t_40294 = slim_v_manifest_95source;
int64_t slim_t_40296 = {0};
slim_t_40296 = slim_v_export_95name;
SlimBytes slim_t_40297 = {0};
slim_t_40297 = slim_v_module_95source;
int64_t slim_t_40299 = {0};
slim_t_40299 = slim_v_next;
slim_v_manifest_95source = slim_t_40294;
slim_v_export_95name = slim_t_40296;
slim_v_module_95source = slim_t_40297;
slim_v_cursor = slim_t_40299;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_project_95append_95interface_95fields(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_name;
{
int64_t slim_v_type_95index = {0};
slim_v_type_95index = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_type_95index;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_95append_95token(slim_v_module_95source, slim_v_module_95tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_project_95append_95interface_95type(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_type_95index, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_40514 = {0};
slim_t_40514 = slim_v_manifest_95source;
int64_t slim_t_40516 = {0};
slim_t_40516 = slim_v_module_95name;
SlimBytes slim_t_40517 = {0};
slim_t_40517 = slim_v_module_95source;
int64_t slim_t_40519 = {0};
slim_t_40519 = slim_v_next;
bool slim_t_40520 = {0};
slim_t_40520 = false;
slim_v_manifest_95source = slim_t_40514;
slim_v_module_95name = slim_t_40516;
slim_v_module_95source = slim_t_40517;
slim_v_cursor = slim_t_40519;
slim_v_first = slim_t_40520;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_95append_95interface_95case_95types(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_typed = {0};
slim_v_typed = slim_fn_project_95append_95interface_95type(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_cursor, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_typed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_40654 = {0};
slim_t_40654 = slim_v_manifest_95source;
int64_t slim_t_40656 = {0};
slim_t_40656 = slim_v_module_95name;
SlimBytes slim_t_40657 = {0};
slim_t_40657 = slim_v_module_95source;
int64_t slim_t_40659 = {0};
slim_t_40659 = slim_v_next;
slim_v_manifest_95source = slim_t_40654;
slim_v_module_95name = slim_t_40656;
slim_v_module_95source = slim_t_40657;
slim_v_cursor = slim_t_40659;
goto slim_recur;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_95append_95interface_95cases(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, bool slim_v_first, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_cursor;
(void)slim_v_first;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
SlimUnit slim_v_separator = {0};
if (slim_v_first) {
slim_v_separator = (SlimUnit){0};
}
else {
slim_v_separator = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_separator;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_name;
{
int64_t slim_v_first_95type = {0};
slim_v_first_95type = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_first_95type;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(", (int64_t)(sizeof("(") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_95append_95token(slim_v_module_95source, slim_v_module_95tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_types = {0};
slim_v_types = slim_fn_project_95append_95interface_95case_95types(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_first_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_types;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_40859 = {0};
slim_t_40859 = slim_v_manifest_95source;
int64_t slim_t_40861 = {0};
slim_t_40861 = slim_v_module_95name;
SlimBytes slim_t_40862 = {0};
slim_t_40862 = slim_v_module_95source;
int64_t slim_t_40864 = {0};
slim_t_40864 = slim_v_next;
bool slim_t_40865 = {0};
slim_t_40865 = false;
slim_v_manifest_95source = slim_t_40859;
slim_v_module_95name = slim_t_40861;
slim_v_module_95source = slim_t_40862;
slim_v_cursor = slim_t_40864;
slim_v_first = slim_t_40865;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_95append_95interface_95declaration(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_item, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_item;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_module_95source, slim_v_module_95tokens, slim_v_item, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
if (slim_v_function_95item) {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_item, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_first_95effect = {0};
slim_v_first_95effect = slim_i64_add(slim_v_effects, INT64_C(2));
(void)slim_v_first_95effect;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(fn ", (int64_t)(sizeof("(fn ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_95append_95token(slim_v_module_95source, slim_v_module_95tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_params_95open = {0};
slim_v_params_95open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" (", (int64_t)(sizeof(" (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_params_95open;
{
int64_t slim_v_first_95parameter = {0};
slim_v_first_95parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_95parameter;
{
SlimUnit slim_v_parameters = {0};
slim_v_parameters = slim_fn_project_95append_95interface_95parameters(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_first_95parameter, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_parameters;
{
SlimUnit slim_v_return_95space = {0};
slim_v_return_95space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_return_95space;
{
SlimUnit slim_v_returned = {0};
slim_v_returned = slim_fn_project_95append_95interface_95type(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_return_95type, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_returned;
{
SlimUnit slim_v_effect_95space = {0};
slim_v_effect_95space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" (effects", (int64_t)(sizeof(" (effects") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_effect_95space;
{
SlimUnit slim_v_emitted_95effects = {0};
slim_v_emitted_95effects = slim_fn_project_95append_95interface_95effects(slim_v_module_95source, slim_v_module_95tokens, slim_v_first_95effect, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted_95effects;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
else {
{
bool slim_v_record_95item = {0};
slim_v_record_95item = slim_fn_syntax_95item_95is(slim_v_module_95source, slim_v_module_95tokens, slim_v_item, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_95item;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_item, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_body = {0};
slim_v_body = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_body;
{
int64_t slim_v_first_95member = {0};
slim_v_first_95member = slim_i64_add(slim_v_body, INT64_C(1));
(void)slim_v_first_95member;
{
SlimUnit slim_v_opened = {0};
if (slim_v_record_95item) {
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(record ", (int64_t)(sizeof("(record ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(variant ", (int64_t)(sizeof("(variant ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_opened;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_95append_95token(slim_v_module_95source, slim_v_module_95tokens, slim_v_name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
SlimUnit slim_v_members_95open = {0};
slim_v_members_95open = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" (", (int64_t)(sizeof(" (") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_members_95open;
{
SlimUnit slim_v_members = {0};
if (slim_v_record_95item) {
slim_v_members = slim_fn_project_95append_95interface_95fields(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_first_95member, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_members = slim_fn_project_95append_95interface_95cases(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_first_95member, true, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
(void)slim_v_members;
slim_result = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")", (int64_t)(sizeof(")") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_95append_95interface_95exports(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_module_95name, int64_t slim_v_cursor, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95name;
(void)slim_v_cursor;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_module_95items;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_item = {0};
slim_v_item = slim_fn_project_95find_95export_95declaration(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_cursor, slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_allocation_region);
(void)slim_v_item;
{
SlimUnit slim_v_space = {0};
slim_v_space = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_space;
{
SlimUnit slim_v_declaration = {0};
slim_v_declaration = slim_fn_project_95append_95interface_95declaration(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_item, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_declaration;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_41408 = {0};
slim_t_41408 = slim_v_manifest_95source;
int64_t slim_t_41410 = {0};
slim_t_41410 = slim_v_module_95name;
int64_t slim_t_41411 = {0};
slim_t_41411 = slim_v_next;
SlimBytes slim_t_41412 = {0};
slim_t_41412 = slim_v_module_95source;
int64_t slim_t_41414 = {0};
slim_t_41414 = slim_v_module_95items;
slim_v_manifest_95source = slim_t_41408;
slim_v_module_95name = slim_t_41410;
slim_v_cursor = slim_t_41411;
slim_v_module_95source = slim_t_41412;
slim_v_module_95items = slim_t_41414;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_project_95append_95project_95interfaces(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_cursor, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95path;
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_cursor;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_module_95name = {0};
slim_v_module_95name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_module_95name;
{
int64_t slim_v_path_95token = {0};
slim_v_path_95token = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path_95token;
{
int64_t slim_v_imports_95form = {0};
slim_v_imports_95form = slim_i64_add(slim_v_cursor, INT64_C(4));
(void)slim_v_imports_95form;
{
int64_t slim_v_exports_95form = {0};
slim_v_exports_95form = slim_fn_syntax_95skip_95form(slim_v_manifest_95tokens, slim_v_imports_95form, slim_allocation_region);
(void)slim_v_exports_95form;
{
int64_t slim_v_first_95export = {0};
slim_v_first_95export = slim_i64_add(slim_v_exports_95form, INT64_C(2));
(void)slim_v_first_95export;
{
SlimBytes slim_v_module_95path = {0};
slim_v_module_95path = slim_fn_project_95project_95module_95path(slim_v_manifest_95path, slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_path_95token, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95path;
{
SlimVec slim_v_module_95source_95bytes = {0};
slim_v_module_95source_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_module_95source_95bytes;
{
bool slim_v_module_95source_95read = {0};
slim_v_module_95source_95read = slim_read_file(slim_v_module_95path, &slim_v_module_95source_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95source_95read;
{
SlimBytes slim_v_module_95source = {0};
slim_v_module_95source = slim_bytes_freeze(slim_v_module_95source_95bytes);
(void)slim_v_module_95source;
{
SlimVec slim_v_module_95tokens = {0};
slim_v_module_95tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_module_95tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_module_95source, INT64_C(0), &slim_v_module_95tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
SlimUnit slim_v_opened = {0};
slim_v_opened = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)"(interface 1 ", (int64_t)(sizeof("(interface 1 ") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_opened;
{
SlimUnit slim_v_named = {0};
slim_v_named = slim_fn_text_95append_95token(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_named;
{
int64_t slim_v_module_95items = {0};
slim_v_module_95items = INT64_C(3);
(void)slim_v_module_95items;
{
SlimUnit slim_v_exports = {0};
slim_v_exports = slim_fn_project_95append_95interface_95exports(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95name, slim_v_first_95export, slim_v_module_95source, &slim_v_module_95tokens, slim_v_module_95items, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_exports;
{
SlimUnit slim_v_closed = {0};
slim_v_closed = slim_fn_text_95append_95text(slim_v_output, slim_bytes_static((const uint8_t *)")\n", (int64_t)(sizeof(")\n") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_closed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_41674 = {0};
slim_t_41674 = slim_v_manifest_95path;
SlimBytes slim_t_41675 = {0};
slim_t_41675 = slim_v_manifest_95source;
int64_t slim_t_41677 = {0};
slim_t_41677 = slim_v_next;
slim_v_manifest_95path = slim_t_41674;
slim_v_manifest_95source = slim_t_41675;
slim_v_cursor = slim_t_41677;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95interfaces_95path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_project_95check_95project_95path(slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_checked;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_checked > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_checked;
}
else {
{
SlimVec slim_v_source_95bytes = {0};
slim_v_source_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_source_95bytes;
{
bool slim_v_source_95read = {0};
slim_v_source_95read = slim_read_file(slim_v_path, &slim_v_source_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_source_95read;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_bytes_freeze(slim_v_source_95bytes);
(void)slim_v_source;
{
SlimVec slim_v_tokens = {0};
slim_v_tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_source, INT64_C(0), &slim_v_tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
int64_t slim_v_entry_95form = {0};
slim_v_entry_95form = INT64_C(3);
(void)slim_v_entry_95form;
{
int64_t slim_v_modules = {0};
slim_v_modules = slim_fn_syntax_95skip_95form(&slim_v_tokens, slim_v_entry_95form, slim_allocation_region);
(void)slim_v_modules;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_project_95append_95project_95interfaces(slim_v_path, slim_v_source, &slim_v_tokens, slim_v_modules, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
SlimBytes slim_v_bytes = {0};
slim_v_bytes = slim_bytes_freeze(slim_v_output);
(void)slim_v_bytes;
{
SlimUnit slim_v_shown = {0};
slim_v_shown = slim_print_bytes(slim_v_bytes);
(void)slim_v_shown;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_95span_95less_95chars(SlimBytes slim_v_source, int64_t slim_v_left, int64_t slim_v_left_95end, int64_t slim_v_right, int64_t slim_v_right_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_left;
(void)slim_v_left_95end;
(void)slim_v_right;
(void)slim_v_right_95end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_left_95done = {0};
slim_v_left_95done = slim_v_left >= slim_v_left_95end;
(void)slim_v_left_95done;
{
bool slim_v_right_95done = {0};
slim_v_right_95done = slim_v_right >= slim_v_right_95end;
(void)slim_v_right_95done;
if (slim_v_left_95done) {
slim_result = !slim_v_right_95done;
}
else {
if (slim_v_right_95done) {
slim_result = false;
}
else {
{
uint8_t slim_v_left_95byte = {0};
slim_v_left_95byte = slim_bytes_get(slim_v_source, slim_v_left);
(void)slim_v_left_95byte;
{
uint8_t slim_v_right_95byte = {0};
slim_v_right_95byte = slim_bytes_get(slim_v_source, slim_v_right);
(void)slim_v_right_95byte;
{
int64_t slim_v_left_95code = {0};
slim_v_left_95code = (int64_t)slim_v_left_95byte;
(void)slim_v_left_95code;
{
int64_t slim_v_right_95code = {0};
slim_v_right_95code = (int64_t)slim_v_right_95byte;
(void)slim_v_right_95code;
{
bool slim_v_equal = {0};
slim_v_equal = slim_v_left_95code == slim_v_right_95code;
(void)slim_v_equal;
if (slim_v_equal) {
{
int64_t slim_v_next_95left = {0};
slim_v_next_95left = slim_i64_add(slim_v_left, INT64_C(1));
(void)slim_v_next_95left;
{
int64_t slim_v_next_95right = {0};
slim_v_next_95right = slim_i64_add(slim_v_right, INT64_C(1));
(void)slim_v_next_95right;
SlimBytes slim_t_42019 = {0};
slim_t_42019 = slim_v_source;
int64_t slim_t_42020 = {0};
slim_t_42020 = slim_v_next_95left;
int64_t slim_t_42021 = {0};
slim_t_42021 = slim_v_left_95end;
int64_t slim_t_42022 = {0};
slim_t_42022 = slim_v_next_95right;
int64_t slim_t_42023 = {0};
slim_t_42023 = slim_v_right_95end;
slim_v_source = slim_t_42019;
slim_v_left = slim_t_42020;
slim_v_left_95end = slim_t_42021;
slim_v_right = slim_t_42022;
slim_v_right_95end = slim_t_42023;
goto slim_recur;
}
}
}
else {
slim_result = slim_v_left_95code < slim_v_right_95code;
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_95span_95less(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_left, int64_t slim_v_right, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_left;
(void)slim_v_right;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_left_95start = {0};
slim_v_left_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_left, slim_allocation_region);
(void)slim_v_left_95start;
{
int64_t slim_v_left_95end = {0};
slim_v_left_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_left, slim_allocation_region);
(void)slim_v_left_95end;
{
int64_t slim_v_right_95start = {0};
slim_v_right_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_right, slim_allocation_region);
(void)slim_v_right_95start;
{
int64_t slim_v_right_95end = {0};
slim_v_right_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_right, slim_allocation_region);
(void)slim_v_right_95end;
slim_result = slim_fn_project_95span_95less_95chars(slim_v_source, slim_v_left_95start, slim_v_left_95end, slim_v_right_95start, slim_v_right_95end, slim_allocation_region);
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95unsorted_95module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_previous, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_previous;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
bool slim_v_first = {0};
slim_v_first = slim_v_previous < INT64_C(0);
(void)slim_v_first;
{
bool slim_v_ordered = {0};
if (slim_v_first) {
slim_v_ordered = true;
}
else {
slim_v_ordered = slim_fn_project_95span_95less(slim_v_source, slim_v_tokens, slim_v_previous, slim_v_name, slim_allocation_region);
}
(void)slim_v_ordered;
if (!slim_v_ordered) {
slim_result = slim_v_name;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_42257 = {0};
slim_t_42257 = slim_v_source;
int64_t slim_t_42259 = {0};
slim_t_42259 = slim_v_next;
int64_t slim_t_42260 = {0};
slim_t_42260 = slim_v_name;
slim_v_source = slim_t_42257;
slim_v_cursor = slim_t_42259;
slim_v_previous = slim_t_42260;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95prior_95module_95name(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_current, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_current;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_reached = {0};
slim_v_reached = slim_v_cursor == slim_v_current;
(void)slim_v_reached;
if (slim_v_reached) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_start, slim_v_end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_name;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_42394 = {0};
slim_t_42394 = slim_v_source;
int64_t slim_t_42396 = {0};
slim_t_42396 = slim_v_next;
int64_t slim_t_42397 = {0};
slim_t_42397 = slim_v_current;
int64_t slim_t_42398 = {0};
slim_t_42398 = slim_v_name_95start;
int64_t slim_t_42399 = {0};
slim_t_42399 = slim_v_name_95end;
slim_v_source = slim_t_42394;
slim_v_cursor = slim_t_42396;
slim_v_current = slim_t_42397;
slim_v_name_95start = slim_t_42398;
slim_v_name_95end = slim_t_42399;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95duplicate_95module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_prior = {0};
slim_v_prior = slim_fn_project_95find_95prior_95module_95name(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_cursor, slim_v_start, slim_v_end, slim_allocation_region);
(void)slim_v_prior;
{
bool slim_v_duplicate = {0};
slim_v_duplicate = slim_v_prior >= INT64_C(0);
(void)slim_v_duplicate;
if (slim_v_duplicate) {
slim_result = slim_v_name;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_42546 = {0};
slim_t_42546 = slim_v_source;
int64_t slim_t_42548 = {0};
slim_t_42548 = slim_v_modules;
int64_t slim_t_42549 = {0};
slim_t_42549 = slim_v_next;
slim_v_source = slim_t_42546;
slim_v_modules = slim_t_42548;
slim_v_cursor = slim_t_42549;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95prior_95path(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_current, int64_t slim_v_path_95start, int64_t slim_v_path_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_current;
(void)slim_v_path_95start;
(void)slim_v_path_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_reached = {0};
slim_v_reached = slim_v_cursor == slim_v_current;
(void)slim_v_reached;
if (slim_v_reached) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_path = {0};
slim_v_path = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_start, slim_v_end, slim_v_path_95start, slim_v_path_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_path;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_42685 = {0};
slim_t_42685 = slim_v_source;
int64_t slim_t_42687 = {0};
slim_t_42687 = slim_v_next;
int64_t slim_t_42688 = {0};
slim_t_42688 = slim_v_current;
int64_t slim_t_42689 = {0};
slim_t_42689 = slim_v_path_95start;
int64_t slim_t_42690 = {0};
slim_t_42690 = slim_v_path_95end;
slim_v_source = slim_t_42685;
slim_v_cursor = slim_t_42687;
slim_v_current = slim_t_42688;
slim_v_path_95start = slim_t_42689;
slim_v_path_95end = slim_t_42690;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95duplicate_95path(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_path = {0};
slim_v_path = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_prior = {0};
slim_v_prior = slim_fn_project_95find_95prior_95path(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_cursor, slim_v_start, slim_v_end, slim_allocation_region);
(void)slim_v_prior;
{
bool slim_v_duplicate = {0};
slim_v_duplicate = slim_v_prior >= INT64_C(0);
(void)slim_v_duplicate;
if (slim_v_duplicate) {
slim_result = slim_v_path;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_42837 = {0};
slim_t_42837 = slim_v_source;
int64_t slim_t_42839 = {0};
slim_t_42839 = slim_v_modules;
int64_t slim_t_42840 = {0};
slim_t_42840 = slim_v_next;
slim_v_source = slim_t_42837;
slim_v_modules = slim_t_42839;
slim_v_cursor = slim_t_42840;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_project_95path_95segment_95valid(SlimBytes slim_v_source, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_start;
(void)slim_v_end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_length = {0};
slim_v_length = slim_i64_sub(slim_v_end, slim_v_start);
(void)slim_v_length;
{
bool slim_v_nonempty = {0};
slim_v_nonempty = slim_v_length > INT64_C(0);
(void)slim_v_nonempty;
if (!slim_v_nonempty) {
slim_result = false;
}
else {
{
bool slim_v_dot = {0};
slim_v_dot = slim_fn_syntax_95span_95equal(slim_v_source, slim_v_start, slim_v_end, slim_bytes_static((const uint8_t *)".", (int64_t)(sizeof(".") - 1)), slim_allocation_region);
(void)slim_v_dot;
{
bool slim_v_parent = {0};
slim_v_parent = slim_fn_syntax_95span_95equal(slim_v_source, slim_v_start, slim_v_end, slim_bytes_static((const uint8_t *)"..", (int64_t)(sizeof("..") - 1)), slim_allocation_region);
(void)slim_v_parent;
{
bool slim_v_special = {0};
slim_v_special = slim_v_dot || slim_v_parent;
(void)slim_v_special;
slim_result = !slim_v_special;
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_95path_95segments_95valid(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_segment_95start, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_segment_95start;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_fn_project_95path_95segment_95valid(slim_v_source, slim_v_segment_95start, slim_v_end, slim_allocation_region);
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_backslash = {0};
slim_v_backslash = slim_v_code == INT64_C(92);
(void)slim_v_backslash;
if (slim_v_backslash) {
slim_result = false;
}
else {
{
bool slim_v_slash = {0};
slim_v_slash = slim_v_code == INT64_C(47);
(void)slim_v_slash;
if (slim_v_slash) {
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_project_95path_95segment_95valid(slim_v_source, slim_v_segment_95start, slim_v_index, slim_allocation_region);
(void)slim_v_valid;
if (!slim_v_valid) {
slim_result = false;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_43089 = {0};
slim_t_43089 = slim_v_source;
int64_t slim_t_43090 = {0};
slim_t_43090 = slim_v_next;
int64_t slim_t_43091 = {0};
slim_t_43091 = slim_v_end;
int64_t slim_t_43092 = {0};
slim_t_43092 = slim_v_next;
slim_v_source = slim_t_43089;
slim_v_index = slim_t_43090;
slim_v_end = slim_t_43091;
slim_v_segment_95start = slim_t_43092;
goto slim_recur;
}
}
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_43113 = {0};
slim_t_43113 = slim_v_source;
int64_t slim_t_43114 = {0};
slim_t_43114 = slim_v_next;
int64_t slim_t_43115 = {0};
slim_t_43115 = slim_v_end;
int64_t slim_t_43116 = {0};
slim_t_43116 = slim_v_segment_95start;
slim_v_source = slim_t_43113;
slim_v_index = slim_t_43114;
slim_v_end = slim_t_43115;
slim_v_segment_95start = slim_t_43116;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_project_95path_95token_95invalid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_path, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_path;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_token_95start = {0};
slim_v_token_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_token_95start;
{
int64_t slim_v_token_95end = {0};
slim_v_token_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_token_95end;
{
int64_t slim_v_start = {0};
slim_v_start = slim_i64_add(slim_v_token_95start, INT64_C(1));
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_i64_sub(slim_v_token_95end, INT64_C(1));
(void)slim_v_end;
{
int64_t slim_v_length = {0};
slim_v_length = slim_i64_sub(slim_v_end, slim_v_start);
(void)slim_v_length;
{
bool slim_v_enough = {0};
slim_v_enough = slim_v_length >= INT64_C(5);
(void)slim_v_enough;
if (!slim_v_enough) {
slim_result = true;
}
else {
{
int64_t slim_v_suffix_95start = {0};
slim_v_suffix_95start = slim_i64_sub(slim_v_end, INT64_C(5));
(void)slim_v_suffix_95start;
{
bool slim_v_suffix = {0};
slim_v_suffix = slim_fn_syntax_95span_95equal(slim_v_source, slim_v_suffix_95start, slim_v_end, slim_bytes_static((const uint8_t *)".slim", (int64_t)(sizeof(".slim") - 1)), slim_allocation_region);
(void)slim_v_suffix;
if (!slim_v_suffix) {
slim_result = true;
}
else {
{
bool slim_v_segments = {0};
slim_v_segments = slim_fn_project_95path_95segments_95valid(slim_v_source, slim_v_start, slim_v_end, slim_v_start, slim_allocation_region);
(void)slim_v_segments;
slim_result = !slim_v_segments;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95invalid_95path(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_path = {0};
slim_v_path = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_fn_project_95path_95token_95invalid(slim_v_source, slim_v_tokens, slim_v_path, slim_allocation_region);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_path;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_43385 = {0};
slim_t_43385 = slim_v_source;
int64_t slim_t_43387 = {0};
slim_t_43387 = slim_v_next;
slim_v_source = slim_t_43385;
slim_v_cursor = slim_t_43387;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95self_95import_95in(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_start, slim_v_end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_43516 = {0};
slim_t_43516 = slim_v_source;
int64_t slim_t_43518 = {0};
slim_t_43518 = slim_v_next;
int64_t slim_t_43519 = {0};
slim_t_43519 = slim_v_name_95start;
int64_t slim_t_43520 = {0};
slim_t_43520 = slim_v_name_95end;
slim_v_source = slim_t_43516;
slim_v_cursor = slim_t_43518;
slim_v_name_95start = slim_t_43519;
slim_v_name_95end = slim_t_43520;
goto slim_recur;
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95self_95import(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
{
int64_t slim_v_imports = {0};
slim_v_imports = slim_i64_add(slim_v_cursor, INT64_C(4));
(void)slim_v_imports;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_imports, INT64_C(2));
(void)slim_v_first;
{
int64_t slim_v_invalid = {0};
slim_v_invalid = slim_fn_project_95find_95self_95import_95in(slim_v_source, slim_v_tokens, slim_v_first, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_invalid;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_43682 = {0};
slim_t_43682 = slim_v_source;
int64_t slim_t_43684 = {0};
slim_t_43684 = slim_v_next;
slim_v_source = slim_t_43682;
slim_v_cursor = slim_t_43684;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95unknown_95import_95in(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_module = {0};
slim_v_module = slim_fn_project_95find_95manifest_95module(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_start, slim_v_end, slim_allocation_region);
(void)slim_v_module;
{
bool slim_v_known = {0};
slim_v_known = slim_v_module >= INT64_C(0);
(void)slim_v_known;
if (!slim_v_known) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_43824 = {0};
slim_t_43824 = slim_v_source;
int64_t slim_t_43826 = {0};
slim_t_43826 = slim_v_modules;
int64_t slim_t_43827 = {0};
slim_t_43827 = slim_v_next;
slim_v_source = slim_t_43824;
slim_v_modules = slim_t_43826;
slim_v_cursor = slim_t_43827;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95unknown_95import(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_modules, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_imports = {0};
slim_v_imports = slim_i64_add(slim_v_cursor, INT64_C(4));
(void)slim_v_imports;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_imports, INT64_C(2));
(void)slim_v_first;
{
int64_t slim_v_invalid = {0};
slim_v_invalid = slim_fn_project_95find_95unknown_95import_95in(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_first, slim_allocation_region);
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (slim_v_found) {
slim_result = slim_v_invalid;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_43963 = {0};
slim_t_43963 = slim_v_source;
int64_t slim_t_43965 = {0};
slim_t_43965 = slim_v_modules;
int64_t slim_t_43966 = {0};
slim_t_43966 = slim_v_next;
slim_v_source = slim_t_43963;
slim_v_modules = slim_t_43965;
slim_v_cursor = slim_t_43966;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95report_95manifest_95rules(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_entry_95form, int64_t slim_v_modules, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_entry_95form;
(void)slim_v_modules;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_entry = {0};
slim_v_entry = slim_i64_add(slim_v_entry_95form, INT64_C(2));
(void)slim_v_entry;
{
int64_t slim_v_entry_95start = {0};
slim_v_entry_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_entry, slim_allocation_region);
(void)slim_v_entry_95start;
{
int64_t slim_v_entry_95end = {0};
slim_v_entry_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_entry, slim_allocation_region);
(void)slim_v_entry_95end;
{
int64_t slim_v_declared = {0};
slim_v_declared = slim_fn_project_95find_95manifest_95module(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_entry_95start, slim_v_entry_95end, slim_allocation_region);
(void)slim_v_declared;
{
bool slim_v_known_95entry = {0};
slim_v_known_95entry = slim_v_declared >= INT64_C(0);
(void)slim_v_known_95entry;
if (!slim_v_known_95entry) {
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0403", (int64_t)(sizeof("E0403") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_entry_95start, slim_v_entry_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
{
int64_t slim_v_unsorted = {0};
slim_v_unsorted = slim_fn_project_95find_95unsorted_95module(slim_v_source, slim_v_tokens, slim_v_modules, INT64_C(-1), slim_allocation_region);
(void)slim_v_unsorted;
{
bool slim_v_has_95unsorted = {0};
slim_v_has_95unsorted = slim_v_unsorted >= INT64_C(0);
(void)slim_v_has_95unsorted;
if (slim_v_has_95unsorted) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_unsorted, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_unsorted, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_order_95report = {0};
slim_v_order_95report = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0406", (int64_t)(sizeof("E0406") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_order_95report;
{
int64_t slim_v_duplicate = {0};
slim_v_duplicate = slim_fn_project_95find_95duplicate_95module(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_modules, slim_allocation_region);
(void)slim_v_duplicate;
{
bool slim_v_same_95span = {0};
slim_v_same_95span = slim_v_duplicate == slim_v_unsorted;
(void)slim_v_same_95span;
if (slim_v_same_95span) {
{
int64_t slim_v_duplicate_95report = {0};
slim_v_duplicate_95report = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0408", (int64_t)(sizeof("E0408") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_duplicate_95report;
slim_result = slim_v_order_95report;
}
}
else {
slim_result = slim_v_order_95report;
}
}
}
}
}
}
}
else {
{
int64_t slim_v_duplicate_95path = {0};
slim_v_duplicate_95path = slim_fn_project_95find_95duplicate_95path(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_modules, slim_allocation_region);
(void)slim_v_duplicate_95path;
{
bool slim_v_has_95duplicate_95path = {0};
slim_v_has_95duplicate_95path = slim_v_duplicate_95path >= INT64_C(0);
(void)slim_v_has_95duplicate_95path;
if (slim_v_has_95duplicate_95path) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_duplicate_95path, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_duplicate_95path, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0408", (int64_t)(sizeof("E0408") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
int64_t slim_v_invalid_95path = {0};
slim_v_invalid_95path = slim_fn_project_95find_95invalid_95path(slim_v_source, slim_v_tokens, slim_v_modules, slim_allocation_region);
(void)slim_v_invalid_95path;
{
bool slim_v_has_95invalid_95path = {0};
slim_v_has_95invalid_95path = slim_v_invalid_95path >= INT64_C(0);
(void)slim_v_has_95invalid_95path;
if (slim_v_has_95invalid_95path) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_invalid_95path, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_invalid_95path, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0407", (int64_t)(sizeof("E0407") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
int64_t slim_v_self_95import = {0};
slim_v_self_95import = slim_fn_project_95find_95self_95import(slim_v_source, slim_v_tokens, slim_v_modules, slim_allocation_region);
(void)slim_v_self_95import;
{
bool slim_v_has_95self = {0};
slim_v_has_95self = slim_v_self_95import >= INT64_C(0);
(void)slim_v_has_95self;
if (slim_v_has_95self) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_self_95import, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_self_95import, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0412", (int64_t)(sizeof("E0412") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
{
int64_t slim_v_unknown_95import = {0};
slim_v_unknown_95import = slim_fn_project_95find_95unknown_95import(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_modules, slim_allocation_region);
(void)slim_v_unknown_95import;
{
bool slim_v_has_95unknown = {0};
slim_v_has_95unknown = slim_v_unknown_95import >= INT64_C(0);
(void)slim_v_has_95unknown;
if (slim_v_has_95unknown) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_unknown_95import, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_unknown_95import, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0411", (int64_t)(sizeof("E0411") - 1)), slim_bytes_static((const uint8_t *)"-", (int64_t)(sizeof("-") - 1)), slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95load_95project_95modules(SlimBytes slim_v_manifest_95path, SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_cursor, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region) {
(void)slim_v_manifest_95path;
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_cursor;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_path = {0};
slim_v_path = slim_i64_add(slim_v_cursor, INT64_C(3));
(void)slim_v_path;
{
SlimBytes slim_v_module_95path = {0};
slim_v_module_95path = slim_fn_project_95project_95module_95path(slim_v_manifest_95path, slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95path;
{
SlimVec slim_v_source_95bytes = {0};
slim_v_source_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_source_95bytes;
{
bool slim_v_source_95read = {0};
slim_v_source_95read = slim_read_file(slim_v_module_95path, &slim_v_source_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_source_95read;
if (!slim_v_source_95read) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_path, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_path, slim_allocation_region);
(void)slim_v_end;
{
SlimBytes slim_v_module_95name = {0};
slim_v_module_95name = slim_fn_project_95token_95bytes(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95name;
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0409", (int64_t)(sizeof("E0409") - 1)), slim_v_module_95name, slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
else {
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_bytes_freeze(slim_v_source_95bytes);
(void)slim_v_source;
{
int64_t slim_v_root = {0};
slim_v_root = ((*slim_v_module_95tokens)).len;
(void)slim_v_root;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_source, INT64_C(0), slim_v_module_95tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
Slim_type_project_95LoadedModule slim_v_module = {0};
{
int64_t slim_t_44704 = {0};
slim_t_44704 = slim_v_cursor;
int64_t slim_t_44708 = {0};
slim_t_44708 = slim_v_name;
int64_t slim_t_44712 = {0};
slim_t_44712 = slim_v_path;
SlimBytes slim_t_44716 = {0};
slim_t_44716 = slim_v_source;
int64_t slim_t_44720 = {0};
slim_t_44720 = slim_v_root;
slim_v_module = (Slim_type_project_95LoadedModule){.slim_field_manifest = slim_t_44704, .slim_field_name = slim_t_44708, .slim_field_path = slim_t_44712, .slim_field_source = slim_t_44716, .slim_field_root = slim_t_44720};
}
(void)slim_v_module;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_loaded, &slim_v_module)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_44747 = {0};
slim_t_44747 = slim_v_manifest_95path;
SlimBytes slim_t_44748 = {0};
slim_t_44748 = slim_v_manifest_95source;
int64_t slim_t_44750 = {0};
slim_t_44750 = slim_v_next;
slim_v_manifest_95path = slim_t_44747;
slim_v_manifest_95source = slim_t_44748;
slim_v_cursor = slim_t_44750;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95report_95module_95identities(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(0);
}
else {
{
Slim_type_project_95LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_95LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_module.slim_field_source;
(void)slim_v_source;
{
int64_t slim_v_root = {0};
slim_v_root = slim_v_module.slim_field_root;
(void)slim_v_root;
{
int64_t slim_v_source_95name = {0};
slim_v_source_95name = slim_i64_add(slim_v_root, INT64_C(2));
(void)slim_v_source_95name;
{
int64_t slim_v_manifest_95start = {0};
slim_v_manifest_95start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
(void)slim_v_manifest_95start;
{
int64_t slim_v_manifest_95end = {0};
slim_v_manifest_95end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
(void)slim_v_manifest_95end;
{
int64_t slim_v_source_95start = {0};
slim_v_source_95start = slim_fn_syntax_95token_95start(slim_v_module_95tokens, slim_v_source_95name, slim_allocation_region);
(void)slim_v_source_95start;
{
int64_t slim_v_source_95end = {0};
slim_v_source_95end = slim_fn_syntax_95token_95end(slim_v_module_95tokens, slim_v_source_95name, slim_allocation_region);
(void)slim_v_source_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_project_95cross_95spans_95equal(slim_v_manifest_95source, slim_v_manifest_95start, slim_v_manifest_95end, slim_v_source, slim_v_source_95start, slim_v_source_95end, slim_allocation_region);
(void)slim_v_same;
if (!slim_v_same) {
{
SlimBytes slim_v_module_95name = {0};
slim_v_module_95name = slim_fn_project_95token_95bytes(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95name;
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0410", (int64_t)(sizeof("E0410") - 1)), slim_v_module_95name, slim_v_manifest_95start, slim_v_manifest_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_44985 = {0};
slim_t_44985 = slim_v_manifest_95source;
int64_t slim_t_44989 = {0};
slim_t_44989 = slim_v_next;
slim_v_manifest_95source = slim_t_44985;
slim_v_index = slim_t_44989;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95find_95missing_95export(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_cursor, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_cursor;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_module_95items;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_declaration = {0};
slim_v_declaration = slim_fn_project_95find_95export_95declaration(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_cursor, slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_allocation_region);
(void)slim_v_declaration;
{
bool slim_v_found = {0};
slim_v_found = slim_v_declaration >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_45125 = {0};
slim_t_45125 = slim_v_manifest_95source;
int64_t slim_t_45127 = {0};
slim_t_45127 = slim_v_next;
SlimBytes slim_t_45128 = {0};
slim_t_45128 = slim_v_module_95source;
int64_t slim_t_45130 = {0};
slim_t_45130 = slim_v_module_95items;
slim_v_manifest_95source = slim_t_45125;
slim_v_cursor = slim_t_45127;
slim_v_module_95source = slim_t_45128;
slim_v_module_95items = slim_t_45130;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95report_95missing_95exports(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(0);
}
else {
{
Slim_type_project_95LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_95LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_manifest = {0};
slim_v_manifest = slim_v_module.slim_field_manifest;
(void)slim_v_manifest;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
int64_t slim_v_imports = {0};
slim_v_imports = slim_i64_add(slim_v_manifest, INT64_C(4));
(void)slim_v_imports;
{
int64_t slim_v_exports = {0};
slim_v_exports = slim_fn_syntax_95skip_95form(slim_v_manifest_95tokens, slim_v_imports, slim_allocation_region);
(void)slim_v_exports;
{
int64_t slim_v_first_95export = {0};
slim_v_first_95export = slim_i64_add(slim_v_exports, INT64_C(2));
(void)slim_v_first_95export;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_module.slim_field_source;
(void)slim_v_source;
{
int64_t slim_v_root = {0};
slim_v_root = slim_v_module.slim_field_root;
(void)slim_v_root;
{
int64_t slim_v_module_95items = {0};
slim_v_module_95items = slim_i64_add(slim_v_root, INT64_C(3));
(void)slim_v_module_95items;
{
int64_t slim_v_missing = {0};
slim_v_missing = slim_fn_project_95find_95missing_95export(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95export, slim_v_source, slim_v_module_95tokens, slim_v_module_95items, slim_allocation_region);
(void)slim_v_missing;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_missing >= INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_missing, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_missing, slim_allocation_region);
(void)slim_v_end;
{
SlimBytes slim_v_module_95name = {0};
slim_v_module_95name = slim_fn_project_95token_95bytes(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95name;
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0414", (int64_t)(sizeof("E0414") - 1)), slim_v_module_95name, slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_45386 = {0};
slim_t_45386 = slim_v_manifest_95source;
int64_t slim_t_45390 = {0};
slim_t_45390 = slim_v_next;
slim_v_manifest_95source = slim_t_45386;
slim_v_index = slim_t_45390;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95find_95named_95declaration(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimBytes slim_v_name, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_name;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_declaration_95name = {0};
slim_v_declaration_95name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_declaration_95name;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_declaration_95name, slim_v_name, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_45513 = {0};
slim_t_45513 = slim_v_source;
int64_t slim_t_45515 = {0};
slim_t_45515 = slim_v_next;
SlimBytes slim_t_45516 = {0};
slim_t_45516 = slim_v_name;
slim_v_source = slim_t_45513;
slim_v_cursor = slim_t_45515;
slim_v_name = slim_t_45516;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_project_95loaded_95is_95entry(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_entry, Slim_type_project_95LoadedModule slim_v_module, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_entry;
(void)slim_v_module;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
{
int64_t slim_v_entry_95start = {0};
slim_v_entry_95start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_entry, slim_allocation_region);
(void)slim_v_entry_95start;
{
int64_t slim_v_entry_95end = {0};
slim_v_entry_95end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_entry, slim_allocation_region);
(void)slim_v_entry_95end;
slim_result = slim_fn_syntax_95spans_95equal(slim_v_manifest_95source, slim_v_name_95start, slim_v_name_95end, slim_v_entry_95start, slim_v_entry_95end, slim_allocation_region);
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95report_95project_95mains(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_entry, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_entry;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(0);
}
else {
{
Slim_type_project_95LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_95LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_module.slim_field_source;
(void)slim_v_source;
{
int64_t slim_v_root = {0};
slim_v_root = slim_v_module.slim_field_root;
(void)slim_v_root;
{
int64_t slim_v_module_95items = {0};
slim_v_module_95items = slim_i64_add(slim_v_root, INT64_C(3));
(void)slim_v_module_95items;
{
int64_t slim_v_main = {0};
slim_v_main = slim_fn_project_95find_95named_95declaration(slim_v_source, slim_v_module_95tokens, slim_v_module_95items, slim_bytes_static((const uint8_t *)"main", (int64_t)(sizeof("main") - 1)), slim_allocation_region);
(void)slim_v_main;
{
bool slim_v_has_95main = {0};
slim_v_has_95main = slim_v_main >= INT64_C(0);
(void)slim_v_has_95main;
{
bool slim_v_is_95entry = {0};
slim_v_is_95entry = slim_fn_project_95loaded_95is_95entry(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_entry, slim_v_module, slim_allocation_region);
(void)slim_v_is_95entry;
if (slim_v_is_95entry) {
if (slim_v_has_95main) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_45792 = {0};
slim_t_45792 = slim_v_manifest_95source;
int64_t slim_t_45794 = {0};
slim_t_45794 = slim_v_entry;
int64_t slim_t_45797 = {0};
slim_t_45797 = slim_v_next;
slim_v_manifest_95source = slim_t_45792;
slim_v_entry = slim_t_45794;
slim_v_index = slim_t_45797;
goto slim_recur;
}
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
SlimBytes slim_v_module_95name = {0};
slim_v_module_95name = slim_fn_project_95token_95bytes(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95name;
{
int64_t slim_v_next_95form = {0};
slim_v_next_95form = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_root, slim_allocation_region);
(void)slim_v_next_95form;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next_95form, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_module_95tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0419", (int64_t)(sizeof("E0419") - 1)), slim_v_module_95name, INT64_C(0), slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
else {
if (!slim_v_has_95main) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_45888 = {0};
slim_t_45888 = slim_v_manifest_95source;
int64_t slim_t_45890 = {0};
slim_t_45890 = slim_v_entry;
int64_t slim_t_45893 = {0};
slim_t_45893 = slim_v_next;
slim_v_manifest_95source = slim_t_45888;
slim_v_entry = slim_t_45890;
slim_v_index = slim_t_45893;
goto slim_recur;
}
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_module_95tokens, slim_v_main, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_next_95form = {0};
slim_v_next_95form = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_main, slim_allocation_region);
(void)slim_v_next_95form;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next_95form, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_module_95tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
SlimBytes slim_v_module_95name = {0};
slim_v_module_95name = slim_fn_project_95token_95bytes(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95name;
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0419", (int64_t)(sizeof("E0419") - 1)), slim_v_module_95name, slim_v_start, slim_v_end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95find_95loaded_95segment(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_loaded, SlimBytes slim_v_reference_95source, int64_t slim_v_segment_95start, int64_t slim_v_segment_95end, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_loaded;
(void)slim_v_reference_95source;
(void)slim_v_segment_95start;
(void)slim_v_segment_95end;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_project_95LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_95LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_project_95cross_95spans_95equal(slim_v_manifest_95source, slim_v_name_95start, slim_v_name_95end, slim_v_reference_95source, slim_v_segment_95start, slim_v_segment_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_46137 = {0};
slim_t_46137 = slim_v_manifest_95source;
SlimBytes slim_t_46140 = {0};
slim_t_46140 = slim_v_reference_95source;
int64_t slim_t_46141 = {0};
slim_t_46141 = slim_v_segment_95start;
int64_t slim_t_46142 = {0};
slim_t_46142 = slim_v_segment_95end;
int64_t slim_t_46143 = {0};
slim_t_46143 = slim_v_next;
slim_v_manifest_95source = slim_t_46137;
slim_v_reference_95source = slim_t_46140;
slim_v_segment_95start = slim_t_46141;
slim_v_segment_95end = slim_t_46142;
slim_v_index = slim_t_46143;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95declaration_95segment(SlimBytes slim_v_declaration_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_cursor, SlimBytes slim_v_reference_95source, int64_t slim_v_segment_95start, int64_t slim_v_segment_95end, SlimRegion *slim_region) {
(void)slim_v_declaration_95source;
(void)slim_v_module_95tokens;
(void)slim_v_cursor;
(void)slim_v_reference_95source;
(void)slim_v_segment_95start;
(void)slim_v_segment_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_module_95tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_module_95tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_project_95cross_95spans_95equal(slim_v_declaration_95source, slim_v_name_95start, slim_v_name_95end, slim_v_reference_95source, slim_v_segment_95start, slim_v_segment_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_46290 = {0};
slim_t_46290 = slim_v_declaration_95source;
int64_t slim_t_46292 = {0};
slim_t_46292 = slim_v_next;
SlimBytes slim_t_46293 = {0};
slim_t_46293 = slim_v_reference_95source;
int64_t slim_t_46294 = {0};
slim_t_46294 = slim_v_segment_95start;
int64_t slim_t_46295 = {0};
slim_t_46295 = slim_v_segment_95end;
slim_v_declaration_95source = slim_t_46290;
slim_v_cursor = slim_t_46292;
slim_v_reference_95source = slim_t_46293;
slim_v_segment_95start = slim_t_46294;
slim_v_segment_95end = slim_t_46295;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95target_95call_95status(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, Slim_type_project_95LoadedModule slim_v_current, int64_t slim_v_target_95index, SlimBytes slim_v_reference_95source, int64_t slim_v_prefix_95start, int64_t slim_v_slash, int64_t slim_v_reference_95end, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_v_current;
(void)slim_v_target_95index;
(void)slim_v_reference_95source;
(void)slim_v_prefix_95start;
(void)slim_v_slash;
(void)slim_v_reference_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_current_95manifest = {0};
slim_v_current_95manifest = slim_v_current.slim_field_manifest;
(void)slim_v_current_95manifest;
{
int64_t slim_v_imports = {0};
slim_v_imports = slim_i64_add(slim_v_current_95manifest, INT64_C(4));
(void)slim_v_imports;
{
int64_t slim_v_first_95import = {0};
slim_v_first_95import = slim_i64_add(slim_v_imports, INT64_C(2));
(void)slim_v_first_95import;
{
bool slim_v_imported = {0};
slim_v_imported = slim_fn_project_95exports_95has_95segment(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95import, slim_v_reference_95source, slim_v_prefix_95start, slim_v_slash, slim_allocation_region);
(void)slim_v_imported;
if (!slim_v_imported) {
slim_result = INT64_C(2);
}
else {
{
Slim_type_project_95LoadedModule slim_v_target = {0};
slim_v_target = ((Slim_type_project_95LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_target_95index)];
(void)slim_v_target;
{
int64_t slim_v_target_95manifest = {0};
slim_v_target_95manifest = slim_v_target.slim_field_manifest;
(void)slim_v_target_95manifest;
{
int64_t slim_v_target_95imports = {0};
slim_v_target_95imports = slim_i64_add(slim_v_target_95manifest, INT64_C(4));
(void)slim_v_target_95imports;
{
int64_t slim_v_exports = {0};
slim_v_exports = slim_fn_syntax_95skip_95form(slim_v_manifest_95tokens, slim_v_target_95imports, slim_allocation_region);
(void)slim_v_exports;
{
int64_t slim_v_first_95export = {0};
slim_v_first_95export = slim_i64_add(slim_v_exports, INT64_C(2));
(void)slim_v_first_95export;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_i64_add(slim_v_slash, INT64_C(1));
(void)slim_v_name_95start;
{
bool slim_v_exported = {0};
slim_v_exported = slim_fn_project_95exports_95has_95segment(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95export, slim_v_reference_95source, slim_v_name_95start, slim_v_reference_95end, slim_allocation_region);
(void)slim_v_exported;
if (!slim_v_exported) {
slim_result = INT64_C(3);
}
else {
{
SlimBytes slim_v_target_95source = {0};
slim_v_target_95source = slim_v_target.slim_field_source;
(void)slim_v_target_95source;
{
int64_t slim_v_target_95root = {0};
slim_v_target_95root = slim_v_target.slim_field_root;
(void)slim_v_target_95root;
{
int64_t slim_v_target_95items = {0};
slim_v_target_95items = slim_i64_add(slim_v_target_95root, INT64_C(3));
(void)slim_v_target_95items;
{
int64_t slim_v_declaration = {0};
slim_v_declaration = slim_fn_project_95find_95declaration_95segment(slim_v_target_95source, slim_v_module_95tokens, slim_v_target_95items, slim_v_reference_95source, slim_v_name_95start, slim_v_reference_95end, slim_allocation_region);
(void)slim_v_declaration;
{
bool slim_v_found = {0};
slim_v_found = slim_v_declaration >= INT64_C(0);
(void)slim_v_found;
{
bool slim_v_callable = {0};
if (!slim_v_found) {
slim_v_callable = false;
}
else {
slim_v_callable = slim_fn_syntax_95item_95is(slim_v_target_95source, slim_v_module_95tokens, slim_v_declaration, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
}
(void)slim_v_callable;
if (slim_v_callable) {
slim_result = INT64_C(0);
}
else {
slim_result = INT64_C(3);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95qualified_95call_95status(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, Slim_type_project_95LoadedModule slim_v_current, int64_t slim_v_callee, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_v_current;
(void)slim_v_callee;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
SlimBytes slim_v_reference_95source = {0};
slim_v_reference_95source = slim_v_current.slim_field_source;
(void)slim_v_reference_95source;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_module_95tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_module_95tokens, slim_v_callee, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_slash = {0};
slim_v_slash = slim_fn_project_95find_95span_95slash(slim_v_reference_95source, slim_v_start, slim_v_end, slim_allocation_region);
(void)slim_v_slash;
{
bool slim_v_qualified = {0};
slim_v_qualified = slim_v_slash >= INT64_C(0);
(void)slim_v_qualified;
if (!slim_v_qualified) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_current_95name = {0};
slim_v_current_95name = slim_v_current.slim_field_name;
(void)slim_v_current_95name;
{
int64_t slim_v_current_95start = {0};
slim_v_current_95start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_current_95name, slim_allocation_region);
(void)slim_v_current_95start;
{
int64_t slim_v_current_95end = {0};
slim_v_current_95end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_current_95name, slim_allocation_region);
(void)slim_v_current_95end;
{
bool slim_v_self = {0};
slim_v_self = slim_fn_project_95cross_95spans_95equal(slim_v_manifest_95source, slim_v_current_95start, slim_v_current_95end, slim_v_reference_95source, slim_v_start, slim_v_slash, slim_allocation_region);
(void)slim_v_self;
if (slim_v_self) {
slim_result = INT64_C(1);
}
else {
{
int64_t slim_v_target = {0};
slim_v_target = slim_fn_project_95find_95loaded_95segment(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_loaded, slim_v_reference_95source, slim_v_start, slim_v_slash, INT64_C(0), slim_allocation_region);
(void)slim_v_target;
{
bool slim_v_found = {0};
slim_v_found = slim_v_target >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = INT64_C(2);
}
else {
slim_result = slim_fn_project_95target_95call_95status(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95tokens, slim_v_loaded, slim_v_current, slim_v_target, slim_v_reference_95source, slim_v_start, slim_v_slash, slim_v_end, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95find_95invalid_95loaded_95call(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, Slim_type_project_95LoadedModule slim_v_current, int64_t slim_v_cursor, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_v_current;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_call_95form = {0};
if (slim_v_form) {
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_current.slim_field_source;
(void)slim_v_source;
slim_v_call_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_module_95tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
}
}
else {
slim_v_call_95form = false;
}
(void)slim_v_call_95form;
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_callee;
{
int64_t slim_v_status = {0};
if (slim_v_call_95form) {
slim_v_status = slim_fn_project_95qualified_95call_95status(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95tokens, slim_v_loaded, slim_v_current, slim_v_callee, slim_allocation_region);
}
else {
slim_v_status = INT64_C(0);
}
(void)slim_v_status;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_status > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = slim_v_cursor;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_47035 = {0};
slim_t_47035 = slim_v_manifest_95source;
Slim_type_project_95LoadedModule slim_t_47039 = {0};
slim_t_47039 = slim_v_current;
int64_t slim_t_47040 = {0};
slim_t_47040 = slim_v_next;
int64_t slim_t_47041 = {0};
slim_t_47041 = slim_v_end;
slim_v_manifest_95source = slim_t_47035;
slim_v_current = slim_t_47039;
slim_v_cursor = slim_t_47040;
slim_v_end = slim_t_47041;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_project_95report_95qualified_95calls(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(0);
}
else {
{
Slim_type_project_95LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_95LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_root = {0};
slim_v_root = slim_v_module.slim_field_root;
(void)slim_v_root;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_root, INT64_C(3));
(void)slim_v_first;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_root, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_invalid = {0};
slim_v_invalid = slim_fn_project_95find_95invalid_95loaded_95call(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95tokens, slim_v_loaded, slim_v_module, slim_v_first, slim_v_end, slim_allocation_region);
(void)slim_v_invalid;
{
bool slim_v_found = {0};
slim_v_found = slim_v_invalid >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_47210 = {0};
slim_t_47210 = slim_v_manifest_95source;
int64_t slim_t_47214 = {0};
slim_t_47214 = slim_v_next;
slim_v_manifest_95source = slim_t_47210;
slim_v_index = slim_t_47214;
goto slim_recur;
}
}
else {
{
int64_t slim_v_callee = {0};
slim_v_callee = slim_i64_add(slim_v_invalid, INT64_C(2));
(void)slim_v_callee;
{
int64_t slim_v_status = {0};
slim_v_status = slim_fn_project_95qualified_95call_95status(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95tokens, slim_v_loaded, slim_v_module, slim_v_callee, slim_allocation_region);
(void)slim_v_status;
{
bool slim_v_self = {0};
slim_v_self = slim_v_status == INT64_C(1);
(void)slim_v_self;
{
SlimBytes slim_v_code = {0};
if (slim_v_self) {
slim_v_code = slim_bytes_static((const uint8_t *)"E0417", (int64_t)(sizeof("E0417") - 1));
}
else {
{
bool slim_v_unimported = {0};
slim_v_unimported = slim_v_status == INT64_C(2);
(void)slim_v_unimported;
if (slim_v_unimported) {
slim_v_code = slim_bytes_static((const uint8_t *)"E0416", (int64_t)(sizeof("E0416") - 1));
}
else {
slim_v_code = slim_bytes_static((const uint8_t *)"E0415", (int64_t)(sizeof("E0415") - 1));
}
}
}
(void)slim_v_code;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_module_95tokens, slim_v_invalid, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_next_95form = {0};
slim_v_next_95form = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_invalid, slim_allocation_region);
(void)slim_v_next_95form;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next_95form, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_call_95end = {0};
slim_v_call_95end = slim_fn_syntax_95token_95end(slim_v_module_95tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_call_95end;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
SlimBytes slim_v_module_95name = {0};
slim_v_module_95name = slim_fn_project_95token_95bytes(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95name;
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_v_code, slim_v_module_95name, slim_v_start, slim_v_call_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95report_95loaded_95project(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_entry, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_entry;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_identities = {0};
slim_v_identities = slim_fn_project_95report_95module_95identities(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95tokens, slim_v_loaded, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_identities;
{
bool slim_v_invalid_95identity = {0};
slim_v_invalid_95identity = slim_v_identities > INT64_C(0);
(void)slim_v_invalid_95identity;
if (slim_v_invalid_95identity) {
slim_result = slim_v_identities;
}
else {
{
int64_t slim_v_exports = {0};
slim_v_exports = slim_fn_project_95report_95missing_95exports(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95tokens, slim_v_loaded, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_exports;
{
bool slim_v_invalid_95export = {0};
slim_v_invalid_95export = slim_v_exports > INT64_C(0);
(void)slim_v_invalid_95export;
if (slim_v_invalid_95export) {
slim_result = slim_v_exports;
}
else {
{
int64_t slim_v_mains = {0};
slim_v_mains = slim_fn_project_95report_95project_95mains(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_entry, slim_v_module_95tokens, slim_v_loaded, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_mains;
{
bool slim_v_invalid_95main = {0};
slim_v_invalid_95main = slim_v_mains > INT64_C(0);
(void)slim_v_invalid_95main;
if (slim_v_invalid_95main) {
slim_result = slim_v_mains;
}
else {
{
int64_t slim_v_calls = {0};
slim_v_calls = slim_fn_project_95report_95qualified_95calls(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95tokens, slim_v_loaded, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_calls;
{
bool slim_v_invalid_95call = {0};
slim_v_invalid_95call = slim_v_calls > INT64_C(0);
(void)slim_v_invalid_95call;
if (slim_v_invalid_95call) {
slim_result = slim_v_calls;
}
else {
slim_result = slim_fn_project_95report_95private_95type_95leaks(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95tokens, slim_v_loaded, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95report_95interface_95type(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_first_95export, SlimBytes slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, int64_t slim_v_type_95index, int64_t slim_v_diagnostic_95start, int64_t slim_v_diagnostic_95end, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_first_95export;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_module_95items;
(void)slim_v_type_95index;
(void)slim_v_diagnostic_95start;
(void)slim_v_diagnostic_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_composite = {0};
slim_v_composite = slim_v_kind == INT64_C(0);
(void)slim_v_composite;
if (slim_v_composite) {
{
int64_t slim_v_inner = {0};
slim_v_inner = slim_i64_add(slim_v_type_95index, INT64_C(2));
(void)slim_v_inner;
slim_result = slim_fn_project_95report_95interface_95type(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95export, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_v_inner, slim_v_diagnostic_95start, slim_v_diagnostic_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
else {
{
int64_t slim_v_scalar_95kind = {0};
slim_v_scalar_95kind = slim_fn_syntax_95scalar_95type_95kind(slim_v_module_95source, slim_v_module_95tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_scalar_95kind;
{
bool slim_v_scalar = {0};
slim_v_scalar = slim_v_scalar_95kind >= INT64_C(0);
(void)slim_v_scalar;
if (slim_v_scalar) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_module_95tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_module_95tokens, slim_v_type_95index, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_slashes = {0};
slim_v_slashes = slim_fn_syntax_95span_95count_95byte(slim_v_module_95source, slim_v_start, slim_v_end, INT64_C(47), INT64_C(0), slim_allocation_region);
(void)slim_v_slashes;
{
bool slim_v_qualified = {0};
slim_v_qualified = slim_v_slashes > INT64_C(0);
(void)slim_v_qualified;
if (slim_v_qualified) {
slim_result = INT64_C(0);
}
else {
{
int64_t slim_v_declaration = {0};
slim_v_declaration = slim_fn_project_95find_95declaration_95segment(slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_v_module_95source, slim_v_start, slim_v_end, slim_allocation_region);
(void)slim_v_declaration;
{
bool slim_v_found = {0};
slim_v_found = slim_v_declaration >= INT64_C(0);
(void)slim_v_found;
{
bool slim_v_record_95item = {0};
if (!slim_v_found) {
slim_v_record_95item = false;
}
else {
slim_v_record_95item = slim_fn_syntax_95item_95is(slim_v_module_95source, slim_v_module_95tokens, slim_v_declaration, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
}
(void)slim_v_record_95item;
{
bool slim_v_variant_95item = {0};
if (!slim_v_found) {
slim_v_variant_95item = false;
}
else {
slim_v_variant_95item = slim_fn_syntax_95item_95is(slim_v_module_95source, slim_v_module_95tokens, slim_v_declaration, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
}
(void)slim_v_variant_95item;
{
bool slim_v_type_95item = {0};
slim_v_type_95item = slim_v_record_95item || slim_v_variant_95item;
(void)slim_v_type_95item;
{
bool slim_v_exported = {0};
slim_v_exported = slim_fn_project_95exports_95has_95segment(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95export, slim_v_module_95source, slim_v_start, slim_v_end, slim_allocation_region);
(void)slim_v_exported;
{
bool slim_v_valid = {0};
slim_v_valid = slim_v_type_95item && slim_v_exported;
(void)slim_v_valid;
if (slim_v_valid) {
slim_result = INT64_C(0);
}
else {
slim_result = slim_fn_project_95report_95project_95diagnostic(slim_bytes_static((const uint8_t *)"E0418", (int64_t)(sizeof("E0418") - 1)), slim_v_module_95name, slim_v_diagnostic_95start, slim_v_diagnostic_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95report_95interface_95bindings(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_first_95export, SlimBytes slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_diagnostic_95start, int64_t slim_v_diagnostic_95end, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_first_95export;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_module_95items;
(void)slim_v_cursor;
(void)slim_v_diagnostic_95start;
(void)slim_v_diagnostic_95end;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_borrowed = {0};
slim_v_borrowed = slim_fn_syntax_95token_95equal(slim_v_module_95source, slim_v_module_95tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_borrowed;
{
int64_t slim_v_offset = {0};
if (slim_v_borrowed) {
slim_v_offset = INT64_C(3);
}
else {
slim_v_offset = INT64_C(2);
}
(void)slim_v_offset;
{
int64_t slim_v_type_95index = {0};
slim_v_type_95index = slim_i64_add(slim_v_cursor, slim_v_offset);
(void)slim_v_type_95index;
{
int64_t slim_v_reported = {0};
slim_v_reported = slim_fn_project_95report_95interface_95type(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95export, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_v_type_95index, slim_v_diagnostic_95start, slim_v_diagnostic_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_48102 = {0};
slim_t_48102 = slim_v_manifest_95source;
int64_t slim_t_48104 = {0};
slim_t_48104 = slim_v_first_95export;
SlimBytes slim_t_48105 = {0};
slim_t_48105 = slim_v_module_95name;
SlimBytes slim_t_48106 = {0};
slim_t_48106 = slim_v_module_95source;
int64_t slim_t_48108 = {0};
slim_t_48108 = slim_v_module_95items;
int64_t slim_t_48109 = {0};
slim_t_48109 = slim_v_next;
int64_t slim_t_48110 = {0};
slim_t_48110 = slim_v_diagnostic_95start;
int64_t slim_t_48111 = {0};
slim_t_48111 = slim_v_diagnostic_95end;
int64_t slim_t_48112 = {0};
slim_t_48112 = slim_v_total;
slim_v_manifest_95source = slim_t_48102;
slim_v_first_95export = slim_t_48104;
slim_v_module_95name = slim_t_48105;
slim_v_module_95source = slim_t_48106;
slim_v_module_95items = slim_t_48108;
slim_v_cursor = slim_t_48109;
slim_v_diagnostic_95start = slim_t_48110;
slim_v_diagnostic_95end = slim_t_48111;
slim_v_count = slim_t_48112;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95report_95interface_95payload(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_first_95export, SlimBytes slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_diagnostic_95start, int64_t slim_v_diagnostic_95end, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_first_95export;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_module_95items;
(void)slim_v_cursor;
(void)slim_v_diagnostic_95start;
(void)slim_v_diagnostic_95end;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_reported = {0};
slim_v_reported = slim_fn_project_95report_95interface_95type(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95export, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_v_cursor, slim_v_diagnostic_95start, slim_v_diagnostic_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_48259 = {0};
slim_t_48259 = slim_v_manifest_95source;
int64_t slim_t_48261 = {0};
slim_t_48261 = slim_v_first_95export;
SlimBytes slim_t_48262 = {0};
slim_t_48262 = slim_v_module_95name;
SlimBytes slim_t_48263 = {0};
slim_t_48263 = slim_v_module_95source;
int64_t slim_t_48265 = {0};
slim_t_48265 = slim_v_module_95items;
int64_t slim_t_48266 = {0};
slim_t_48266 = slim_v_next;
int64_t slim_t_48267 = {0};
slim_t_48267 = slim_v_diagnostic_95start;
int64_t slim_t_48268 = {0};
slim_t_48268 = slim_v_diagnostic_95end;
int64_t slim_t_48269 = {0};
slim_t_48269 = slim_v_total;
slim_v_manifest_95source = slim_t_48259;
slim_v_first_95export = slim_t_48261;
slim_v_module_95name = slim_t_48262;
slim_v_module_95source = slim_t_48263;
slim_v_module_95items = slim_t_48265;
slim_v_cursor = slim_t_48266;
slim_v_diagnostic_95start = slim_t_48267;
slim_v_diagnostic_95end = slim_t_48268;
slim_v_count = slim_t_48269;
goto slim_recur;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95report_95interface_95cases(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_first_95export, SlimBytes slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_diagnostic_95start, int64_t slim_v_diagnostic_95end, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_first_95export;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_module_95items;
(void)slim_v_cursor;
(void)slim_v_diagnostic_95start;
(void)slim_v_diagnostic_95end;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_first_95type = {0};
slim_v_first_95type = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_first_95type;
{
int64_t slim_v_reported = {0};
slim_v_reported = slim_fn_project_95report_95interface_95payload(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95export, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_v_first_95type, slim_v_diagnostic_95start, slim_v_diagnostic_95end, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_48423 = {0};
slim_t_48423 = slim_v_manifest_95source;
int64_t slim_t_48425 = {0};
slim_t_48425 = slim_v_first_95export;
SlimBytes slim_t_48426 = {0};
slim_t_48426 = slim_v_module_95name;
SlimBytes slim_t_48427 = {0};
slim_t_48427 = slim_v_module_95source;
int64_t slim_t_48429 = {0};
slim_t_48429 = slim_v_module_95items;
int64_t slim_t_48430 = {0};
slim_t_48430 = slim_v_next;
int64_t slim_t_48431 = {0};
slim_t_48431 = slim_v_diagnostic_95start;
int64_t slim_t_48432 = {0};
slim_t_48432 = slim_v_diagnostic_95end;
int64_t slim_t_48433 = {0};
slim_t_48433 = slim_v_total;
slim_v_manifest_95source = slim_t_48423;
slim_v_first_95export = slim_t_48425;
slim_v_module_95name = slim_t_48426;
slim_v_module_95source = slim_t_48427;
slim_v_module_95items = slim_t_48429;
slim_v_cursor = slim_t_48430;
slim_v_diagnostic_95start = slim_t_48431;
slim_v_diagnostic_95end = slim_t_48432;
slim_v_count = slim_t_48433;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95report_95export_95interfaces(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, int64_t slim_v_first_95export, SlimBytes slim_v_module_95name, SlimBytes slim_v_module_95source, SlimVec * slim_v_module_95tokens, int64_t slim_v_module_95items, int64_t slim_v_cursor, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_first_95export;
(void)slim_v_module_95name;
(void)slim_v_module_95source;
(void)slim_v_module_95tokens;
(void)slim_v_module_95items;
(void)slim_v_cursor;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
int64_t slim_v_item = {0};
slim_v_item = slim_fn_project_95find_95export_95declaration(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_cursor, slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_allocation_region);
(void)slim_v_item;
{
int64_t slim_v_diagnostic_95start = {0};
slim_v_diagnostic_95start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_diagnostic_95start;
{
int64_t slim_v_diagnostic_95end = {0};
slim_v_diagnostic_95end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_diagnostic_95end;
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_module_95source, slim_v_module_95tokens, slim_v_item, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
{
int64_t slim_v_reported = {0};
if (slim_v_function_95item) {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_first_95parameter = {0};
slim_v_first_95parameter = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_first_95parameter;
{
int64_t slim_v_parameter_95errors = {0};
slim_v_parameter_95errors = slim_fn_project_95report_95interface_95bindings(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95export, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_v_first_95parameter, slim_v_diagnostic_95start, slim_v_diagnostic_95end, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_parameter_95errors;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_return_95errors = {0};
slim_v_return_95errors = slim_fn_project_95report_95interface_95type(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95export, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_v_return_95type, slim_v_diagnostic_95start, slim_v_diagnostic_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_return_95errors;
slim_v_reported = slim_i64_add(slim_v_parameter_95errors, slim_v_return_95errors);
}
}
}
}
}
}
else {
{
bool slim_v_record_95item = {0};
slim_v_record_95item = slim_fn_syntax_95item_95is(slim_v_module_95source, slim_v_module_95tokens, slim_v_item, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_95item;
{
int64_t slim_v_body = {0};
slim_v_body = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_body;
{
int64_t slim_v_first_95member = {0};
slim_v_first_95member = slim_i64_add(slim_v_body, INT64_C(1));
(void)slim_v_first_95member;
if (slim_v_record_95item) {
slim_v_reported = slim_fn_project_95report_95interface_95bindings(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95export, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_v_first_95member, slim_v_diagnostic_95start, slim_v_diagnostic_95end, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
else {
slim_v_reported = slim_fn_project_95report_95interface_95cases(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95export, slim_v_module_95name, slim_v_module_95source, slim_v_module_95tokens, slim_v_module_95items, slim_v_first_95member, slim_v_diagnostic_95start, slim_v_diagnostic_95end, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_48764 = {0};
slim_t_48764 = slim_v_manifest_95source;
int64_t slim_t_48766 = {0};
slim_t_48766 = slim_v_first_95export;
SlimBytes slim_t_48767 = {0};
slim_t_48767 = slim_v_module_95name;
SlimBytes slim_t_48768 = {0};
slim_t_48768 = slim_v_module_95source;
int64_t slim_t_48770 = {0};
slim_t_48770 = slim_v_module_95items;
int64_t slim_t_48771 = {0};
slim_t_48771 = slim_v_next;
int64_t slim_t_48772 = {0};
slim_t_48772 = slim_v_total;
slim_v_manifest_95source = slim_t_48764;
slim_v_first_95export = slim_t_48766;
slim_v_module_95name = slim_t_48767;
slim_v_module_95source = slim_t_48768;
slim_v_module_95items = slim_t_48770;
slim_v_cursor = slim_t_48771;
slim_v_count = slim_t_48772;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95report_95private_95type_95leaks_95from(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
Slim_type_project_95LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_95LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_manifest = {0};
slim_v_manifest = slim_v_module.slim_field_manifest;
(void)slim_v_manifest;
{
int64_t slim_v_name = {0};
slim_v_name = slim_v_module.slim_field_name;
(void)slim_v_name;
{
SlimBytes slim_v_module_95name = {0};
slim_v_module_95name = slim_fn_project_95token_95bytes(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95name;
{
int64_t slim_v_imports = {0};
slim_v_imports = slim_i64_add(slim_v_manifest, INT64_C(4));
(void)slim_v_imports;
{
int64_t slim_v_exports = {0};
slim_v_exports = slim_fn_syntax_95skip_95form(slim_v_manifest_95tokens, slim_v_imports, slim_allocation_region);
(void)slim_v_exports;
{
int64_t slim_v_first_95export = {0};
slim_v_first_95export = slim_i64_add(slim_v_exports, INT64_C(2));
(void)slim_v_first_95export;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_module.slim_field_source;
(void)slim_v_source;
{
int64_t slim_v_root = {0};
slim_v_root = slim_v_module.slim_field_root;
(void)slim_v_root;
{
int64_t slim_v_module_95items = {0};
slim_v_module_95items = slim_i64_add(slim_v_root, INT64_C(3));
(void)slim_v_module_95items;
{
int64_t slim_v_reported = {0};
slim_v_reported = slim_fn_project_95report_95export_95interfaces(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_first_95export, slim_v_module_95name, slim_v_source, slim_v_module_95tokens, slim_v_module_95items, slim_v_first_95export, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_reported;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_reported);
(void)slim_v_total;
SlimBytes slim_t_48998 = {0};
slim_t_48998 = slim_v_manifest_95source;
int64_t slim_t_49002 = {0};
slim_t_49002 = slim_v_next;
int64_t slim_t_49003 = {0};
slim_t_49003 = slim_v_total;
slim_v_manifest_95source = slim_t_48998;
slim_v_index = slim_t_49002;
slim_v_count = slim_t_49003;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_project_95report_95private_95type_95leaks(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_errors = {0};
slim_v_errors = slim_fn_project_95report_95private_95type_95leaks_95from(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95tokens, slim_v_loaded, INT64_C(0), INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_errors;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_errors > INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = INT64_C(1);
}
else {
slim_result = INT64_C(0);
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimBytes slim_fn_query_95copy_95span(SlimBytes slim_v_source, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_start;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_text_95append_95span(slim_v_source, slim_v_start, slim_v_end, &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_query_95bytes_95equal_95from(SlimBytes slim_v_left, SlimBytes slim_v_right, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_right;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
uint8_t slim_v_left_95byte = {0};
slim_v_left_95byte = slim_bytes_get(slim_v_left, slim_v_index);
(void)slim_v_left_95byte;
{
uint8_t slim_v_right_95byte = {0};
slim_v_right_95byte = slim_bytes_get(slim_v_right, slim_v_index);
(void)slim_v_right_95byte;
{
int64_t slim_v_left_95code = {0};
slim_v_left_95code = (int64_t)slim_v_left_95byte;
(void)slim_v_left_95code;
{
int64_t slim_v_right_95code = {0};
slim_v_right_95code = (int64_t)slim_v_right_95byte;
(void)slim_v_right_95code;
{
bool slim_v_same = {0};
slim_v_same = slim_v_left_95code == slim_v_right_95code;
(void)slim_v_same;
if (!slim_v_same) {
slim_result = false;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_49368 = {0};
slim_t_49368 = slim_v_left;
SlimBytes slim_t_49369 = {0};
slim_t_49369 = slim_v_right;
int64_t slim_t_49370 = {0};
slim_t_49370 = slim_v_next;
int64_t slim_t_49371 = {0};
slim_t_49371 = slim_v_length;
slim_v_left = slim_t_49368;
slim_v_right = slim_t_49369;
slim_v_index = slim_t_49370;
slim_v_length = slim_t_49371;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_query_95bytes_95equal(SlimBytes slim_v_left, SlimBytes slim_v_right, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_right;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_left_95length = {0};
slim_v_left_95length = slim_bytes_len(slim_v_left);
(void)slim_v_left_95length;
{
int64_t slim_v_right_95length = {0};
slim_v_right_95length = slim_bytes_len(slim_v_right);
(void)slim_v_right_95length;
{
bool slim_v_same_95length = {0};
slim_v_same_95length = slim_v_left_95length == slim_v_right_95length;
(void)slim_v_same_95length;
if (!slim_v_same_95length) {
slim_result = false;
}
else {
slim_result = slim_fn_query_95bytes_95equal_95from(slim_v_left, slim_v_right, INT64_C(0), slim_v_left_95length, slim_allocation_region);
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_query_95declaration_95kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_item;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_item, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
if (slim_v_function_95item) {
slim_result = INT64_C(0);
}
else {
{
bool slim_v_record_95item = {0};
slim_v_record_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_item, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record_95item;
if (slim_v_record_95item) {
slim_result = INT64_C(1);
}
else {
slim_result = INT64_C(2);
}
}
}
}
return slim_result;
}

static int64_t slim_fn_query_95declaration_95interface_95end(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_item;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_function_95item = {0};
slim_v_function_95item = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_item, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function_95item;
if (!slim_v_function_95item) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_item, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next, INT64_C(1));
(void)slim_v_closing;
slim_result = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_closing, slim_allocation_region);
}
}
}
else {
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_item, INT64_C(3));
(void)slim_v_params;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_after_95effects = {0};
slim_v_after_95effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_after_95effects;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_after_95effects, INT64_C(1));
(void)slim_v_closing;
slim_result = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_closing, slim_allocation_region);
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_query_95append_95module_95snapshots(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, Slim_type_project_95LoadedModule slim_v_module, int64_t slim_v_cursor, SlimBytes slim_v_module_95name, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95tokens;
(void)slim_v_module;
(void)slim_v_cursor;
(void)slim_v_module_95name;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_module.slim_field_source;
(void)slim_v_source;
{
int64_t slim_v_name_95token = {0};
slim_v_name_95token = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name_95token;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_module_95tokens, slim_v_name_95token, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_module_95tokens, slim_v_name_95token, slim_allocation_region);
(void)slim_v_name_95end;
{
SlimBytes slim_v_name = {0};
slim_v_name = slim_fn_query_95copy_95span(slim_v_source, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_name;
{
int64_t slim_v_query_95declaration_95kind = {0};
slim_v_query_95declaration_95kind = slim_fn_query_95declaration_95kind(slim_v_source, slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_query_95declaration_95kind;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_interface_95end = {0};
slim_v_interface_95end = slim_fn_query_95declaration_95interface_95end(slim_v_source, slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_interface_95end;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_module_95tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_next, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_module_95tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_end;
{
Slim_type_query_95Snapshot slim_v_snapshot = {0};
{
SlimBytes slim_t_49868 = {0};
slim_t_49868 = slim_v_module_95name;
int64_t slim_t_49872 = {0};
slim_t_49872 = slim_v_query_95declaration_95kind;
SlimBytes slim_t_49876 = {0};
slim_t_49876 = slim_v_name;
SlimBytes slim_t_49880 = {0};
slim_t_49880 = slim_v_source;
int64_t slim_t_49884 = {0};
slim_t_49884 = slim_v_cursor;
int64_t slim_t_49888 = {0};
slim_t_49888 = slim_v_next;
int64_t slim_t_49892 = {0};
slim_t_49892 = slim_v_start;
int64_t slim_t_49896 = {0};
slim_t_49896 = slim_v_interface_95end;
int64_t slim_t_49900 = {0};
slim_t_49900 = slim_v_end;
slim_v_snapshot = (Slim_type_query_95Snapshot){.slim_field_module = slim_t_49868, .slim_field_kind = slim_t_49872, .slim_field_name = slim_t_49876, .slim_field_source = slim_t_49880, .slim_field_form = slim_t_49884, .slim_field_next = slim_t_49888, .slim_field_start = slim_t_49892, .slim_field_interface_95end = slim_t_49896, .slim_field_end = slim_t_49900};
}
(void)slim_v_snapshot;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_output, &slim_v_snapshot)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
SlimBytes slim_t_49917 = {0};
slim_t_49917 = slim_v_manifest_95source;
Slim_type_project_95LoadedModule slim_t_49920 = {0};
slim_t_49920 = slim_v_module;
int64_t slim_t_49921 = {0};
slim_t_49921 = slim_v_next;
SlimBytes slim_t_49922 = {0};
slim_t_49922 = slim_v_module_95name;
slim_v_manifest_95source = slim_t_49917;
slim_v_module = slim_t_49920;
slim_v_cursor = slim_t_49921;
slim_v_module_95name = slim_t_49922;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_query_95build_95snapshots_95from(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_loaded)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
Slim_type_project_95LoadedModule slim_v_module = {0};
slim_v_module = ((Slim_type_project_95LoadedModule *)((*slim_v_loaded)).data)[slim_vec_check_index(&((*slim_v_loaded)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_manifest_95name = {0};
slim_v_manifest_95name = slim_v_module.slim_field_name;
(void)slim_v_manifest_95name;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_manifest_95tokens, slim_v_manifest_95name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_manifest_95tokens, slim_v_manifest_95name, slim_allocation_region);
(void)slim_v_name_95end;
{
SlimBytes slim_v_module_95name = {0};
slim_v_module_95name = slim_fn_query_95copy_95span(slim_v_manifest_95source, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95name;
{
int64_t slim_v_root = {0};
slim_v_root = slim_v_module.slim_field_root;
(void)slim_v_root;
{
int64_t slim_v_first = {0};
slim_v_first = slim_i64_add(slim_v_root, INT64_C(3));
(void)slim_v_first;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_query_95append_95module_95snapshots(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95tokens, slim_v_module, slim_v_first, slim_v_module_95name, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_50118 = {0};
slim_t_50118 = slim_v_manifest_95source;
int64_t slim_t_50122 = {0};
slim_t_50122 = slim_v_next;
slim_v_manifest_95source = slim_t_50118;
slim_v_index = slim_t_50122;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimVec slim_fn_query_95build_95snapshots(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimVec slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(Slim_type_query_95Snapshot), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_built = {0};
slim_v_built = slim_fn_query_95build_95snapshots_95from(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95tokens, slim_v_loaded, INT64_C(0), &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_built;
slim_result = slim_v_output;
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_query_95snapshot_95identity_95equal(Slim_type_query_95Snapshot slim_v_left, Slim_type_query_95Snapshot slim_v_right, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_right;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
SlimBytes slim_v_left_95module = {0};
slim_v_left_95module = slim_v_left.slim_field_module;
(void)slim_v_left_95module;
{
SlimBytes slim_v_right_95module = {0};
slim_v_right_95module = slim_v_right.slim_field_module;
(void)slim_v_right_95module;
{
bool slim_v_same_95module = {0};
slim_v_same_95module = slim_fn_query_95bytes_95equal(slim_v_left_95module, slim_v_right_95module, slim_allocation_region);
(void)slim_v_same_95module;
{
SlimBytes slim_v_left_95name = {0};
slim_v_left_95name = slim_v_left.slim_field_name;
(void)slim_v_left_95name;
{
SlimBytes slim_v_right_95name = {0};
slim_v_right_95name = slim_v_right.slim_field_name;
(void)slim_v_right_95name;
{
bool slim_v_same_95name = {0};
slim_v_same_95name = slim_fn_query_95bytes_95equal(slim_v_left_95name, slim_v_right_95name, slim_allocation_region);
(void)slim_v_same_95name;
{
int64_t slim_v_left_95kind = {0};
slim_v_left_95kind = slim_v_left.slim_field_kind;
(void)slim_v_left_95kind;
{
int64_t slim_v_right_95kind = {0};
slim_v_right_95kind = slim_v_right.slim_field_kind;
(void)slim_v_right_95kind;
{
bool slim_v_same_95kind = {0};
slim_v_same_95kind = slim_v_left_95kind == slim_v_right_95kind;
(void)slim_v_same_95kind;
{
bool slim_v_first = {0};
slim_v_first = slim_v_same_95module && slim_v_same_95name;
(void)slim_v_first;
slim_result = slim_v_first && slim_v_same_95kind;
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_query_95snapshot_95body_95equal(Slim_type_query_95Snapshot slim_v_left, Slim_type_query_95Snapshot slim_v_right, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_right;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_left_95end = {0};
slim_v_left_95end = slim_v_left.slim_field_end;
(void)slim_v_left_95end;
{
int64_t slim_v_left_95start = {0};
slim_v_left_95start = slim_v_left.slim_field_start;
(void)slim_v_left_95start;
{
int64_t slim_v_left_95length = {0};
slim_v_left_95length = slim_i64_sub(slim_v_left_95end, slim_v_left_95start);
(void)slim_v_left_95length;
{
int64_t slim_v_right_95end = {0};
slim_v_right_95end = slim_v_right.slim_field_end;
(void)slim_v_right_95end;
{
int64_t slim_v_right_95start = {0};
slim_v_right_95start = slim_v_right.slim_field_start;
(void)slim_v_right_95start;
{
int64_t slim_v_right_95length = {0};
slim_v_right_95length = slim_i64_sub(slim_v_right_95end, slim_v_right_95start);
(void)slim_v_right_95length;
{
bool slim_v_same_95length = {0};
slim_v_same_95length = slim_v_left_95length == slim_v_right_95length;
(void)slim_v_same_95length;
if (!slim_v_same_95length) {
slim_result = false;
}
else {
{
SlimBytes slim_v_left_95bytes = {0};
slim_v_left_95bytes = slim_v_left.slim_field_source;
(void)slim_v_left_95bytes;
{
SlimBytes slim_v_right_95bytes = {0};
slim_v_right_95bytes = slim_v_right.slim_field_source;
(void)slim_v_right_95bytes;
slim_result = slim_fn_project_95cross_95span_95chars_95equal(slim_v_left_95bytes, slim_v_left_95start, slim_v_right_95bytes, slim_v_right_95start, INT64_C(0), slim_v_left_95length, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_query_95find_95snapshot(SlimVec * slim_v_snapshots, Slim_type_query_95Snapshot slim_v_candidate, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_snapshots;
(void)slim_v_candidate;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_snapshots)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_query_95Snapshot slim_v_snapshot = {0};
slim_v_snapshot = ((Slim_type_query_95Snapshot *)((*slim_v_snapshots)).data)[slim_vec_check_index(&((*slim_v_snapshots)), slim_v_index)];
(void)slim_v_snapshot;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_query_95snapshot_95identity_95equal(slim_v_snapshot, slim_v_candidate, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
Slim_type_query_95Snapshot slim_t_50568 = {0};
slim_t_50568 = slim_v_candidate;
int64_t slim_t_50569 = {0};
slim_t_50569 = slim_v_next;
slim_v_candidate = slim_t_50568;
slim_v_index = slim_t_50569;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static SlimBytes slim_fn_query_95snapshot_95qualified_95name(Slim_type_query_95Snapshot slim_v_snapshot, SlimRegion *slim_region) {
(void)slim_v_snapshot;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimBytes slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_output;
{
SlimBytes slim_v_module = {0};
slim_v_module = slim_v_snapshot.slim_field_module;
(void)slim_v_module;
{
SlimUnit slim_v_module_95text = {0};
slim_v_module_95text = slim_fn_text_95append_95text(&slim_v_output, slim_v_module, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_module_95text;
{
SlimUnit slim_v_slash = {0};
slim_v_slash = slim_fn_text_95append_95text(&slim_v_output, slim_bytes_static((const uint8_t *)"/", (int64_t)(sizeof("/") - 1)), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_slash;
{
SlimBytes slim_v_name = {0};
slim_v_name = slim_v_snapshot.slim_field_name;
(void)slim_v_name;
{
SlimUnit slim_v_name_95text = {0};
slim_v_name_95text = slim_fn_text_95append_95text(&slim_v_output, slim_v_name, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_name_95text;
slim_result = slim_bytes_freeze(slim_v_output);
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_query_95find_95provider(SlimBytes slim_v_atom, SlimVec * slim_v_snapshots, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_atom;
(void)slim_v_snapshots;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_snapshots)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_query_95Snapshot slim_v_snapshot = {0};
slim_v_snapshot = ((Slim_type_query_95Snapshot *)((*slim_v_snapshots)).data)[slim_vec_check_index(&((*slim_v_snapshots)), slim_v_index)];
(void)slim_v_snapshot;
{
SlimBytes slim_v_qualified = {0};
slim_v_qualified = slim_fn_query_95snapshot_95qualified_95name(slim_v_snapshot, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_qualified;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_query_95bytes_95equal(slim_v_atom, slim_v_qualified, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_50772 = {0};
slim_t_50772 = slim_v_atom;
int64_t slim_t_50774 = {0};
slim_t_50774 = slim_v_next;
slim_v_atom = slim_t_50772;
slim_v_index = slim_t_50774;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_query_95dependency_95exists(SlimVec * slim_v_dependencies, int64_t slim_v_consumer, int64_t slim_v_provider, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_dependencies;
(void)slim_v_consumer;
(void)slim_v_provider;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_dependencies)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
Slim_type_query_95Dependency slim_v_dependency = {0};
slim_v_dependency = ((Slim_type_query_95Dependency *)((*slim_v_dependencies)).data)[slim_vec_check_index(&((*slim_v_dependencies)), slim_v_index)];
(void)slim_v_dependency;
{
int64_t slim_v_candidate_95consumer = {0};
slim_v_candidate_95consumer = slim_v_dependency.slim_field_consumer;
(void)slim_v_candidate_95consumer;
{
bool slim_v_same_95consumer = {0};
slim_v_same_95consumer = slim_v_candidate_95consumer == slim_v_consumer;
(void)slim_v_same_95consumer;
{
int64_t slim_v_candidate_95provider = {0};
slim_v_candidate_95provider = slim_v_dependency.slim_field_provider;
(void)slim_v_candidate_95provider;
{
bool slim_v_same_95provider = {0};
slim_v_same_95provider = slim_v_candidate_95provider == slim_v_provider;
(void)slim_v_same_95provider;
{
bool slim_v_same = {0};
slim_v_same = slim_v_same_95consumer && slim_v_same_95provider;
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_50925 = {0};
slim_t_50925 = slim_v_consumer;
int64_t slim_t_50926 = {0};
slim_t_50926 = slim_v_provider;
int64_t slim_t_50927 = {0};
slim_t_50927 = slim_v_next;
slim_v_consumer = slim_t_50925;
slim_v_provider = slim_t_50926;
slim_v_index = slim_t_50927;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_query_95append_95snapshot_95dependencies(SlimVec * slim_v_tokens, SlimVec * slim_v_snapshots, int64_t slim_v_consumer, int64_t slim_v_cursor, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_snapshots;
(void)slim_v_consumer;
(void)slim_v_cursor;
(void)slim_v_end;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
Slim_type_query_95Snapshot slim_v_snapshot = {0};
slim_v_snapshot = ((Slim_type_query_95Snapshot *)((*slim_v_snapshots)).data)[slim_vec_check_index(&((*slim_v_snapshots)), slim_v_consumer)];
(void)slim_v_snapshot;
{
SlimBytes slim_v_source = {0};
slim_v_source = slim_v_snapshot.slim_field_source;
(void)slim_v_source;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_atom_95kind = {0};
slim_v_atom_95kind = slim_v_kind == INT64_C(2);
(void)slim_v_atom_95kind;
{
int64_t slim_v_provider = {0};
if (slim_v_atom_95kind) {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_finish = {0};
slim_v_finish = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_finish;
{
SlimBytes slim_v_atom = {0};
slim_v_atom = slim_fn_query_95copy_95span(slim_v_source, slim_v_start, slim_v_finish, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_atom;
slim_v_provider = slim_fn_query_95find_95provider(slim_v_atom, slim_v_snapshots, INT64_C(0), slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
else {
slim_v_provider = INT64_C(-1);
}
(void)slim_v_provider;
{
bool slim_v_found = {0};
slim_v_found = slim_v_provider >= INT64_C(0);
(void)slim_v_found;
{
bool slim_v_self = {0};
slim_v_self = slim_v_consumer == slim_v_provider;
(void)slim_v_self;
{
bool slim_v_not_95self = {0};
slim_v_not_95self = !slim_v_self;
(void)slim_v_not_95self;
{
bool slim_v_cross = {0};
slim_v_cross = slim_v_found && slim_v_not_95self;
(void)slim_v_cross;
{
bool slim_v_duplicate = {0};
if (slim_v_cross) {
slim_v_duplicate = slim_fn_query_95dependency_95exists(slim_v_output, slim_v_consumer, slim_v_provider, INT64_C(0), slim_allocation_region);
}
else {
slim_v_duplicate = false;
}
(void)slim_v_duplicate;
{
bool slim_v_unique = {0};
slim_v_unique = !slim_v_duplicate;
(void)slim_v_unique;
{
bool slim_v_append = {0};
slim_v_append = slim_v_cross && slim_v_unique;
(void)slim_v_append;
{
SlimUnit slim_v_pushed = {0};
if (slim_v_append) {
{
Slim_type_query_95Dependency slim_v_dependency = {0};
{
int64_t slim_t_51201 = {0};
slim_t_51201 = slim_v_consumer;
int64_t slim_t_51205 = {0};
slim_t_51205 = slim_v_provider;
slim_v_dependency = (Slim_type_query_95Dependency){.slim_field_consumer = slim_t_51201, .slim_field_provider = slim_t_51205};
}
(void)slim_v_dependency;
if (!slim_vec_push(slim_v_output, &slim_v_dependency)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
}
}
else {
slim_v_pushed = (SlimUnit){0};
}
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_51237 = {0};
slim_t_51237 = slim_v_consumer;
int64_t slim_t_51238 = {0};
slim_t_51238 = slim_v_next;
int64_t slim_t_51239 = {0};
slim_t_51239 = slim_v_end;
slim_v_consumer = slim_t_51237;
slim_v_cursor = slim_t_51238;
slim_v_end = slim_t_51239;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_query_95build_95dependencies_95from(SlimVec * slim_v_tokens, SlimVec * slim_v_snapshots, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_snapshots;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_snapshots)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
Slim_type_query_95Snapshot slim_v_snapshot = {0};
slim_v_snapshot = ((Slim_type_query_95Snapshot *)((*slim_v_snapshots)).data)[slim_vec_check_index(&((*slim_v_snapshots)), slim_v_index)];
(void)slim_v_snapshot;
{
int64_t slim_v_form = {0};
slim_v_form = slim_v_snapshot.slim_field_form;
(void)slim_v_form;
{
int64_t slim_v_next_95form = {0};
slim_v_next_95form = slim_v_snapshot.slim_field_next;
(void)slim_v_next_95form;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_query_95append_95snapshot_95dependencies(slim_v_tokens, slim_v_snapshots, slim_v_index, slim_v_form, slim_v_next_95form, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_51383 = {0};
slim_t_51383 = slim_v_next;
slim_v_index = slim_t_51383;
goto slim_recur;
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimVec slim_fn_query_95build_95dependencies(SlimVec * slim_v_tokens, SlimVec * slim_v_snapshots, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_snapshots;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimVec slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_output = {0};
slim_v_output = slim_vec_new(sizeof(Slim_type_query_95Dependency), slim_allocation_region);
(void)slim_v_output;
{
SlimUnit slim_v_built = {0};
slim_v_built = slim_fn_query_95build_95dependencies_95from(slim_v_tokens, slim_v_snapshots, INT64_C(0), &slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_built;
slim_result = slim_v_output;
}
}
slim_allocation_failed: ;
return slim_result;
}

static Slim_type_query_95State slim_fn_query_95build_95state(SlimBytes slim_v_manifest_95source, SlimVec * slim_v_manifest_95tokens, SlimVec * slim_v_module_95tokens, SlimVec * slim_v_loaded, SlimRegion *slim_region) {
(void)slim_v_manifest_95source;
(void)slim_v_manifest_95tokens;
(void)slim_v_module_95tokens;
(void)slim_v_loaded;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_query_95State slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_snapshots = {0};
slim_v_snapshots = slim_fn_query_95build_95snapshots(slim_v_manifest_95source, slim_v_manifest_95tokens, slim_v_module_95tokens, slim_v_loaded, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_snapshots;
{
SlimVec slim_v_dependencies = {0};
slim_v_dependencies = slim_fn_query_95build_95dependencies(slim_v_module_95tokens, &slim_v_snapshots, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_dependencies;
{
SlimVec slim_t_51523 = {0};
slim_t_51523 = slim_v_snapshots;
SlimVec slim_t_51527 = {0};
slim_t_51527 = slim_v_dependencies;
slim_result = (Slim_type_query_95State){.slim_field_snapshots = slim_t_51523, .slim_field_dependencies = slim_t_51527};
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_query_95snapshot_95interface_95equal(Slim_type_query_95Snapshot slim_v_left, Slim_type_query_95Snapshot slim_v_right, SlimRegion *slim_region) {
(void)slim_v_left;
(void)slim_v_right;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_left_95end = {0};
slim_v_left_95end = slim_v_left.slim_field_interface_95end;
(void)slim_v_left_95end;
{
int64_t slim_v_left_95start = {0};
slim_v_left_95start = slim_v_left.slim_field_start;
(void)slim_v_left_95start;
{
int64_t slim_v_left_95length = {0};
slim_v_left_95length = slim_i64_sub(slim_v_left_95end, slim_v_left_95start);
(void)slim_v_left_95length;
{
int64_t slim_v_right_95end = {0};
slim_v_right_95end = slim_v_right.slim_field_interface_95end;
(void)slim_v_right_95end;
{
int64_t slim_v_right_95start = {0};
slim_v_right_95start = slim_v_right.slim_field_start;
(void)slim_v_right_95start;
{
int64_t slim_v_right_95length = {0};
slim_v_right_95length = slim_i64_sub(slim_v_right_95end, slim_v_right_95start);
(void)slim_v_right_95length;
{
bool slim_v_same_95length = {0};
slim_v_same_95length = slim_v_left_95length == slim_v_right_95length;
(void)slim_v_same_95length;
if (!slim_v_same_95length) {
slim_result = false;
}
else {
{
SlimBytes slim_v_left_95bytes = {0};
slim_v_left_95bytes = slim_v_left.slim_field_source;
(void)slim_v_left_95bytes;
{
SlimBytes slim_v_right_95bytes = {0};
slim_v_right_95bytes = slim_v_right.slim_field_source;
(void)slim_v_right_95bytes;
slim_result = slim_fn_project_95cross_95span_95chars_95equal(slim_v_left_95bytes, slim_v_left_95start, slim_v_right_95bytes, slim_v_right_95start, INT64_C(0), slim_v_left_95length, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimUnit slim_fn_query_95fill_95flags(int64_t slim_v_length, int64_t slim_v_index, SlimVec * slim_v_flags, SlimRegion *slim_region) {
(void)slim_v_length;
(void)slim_v_index;
(void)slim_v_flags;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_initial = {0};
slim_v_initial = false;
(void)slim_v_initial;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_flags, &slim_v_initial)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_51741 = {0};
slim_t_51741 = slim_v_length;
int64_t slim_t_51742 = {0};
slim_t_51742 = slim_v_next;
slim_v_length = slim_t_51741;
slim_v_index = slim_t_51742;
goto slim_recur;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_query_95classify_95changes(SlimVec * slim_v_previous, SlimVec * slim_v_current, int64_t slim_v_index, int64_t slim_v_count, SlimVec * slim_v_invalidated, SlimVec * slim_v_queue, SlimRegion *slim_region) {
(void)slim_v_previous;
(void)slim_v_current;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_v_invalidated;
(void)slim_v_queue;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_current)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
Slim_type_query_95Snapshot slim_v_candidate = {0};
slim_v_candidate = ((Slim_type_query_95Snapshot *)((*slim_v_current)).data)[slim_vec_check_index(&((*slim_v_current)), slim_v_index)];
(void)slim_v_candidate;
{
int64_t slim_v_prior = {0};
slim_v_prior = slim_fn_query_95find_95snapshot(slim_v_previous, slim_v_candidate, INT64_C(0), slim_allocation_region);
(void)slim_v_prior;
{
bool slim_v_found = {0};
slim_v_found = slim_v_prior >= INT64_C(0);
(void)slim_v_found;
{
bool slim_v_body_95changed = {0};
if (!slim_v_found) {
slim_v_body_95changed = true;
}
else {
{
Slim_type_query_95Snapshot slim_v_old = {0};
slim_v_old = ((Slim_type_query_95Snapshot *)((*slim_v_previous)).data)[slim_vec_check_index(&((*slim_v_previous)), slim_v_prior)];
(void)slim_v_old;
{
bool slim_v_same_95body = {0};
slim_v_same_95body = slim_fn_query_95snapshot_95body_95equal(slim_v_old, slim_v_candidate, slim_allocation_region);
(void)slim_v_same_95body;
slim_v_body_95changed = !slim_v_same_95body;
}
}
}
(void)slim_v_body_95changed;
{
bool slim_v_interface_95changed = {0};
if (!slim_v_found) {
slim_v_interface_95changed = true;
}
else {
{
Slim_type_query_95Snapshot slim_v_old = {0};
slim_v_old = ((Slim_type_query_95Snapshot *)((*slim_v_previous)).data)[slim_vec_check_index(&((*slim_v_previous)), slim_v_prior)];
(void)slim_v_old;
{
bool slim_v_same_95interface = {0};
slim_v_same_95interface = slim_fn_query_95snapshot_95interface_95equal(slim_v_old, slim_v_candidate, slim_allocation_region);
(void)slim_v_same_95interface;
slim_v_interface_95changed = !slim_v_same_95interface;
}
}
}
(void)slim_v_interface_95changed;
{
SlimUnit slim_v_marked = {0};
if (slim_v_body_95changed) {
{
bool slim_v_value = {0};
slim_v_value = true;
(void)slim_v_value;
slim_vec_set(slim_v_invalidated, slim_v_index, &slim_v_value); slim_v_marked = (SlimUnit){0};
}
}
else {
slim_v_marked = (SlimUnit){0};
}
(void)slim_v_marked;
{
SlimUnit slim_v_enqueued = {0};
if (slim_v_interface_95changed) {
if (!slim_vec_push(slim_v_queue, &slim_v_index)) goto slim_allocation_failed; slim_v_enqueued = (SlimUnit){0};
}
else {
slim_v_enqueued = (SlimUnit){0};
}
(void)slim_v_enqueued;
{
int64_t slim_v_increment = {0};
if (slim_v_body_95changed) {
slim_v_increment = INT64_C(1);
}
else {
slim_v_increment = INT64_C(0);
}
(void)slim_v_increment;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_increment);
(void)slim_v_total;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_52035 = {0};
slim_t_52035 = slim_v_next;
int64_t slim_t_52036 = {0};
slim_t_52036 = slim_v_total;
slim_v_index = slim_t_52035;
slim_v_count = slim_t_52036;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_query_95mark_95consumers(SlimVec * slim_v_dependencies, int64_t slim_v_provider, int64_t slim_v_index, SlimVec * slim_v_invalidated, SlimVec * slim_v_queue, SlimRegion *slim_region) {
(void)slim_v_dependencies;
(void)slim_v_provider;
(void)slim_v_index;
(void)slim_v_invalidated;
(void)slim_v_queue;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_dependencies)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
Slim_type_query_95Dependency slim_v_dependency = {0};
slim_v_dependency = ((Slim_type_query_95Dependency *)((*slim_v_dependencies)).data)[slim_vec_check_index(&((*slim_v_dependencies)), slim_v_index)];
(void)slim_v_dependency;
{
int64_t slim_v_candidate_95provider = {0};
slim_v_candidate_95provider = slim_v_dependency.slim_field_provider;
(void)slim_v_candidate_95provider;
{
bool slim_v_same = {0};
slim_v_same = slim_v_candidate_95provider == slim_v_provider;
(void)slim_v_same;
{
int64_t slim_v_consumer = {0};
slim_v_consumer = slim_v_dependency.slim_field_consumer;
(void)slim_v_consumer;
{
bool slim_v_already = {0};
slim_v_already = ((bool *)((*slim_v_invalidated)).data)[slim_vec_check_index(&((*slim_v_invalidated)), slim_v_consumer)];
(void)slim_v_already;
{
bool slim_v_fresh = {0};
slim_v_fresh = !slim_v_already;
(void)slim_v_fresh;
{
bool slim_v_mark = {0};
slim_v_mark = slim_v_same && slim_v_fresh;
(void)slim_v_mark;
{
SlimUnit slim_v_marked = {0};
if (slim_v_mark) {
{
bool slim_v_value = {0};
slim_v_value = true;
(void)slim_v_value;
slim_vec_set(slim_v_invalidated, slim_v_consumer, &slim_v_value); slim_v_marked = (SlimUnit){0};
}
}
else {
slim_v_marked = (SlimUnit){0};
}
(void)slim_v_marked;
{
SlimUnit slim_v_enqueued = {0};
if (slim_v_mark) {
if (!slim_vec_push(slim_v_queue, &slim_v_consumer)) goto slim_allocation_failed; slim_v_enqueued = (SlimUnit){0};
}
else {
slim_v_enqueued = (SlimUnit){0};
}
(void)slim_v_enqueued;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_52255 = {0};
slim_t_52255 = slim_v_provider;
int64_t slim_t_52256 = {0};
slim_t_52256 = slim_v_next;
slim_v_provider = slim_t_52255;
slim_v_index = slim_t_52256;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_query_95propagate_95invalidations(SlimVec * slim_v_dependencies, SlimVec * slim_v_queue, int64_t slim_v_cursor, SlimVec * slim_v_invalidated, SlimRegion *slim_region) {
(void)slim_v_dependencies;
(void)slim_v_queue;
(void)slim_v_cursor;
(void)slim_v_invalidated;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_queue)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_provider = {0};
slim_v_provider = ((int64_t *)((*slim_v_queue)).data)[slim_vec_check_index(&((*slim_v_queue)), slim_v_cursor)];
(void)slim_v_provider;
{
SlimUnit slim_v_marked = {0};
slim_v_marked = slim_fn_query_95mark_95consumers(slim_v_dependencies, slim_v_provider, INT64_C(0), slim_v_invalidated, slim_v_queue, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_marked;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_52379 = {0};
slim_t_52379 = slim_v_next;
slim_v_cursor = slim_t_52379;
goto slim_recur;
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_query_95count_95flags(SlimVec * slim_v_flags, int64_t slim_v_index, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_flags;
(void)slim_v_index;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_flags)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
bool slim_v_flag = {0};
slim_v_flag = ((bool *)((*slim_v_flags)).data)[slim_vec_check_index(&((*slim_v_flags)), slim_v_index)];
(void)slim_v_flag;
{
int64_t slim_v_increment = {0};
if (slim_v_flag) {
slim_v_increment = INT64_C(1);
}
else {
slim_v_increment = INT64_C(0);
}
(void)slim_v_increment;
{
int64_t slim_v_total = {0};
slim_v_total = slim_i64_add(slim_v_count, slim_v_increment);
(void)slim_v_total;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_52493 = {0};
slim_t_52493 = slim_v_next;
int64_t slim_t_52494 = {0};
slim_t_52494 = slim_v_total;
slim_v_index = slim_t_52493;
slim_v_count = slim_t_52494;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static Slim_type_query_95Work slim_fn_query_95measure_95update(SlimVec * slim_v_previous, SlimVec * slim_v_current, SlimVec * slim_v_dependencies, SlimRegion *slim_region) {
(void)slim_v_previous;
(void)slim_v_current;
(void)slim_v_dependencies;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_query_95Work slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_current)).len;
(void)slim_v_length;
{
SlimVec slim_v_invalidated = {0};
slim_v_invalidated = slim_vec_new(sizeof(bool), slim_allocation_region);
(void)slim_v_invalidated;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_query_95fill_95flags(slim_v_length, INT64_C(0), &slim_v_invalidated, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
SlimVec slim_v_queue = {0};
slim_v_queue = slim_vec_new(sizeof(int64_t), slim_allocation_region);
(void)slim_v_queue;
{
int64_t slim_v_parsed = {0};
slim_v_parsed = slim_fn_query_95classify_95changes(slim_v_previous, slim_v_current, INT64_C(0), INT64_C(0), &slim_v_invalidated, &slim_v_queue, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_parsed;
{
SlimUnit slim_v_propagated = {0};
slim_v_propagated = slim_fn_query_95propagate_95invalidations(slim_v_dependencies, &slim_v_queue, INT64_C(0), &slim_v_invalidated, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_propagated;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_fn_query_95count_95flags(&slim_v_invalidated, INT64_C(0), INT64_C(0), slim_allocation_region);
(void)slim_v_checked;
{
int64_t slim_t_52622 = {0};
slim_t_52622 = slim_v_parsed;
int64_t slim_t_52626 = {0};
slim_t_52626 = slim_v_parsed;
int64_t slim_t_52630 = {0};
slim_t_52630 = slim_v_checked;
int64_t slim_t_52634 = {0};
slim_t_52634 = slim_v_checked;
slim_result = (Slim_type_query_95Work){.slim_field_parsed = slim_t_52622, .slim_field_lowered = slim_t_52626, .slim_field_checked = slim_t_52630, .slim_field_generated = slim_t_52634};
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_scheduler_95collect_95modules(SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_modules, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_modules;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_modules, &slim_v_cursor)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
int64_t slim_t_52786 = {0};
slim_t_52786 = slim_v_next;
slim_v_cursor = slim_t_52786;
goto slim_recur;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_scheduler_95fill_95pending(int64_t slim_v_length, int64_t slim_v_index, SlimVec * slim_v_scheduled, SlimRegion *slim_region) {
(void)slim_v_length;
(void)slim_v_index;
(void)slim_v_scheduled;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_value = {0};
slim_v_value = false;
(void)slim_v_value;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_scheduled, &slim_v_value)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_52869 = {0};
slim_t_52869 = slim_v_length;
int64_t slim_t_52870 = {0};
slim_t_52870 = slim_v_next;
slim_v_length = slim_t_52869;
slim_v_index = slim_t_52870;
goto slim_recur;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_scheduler_95find_95module(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, int64_t slim_v_name, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_name;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_modules)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_module = {0};
slim_v_module = ((int64_t *)((*slim_v_modules)).data)[slim_vec_check_index(&((*slim_v_modules)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_candidate = {0};
slim_v_candidate = slim_i64_add(slim_v_module, INT64_C(2));
(void)slim_v_candidate;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_candidate, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_candidate, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_name_95start, slim_v_name_95end, slim_v_candidate_95start, slim_v_candidate_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_53040 = {0};
slim_t_53040 = slim_v_source;
int64_t slim_t_53043 = {0};
slim_t_53043 = slim_v_name;
int64_t slim_t_53044 = {0};
slim_t_53044 = slim_v_next;
slim_v_source = slim_t_53040;
slim_v_name = slim_t_53043;
slim_v_index = slim_t_53044;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_scheduler_95imports_95ready(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, SlimVec * slim_v_scheduled, int64_t slim_v_cursor, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_scheduled;
(void)slim_v_cursor;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
int64_t slim_v_dependency = {0};
slim_v_dependency = slim_fn_scheduler_95find_95module(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_cursor, INT64_C(0), slim_allocation_region);
(void)slim_v_dependency;
{
bool slim_v_found = {0};
slim_v_found = slim_v_dependency >= INT64_C(0);
(void)slim_v_found;
if (!slim_v_found) {
slim_result = false;
}
else {
{
bool slim_v_ready = {0};
slim_v_ready = ((bool *)((*slim_v_scheduled)).data)[slim_vec_check_index(&((*slim_v_scheduled)), slim_v_dependency)];
(void)slim_v_ready;
if (!slim_v_ready) {
slim_result = false;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_53195 = {0};
slim_t_53195 = slim_v_source;
int64_t slim_t_53199 = {0};
slim_t_53199 = slim_v_next;
slim_v_source = slim_t_53195;
slim_v_cursor = slim_t_53199;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_scheduler_95collect_95ready(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, SlimVec * slim_v_scheduled, int64_t slim_v_index, SlimVec * slim_v_ready, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_scheduled;
(void)slim_v_index;
(void)slim_v_ready;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_modules)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_complete = {0};
slim_v_complete = ((bool *)((*slim_v_scheduled)).data)[slim_vec_check_index(&((*slim_v_scheduled)), slim_v_index)];
(void)slim_v_complete;
{
bool slim_v_pending = {0};
slim_v_pending = !slim_v_complete;
(void)slim_v_pending;
{
int64_t slim_v_module = {0};
slim_v_module = ((int64_t *)((*slim_v_modules)).data)[slim_vec_check_index(&((*slim_v_modules)), slim_v_index)];
(void)slim_v_module;
{
int64_t slim_v_imports = {0};
slim_v_imports = slim_i64_add(slim_v_module, INT64_C(4));
(void)slim_v_imports;
{
int64_t slim_v_first_95import = {0};
slim_v_first_95import = slim_i64_add(slim_v_imports, INT64_C(2));
(void)slim_v_first_95import;
{
bool slim_v_dependencies_95ready = {0};
slim_v_dependencies_95ready = slim_fn_scheduler_95imports_95ready(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_scheduled, slim_v_first_95import, slim_allocation_region);
(void)slim_v_dependencies_95ready;
{
bool slim_v_selectable = {0};
slim_v_selectable = slim_v_pending && slim_v_dependencies_95ready;
(void)slim_v_selectable;
{
SlimUnit slim_v_pushed = {0};
if (slim_v_selectable) {
if (!slim_vec_push(slim_v_ready, &slim_v_index)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
}
else {
slim_v_pushed = (SlimUnit){0};
}
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_53398 = {0};
slim_t_53398 = slim_v_source;
int64_t slim_t_53402 = {0};
slim_t_53402 = slim_v_next;
slim_v_source = slim_t_53398;
slim_v_index = slim_t_53402;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_scheduler_95append_95tasks(SlimVec * slim_v_ready, int64_t slim_v_index, int64_t slim_v_layer, SlimVec * slim_v_tasks, SlimRegion *slim_region) {
(void)slim_v_ready;
(void)slim_v_index;
(void)slim_v_layer;
(void)slim_v_tasks;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_ready)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_module = {0};
slim_v_module = ((int64_t *)((*slim_v_ready)).data)[slim_vec_check_index(&((*slim_v_ready)), slim_v_index)];
(void)slim_v_module;
{
Slim_type_scheduler_95Task slim_v_task = {0};
{
int64_t slim_t_53499 = {0};
slim_t_53499 = slim_v_module;
int64_t slim_t_53503 = {0};
slim_t_53503 = slim_v_layer;
slim_v_task = (Slim_type_scheduler_95Task){.slim_field_module = slim_t_53499, .slim_field_layer = slim_t_53503};
}
(void)slim_v_task;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_tasks, &slim_v_task)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_53531 = {0};
slim_t_53531 = slim_v_next;
int64_t slim_t_53532 = {0};
slim_t_53532 = slim_v_layer;
slim_v_index = slim_t_53531;
slim_v_layer = slim_t_53532;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_scheduler_95append_95batches(int64_t slim_v_start, int64_t slim_v_end, int64_t slim_v_workers, SlimVec * slim_v_batches, SlimRegion *slim_region) {
(void)slim_v_start;
(void)slim_v_end;
(void)slim_v_workers;
(void)slim_v_batches;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_start >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_i64_add(slim_v_start, slim_v_workers);
(void)slim_v_candidate_95end;
{
bool slim_v_exceeds = {0};
slim_v_exceeds = slim_v_candidate_95end > slim_v_end;
(void)slim_v_exceeds;
{
int64_t slim_v_batch_95end = {0};
if (slim_v_exceeds) {
slim_v_batch_95end = slim_v_end;
}
else {
slim_v_batch_95end = slim_v_candidate_95end;
}
(void)slim_v_batch_95end;
{
Slim_type_scheduler_95Batch slim_v_batch = {0};
{
int64_t slim_t_53637 = {0};
slim_t_53637 = slim_v_start;
int64_t slim_t_53641 = {0};
slim_t_53641 = slim_v_batch_95end;
slim_v_batch = (Slim_type_scheduler_95Batch){.slim_field_start = slim_t_53637, .slim_field_end = slim_t_53641};
}
(void)slim_v_batch;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_batches, &slim_v_batch)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
int64_t slim_t_53658 = {0};
slim_t_53658 = slim_v_batch_95end;
int64_t slim_t_53659 = {0};
slim_t_53659 = slim_v_end;
int64_t slim_t_53660 = {0};
slim_t_53660 = slim_v_workers;
slim_v_start = slim_t_53658;
slim_v_end = slim_t_53659;
slim_v_workers = slim_t_53660;
goto slim_recur;
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_scheduler_95mark_95ready(SlimVec * slim_v_ready, int64_t slim_v_index, SlimVec * slim_v_scheduled, SlimRegion *slim_region) {
(void)slim_v_ready;
(void)slim_v_index;
(void)slim_v_scheduled;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_ready)).len;
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
int64_t slim_v_module = {0};
slim_v_module = ((int64_t *)((*slim_v_ready)).data)[slim_vec_check_index(&((*slim_v_ready)), slim_v_index)];
(void)slim_v_module;
{
bool slim_v_value = {0};
slim_v_value = true;
(void)slim_v_value;
{
SlimUnit slim_v_marked = {0};
slim_vec_set(slim_v_scheduled, slim_v_module, &slim_v_value); slim_v_marked = (SlimUnit){0};
(void)slim_v_marked;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_53769 = {0};
slim_t_53769 = slim_v_next;
slim_v_index = slim_t_53769;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_scheduler_95schedule_95layers(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimVec * slim_v_modules, SlimVec * slim_v_scheduled, int64_t slim_v_completed, int64_t slim_v_layer, int64_t slim_v_workers, SlimVec * slim_v_tasks, SlimVec * slim_v_batches, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_modules;
(void)slim_v_scheduled;
(void)slim_v_completed;
(void)slim_v_layer;
(void)slim_v_workers;
(void)slim_v_tasks;
(void)slim_v_batches;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_module_95count = {0};
slim_v_module_95count = ((*slim_v_modules)).len;
(void)slim_v_module_95count;
{
bool slim_v_done = {0};
slim_v_done = slim_v_completed >= slim_v_module_95count;
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
SlimVec slim_v_ready = {0};
slim_v_ready = slim_vec_new(sizeof(int64_t), slim_allocation_region);
(void)slim_v_ready;
{
SlimUnit slim_v_collected = {0};
slim_v_collected = slim_fn_scheduler_95collect_95ready(slim_v_source, slim_v_tokens, slim_v_modules, slim_v_scheduled, INT64_C(0), &slim_v_ready, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_collected;
{
int64_t slim_v_ready_95count = {0};
slim_v_ready_95count = (slim_v_ready).len;
(void)slim_v_ready_95count;
{
bool slim_v_progress = {0};
slim_v_progress = slim_v_ready_95count > INT64_C(0);
(void)slim_v_progress;
if (!slim_v_progress) {
slim_result = false;
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = ((*slim_v_tasks)).len;
(void)slim_v_start;
{
SlimUnit slim_v_appended = {0};
slim_v_appended = slim_fn_scheduler_95append_95tasks(&slim_v_ready, INT64_C(0), slim_v_layer, slim_v_tasks, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_appended;
{
int64_t slim_v_end = {0};
slim_v_end = ((*slim_v_tasks)).len;
(void)slim_v_end;
{
SlimUnit slim_v_batched = {0};
slim_v_batched = slim_fn_scheduler_95append_95batches(slim_v_start, slim_v_end, slim_v_workers, slim_v_batches, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_batched;
{
SlimUnit slim_v_marked = {0};
slim_v_marked = slim_fn_scheduler_95mark_95ready(&slim_v_ready, INT64_C(0), slim_v_scheduled, slim_allocation_region);
(void)slim_v_marked;
{
int64_t slim_v_next_95completed = {0};
slim_v_next_95completed = slim_i64_add(slim_v_completed, slim_v_ready_95count);
(void)slim_v_next_95completed;
{
int64_t slim_v_next_95layer = {0};
slim_v_next_95layer = slim_i64_add(slim_v_layer, INT64_C(1));
(void)slim_v_next_95layer;
SlimBytes slim_t_54004 = {0};
slim_t_54004 = slim_v_source;
int64_t slim_t_54008 = {0};
slim_t_54008 = slim_v_next_95completed;
int64_t slim_t_54009 = {0};
slim_t_54009 = slim_v_next_95layer;
int64_t slim_t_54010 = {0};
slim_t_54010 = slim_v_workers;
slim_v_source = slim_t_54004;
slim_v_completed = slim_t_54008;
slim_v_layer = slim_t_54009;
slim_v_workers = slim_t_54010;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_scheduler_95bounded_95workers(int64_t slim_v_requested, int64_t slim_v_module_95count, SlimRegion *slim_region) {
(void)slim_v_requested;
(void)slim_v_module_95count;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_positive = {0};
slim_v_positive = slim_v_requested > INT64_C(0);
(void)slim_v_positive;
{
int64_t slim_v_at_95least_95one = {0};
if (slim_v_positive) {
slim_v_at_95least_95one = slim_v_requested;
}
else {
slim_v_at_95least_95one = INT64_C(1);
}
(void)slim_v_at_95least_95one;
{
bool slim_v_above_95ceiling = {0};
slim_v_above_95ceiling = slim_v_at_95least_95one > INT64_C(4);
(void)slim_v_above_95ceiling;
{
int64_t slim_v_implementation_95bound = {0};
if (slim_v_above_95ceiling) {
slim_v_implementation_95bound = INT64_C(4);
}
else {
slim_v_implementation_95bound = slim_v_at_95least_95one;
}
(void)slim_v_implementation_95bound;
{
bool slim_v_above_95modules = {0};
slim_v_above_95modules = slim_v_implementation_95bound > slim_v_module_95count;
(void)slim_v_above_95modules;
if (slim_v_above_95modules) {
slim_result = slim_v_module_95count;
}
else {
slim_result = slim_v_implementation_95bound;
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static Slim_type_scheduler_95Schedule slim_fn_scheduler_95make_95schedule(bool slim_v_valid, int64_t slim_v_workers, SlimVec slim_v_tasks, SlimVec slim_v_batches, SlimRegion *slim_region) {
(void)slim_v_valid;
(void)slim_v_workers;
(void)slim_v_tasks;
(void)slim_v_batches;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_scheduler_95Schedule slim_result = {0};
{
bool slim_t_54163 = {0};
slim_t_54163 = slim_v_valid;
int64_t slim_t_54167 = {0};
slim_t_54167 = slim_v_workers;
SlimVec slim_t_54171 = {0};
slim_t_54171 = slim_v_tasks;
SlimVec slim_t_54175 = {0};
slim_t_54175 = slim_v_batches;
slim_result = (Slim_type_scheduler_95Schedule){.slim_field_valid = slim_t_54163, .slim_field_workers = slim_t_54167, .slim_field_tasks = slim_t_54171, .slim_field_batches = slim_t_54175};
}
return slim_result;
}

static Slim_type_scheduler_95Schedule slim_fn_scheduler_95plan(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_requested, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_requested;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_scheduler_95Schedule slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_modules = {0};
slim_v_modules = slim_vec_new(sizeof(int64_t), slim_allocation_region);
(void)slim_v_modules;
{
int64_t slim_v_entry_95form = {0};
slim_v_entry_95form = INT64_C(3);
(void)slim_v_entry_95form;
{
int64_t slim_v_first_95module = {0};
slim_v_first_95module = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_entry_95form, slim_allocation_region);
(void)slim_v_first_95module;
{
SlimUnit slim_v_collected = {0};
slim_v_collected = slim_fn_scheduler_95collect_95modules(slim_v_tokens, slim_v_first_95module, &slim_v_modules, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_collected;
{
int64_t slim_v_module_95count = {0};
slim_v_module_95count = (slim_v_modules).len;
(void)slim_v_module_95count;
{
int64_t slim_v_workers = {0};
slim_v_workers = slim_fn_scheduler_95bounded_95workers(slim_v_requested, slim_v_module_95count, slim_allocation_region);
(void)slim_v_workers;
{
SlimVec slim_v_scheduled = {0};
slim_v_scheduled = slim_vec_new(sizeof(bool), slim_allocation_region);
(void)slim_v_scheduled;
{
SlimUnit slim_v_initialized = {0};
slim_v_initialized = slim_fn_scheduler_95fill_95pending(slim_v_module_95count, INT64_C(0), &slim_v_scheduled, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initialized;
{
SlimVec slim_v_tasks = {0};
slim_v_tasks = slim_vec_new(sizeof(Slim_type_scheduler_95Task), slim_allocation_region);
(void)slim_v_tasks;
{
SlimVec slim_v_batches = {0};
slim_v_batches = slim_vec_new(sizeof(Slim_type_scheduler_95Batch), slim_allocation_region);
(void)slim_v_batches;
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_scheduler_95schedule_95layers(slim_v_source, slim_v_tokens, &slim_v_modules, &slim_v_scheduled, INT64_C(0), INT64_C(0), slim_v_workers, &slim_v_tasks, &slim_v_batches, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_valid;
slim_result = slim_fn_scheduler_95make_95schedule(slim_v_valid, slim_v_workers, slim_v_tasks, slim_v_batches, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_scheduler_95parse_95jobs_95from(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, int64_t slim_v_value, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_v_value;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_value;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_input, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_lower = {0};
slim_v_lower = slim_v_code >= INT64_C(48);
(void)slim_v_lower;
{
bool slim_v_upper = {0};
slim_v_upper = slim_v_code <= INT64_C(57);
(void)slim_v_upper;
{
bool slim_v_digit = {0};
slim_v_digit = slim_v_lower && slim_v_upper;
(void)slim_v_digit;
if (!slim_v_digit) {
slim_result = INT64_C(1);
}
else {
{
bool slim_v_already_95capped = {0};
slim_v_already_95capped = slim_v_value >= INT64_C(4);
(void)slim_v_already_95capped;
{
int64_t slim_v_next_95value = {0};
if (slim_v_already_95capped) {
slim_v_next_95value = INT64_C(4);
}
else {
{
int64_t slim_v_shifted = {0};
slim_v_shifted = slim_i64_mul(slim_v_value, INT64_C(10));
(void)slim_v_shifted;
{
int64_t slim_v_numeric = {0};
slim_v_numeric = slim_i64_sub(slim_v_code, INT64_C(48));
(void)slim_v_numeric;
{
int64_t slim_v_candidate = {0};
slim_v_candidate = slim_i64_add(slim_v_shifted, slim_v_numeric);
(void)slim_v_candidate;
{
bool slim_v_above = {0};
slim_v_above = slim_v_candidate > INT64_C(4);
(void)slim_v_above;
if (slim_v_above) {
slim_v_next_95value = INT64_C(4);
}
else {
slim_v_next_95value = slim_v_candidate;
}
}
}
}
}
}
(void)slim_v_next_95value;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_54541 = {0};
slim_t_54541 = slim_v_input;
int64_t slim_t_54542 = {0};
slim_t_54542 = slim_v_next;
int64_t slim_t_54543 = {0};
slim_t_54543 = slim_v_length;
int64_t slim_t_54544 = {0};
slim_t_54544 = slim_v_next_95value;
slim_v_input = slim_t_54541;
slim_v_index = slim_t_54542;
slim_v_length = slim_t_54543;
slim_v_value = slim_t_54544;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_scheduler_95parse_95jobs(SlimBytes slim_v_input, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_length = {0};
slim_v_length = slim_bytes_len(slim_v_input);
(void)slim_v_length;
slim_result = slim_fn_scheduler_95parse_95jobs_95from(slim_v_input, INT64_C(0), slim_v_length, INT64_C(0), slim_allocation_region);
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static Slim_type_query_95State slim_fn_session_95state_95for_95path(SlimBytes slim_v_path, SlimRegion *slim_region) {
(void)slim_v_path;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_query_95State slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_manifest_95bytes = {0};
slim_v_manifest_95bytes = slim_vec_new(sizeof(uint8_t), slim_allocation_region);
(void)slim_v_manifest_95bytes;
{
bool slim_v_manifest_95read = {0};
slim_v_manifest_95read = slim_read_file(slim_v_path, &slim_v_manifest_95bytes);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_manifest_95read;
{
SlimBytes slim_v_manifest_95source = {0};
slim_v_manifest_95source = slim_bytes_freeze(slim_v_manifest_95bytes);
(void)slim_v_manifest_95source;
{
SlimVec slim_v_manifest_95tokens = {0};
slim_v_manifest_95tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_manifest_95tokens;
{
SlimUnit slim_v_tokenized = {0};
slim_v_tokenized = slim_fn_syntax_95lex(slim_v_manifest_95source, INT64_C(0), &slim_v_manifest_95tokens, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_tokenized;
{
int64_t slim_v_entry_95form = {0};
slim_v_entry_95form = INT64_C(3);
(void)slim_v_entry_95form;
{
int64_t slim_v_modules = {0};
slim_v_modules = slim_fn_syntax_95skip_95form(&slim_v_manifest_95tokens, slim_v_entry_95form, slim_allocation_region);
(void)slim_v_modules;
{
SlimVec slim_v_module_95tokens = {0};
slim_v_module_95tokens = slim_vec_new(sizeof(Slim_type_syntax_95Token), slim_allocation_region);
(void)slim_v_module_95tokens;
{
SlimVec slim_v_loaded = {0};
slim_v_loaded = slim_vec_new(sizeof(Slim_type_project_95LoadedModule), slim_allocation_region);
(void)slim_v_loaded;
{
int64_t slim_v_loaded_95result = {0};
slim_v_loaded_95result = slim_fn_project_95load_95project_95modules(slim_v_path, slim_v_manifest_95source, &slim_v_manifest_95tokens, slim_v_modules, &slim_v_module_95tokens, &slim_v_loaded, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_loaded_95result;
slim_result = slim_fn_query_95build_95state(slim_v_manifest_95source, &slim_v_manifest_95tokens, &slim_v_module_95tokens, &slim_v_loaded, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static int64_t slim_fn_session_95print_95work(Slim_type_query_95Work slim_v_work, SlimRegion *slim_region) {
(void)slim_v_work;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_parsed = {0};
slim_v_parsed = slim_v_work.slim_field_parsed;
(void)slim_v_parsed;
{
int64_t slim_v_lowered = {0};
slim_v_lowered = slim_v_work.slim_field_lowered;
(void)slim_v_lowered;
{
int64_t slim_v_checked = {0};
slim_v_checked = slim_v_work.slim_field_checked;
(void)slim_v_checked;
{
int64_t slim_v_generated = {0};
slim_v_generated = slim_v_work.slim_field_generated;
(void)slim_v_generated;
{
SlimUnit slim_v_first = {0};
slim_v_first = slim_print_i64(slim_v_parsed);
(void)slim_v_first;
{
SlimUnit slim_v_first_95space = {0};
slim_v_first_95space = slim_print_bytes(slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)));
(void)slim_v_first_95space;
{
SlimUnit slim_v_second = {0};
slim_v_second = slim_print_i64(slim_v_lowered);
(void)slim_v_second;
{
SlimUnit slim_v_second_95space = {0};
slim_v_second_95space = slim_print_bytes(slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)));
(void)slim_v_second_95space;
{
SlimUnit slim_v_third = {0};
slim_v_third = slim_print_i64(slim_v_checked);
(void)slim_v_third;
{
SlimUnit slim_v_third_95space = {0};
slim_v_third_95space = slim_print_bytes(slim_bytes_static((const uint8_t *)" ", (int64_t)(sizeof(" ") - 1)));
(void)slim_v_third_95space;
{
SlimUnit slim_v_fourth = {0};
slim_v_fourth = slim_print_i64(slim_v_generated);
(void)slim_v_fourth;
{
SlimUnit slim_v_newline = {0};
slim_v_newline = slim_println(slim_bytes_static((const uint8_t *)"", (int64_t)(sizeof("") - 1)));
(void)slim_v_newline;
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_session_95run(SlimBytes slim_v_initial_95path, SlimBytes slim_v_updated_95path, SlimRegion *slim_region) {
(void)slim_v_initial_95path;
(void)slim_v_updated_95path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
Slim_type_query_95State slim_v_previous_95state = {0};
slim_v_previous_95state = slim_fn_session_95state_95for_95path(slim_v_initial_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_previous_95state;
{
Slim_type_query_95State slim_v_current_95state = {0};
slim_v_current_95state = slim_fn_session_95state_95for_95path(slim_v_updated_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_current_95state;
{
SlimVec slim_v_previous = {0};
slim_v_previous = slim_v_previous_95state.slim_field_snapshots;
(void)slim_v_previous;
{
SlimVec slim_v_current = {0};
slim_v_current = slim_v_current_95state.slim_field_snapshots;
(void)slim_v_current;
{
SlimVec slim_v_dependencies = {0};
slim_v_dependencies = slim_v_current_95state.slim_field_dependencies;
(void)slim_v_dependencies;
{
Slim_type_query_95Work slim_v_work = {0};
slim_v_work = slim_fn_query_95measure_95update(&slim_v_previous, &slim_v_current, &slim_v_dependencies, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_work;
slim_result = slim_fn_session_95print_95work(slim_v_work, slim_allocation_region);
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_session_95run_95recovery(SlimBytes slim_v_initial_95path, SlimBytes slim_v_invalid_95path, SlimBytes slim_v_recovered_95path, SlimRegion *slim_region) {
(void)slim_v_initial_95path;
(void)slim_v_invalid_95path;
(void)slim_v_recovered_95path;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_initial_95errors = {0};
slim_v_initial_95errors = slim_fn_project_95check_95project_95path(slim_v_initial_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_initial_95errors;
{
bool slim_v_initial_95invalid = {0};
slim_v_initial_95invalid = slim_v_initial_95errors > INT64_C(0);
(void)slim_v_initial_95invalid;
if (slim_v_initial_95invalid) {
slim_result = slim_v_initial_95errors;
}
else {
{
Slim_type_query_95State slim_v_previous_95state = {0};
slim_v_previous_95state = slim_fn_session_95state_95for_95path(slim_v_initial_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_previous_95state;
{
int64_t slim_v_rejected = {0};
slim_v_rejected = slim_fn_project_95check_95project_95path(slim_v_invalid_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_rejected;
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_rejected > INT64_C(0);
(void)slim_v_invalid;
if (!slim_v_invalid) {
slim_result = INT64_C(65);
}
else {
{
int64_t slim_v_recovered_95errors = {0};
slim_v_recovered_95errors = slim_fn_project_95check_95project_95path(slim_v_recovered_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_recovered_95errors;
{
bool slim_v_recovered_95invalid = {0};
slim_v_recovered_95invalid = slim_v_recovered_95errors > INT64_C(0);
(void)slim_v_recovered_95invalid;
if (slim_v_recovered_95invalid) {
slim_result = slim_v_recovered_95errors;
}
else {
{
Slim_type_query_95State slim_v_current_95state = {0};
slim_v_current_95state = slim_fn_session_95state_95for_95path(slim_v_recovered_95path, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_current_95state;
{
SlimVec slim_v_previous = {0};
slim_v_previous = slim_v_previous_95state.slim_field_snapshots;
(void)slim_v_previous;
{
SlimVec slim_v_current = {0};
slim_v_current = slim_v_current_95state.slim_field_snapshots;
(void)slim_v_current;
{
SlimVec slim_v_dependencies = {0};
slim_v_dependencies = slim_v_current_95state.slim_field_dependencies;
(void)slim_v_dependencies;
{
Slim_type_query_95Work slim_v_work = {0};
slim_v_work = slim_fn_query_95measure_95update(&slim_v_previous, &slim_v_current, &slim_v_dependencies, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_work;
slim_result = slim_fn_session_95print_95work(slim_v_work, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_syntax_95token_95index_95valid(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_nonnegative = {0};
slim_v_nonnegative = slim_v_index >= INT64_C(0);
(void)slim_v_nonnegative;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_tokens)).len;
(void)slim_v_length;
{
bool slim_v_below = {0};
slim_v_below = slim_v_index < slim_v_length;
(void)slim_v_below;
slim_result = slim_v_nonnegative && slim_v_below;
}
}
}
return slim_result;
}

static int64_t slim_fn_syntax_95token_95kind(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_syntax_95token_95index_95valid(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_valid;
if (!slim_v_valid) {
slim_result = INT64_C(-1);
}
else {
{
Slim_type_syntax_95Token slim_v_token = {0};
slim_v_token = ((Slim_type_syntax_95Token *)((*slim_v_tokens)).data)[slim_vec_check_index(&((*slim_v_tokens)), slim_v_index)];
(void)slim_v_token;
slim_result = slim_v_token.slim_field_kind;
}
}
}
return slim_result;
}

static int64_t slim_fn_syntax_95token_95start(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_syntax_95token_95index_95valid(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_valid;
if (!slim_v_valid) {
slim_result = INT64_C(0);
}
else {
{
Slim_type_syntax_95Token slim_v_token = {0};
slim_v_token = ((Slim_type_syntax_95Token *)((*slim_v_tokens)).data)[slim_vec_check_index(&((*slim_v_tokens)), slim_v_index)];
(void)slim_v_token;
slim_result = slim_v_token.slim_field_start;
}
}
}
return slim_result;
}

static int64_t slim_fn_syntax_95token_95end(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_syntax_95token_95index_95valid(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_valid;
if (!slim_v_valid) {
slim_result = INT64_C(0);
}
else {
{
Slim_type_syntax_95Token slim_v_token = {0};
slim_v_token = ((Slim_type_syntax_95Token *)((*slim_v_tokens)).data)[slim_vec_check_index(&((*slim_v_tokens)), slim_v_index)];
(void)slim_v_token;
slim_result = slim_v_token.slim_field_end;
}
}
}
return slim_result;
}

static bool slim_fn_syntax_95span_95equal_95chars(SlimBytes slim_v_source, int64_t slim_v_start, SlimBytes slim_v_text, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_start;
(void)slim_v_text;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index == slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
int64_t slim_v_source_95index = {0};
slim_v_source_95index = slim_i64_add(slim_v_start, slim_v_index);
(void)slim_v_source_95index;
{
uint8_t slim_v_source_95byte = {0};
slim_v_source_95byte = slim_bytes_get(slim_v_source, slim_v_source_95index);
(void)slim_v_source_95byte;
{
uint8_t slim_v_text_95byte = {0};
slim_v_text_95byte = slim_bytes_get(slim_v_text, slim_v_index);
(void)slim_v_text_95byte;
{
int64_t slim_v_source_95code = {0};
slim_v_source_95code = (int64_t)slim_v_source_95byte;
(void)slim_v_source_95code;
{
int64_t slim_v_text_95code = {0};
slim_v_text_95code = (int64_t)slim_v_text_95byte;
(void)slim_v_text_95code;
{
bool slim_v_same = {0};
slim_v_same = slim_v_source_95code == slim_v_text_95code;
(void)slim_v_same;
if (slim_v_same) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_55546 = {0};
slim_t_55546 = slim_v_source;
int64_t slim_t_55547 = {0};
slim_t_55547 = slim_v_start;
SlimBytes slim_t_55548 = {0};
slim_t_55548 = slim_v_text;
int64_t slim_t_55549 = {0};
slim_t_55549 = slim_v_next;
int64_t slim_t_55550 = {0};
slim_t_55550 = slim_v_length;
slim_v_source = slim_t_55546;
slim_v_start = slim_t_55547;
slim_v_text = slim_t_55548;
slim_v_index = slim_t_55549;
slim_v_length = slim_t_55550;
goto slim_recur;
}
}
else {
slim_result = false;
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_syntax_95span_95equal(SlimBytes slim_v_source, int64_t slim_v_start, int64_t slim_v_end, SlimBytes slim_v_text, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_start;
(void)slim_v_end;
(void)slim_v_text;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_span_95length = {0};
slim_v_span_95length = slim_i64_sub(slim_v_end, slim_v_start);
(void)slim_v_span_95length;
{
int64_t slim_v_text_95length = {0};
slim_v_text_95length = slim_bytes_len(slim_v_text);
(void)slim_v_text_95length;
{
bool slim_v_same_95length = {0};
slim_v_same_95length = slim_v_span_95length == slim_v_text_95length;
(void)slim_v_same_95length;
if (slim_v_same_95length) {
slim_result = slim_fn_syntax_95span_95equal_95chars(slim_v_source, slim_v_start, slim_v_text, INT64_C(0), slim_v_span_95length, slim_allocation_region);
}
else {
slim_result = false;
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_syntax_95spans_95equal_95chars(SlimBytes slim_v_source, int64_t slim_v_left_95start, int64_t slim_v_right_95start, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_left_95start;
(void)slim_v_right_95start;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index == slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
int64_t slim_v_left_95index = {0};
slim_v_left_95index = slim_i64_add(slim_v_left_95start, slim_v_index);
(void)slim_v_left_95index;
{
int64_t slim_v_right_95index = {0};
slim_v_right_95index = slim_i64_add(slim_v_right_95start, slim_v_index);
(void)slim_v_right_95index;
{
uint8_t slim_v_left_95byte = {0};
slim_v_left_95byte = slim_bytes_get(slim_v_source, slim_v_left_95index);
(void)slim_v_left_95byte;
{
uint8_t slim_v_right_95byte = {0};
slim_v_right_95byte = slim_bytes_get(slim_v_source, slim_v_right_95index);
(void)slim_v_right_95byte;
{
int64_t slim_v_left_95code = {0};
slim_v_left_95code = (int64_t)slim_v_left_95byte;
(void)slim_v_left_95code;
{
int64_t slim_v_right_95code = {0};
slim_v_right_95code = (int64_t)slim_v_right_95byte;
(void)slim_v_right_95code;
{
bool slim_v_same = {0};
slim_v_same = slim_v_left_95code == slim_v_right_95code;
(void)slim_v_same;
if (slim_v_same) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_55782 = {0};
slim_t_55782 = slim_v_source;
int64_t slim_t_55783 = {0};
slim_t_55783 = slim_v_left_95start;
int64_t slim_t_55784 = {0};
slim_t_55784 = slim_v_right_95start;
int64_t slim_t_55785 = {0};
slim_t_55785 = slim_v_next;
int64_t slim_t_55786 = {0};
slim_t_55786 = slim_v_length;
slim_v_source = slim_t_55782;
slim_v_left_95start = slim_t_55783;
slim_v_right_95start = slim_t_55784;
slim_v_index = slim_t_55785;
slim_v_length = slim_t_55786;
goto slim_recur;
}
}
else {
slim_result = false;
}
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_syntax_95spans_95equal(SlimBytes slim_v_source, int64_t slim_v_left_95start, int64_t slim_v_left_95end, int64_t slim_v_right_95start, int64_t slim_v_right_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_left_95start;
(void)slim_v_left_95end;
(void)slim_v_right_95start;
(void)slim_v_right_95end;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_left_95length = {0};
slim_v_left_95length = slim_i64_sub(slim_v_left_95end, slim_v_left_95start);
(void)slim_v_left_95length;
{
int64_t slim_v_right_95length = {0};
slim_v_right_95length = slim_i64_sub(slim_v_right_95end, slim_v_right_95start);
(void)slim_v_right_95length;
{
bool slim_v_same_95length = {0};
slim_v_same_95length = slim_v_left_95length == slim_v_right_95length;
(void)slim_v_same_95length;
if (slim_v_same_95length) {
slim_result = slim_fn_syntax_95spans_95equal_95chars(slim_v_source, slim_v_left_95start, slim_v_right_95start, INT64_C(0), slim_v_left_95length, slim_allocation_region);
}
else {
slim_result = false;
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static bool slim_fn_syntax_95is_95delimiter(int64_t slim_v_code, SlimRegion *slim_region) {
(void)slim_v_code;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_whitespace = {0};
slim_v_whitespace = slim_v_code <= INT64_C(32);
(void)slim_v_whitespace;
{
bool slim_v_left = {0};
slim_v_left = slim_v_code == INT64_C(40);
(void)slim_v_left;
{
bool slim_v_right = {0};
slim_v_right = slim_v_code == INT64_C(41);
(void)slim_v_right;
{
bool slim_v_comment = {0};
slim_v_comment = slim_v_code == INT64_C(59);
(void)slim_v_comment;
{
bool slim_v_quote = {0};
slim_v_quote = slim_v_code == INT64_C(34);
(void)slim_v_quote;
{
bool slim_v_any_95paren = {0};
slim_v_any_95paren = slim_v_left || slim_v_right;
(void)slim_v_any_95paren;
{
bool slim_v_any_95control = {0};
slim_v_any_95control = slim_v_whitespace || slim_v_comment;
(void)slim_v_any_95control;
{
bool slim_v_any_95delimiter = {0};
slim_v_any_95delimiter = slim_v_any_95paren || slim_v_any_95control;
(void)slim_v_any_95delimiter;
slim_result = slim_v_any_95delimiter || slim_v_quote;
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_syntax_95scan_95atom(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_index;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_input, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_delimiter = {0};
slim_v_delimiter = slim_fn_syntax_95is_95delimiter(slim_v_code, slim_allocation_region);
(void)slim_v_delimiter;
if (slim_v_delimiter) {
slim_result = slim_v_index;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_56088 = {0};
slim_t_56088 = slim_v_input;
int64_t slim_t_56089 = {0};
slim_t_56089 = slim_v_next;
int64_t slim_t_56090 = {0};
slim_t_56090 = slim_v_length;
slim_v_input = slim_t_56088;
slim_v_index = slim_t_56089;
slim_v_length = slim_t_56090;
goto slim_recur;
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_syntax_95scan_95string(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_index;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_input, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_escape = {0};
slim_v_escape = slim_v_code == INT64_C(92);
(void)slim_v_escape;
if (slim_v_escape) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(2));
(void)slim_v_next;
SlimBytes slim_t_56189 = {0};
slim_t_56189 = slim_v_input;
int64_t slim_t_56190 = {0};
slim_t_56190 = slim_v_next;
int64_t slim_t_56191 = {0};
slim_t_56191 = slim_v_length;
slim_v_input = slim_t_56189;
slim_v_index = slim_t_56190;
slim_v_length = slim_t_56191;
goto slim_recur;
}
}
else {
{
bool slim_v_quote = {0};
slim_v_quote = slim_v_code == INT64_C(34);
(void)slim_v_quote;
if (slim_v_quote) {
slim_result = slim_i64_add(slim_v_index, INT64_C(1));
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_56233 = {0};
slim_t_56233 = slim_v_input;
int64_t slim_t_56234 = {0};
slim_t_56234 = slim_v_next;
int64_t slim_t_56235 = {0};
slim_t_56235 = slim_v_length;
slim_v_input = slim_t_56233;
slim_v_index = slim_t_56234;
slim_v_length = slim_t_56235;
goto slim_recur;
}
}
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_syntax_95scan_95comment(SlimBytes slim_v_input, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_index;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_input, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_newline = {0};
slim_v_newline = slim_v_code == INT64_C(10);
(void)slim_v_newline;
if (slim_v_newline) {
slim_result = slim_i64_add(slim_v_index, INT64_C(1));
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_56346 = {0};
slim_t_56346 = slim_v_input;
int64_t slim_t_56347 = {0};
slim_t_56347 = slim_v_next;
int64_t slim_t_56348 = {0};
slim_t_56348 = slim_v_length;
slim_v_input = slim_t_56346;
slim_v_index = slim_t_56347;
slim_v_length = slim_t_56348;
goto slim_recur;
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static SlimUnit slim_fn_syntax_95push_95token(SlimVec * slim_v_tokens, int64_t slim_v_kind, int64_t slim_v_start, int64_t slim_v_end, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_kind;
(void)slim_v_start;
(void)slim_v_end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
Slim_type_syntax_95Token slim_v_token = {0};
{
int64_t slim_t_56397 = {0};
slim_t_56397 = slim_v_kind;
int64_t slim_t_56401 = {0};
slim_t_56401 = slim_v_start;
int64_t slim_t_56405 = {0};
slim_t_56405 = slim_v_end;
slim_v_token = (Slim_type_syntax_95Token){.slim_field_kind = slim_t_56397, .slim_field_start = slim_t_56401, .slim_field_end = slim_t_56405};
}
(void)slim_v_token;
if (!slim_vec_push(slim_v_tokens, &slim_v_token)) goto slim_allocation_failed; slim_result = (SlimUnit){0};
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_syntax_95lex(SlimBytes slim_v_input, int64_t slim_v_index, SlimVec * slim_v_tokens, SlimRegion *slim_region) {
(void)slim_v_input;
(void)slim_v_index;
(void)slim_v_tokens;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = slim_bytes_len(slim_v_input);
(void)slim_v_length;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_input, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_whitespace = {0};
slim_v_whitespace = slim_v_code <= INT64_C(32);
(void)slim_v_whitespace;
if (slim_v_whitespace) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_56519 = {0};
slim_t_56519 = slim_v_input;
int64_t slim_t_56520 = {0};
slim_t_56520 = slim_v_next;
slim_v_input = slim_t_56519;
slim_v_index = slim_t_56520;
goto slim_recur;
}
}
else {
{
bool slim_v_comment = {0};
slim_v_comment = slim_v_code == INT64_C(59);
(void)slim_v_comment;
if (slim_v_comment) {
{
int64_t slim_v_content_95start = {0};
slim_v_content_95start = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_content_95start;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95scan_95comment(slim_v_input, slim_v_content_95start, slim_v_length, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_56565 = {0};
slim_t_56565 = slim_v_input;
int64_t slim_t_56566 = {0};
slim_t_56566 = slim_v_next;
slim_v_input = slim_t_56565;
slim_v_index = slim_t_56566;
goto slim_recur;
}
}
}
else {
{
bool slim_v_left = {0};
slim_v_left = slim_v_code == INT64_C(40);
(void)slim_v_left;
if (slim_v_left) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
SlimUnit slim_v_pushed = {0};
slim_v_pushed = slim_fn_syntax_95push_95token(slim_v_tokens, INT64_C(0), slim_v_index, slim_v_next, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_pushed;
SlimBytes slim_t_56613 = {0};
slim_t_56613 = slim_v_input;
int64_t slim_t_56614 = {0};
slim_t_56614 = slim_v_next;
slim_v_input = slim_t_56613;
slim_v_index = slim_t_56614;
goto slim_recur;
}
}
}
else {
{
bool slim_v_right = {0};
slim_v_right = slim_v_code == INT64_C(41);
(void)slim_v_right;
if (slim_v_right) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
SlimUnit slim_v_pushed = {0};
slim_v_pushed = slim_fn_syntax_95push_95token(slim_v_tokens, INT64_C(1), slim_v_index, slim_v_next, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_pushed;
SlimBytes slim_t_56661 = {0};
slim_t_56661 = slim_v_input;
int64_t slim_t_56662 = {0};
slim_t_56662 = slim_v_next;
slim_v_input = slim_t_56661;
slim_v_index = slim_t_56662;
goto slim_recur;
}
}
}
else {
{
bool slim_v_quote = {0};
slim_v_quote = slim_v_code == INT64_C(34);
(void)slim_v_quote;
if (slim_v_quote) {
{
int64_t slim_v_content_95start = {0};
slim_v_content_95start = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_content_95start;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95scan_95string(slim_v_input, slim_v_content_95start, slim_v_length, slim_allocation_region);
(void)slim_v_next;
{
SlimUnit slim_v_pushed = {0};
slim_v_pushed = slim_fn_syntax_95push_95token(slim_v_tokens, INT64_C(3), slim_v_index, slim_v_next, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_pushed;
SlimBytes slim_t_56720 = {0};
slim_t_56720 = slim_v_input;
int64_t slim_t_56721 = {0};
slim_t_56721 = slim_v_next;
slim_v_input = slim_t_56720;
slim_v_index = slim_t_56721;
goto slim_recur;
}
}
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95scan_95atom(slim_v_input, slim_v_index, slim_v_length, slim_allocation_region);
(void)slim_v_next;
{
SlimUnit slim_v_pushed = {0};
slim_v_pushed = slim_fn_syntax_95push_95token(slim_v_tokens, INT64_C(2), slim_v_index, slim_v_next, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_pushed;
SlimBytes slim_t_56755 = {0};
slim_t_56755 = slim_v_input;
int64_t slim_t_56756 = {0};
slim_t_56756 = slim_v_next;
slim_v_input = slim_t_56755;
slim_v_index = slim_t_56756;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_syntax_95token_95equal(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimBytes slim_v_text, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_text;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_syntax_95span_95equal(slim_v_source, slim_v_start, slim_v_end, slim_v_text, slim_allocation_region);
}
}
return slim_result;
}

static int64_t slim_fn_syntax_95skip_95nested(SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_depth, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_depth;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_tokens)).len;
(void)slim_v_length;
{
bool slim_v_exhausted = {0};
slim_v_exhausted = slim_v_index >= slim_v_length;
(void)slim_v_exhausted;
if (slim_v_exhausted) {
slim_result = slim_v_length;
}
else {
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_left = {0};
slim_v_left = slim_v_kind == INT64_C(0);
(void)slim_v_left;
{
bool slim_v_right = {0};
slim_v_right = slim_v_kind == INT64_C(1);
(void)slim_v_right;
if (slim_v_left) {
{
int64_t slim_v_next_95depth = {0};
slim_v_next_95depth = slim_i64_add(slim_v_depth, INT64_C(1));
(void)slim_v_next_95depth;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_56956 = {0};
slim_t_56956 = slim_v_next;
int64_t slim_t_56957 = {0};
slim_t_56957 = slim_v_next_95depth;
slim_v_index = slim_t_56956;
slim_v_depth = slim_t_56957;
goto slim_recur;
}
}
}
else {
if (slim_v_right) {
{
int64_t slim_v_next_95depth = {0};
slim_v_next_95depth = slim_i64_sub(slim_v_depth, INT64_C(1));
(void)slim_v_next_95depth;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
{
bool slim_v_done = {0};
slim_v_done = slim_v_next_95depth == INT64_C(0);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_next;
}
else {
int64_t slim_t_57011 = {0};
slim_t_57011 = slim_v_next;
int64_t slim_t_57012 = {0};
slim_t_57012 = slim_v_next_95depth;
slim_v_index = slim_t_57011;
slim_v_depth = slim_t_57012;
goto slim_recur;
}
}
}
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
int64_t slim_t_57035 = {0};
slim_t_57035 = slim_v_next;
int64_t slim_t_57036 = {0};
slim_t_57036 = slim_v_depth;
slim_v_index = slim_t_57035;
slim_v_depth = slim_t_57036;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_syntax_95skip_95form(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_left = {0};
slim_v_left = slim_v_kind == INT64_C(0);
(void)slim_v_left;
if (slim_v_left) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
slim_result = slim_fn_syntax_95skip_95nested(slim_v_tokens, slim_v_next, INT64_C(1), slim_allocation_region);
}
}
else {
slim_result = slim_i64_add(slim_v_index, INT64_C(1));
}
}
}
return slim_result;
}

static bool slim_fn_syntax_95item_95is(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_item, SlimBytes slim_v_text, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_item;
(void)slim_v_text;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_item, INT64_C(1));
(void)slim_v_head;
slim_result = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_v_text, slim_allocation_region);
}
return slim_result;
}

static bool slim_fn_syntax_95binding_95is_95inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_cursor, int64_t slim_v_name_95start, int64_t slim_v_name_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_cursor;
(void)slim_v_name_95start;
(void)slim_v_name_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = false;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_is_95inout = {0};
slim_v_is_95inout = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"inout", (int64_t)(sizeof("inout") - 1)), slim_allocation_region);
(void)slim_v_is_95inout;
if (slim_v_is_95inout) {
{
int64_t slim_v_name_95index = {0};
slim_v_name_95index = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name_95index;
{
int64_t slim_v_candidate_95start = {0};
slim_v_candidate_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name_95index, slim_allocation_region);
(void)slim_v_candidate_95start;
{
int64_t slim_v_candidate_95end = {0};
slim_v_candidate_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name_95index, slim_allocation_region);
(void)slim_v_candidate_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_candidate_95start, slim_v_candidate_95end, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = true;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_57338 = {0};
slim_t_57338 = slim_v_source;
int64_t slim_t_57340 = {0};
slim_t_57340 = slim_v_params;
int64_t slim_t_57341 = {0};
slim_t_57341 = slim_v_next;
int64_t slim_t_57342 = {0};
slim_t_57342 = slim_v_name_95start;
int64_t slim_t_57343 = {0};
slim_t_57343 = slim_v_name_95end;
slim_v_source = slim_t_57338;
slim_v_params = slim_t_57340;
slim_v_cursor = slim_t_57341;
slim_v_name_95start = slim_t_57342;
slim_v_name_95end = slim_t_57343;
goto slim_recur;
}
}
}
}
}
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_57367 = {0};
slim_t_57367 = slim_v_source;
int64_t slim_t_57369 = {0};
slim_t_57369 = slim_v_params;
int64_t slim_t_57370 = {0};
slim_t_57370 = slim_v_next;
int64_t slim_t_57371 = {0};
slim_t_57371 = slim_v_name_95start;
int64_t slim_t_57372 = {0};
slim_t_57372 = slim_v_name_95end;
slim_v_source = slim_t_57367;
slim_v_params = slim_t_57369;
slim_v_cursor = slim_t_57370;
slim_v_name_95start = slim_t_57371;
slim_v_name_95end = slim_t_57372;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_syntax_95name_95is_95inout(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_params, int64_t slim_v_name, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_params;
(void)slim_v_name;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_cursor = {0};
slim_v_cursor = slim_i64_add(slim_v_params, INT64_C(1));
(void)slim_v_cursor;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
slim_result = slim_fn_syntax_95binding_95is_95inout(slim_v_source, slim_v_tokens, slim_v_params, slim_v_cursor, slim_v_name_95start, slim_v_name_95end, slim_allocation_region);
}
}
}
return slim_result;
}

static bool slim_fn_syntax_95first_95byte_95is_95number(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_valid_95token = {0};
slim_v_valid_95token = slim_fn_syntax_95token_95index_95valid(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_valid_95token;
if (!slim_v_valid_95token) {
slim_result = false;
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_length = {0};
slim_v_length = slim_bytes_len(slim_v_source);
(void)slim_v_length;
{
bool slim_v_available = {0};
slim_v_available = slim_v_start < slim_v_length;
(void)slim_v_available;
if (!slim_v_available) {
slim_result = false;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_start);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
{
bool slim_v_minus = {0};
slim_v_minus = slim_v_code == INT64_C(45);
(void)slim_v_minus;
{
bool slim_v_lower = {0};
slim_v_lower = slim_v_code >= INT64_C(48);
(void)slim_v_lower;
{
bool slim_v_upper = {0};
slim_v_upper = slim_v_code <= INT64_C(57);
(void)slim_v_upper;
{
bool slim_v_digit = {0};
slim_v_digit = slim_v_lower && slim_v_upper;
(void)slim_v_digit;
slim_result = slim_v_minus || slim_v_digit;
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_syntax_95find_95function_95params(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_callee_95start, int64_t slim_v_callee_95end, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_callee_95start;
(void)slim_v_callee_95end;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = INT64_C(-1);
}
else {
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_name_95start = {0};
slim_v_name_95start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95start;
{
int64_t slim_v_name_95end = {0};
slim_v_name_95end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95end;
{
bool slim_v_same = {0};
slim_v_same = slim_fn_syntax_95spans_95equal(slim_v_source, slim_v_name_95start, slim_v_name_95end, slim_v_callee_95start, slim_v_callee_95end, slim_allocation_region);
(void)slim_v_same;
if (slim_v_same) {
slim_result = slim_i64_add(slim_v_cursor, INT64_C(3));
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_57753 = {0};
slim_t_57753 = slim_v_source;
int64_t slim_t_57755 = {0};
slim_t_57755 = slim_v_next;
int64_t slim_t_57756 = {0};
slim_t_57756 = slim_v_callee_95start;
int64_t slim_t_57757 = {0};
slim_t_57757 = slim_v_callee_95end;
slim_v_source = slim_t_57753;
slim_v_cursor = slim_t_57755;
slim_v_callee_95start = slim_t_57756;
slim_v_callee_95end = slim_t_57757;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_syntax_95span_95count_95byte(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, int64_t slim_v_code, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_code;
(void)slim_v_count;
(void)slim_region;
SlimRegion slim_function_region;
slim_region_init(&slim_function_region, slim_region);
SlimRegion *slim_allocation_region = &slim_function_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
int64_t slim_v_current = {0};
slim_v_current = (int64_t)slim_v_byte;
(void)slim_v_current;
{
bool slim_v_same = {0};
slim_v_same = slim_v_current == slim_v_code;
(void)slim_v_same;
{
int64_t slim_v_next_95count = {0};
if (slim_v_same) {
slim_v_next_95count = slim_i64_add(slim_v_count, INT64_C(1));
}
else {
slim_v_next_95count = slim_v_count;
}
(void)slim_v_next_95count;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_57882 = {0};
slim_t_57882 = slim_v_source;
int64_t slim_t_57883 = {0};
slim_t_57883 = slim_v_next;
int64_t slim_t_57884 = {0};
slim_t_57884 = slim_v_end;
int64_t slim_t_57885 = {0};
slim_t_57885 = slim_v_code;
int64_t slim_t_57886 = {0};
slim_t_57886 = slim_v_next_95count;
slim_v_source = slim_t_57882;
slim_v_index = slim_t_57883;
slim_v_end = slim_t_57884;
slim_v_code = slim_t_57885;
slim_v_count = slim_t_57886;
goto slim_recur;
}
}
}
}
}
}
}
slim_region_destroy(&slim_function_region);
return slim_result;
}

static int64_t slim_fn_syntax_95scalar_95type_95kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_type_95index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_type_95index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_unit_95type = {0};
slim_v_unit_95type = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_bytes_static((const uint8_t *)"Unit", (int64_t)(sizeof("Unit") - 1)), slim_allocation_region);
(void)slim_v_unit_95type;
if (slim_v_unit_95type) {
slim_result = INT64_C(0);
}
else {
{
bool slim_v_bool_95type = {0};
slim_v_bool_95type = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_bytes_static((const uint8_t *)"Bool", (int64_t)(sizeof("Bool") - 1)), slim_allocation_region);
(void)slim_v_bool_95type;
if (slim_v_bool_95type) {
slim_result = INT64_C(1);
}
else {
{
bool slim_v_u8_95type = {0};
slim_v_u8_95type = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_bytes_static((const uint8_t *)"U8", (int64_t)(sizeof("U8") - 1)), slim_allocation_region);
(void)slim_v_u8_95type;
if (slim_v_u8_95type) {
slim_result = INT64_C(2);
}
else {
{
bool slim_v_i64_95type = {0};
slim_v_i64_95type = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_bytes_static((const uint8_t *)"I64", (int64_t)(sizeof("I64") - 1)), slim_allocation_region);
(void)slim_v_i64_95type;
if (slim_v_i64_95type) {
slim_result = INT64_C(3);
}
else {
{
bool slim_v_bytes_95type = {0};
slim_v_bytes_95type = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_type_95index, slim_bytes_static((const uint8_t *)"Bytes", (int64_t)(sizeof("Bytes") - 1)), slim_allocation_region);
(void)slim_v_bytes_95type;
if (slim_v_bytes_95type) {
slim_result = INT64_C(4);
}
else {
slim_result = INT64_C(-1);
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static Slim_type_ir_95DeclarationKind slim_fn_syntax_95declaration_95kind(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_form, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_form;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
Slim_type_ir_95DeclarationKind slim_result = {0};
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_form, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_function = {0};
slim_v_function = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function;
if (slim_v_function) {
{
Slim_type_ir_95DeclarationKind slim_v_kind = {0};
{
slim_v_kind = (Slim_type_ir_95DeclarationKind){.tag = SLIM_TAG_ir_95DeclarationKind_Function};
}
(void)slim_v_kind;
slim_result = slim_v_kind;
}
}
else {
{
bool slim_v_record = {0};
slim_v_record = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record;
if (slim_v_record) {
{
Slim_type_ir_95DeclarationKind slim_v_kind = {0};
{
slim_v_kind = (Slim_type_ir_95DeclarationKind){.tag = SLIM_TAG_ir_95DeclarationKind_Record};
}
(void)slim_v_kind;
slim_result = slim_v_kind;
}
}
else {
{
Slim_type_ir_95DeclarationKind slim_v_kind = {0};
{
slim_v_kind = (Slim_type_ir_95DeclarationKind){.tag = SLIM_TAG_ir_95DeclarationKind_Variant};
}
(void)slim_v_kind;
slim_result = slim_v_kind;
}
}
}
}
}
}
return slim_result;
}

static SlimUnit slim_fn_syntax_95index_95declarations_95from(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, SlimVec * slim_v_declarations, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_declarations;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
int64_t slim_v_token = {0};
slim_v_token = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_token;
{
bool slim_v_done = {0};
slim_v_done = slim_v_token == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
bool slim_v_form = {0};
slim_v_form = slim_v_token == INT64_C(0);
(void)slim_v_form;
if (!slim_v_form) {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_cursor, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_58253 = {0};
slim_t_58253 = slim_v_source;
int64_t slim_t_58255 = {0};
slim_t_58255 = slim_v_next;
slim_v_source = slim_t_58253;
slim_v_cursor = slim_t_58255;
goto slim_recur;
}
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
Slim_type_ir_95DeclarationKind slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95declaration_95kind(slim_v_source, slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
Slim_type_ir_95Declaration slim_v_declaration = {0};
{
Slim_type_ir_95DeclarationKind slim_t_58300 = {0};
slim_t_58300 = slim_v_kind;
int64_t slim_t_58304 = {0};
slim_t_58304 = slim_v_cursor;
int64_t slim_t_58308 = {0};
slim_t_58308 = slim_v_name;
int64_t slim_t_58312 = {0};
slim_t_58312 = slim_v_next;
slim_v_declaration = (Slim_type_ir_95Declaration){.slim_field_kind = slim_t_58300, .slim_field_form = slim_t_58304, .slim_field_name = slim_t_58308, .slim_field_next = slim_t_58312};
}
(void)slim_v_declaration;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_declarations, &slim_v_declaration)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
SlimBytes slim_t_58329 = {0};
slim_t_58329 = slim_v_source;
int64_t slim_t_58331 = {0};
slim_t_58331 = slim_v_next;
slim_v_source = slim_t_58329;
slim_v_cursor = slim_t_58331;
goto slim_recur;
}
}
}
}
}
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimVec slim_fn_syntax_95index_95declarations(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimVec slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
SlimVec slim_v_declarations = {0};
slim_v_declarations = slim_vec_new(sizeof(Slim_type_ir_95Declaration), slim_allocation_region);
(void)slim_v_declarations;
{
SlimUnit slim_v_indexed = {0};
slim_v_indexed = slim_fn_syntax_95index_95declarations_95from(slim_v_source, slim_v_tokens, INT64_C(3), &slim_v_declarations, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_indexed;
slim_result = slim_v_declarations;
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_text_95append_95span(SlimBytes slim_v_source, int64_t slim_v_index, int64_t slim_v_end, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_index;
(void)slim_v_end;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_end;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_index);
(void)slim_v_byte;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_output, &slim_v_byte)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_58482 = {0};
slim_t_58482 = slim_v_source;
int64_t slim_t_58483 = {0};
slim_t_58483 = slim_v_next;
int64_t slim_t_58484 = {0};
slim_t_58484 = slim_v_end;
slim_v_source = slim_t_58482;
slim_v_index = slim_t_58483;
slim_v_end = slim_t_58484;
goto slim_recur;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_text_95append_95text_95chars(SlimBytes slim_v_text, int64_t slim_v_index, int64_t slim_v_length, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_text;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = (SlimUnit){0};
}
else {
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_text, slim_v_index);
(void)slim_v_byte;
{
SlimUnit slim_v_pushed = {0};
if (!slim_vec_push(slim_v_output, &slim_v_byte)) goto slim_allocation_failed; slim_v_pushed = (SlimUnit){0};
(void)slim_v_pushed;
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_58576 = {0};
slim_t_58576 = slim_v_text;
int64_t slim_t_58577 = {0};
slim_t_58577 = slim_v_next;
int64_t slim_t_58578 = {0};
slim_t_58578 = slim_v_length;
slim_v_text = slim_t_58576;
slim_v_index = slim_t_58577;
slim_v_length = slim_t_58578;
goto slim_recur;
}
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_text_95append_95text(SlimVec * slim_v_output, SlimBytes slim_v_text, SlimRegion *slim_region) {
(void)slim_v_output;
(void)slim_v_text;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_length = {0};
slim_v_length = slim_bytes_len(slim_v_text);
(void)slim_v_length;
slim_result = slim_fn_text_95append_95text_95chars(slim_v_text, INT64_C(0), slim_v_length, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_text_95append_95nonnegative_95i64(int64_t slim_v_value, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_value;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
bool slim_v_multiple = {0};
slim_v_multiple = slim_v_value >= INT64_C(10);
(void)slim_v_multiple;
if (slim_v_multiple) {
{
int64_t slim_v_prefix = {0};
slim_v_prefix = slim_i64_div(slim_v_value, INT64_C(10));
(void)slim_v_prefix;
{
int64_t slim_v_remainder = {0};
slim_v_remainder = slim_i64_rem(slim_v_value, INT64_C(10));
(void)slim_v_remainder;
{
SlimUnit slim_v_emitted = {0};
slim_v_emitted = slim_fn_text_95append_95nonnegative_95i64(slim_v_prefix, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
(void)slim_v_emitted;
{
int64_t slim_v_code = {0};
slim_v_code = slim_i64_add(slim_v_remainder, INT64_C(48));
(void)slim_v_code;
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_i64_to_u8(slim_v_code);
(void)slim_v_byte;
if (!slim_vec_push(slim_v_output, &slim_v_byte)) goto slim_allocation_failed; slim_result = (SlimUnit){0};
}
}
}
}
}
}
else {
{
int64_t slim_v_code = {0};
slim_v_code = slim_i64_add(slim_v_value, INT64_C(48));
(void)slim_v_code;
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_i64_to_u8(slim_v_code);
(void)slim_v_byte;
if (!slim_vec_push(slim_v_output, &slim_v_byte)) goto slim_allocation_failed; slim_result = (SlimUnit){0};
}
}
}
}
slim_allocation_failed: ;
return slim_result;
}

static SlimUnit slim_fn_text_95append_95token(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimVec * slim_v_output, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_output;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
SlimUnit slim_result = {0};
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_end;
slim_result = slim_fn_text_95append_95span(slim_v_source, slim_v_start, slim_v_end, slim_v_output, slim_allocation_region);
if (slim_region_failed(slim_allocation_region)) goto slim_allocation_failed;
}
}
slim_allocation_failed: ;
return slim_result;
}

static bool slim_fn_validate_95atom_95at(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
slim_result = slim_v_kind == INT64_C(2);
}
return slim_result;
}

static bool slim_fn_validate_95string_95token_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_string_95value = {0};
slim_v_string_95value = slim_v_kind == INT64_C(3);
(void)slim_v_string_95value;
if (!slim_v_string_95value) {
slim_result = true;
}
else {
{
int64_t slim_v_start = {0};
slim_v_start = slim_fn_syntax_95token_95start(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_start;
{
int64_t slim_v_end = {0};
slim_v_end = slim_fn_syntax_95token_95end(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_end;
{
int64_t slim_v_source_95length = {0};
slim_v_source_95length = slim_bytes_len(slim_v_source);
(void)slim_v_source_95length;
{
bool slim_v_nonempty = {0};
slim_v_nonempty = slim_v_end > slim_v_start;
(void)slim_v_nonempty;
{
bool slim_v_in_95bounds = {0};
slim_v_in_95bounds = slim_v_end <= slim_v_source_95length;
(void)slim_v_in_95bounds;
{
bool slim_v_can_95read = {0};
slim_v_can_95read = slim_v_nonempty && slim_v_in_95bounds;
(void)slim_v_can_95read;
if (!slim_v_can_95read) {
slim_result = false;
}
else {
{
int64_t slim_v_last = {0};
slim_v_last = slim_i64_sub(slim_v_end, INT64_C(1));
(void)slim_v_last;
{
uint8_t slim_v_byte = {0};
slim_v_byte = slim_bytes_get(slim_v_source, slim_v_last);
(void)slim_v_byte;
{
int64_t slim_v_code = {0};
slim_v_code = (int64_t)slim_v_byte;
(void)slim_v_code;
slim_result = slim_v_code == INT64_C(34);
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_validate_95string_95tokens_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, int64_t slim_v_length, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_v_length;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_index >= slim_v_length;
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_validate_95string_95token_95valid(slim_v_source, slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_valid;
if (!slim_v_valid) {
slim_result = false;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_next;
SlimBytes slim_t_59123 = {0};
slim_t_59123 = slim_v_source;
int64_t slim_t_59125 = {0};
slim_t_59125 = slim_v_next;
int64_t slim_t_59126 = {0};
slim_t_59126 = slim_v_length;
slim_v_source = slim_t_59123;
slim_v_index = slim_t_59125;
slim_v_length = slim_t_59126;
goto slim_recur;
}
}
}
}
}
return slim_result;
}

static bool slim_fn_validate_95form_95count_95at_95least(SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_count, int64_t slim_v_minimum, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_count;
(void)slim_v_minimum;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_done = {0};
slim_v_done = slim_v_kind == INT64_C(1);
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count >= slim_v_minimum;
}
else {
{
bool slim_v_invalid = {0};
slim_v_invalid = slim_v_kind < INT64_C(0);
(void)slim_v_invalid;
if (slim_v_invalid) {
slim_result = false;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
bool slim_v_progressed = {0};
slim_v_progressed = slim_v_next > slim_v_cursor;
(void)slim_v_progressed;
if (!slim_v_progressed) {
slim_result = false;
}
else {
{
int64_t slim_v_next_95count = {0};
slim_v_next_95count = slim_i64_add(slim_v_count, INT64_C(1));
(void)slim_v_next_95count;
int64_t slim_t_59261 = {0};
slim_t_59261 = slim_v_next;
int64_t slim_t_59262 = {0};
slim_t_59262 = slim_v_next_95count;
int64_t slim_t_59263 = {0};
slim_t_59263 = slim_v_minimum;
slim_v_cursor = slim_t_59261;
slim_v_count = slim_t_59262;
slim_v_minimum = slim_t_59263;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_validate_95expression_95head_95known(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_head, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_head;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_let_95form = {0};
slim_v_let_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
(void)slim_v_let_95form;
{
bool slim_v_call_95form = {0};
slim_v_call_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
(void)slim_v_call_95form;
{
bool slim_v_make_95form = {0};
slim_v_make_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"make", (int64_t)(sizeof("make") - 1)), slim_allocation_region);
(void)slim_v_make_95form;
{
bool slim_v_get_95form = {0};
slim_v_get_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"get", (int64_t)(sizeof("get") - 1)), slim_allocation_region);
(void)slim_v_get_95form;
{
bool slim_v_case_95form = {0};
slim_v_case_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"case", (int64_t)(sizeof("case") - 1)), slim_allocation_region);
(void)slim_v_case_95form;
{
bool slim_v_match_95form = {0};
slim_v_match_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"match", (int64_t)(sizeof("match") - 1)), slim_allocation_region);
(void)slim_v_match_95form;
{
bool slim_v_recur_95form = {0};
slim_v_recur_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"recur", (int64_t)(sizeof("recur") - 1)), slim_allocation_region);
(void)slim_v_recur_95form;
{
bool slim_v_set_95form = {0};
slim_v_set_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"set", (int64_t)(sizeof("set") - 1)), slim_allocation_region);
(void)slim_v_set_95form;
{
bool slim_v_first = {0};
slim_v_first = slim_v_let_95form || slim_v_call_95form;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_make_95form || slim_v_get_95form;
(void)slim_v_second;
{
bool slim_v_third = {0};
slim_v_third = slim_v_case_95form || slim_v_match_95form;
(void)slim_v_third;
{
bool slim_v_fourth = {0};
slim_v_fourth = slim_v_recur_95form || slim_v_set_95form;
(void)slim_v_fourth;
{
bool slim_v_left = {0};
slim_v_left = slim_v_first || slim_v_second;
(void)slim_v_left;
{
bool slim_v_right = {0};
slim_v_right = slim_v_third || slim_v_fourth;
(void)slim_v_right;
slim_result = slim_v_left || slim_v_right;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_validate_95expression_95minimum(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_head, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_head;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
{
bool slim_v_let_95form = {0};
slim_v_let_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"let", (int64_t)(sizeof("let") - 1)), slim_allocation_region);
(void)slim_v_let_95form;
if (slim_v_let_95form) {
slim_result = INT64_C(4);
}
else {
{
bool slim_v_call_95form = {0};
slim_v_call_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"call", (int64_t)(sizeof("call") - 1)), slim_allocation_region);
(void)slim_v_call_95form;
if (slim_v_call_95form) {
slim_result = INT64_C(1);
}
else {
{
bool slim_v_make_95form = {0};
slim_v_make_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"make", (int64_t)(sizeof("make") - 1)), slim_allocation_region);
(void)slim_v_make_95form;
if (slim_v_make_95form) {
slim_result = INT64_C(1);
}
else {
{
bool slim_v_get_95form = {0};
slim_v_get_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"get", (int64_t)(sizeof("get") - 1)), slim_allocation_region);
(void)slim_v_get_95form;
if (slim_v_get_95form) {
slim_result = INT64_C(2);
}
else {
{
bool slim_v_case_95form = {0};
slim_v_case_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"case", (int64_t)(sizeof("case") - 1)), slim_allocation_region);
(void)slim_v_case_95form;
if (slim_v_case_95form) {
slim_result = INT64_C(2);
}
else {
{
bool slim_v_match_95form = {0};
slim_v_match_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"match", (int64_t)(sizeof("match") - 1)), slim_allocation_region);
(void)slim_v_match_95form;
if (slim_v_match_95form) {
slim_result = INT64_C(2);
}
else {
{
bool slim_v_set_95form = {0};
slim_v_set_95form = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"set", (int64_t)(sizeof("set") - 1)), slim_allocation_region);
(void)slim_v_set_95form;
if (slim_v_set_95form) {
slim_result = INT64_C(3);
}
else {
slim_result = INT64_C(0);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_validate_95expression_95shape_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_scalar = {0};
slim_v_scalar = slim_v_kind >= INT64_C(2);
(void)slim_v_scalar;
if (slim_v_scalar) {
slim_result = true;
}
else {
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
if (!slim_v_form) {
slim_result = false;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_head_95atom = {0};
slim_v_head_95atom = slim_fn_validate_95atom_95at(slim_v_tokens, slim_v_head, slim_allocation_region);
(void)slim_v_head_95atom;
if (!slim_v_head_95atom) {
slim_result = false;
}
else {
{
bool slim_v_known = {0};
slim_v_known = slim_fn_validate_95expression_95head_95known(slim_v_source, slim_v_tokens, slim_v_head, slim_allocation_region);
(void)slim_v_known;
{
int64_t slim_v_minimum = {0};
slim_v_minimum = slim_fn_validate_95expression_95minimum(slim_v_source, slim_v_tokens, slim_v_head, slim_allocation_region);
(void)slim_v_minimum;
{
int64_t slim_v_first_95child = {0};
slim_v_first_95child = slim_i64_add(slim_v_head, INT64_C(1));
(void)slim_v_first_95child;
{
bool slim_v_enough = {0};
slim_v_enough = slim_fn_validate_95form_95count_95at_95least(slim_v_tokens, slim_v_first_95child, INT64_C(0), slim_v_minimum, slim_allocation_region);
(void)slim_v_enough;
if (slim_v_known) {
slim_result = slim_v_enough;
}
else {
slim_result = slim_v_enough;
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_validate_95function_95item_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_index, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_params = {0};
slim_v_params = slim_i64_add(slim_v_index, INT64_C(3));
(void)slim_v_params;
{
bool slim_v_name_95valid = {0};
slim_v_name_95valid = slim_fn_validate_95atom_95at(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95valid;
{
int64_t slim_v_params_95kind = {0};
slim_v_params_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_params_95kind;
{
bool slim_v_params_95valid = {0};
slim_v_params_95valid = slim_v_params_95kind == INT64_C(0);
(void)slim_v_params_95valid;
{
int64_t slim_v_return_95type = {0};
slim_v_return_95type = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_params, slim_allocation_region);
(void)slim_v_return_95type;
{
int64_t slim_v_return_95kind = {0};
slim_v_return_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_return_95kind;
{
bool slim_v_return_95valid = {0};
slim_v_return_95valid = slim_v_return_95kind >= INT64_C(0);
(void)slim_v_return_95valid;
{
int64_t slim_v_effects = {0};
slim_v_effects = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_return_95type, slim_allocation_region);
(void)slim_v_effects;
{
int64_t slim_v_effects_95kind = {0};
slim_v_effects_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_effects_95kind;
{
bool slim_v_effects_95form = {0};
slim_v_effects_95form = slim_v_effects_95kind == INT64_C(0);
(void)slim_v_effects_95form;
{
int64_t slim_v_effects_95head = {0};
slim_v_effects_95head = slim_i64_add(slim_v_effects, INT64_C(1));
(void)slim_v_effects_95head;
{
bool slim_v_effects_95valid = {0};
if (slim_v_effects_95form) {
slim_v_effects_95valid = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_effects_95head, slim_bytes_static((const uint8_t *)"effects", (int64_t)(sizeof("effects") - 1)), slim_allocation_region);
}
else {
slim_v_effects_95valid = false;
}
(void)slim_v_effects_95valid;
{
int64_t slim_v_body = {0};
slim_v_body = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_effects, slim_allocation_region);
(void)slim_v_body;
{
bool slim_v_body_95valid = {0};
slim_v_body_95valid = slim_fn_validate_95expression_95shape_95valid(slim_v_source, slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95valid;
{
int64_t slim_v_body_95end = {0};
slim_v_body_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95end;
{
int64_t slim_v_item_95end = {0};
slim_v_item_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_item_95end;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_item_95end, INT64_C(1));
(void)slim_v_closing;
{
bool slim_v_exact = {0};
slim_v_exact = slim_v_body_95end == slim_v_closing;
(void)slim_v_exact;
{
bool slim_v_first = {0};
slim_v_first = slim_v_name_95valid && slim_v_params_95valid;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_return_95valid && slim_v_effects_95valid;
(void)slim_v_second;
{
bool slim_v_third = {0};
slim_v_third = slim_v_body_95valid && slim_v_exact;
(void)slim_v_third;
{
bool slim_v_left = {0};
slim_v_left = slim_v_first && slim_v_second;
(void)slim_v_left;
slim_result = slim_v_left && slim_v_third;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_validate_95data_95item_95valid(SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_index, INT64_C(2));
(void)slim_v_name;
{
int64_t slim_v_body = {0};
slim_v_body = slim_i64_add(slim_v_index, INT64_C(3));
(void)slim_v_body;
{
bool slim_v_name_95valid = {0};
slim_v_name_95valid = slim_fn_validate_95atom_95at(slim_v_tokens, slim_v_name, slim_allocation_region);
(void)slim_v_name_95valid;
{
int64_t slim_v_body_95kind = {0};
slim_v_body_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95kind;
{
bool slim_v_body_95valid = {0};
slim_v_body_95valid = slim_v_body_95kind == INT64_C(0);
(void)slim_v_body_95valid;
{
int64_t slim_v_body_95end = {0};
slim_v_body_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_body, slim_allocation_region);
(void)slim_v_body_95end;
{
int64_t slim_v_item_95end = {0};
slim_v_item_95end = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_item_95end;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_item_95end, INT64_C(1));
(void)slim_v_closing;
{
bool slim_v_exact = {0};
slim_v_exact = slim_v_body_95end == slim_v_closing;
(void)slim_v_exact;
{
bool slim_v_left = {0};
slim_v_left = slim_v_name_95valid && slim_v_body_95valid;
(void)slim_v_left;
slim_result = slim_v_left && slim_v_exact;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_validate_95item_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_index, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_index;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_kind = {0};
slim_v_kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_index, slim_allocation_region);
(void)slim_v_kind;
{
bool slim_v_form = {0};
slim_v_form = slim_v_kind == INT64_C(0);
(void)slim_v_form;
if (!slim_v_form) {
slim_result = false;
}
else {
{
int64_t slim_v_head = {0};
slim_v_head = slim_i64_add(slim_v_index, INT64_C(1));
(void)slim_v_head;
{
bool slim_v_function = {0};
slim_v_function = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function;
if (slim_v_function) {
slim_result = slim_fn_validate_95function_95item_95valid(slim_v_source, slim_v_tokens, slim_v_index, slim_allocation_region);
}
else {
{
bool slim_v_record = {0};
slim_v_record = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"record", (int64_t)(sizeof("record") - 1)), slim_allocation_region);
(void)slim_v_record;
{
bool slim_v_variant = {0};
slim_v_variant = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_head, slim_bytes_static((const uint8_t *)"variant", (int64_t)(sizeof("variant") - 1)), slim_allocation_region);
(void)slim_v_variant;
{
bool slim_v_data = {0};
slim_v_data = slim_v_record || slim_v_variant;
(void)slim_v_data;
if (slim_v_data) {
slim_result = slim_fn_validate_95data_95item_95valid(slim_v_tokens, slim_v_index, slim_allocation_region);
}
else {
slim_result = false;
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_validate_95item_95list_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_closing, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_closing;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor == slim_v_closing;
(void)slim_v_done;
if (slim_v_done) {
slim_result = true;
}
else {
{
bool slim_v_below = {0};
slim_v_below = slim_v_cursor < slim_v_closing;
(void)slim_v_below;
if (!slim_v_below) {
slim_result = false;
}
else {
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_validate_95item_95valid(slim_v_source, slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_valid;
if (!slim_v_valid) {
slim_result = false;
}
else {
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
{
bool slim_v_progressed = {0};
slim_v_progressed = slim_v_next > slim_v_cursor;
(void)slim_v_progressed;
if (!slim_v_progressed) {
slim_result = false;
}
else {
SlimBytes slim_t_60565 = {0};
slim_t_60565 = slim_v_source;
int64_t slim_t_60567 = {0};
slim_t_60567 = slim_v_next;
int64_t slim_t_60568 = {0};
slim_t_60568 = slim_v_closing;
slim_v_source = slim_t_60565;
slim_v_cursor = slim_t_60567;
slim_v_closing = slim_t_60568;
goto slim_recur;
}
}
}
}
}
}
}
}
}
return slim_result;
}

static int64_t slim_fn_validate_95main_95count(SlimBytes slim_v_source, SlimVec * slim_v_tokens, int64_t slim_v_cursor, int64_t slim_v_closing, int64_t slim_v_count, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_v_cursor;
(void)slim_v_closing;
(void)slim_v_count;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
int64_t slim_result = {0};
slim_recur: ;
{
bool slim_v_done = {0};
slim_v_done = slim_v_cursor == slim_v_closing;
(void)slim_v_done;
if (slim_v_done) {
slim_result = slim_v_count;
}
else {
{
bool slim_v_function = {0};
slim_v_function = slim_fn_syntax_95item_95is(slim_v_source, slim_v_tokens, slim_v_cursor, slim_bytes_static((const uint8_t *)"fn", (int64_t)(sizeof("fn") - 1)), slim_allocation_region);
(void)slim_v_function;
{
int64_t slim_v_name = {0};
slim_v_name = slim_i64_add(slim_v_cursor, INT64_C(2));
(void)slim_v_name;
{
bool slim_v_named_95main = {0};
if (slim_v_function) {
slim_v_named_95main = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, slim_v_name, slim_bytes_static((const uint8_t *)"main", (int64_t)(sizeof("main") - 1)), slim_allocation_region);
}
else {
slim_v_named_95main = false;
}
(void)slim_v_named_95main;
{
int64_t slim_v_next_95count = {0};
if (slim_v_named_95main) {
slim_v_next_95count = slim_i64_add(slim_v_count, INT64_C(1));
}
else {
slim_v_next_95count = slim_v_count;
}
(void)slim_v_next_95count;
{
int64_t slim_v_next = {0};
slim_v_next = slim_fn_syntax_95skip_95form(slim_v_tokens, slim_v_cursor, slim_allocation_region);
(void)slim_v_next;
SlimBytes slim_t_60715 = {0};
slim_t_60715 = slim_v_source;
int64_t slim_t_60717 = {0};
slim_t_60717 = slim_v_next;
int64_t slim_t_60718 = {0};
slim_t_60718 = slim_v_closing;
int64_t slim_t_60719 = {0};
slim_t_60719 = slim_v_next_95count;
slim_v_source = slim_t_60715;
slim_v_cursor = slim_t_60717;
slim_v_closing = slim_t_60718;
slim_v_count = slim_t_60719;
goto slim_recur;
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_validate_95module_95shape_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_tokens)).len;
(void)slim_v_length;
{
bool slim_v_bounded = {0};
slim_v_bounded = slim_v_length <= INT64_C(1000000);
(void)slim_v_bounded;
{
bool slim_v_enough = {0};
slim_v_enough = slim_v_length >= INT64_C(4);
(void)slim_v_enough;
{
bool slim_v_sized = {0};
slim_v_sized = slim_v_bounded && slim_v_enough;
(void)slim_v_sized;
if (!slim_v_sized) {
slim_result = false;
}
else {
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_length, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_root_95kind = {0};
slim_v_root_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, INT64_C(0), slim_allocation_region);
(void)slim_v_root_95kind;
{
bool slim_v_root_95form = {0};
slim_v_root_95form = slim_v_root_95kind == INT64_C(0);
(void)slim_v_root_95form;
{
bool slim_v_module_95head = {0};
slim_v_module_95head = slim_fn_syntax_95token_95equal(slim_v_source, slim_v_tokens, INT64_C(1), slim_bytes_static((const uint8_t *)"module", (int64_t)(sizeof("module") - 1)), slim_allocation_region);
(void)slim_v_module_95head;
{
bool slim_v_module_95name = {0};
slim_v_module_95name = slim_fn_validate_95atom_95at(slim_v_tokens, INT64_C(2), slim_allocation_region);
(void)slim_v_module_95name;
{
int64_t slim_v_close_95kind = {0};
slim_v_close_95kind = slim_fn_syntax_95token_95kind(slim_v_tokens, slim_v_closing, slim_allocation_region);
(void)slim_v_close_95kind;
{
bool slim_v_close_95valid = {0};
slim_v_close_95valid = slim_v_close_95kind == INT64_C(1);
(void)slim_v_close_95valid;
{
bool slim_v_first = {0};
slim_v_first = slim_v_root_95form && slim_v_module_95head;
(void)slim_v_first;
{
bool slim_v_second = {0};
slim_v_second = slim_v_module_95name && slim_v_close_95valid;
(void)slim_v_second;
{
bool slim_v_header = {0};
slim_v_header = slim_v_first && slim_v_second;
(void)slim_v_header;
if (!slim_v_header) {
slim_result = false;
}
else {
{
bool slim_v_strings = {0};
slim_v_strings = slim_fn_validate_95string_95tokens_95valid(slim_v_source, slim_v_tokens, INT64_C(0), slim_v_length, slim_allocation_region);
(void)slim_v_strings;
if (!slim_v_strings) {
slim_result = false;
}
else {
slim_result = slim_fn_validate_95item_95list_95valid(slim_v_source, slim_v_tokens, INT64_C(3), slim_v_closing, slim_allocation_region);
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
return slim_result;
}

static bool slim_fn_validate_95executable_95shape_95valid(SlimBytes slim_v_source, SlimVec * slim_v_tokens, SlimRegion *slim_region) {
(void)slim_v_source;
(void)slim_v_tokens;
(void)slim_region;
SlimRegion *slim_allocation_region = slim_region;
(void)slim_allocation_region;
bool slim_result = {0};
{
bool slim_v_valid = {0};
slim_v_valid = slim_fn_validate_95module_95shape_95valid(slim_v_source, slim_v_tokens, slim_allocation_region);
(void)slim_v_valid;
if (!slim_v_valid) {
slim_result = false;
}
else {
{
int64_t slim_v_length = {0};
slim_v_length = ((*slim_v_tokens)).len;
(void)slim_v_length;
{
int64_t slim_v_closing = {0};
slim_v_closing = slim_i64_sub(slim_v_length, INT64_C(1));
(void)slim_v_closing;
{
int64_t slim_v_mains = {0};
slim_v_mains = slim_fn_validate_95main_95count(slim_v_source, slim_v_tokens, INT64_C(3), slim_v_closing, INT64_C(0), slim_allocation_region);
(void)slim_v_mains;
slim_result = slim_v_mains == INT64_C(1);
}
}
}
}
}
return slim_result;
}

int main(int argc, char **argv) {
SlimAllocStatus slim_status;
slim_alloc_status_init(&slim_status);
SlimRegion slim_root;
slim_rt_init(&slim_root, &slim_status);
SlimVec slim_args = slim_vec_new(sizeof(SlimBytes), &slim_root);
for (int slim_i = 0; slim_i < argc; ++slim_i) {
SlimBytes slim_arg = slim_bytes_static((const uint8_t *)argv[slim_i], (int64_t)strlen(argv[slim_i]));
if (!slim_vec_push(&slim_args, &slim_arg)) {
slim_alloc_report(&slim_status);
slim_rt_shutdown();
return 71;
}
}
int64_t slim_exit_code = slim_fn_main(slim_args, &slim_root);
if (slim_region_failed(&slim_root)) {
slim_alloc_report(&slim_status);
slim_rt_shutdown();
return 71;
}
if (slim_exit_code < 0 || slim_exit_code > 255) {
slim_rt_trap("main result is outside 0..255");
}
slim_rt_shutdown();
return (int)slim_exit_code;
}
